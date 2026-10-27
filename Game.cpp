#include "Game.h"
#include "Theme.h" // Pragma once in Theme.h will skip this include. Added here for readability 
#include "Player.h"
#include <string>
#include <random>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <optional>
#include "PixelFont.h"
#include "AudioPaths.h"

// SCORE: Namespace and Constants 
namespace{
    constexpr int WINDOW_WIDTH = 1920;
    constexpr int WINDOW_HEIGHT = 1080;
    constexpr float MAX_DELTA = 0.05f;
    constexpr const char *WINDOW_TITLE = "Padel";
    constexpr const int GRAPHICS_DRIVER = -1; // -1 Makes SDL choose the graphics driver for us
    constexpr float PADDLE_WIDTH = 10.0f;
    constexpr float PADDLE_HEIGHT = 100.0f;
    constexpr float PADDLE_MARGIN = 100.0f; // Distance from screen edge
    constexpr float PADDLE_MIN_Y = PADDLE_HEIGHT / 2.0f;
    constexpr float PADDLE_MAX_Y = WINDOW_HEIGHT - PADDLE_HEIGHT/2.0f;
    constexpr float BALL_SIZE = 50.0f;
    constexpr float BALL_SERVE_SPEED_X = 250.0f;
    constexpr float BALL_SPEED_X = 650.0f;
    constexpr float BALL_SPEED_Y = 450.0f;
    constexpr float BALL_MIN_Y = BALL_SIZE / 2.0f;
    constexpr float BALL_MAX_Y = WINDOW_HEIGHT - BALL_SIZE / 2.0f;
    constexpr float BALL_SPEEDUP = 1.25f; // Each return will speed up ball by 25%
    constexpr float BALL_MAX_SPEED_X = 1000.0f;
    constexpr float BALL_MAX_SPEED_Y = 350.0f;
    constexpr float HIT_ZONE = PADDLE_MARGIN + PADDLE_WIDTH / 2.0f + BALL_SIZE + 20.0f;
    constexpr int GAMES_TO_WIN_SET = 2;
    constexpr int BEST_OF_SETS = 3;
    constexpr int SETS_TO_WIN_MATCH = BEST_OF_SETS / 2 + 1;
    constexpr float SCORE_OFFSET = 160.0f;
    constexpr float SCORE_Y = 50.0f;
    constexpr float SCORE_CELL = 10.0f;
    constexpr float SETS_GAP = 16.0f;
    constexpr float SET_BOX_SIZE = BALL_SIZE/2.0f;
    constexpr float SET_BOX_GAP = BALL_SIZE/3.0f;

    bool OverlapsVertically(float BallY, float PaddleY){
        return std::abs(BallY - PaddleY) <= PADDLE_HEIGHT/2.0f + BALL_SIZE /2.0f;
    }

    const char* PanSide(float Pan){
        if (Pan < -0.33f) return "LEFT";
        if (Pan <  0.33f) return "CENTRE";
        return "RIGHT";
    }

    // Win by 2
    bool WonGame(int ScorerPoints, int OpponentPoints){
        return ScorerPoints >= 4 && ScorerPoints - OpponentPoints >= 2;
    }
    const char *OutcomeName(PointOutcome Outcome){
        switch(Outcome){
            case PointOutcome::Point : return "Point";
            case PointOutcome::Game : return "Game";
            case PointOutcome::Set : return "Set";
            case PointOutcome::Match : return "Match";
        }
        return "?";
    }
    const char* PointLabel(int MyPoints, int OpponentPoints){
        if(MyPoints >= 3 && OpponentPoints >= 3){
            return (MyPoints > OpponentPoints) ? "AD" : "40";
        }
        static constexpr const char* LABELS[] = {"00", "15", "30","40"};
        return LABELS[std::clamp(MyPoints,0,3)];
    }
    void DrawSetBoxes(SDL_Renderer* Renderer, int SetsWon, float CentreX, float Y){
        const float Total = SETS_TO_WIN_MATCH * SET_BOX_SIZE + (SETS_TO_WIN_MATCH - 1) * SET_BOX_GAP;
        float X = CentreX - Total/2.0f;
        for(int i = 0; i < SETS_TO_WIN_MATCH; ++i){
            const SDL_FRect Box = {X,Y,SET_BOX_SIZE, SET_BOX_SIZE};
            if (i < SetsWon) {SDL_RenderFillRectF(Renderer, &Box);}
            else {SDL_RenderDrawRectF(Renderer, &Box); }
            X += SET_BOX_SIZE + SET_BOX_GAP;
        }
    }
    void DrawPlayerScore(SDL_Renderer* Renderer, const Player& Me, const Player& Opponent, float CentreX){
        const std::string Score = std::to_string(Me.GetGames()) + "." + PointLabel(Me.GetScore(), Opponent.GetScore());
        const float BoxesY = SCORE_Y + PixelFont::TextHeight(SCORE_CELL) + SETS_GAP;

        PixelFont::DrawTextCentred(Renderer, Score.c_str(), CentreX, SCORE_Y, SCORE_CELL);
        DrawSetBoxes(Renderer, Me.GetSets(), CentreX, BoxesY);
    }

    // Template for Drawing On Screen an Object with a root that is centered
    template <typename T>
    void DrawCentred(SDL_Renderer* Renderer, const T& Object, float Width, float Height){
        SDL_FRect Rect = {Object.X - Width/2.0f,
                          Object.Y - Height/2.0f,
                          Width,
                          Height};
        SDL_RenderFillRectF(Renderer, &Rect);
    }
    // Templates for Guards
    template <typename T>
    bool NullGuard(const T* Resource, const char* CreatorName) // NullGuard: True if the resource exists. If false, then it will log which SDL call failed.
    {
        if(Resource) return true;
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"%s failed: %s", CreatorName, SDL_GetError());
        return false;
    }

    bool SDLGuard(int Result, const char* CallName)
    {
        if (Result == 0) return true;
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"%s failed: %s", CallName, SDL_GetError());
        return false;
    }
     
    // Templates for packdown
    template <typename T, typename ReleaseFunction>
    void Release(T*& Resource, ReleaseFunction Destroy)
    {
        if(Resource){Destroy(Resource); Resource = nullptr;}
    }
    void Release(bool& bStarted, void(*Shutdown)())
    {
        if(bStarted){Shutdown(); bStarted = false;}
    }

    // Math Helpers
    std::mt19937& RandomEngine(){ static std::mt19937 Engine{ std::random_device{}() }; return Engine;}
    float RandomRange(float Min, float Max){std::uniform_real_distribution<float>Range(Min,Max); return Range(RandomEngine());}
    bool CoinFlip(){ return std::bernoulli_distribution(0.5)(RandomEngine());}
    float NormalisedSpeed(float Velocity, float MaxSpeed){return std::clamp(std::abs(Velocity) / MaxSpeed,0.0f,1.0f);}

    // Debug and Development Helpers
    void checkSDLVersion(){SDL_version Version; SDL_GetVersion(&Version);SDL_Log("SDL: %d.%d.%d", Version.major, Version.minor, Version.patch);}

    // Raw input → menu action. Screens never see keys or buttons, only actions.
    std::optional<MenuAction> ToMenuAction(const SDL_Event& Event){
        if (Event.type == SDL_KEYDOWN){
            switch (Event.key.keysym.scancode){
                case SDL_SCANCODE_UP:     case SDL_SCANCODE_W:     return MenuAction::Up;
                case SDL_SCANCODE_DOWN:   case SDL_SCANCODE_S:     return MenuAction::Down;
                case SDL_SCANCODE_RETURN: case SDL_SCANCODE_SPACE: return MenuAction::Accept;
                case SDL_SCANCODE_BACKSPACE:                       return MenuAction::Cancel;
                default: break;
            }
        }
        if (Event.type == SDL_CONTROLLERBUTTONDOWN){
            switch (Event.cbutton.button){
                case SDL_CONTROLLER_BUTTON_DPAD_UP:   return MenuAction::Up;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return MenuAction::Down;
                case SDL_CONTROLLER_BUTTON_A:         return MenuAction::Accept;
                case SDL_CONTROLLER_BUTTON_B:         return MenuAction::Cancel;
                default: break;
            }
        }
        return std::nullopt;   // not a menu input
    }
}

bool Game::Initialize(){
    // 1. Start SDL: Ask, check, record pattern
    if (!SDLGuard(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER), "SDL_Init")) return false;// Success is zero as per SDL2 SDL_Init Documentation
    bIsSdlStarted = true; // Successful Initiation  : record SDL start success bool
                          
    Window = SDL_CreateWindow(WINDOW_TITLE,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH, WINDOW_HEIGHT,SDL_WINDOW_FULLSCREEN_DESKTOP);
    // Window
    if (!NullGuard(Window, "SDL_CreateWindow")) return false;
    // Renderer
    Renderer = SDL_CreateRenderer(Window,GRAPHICS_DRIVER,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!NullGuard(Renderer, "SDL_CreateRenderer")) return false;
    if (!SDLGuard(SDL_RenderSetLogicalSize(Renderer, WINDOW_WIDTH,WINDOW_HEIGHT), "SDL_RenderSetLogicalSize")) return false;
    // Load Title Screen
    if (!Title.Load(Renderer)) return false;
    // Initialize Audio and Load Master Banks
    if (!Audio.Initialize()){return false;} // AudioSystem logs the FMOD error itself no need for SDL log        
    Audio.LoadBank(AudioPaths::MASTER_STRINGS_BANK);
    Audio.LoadBank(AudioPaths::MASTER_BANK);
    CheckAudioEvents();

    LastCounter = SDL_GetPerformanceCounter(); 
    SpawnPlayers();
    // Event Subscription
    Events.Subscribe(&Logger);
    Events.Subscribe(&Overlay);
    Events.Subscribe(&Director);
    ServeBall(CoinFlip() ? 1.0f : -1.0f);
    bIsRunning = true;

    // Temporary Helper for upgrade - Delete for production
    checkSDLVersion();

    return true;
}
void Game::ProcessInput(){
    // 1. Events: global and system first, then the screen gets what's left (my _unhandled_input rule)
    SDL_Event Event;
    while (SDL_PollEvent(&Event)){
        if(Event.type == SDL_QUIT){bIsRunning = false;}
        else if(Event.type == SDL_KEYDOWN && Event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){bIsRunning = false;}
        else if(Event.type == SDL_KEYDOWN && State == GameState::Title){ Title.Skip(); }
        else if(Event.type == SDL_KEYDOWN && Event.key.keysym.scancode == SDL_SCANCODE_F1){Overlay.Toggle();}
        else if(Event.type == SDL_CONTROLLERDEVICEADDED) {AssignController(Event.cdevice.which);}
        else if(Event.type == SDL_CONTROLLERDEVICEREMOVED){RemoveController(Event.cdevice.which);}
        else if(State == GameState::Menu){
            if (const auto Action = ToMenuAction(Event)){             // only if it IS a menu action
                if (Menu.HandleAction(*Action)) { ChooseMenuItem(); } // Accept: Game decides what it means
            }
        }
    }

    //Create the array of the current state of the keyboard, 1 is pressed and 0 is not
    const Uint8* Keys = SDL_GetKeyboardState(nullptr);
    const float LeftKeys  = static_cast<float>(Keys[SDL_SCANCODE_S] - Keys[SDL_SCANCODE_W]);
    const float RightKeys = static_cast<float>(Keys[SDL_SCANCODE_DOWN] - Keys[SDL_SCANCODE_UP]);
    const float LeftPad   = LeftPlayer.ReadPadY();
    const float RightPad  = RightPlayer.ReadPadY();
    // Pad wins when it's being used; otherwise the keyboard
    LeftPlayer.SetMoveDirection(LeftPad   != 0.0f ? LeftPad   : LeftKeys);
    RightPlayer.SetMoveDirection(RightPad != 0.0f ? RightPad  : RightKeys);
}

void Game::UpdateGame(float DeltaTime){
    if(State == GameState::Title){
        Title.Update(DeltaTime);
        if(Title.ConsumeStingCue()){Audio.PlayEvent(AudioPaths::TITLE_STING_EVENT);}
        if(Title.IsFinished()){
            State = GameState::Menu;
            Menu.OnOpen();
            if(!MenuMusic.IsValid()){MenuMusic = Audio.PlayEvent(AudioPaths::MENU_MUSIC_EVENT);}
        }
        Audio.Update(DeltaTime);
        return;
    }
    //Freeze Gamestate until player selects a menu option
    if(State == GameState::Menu){
        Title.Update(DeltaTime);
        Audio.Update(DeltaTime);
        return;
    }
    MovePlayers(DeltaTime);
    MoveBall(DeltaTime);
    UpdateOverlayStatus();
    Director.Update();
    Audio.Update(DeltaTime);
}
void Game::GenerateOutput(){
    if (State == GameState::Title || State == GameState::Menu){
        Title.Draw(Renderer);
        if(State == GameState::Menu) {Menu.Draw(Renderer);}
        SDL_RenderPresent(Renderer);
        return;
    }
    //Ensure blank canvas
    Theme::SetDrawColor(Renderer, CourtColour);
    SDL_RenderClear(Renderer);

    // Draw Points and Sets Order The Scoring first so that ball renders over
    const float Middle = WINDOW_WIDTH/2.0f;
    Theme::SetDrawColor(Renderer, Theme::Color::TEXT_NORMAL);
    DrawPlayerScore(Renderer, LeftPlayer, RightPlayer, Middle - SCORE_OFFSET);
    DrawPlayerScore(Renderer, RightPlayer, LeftPlayer, Middle + SCORE_OFFSET);

    // Draw Paddles
    Theme::SetDrawColor(Renderer, Theme::Color::PADDLE);
    DrawCentred(Renderer, LeftPlayer.GetPaddle(), PADDLE_WIDTH,PADDLE_HEIGHT);
    DrawCentred(Renderer, RightPlayer.GetPaddle(), PADDLE_WIDTH,PADDLE_HEIGHT);

    // Draw Ball
    Theme::SetDrawColor(Renderer, Theme::Color::BALL);
    DrawCentred(Renderer, Ball, BALL_SIZE, BALL_SIZE); 
    
    //Draw Overlay
    Overlay.Draw(Renderer);
    // Render 
    SDL_RenderPresent(Renderer);
}

void Game::RunLoop(){
    while (bIsRunning){

    //1. Cues
    ProcessInput();
    //2. Measure deltaTime
    float DeltaTime = ComputeDeltaTime();
    UpdateGame(DeltaTime);
    //3. Output Simulation 
    GenerateOutput();
    }
}
void Game::ShutDown(){// Ensure order of shutdown is Renderer, Window and the Game
    LeftPlayer.ReleaseController();
    RightPlayer.ReleaseController();
    Title.Release();
    Audio.ShutDown();
    Release(Renderer, SDL_DestroyRenderer);//1. Renderer: If It exists,destroy the Renderer
    Release(Window, SDL_DestroyWindow);//2. Window: If it exists, destroy the Window
    Release(bIsSdlStarted,SDL_Quit);//3. If started, run SDL_Quit and reset bIsSdlStarted to false
}

Game::~Game(){ShutDown();} // Safety call. Destructor will ShutDown


// HELPER FUNCTIONS

void Game::CloseLevels(PointOutcome Outcome, CourtSide Winner)
{
    for (int Level = static_cast<int>(PointOutcome::Game); 
         Level <= static_cast<int>(Outcome); 
         ++Level)
    {
        switch (static_cast<PointOutcome>(Level)){
            case PointOutcome::Game:
                LeftPlayer.ResetScore(); RightPlayer.ResetScore();
                break;
            case PointOutcome::Set:
                LeftPlayer.ResetGames(); RightPlayer.ResetGames();
                Events.Publish({MatchEventType::SetWon, Winner});
                break;
            case PointOutcome::Match:
                Events.Publish({MatchEventType::MatchEnd, Winner});
                LeftPlayer.ResetMatch() ; RightPlayer.ResetMatch();
                break;
            case PointOutcome::Point:
                break;
        }
    }
}
float Game::ComputeDeltaTime(){
    const Uint64 Now = SDL_GetPerformanceCounter();
    float DeltaTime = static_cast<float>(Now-LastCounter)
                    / static_cast<float>(SDL_GetPerformanceFrequency());
    LastCounter = Now;
    if(DeltaTime > MAX_DELTA){DeltaTime = MAX_DELTA;}
    return DeltaTime;
}

void Game::SpawnPlayers(){
    LeftPlayer.SpawnAt(PADDLE_MARGIN,WINDOW_HEIGHT/2.0f);
    RightPlayer.SpawnAt(WINDOW_WIDTH-PADDLE_MARGIN,WINDOW_HEIGHT/2.0f);
}
void Game::MovePlayers(float DeltaTime){
    LeftPlayer.Move(DeltaTime, PADDLE_MIN_Y,PADDLE_MAX_Y);
    RightPlayer.Move(DeltaTime, PADDLE_MIN_Y,PADDLE_MAX_Y);
}
void Game::ServeBall(float Direction){
 // Start ball in the middle of the court and randomize X Direction and Y Velocityat 
 const float VelocityY = RandomRange(-BALL_SPEED_Y, BALL_SPEED_Y);
 Ball = {WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f, Direction * BALL_SERVE_SPEED_X, VelocityY};  
}

void Game::MoveBall(float DeltaTime){
    //Remember X before moving
    const float PreviousX = Ball.X;

    //Place updated Velocities into Ball.X and Y
    Ball.X += Ball.VelocityX * DeltaTime;
    Ball.Y += Ball.VelocityY * DeltaTime;
    //When the ball moves beyond the window top and bottom edges, place it back in and then reverse the y direction
    if(Ball.Y < BALL_MIN_Y){ Ball.Y = BALL_MIN_Y; Ball.VelocityY = -Ball.VelocityY;
        Events.Publish({MatchEventType::WallBounce,
                             CourtSide::None, 
                             Ball.X,Ball.Y,
                             NormalisedSpeed(Ball.VelocityY, BALL_MAX_SPEED_Y)});
    }
    else if(Ball.Y > BALL_MAX_Y){ Ball.Y = BALL_MAX_Y; Ball.VelocityY = -Ball.VelocityY;
        Events.Publish({MatchEventType::WallBounce,
                             CourtSide::None,
                             Ball.X,Ball.Y,
                             NormalisedSpeed(Ball.VelocityY,BALL_MAX_SPEED_Y)});
    }
    //Start checking for collisions only when Ball is in the HitZone
    if(Ball.X < HIT_ZONE || Ball.X > WINDOW_WIDTH - HIT_ZONE){ CheckPaddleHits(PreviousX);}
    if(Ball.X < 0.0f){ScorePoint(RightPlayer, -1.0f);}
    else if(Ball.X > WINDOW_WIDTH){ScorePoint(LeftPlayer, 1.0f);}
}

void Game::ScorePoint(Player &Scorer, float ServeDirection){
    Player& Opponent = (&Scorer == &LeftPlayer) ? RightPlayer : LeftPlayer;
    const CourtSide ScorerSide = (&Scorer == &LeftPlayer) ? CourtSide::Left : CourtSide::Right;

    // 1. Score point then work out what the point does to game,set and match
    Scorer.AddPoint();
    PointOutcome Outcome = PointOutcome::Point;
    if(WonGame(Scorer.GetScore(), Opponent.GetScore())){
        Outcome = PointOutcome::Game;
        Scorer.AddGame();
        if(Scorer.GetGames() >= GAMES_TO_WIN_SET){
            Outcome = PointOutcome::Set;
            Scorer.AddSet();
            if(Scorer.GetSets() >= SETS_TO_WIN_MATCH){Outcome = PointOutcome::Match;}
        }
    }

    // Job 2. Assign Data to Scores
    MatchEvent Scored{MatchEventType::PointScored, ScorerSide, Ball.X,Ball.Y,0.0f, Rally};
    Scored.Outcome = Outcome;
    Scored.LeftPoints = LeftPlayer.GetScore();
    Scored.RightPoints = RightPlayer.GetScore();
    Events.Publish(Scored);
    SDL_Log("%s scores | points %d-%d | games %d-%d | sets %d-%d | %s",
            ScorerSide == CourtSide::Left ? "Left" : "Right",
            LeftPlayer.GetScore(), RightPlayer.GetScore(),
            LeftPlayer.GetGames(), RightPlayer.GetGames(),
            LeftPlayer.GetSets(),  RightPlayer.GetSets(),
            OutcomeName(Outcome));   
    // Job 3. Close
    CloseLevels(Outcome, ScorerSide);
   
    Rally = 0;
    ServeBall(ServeDirection);
}

void Game::CheckPaddleHits(float PreviousX){
    const float Direction = (Ball.VelocityX < 0.0f) ? -1.0f : 1.0f;
    const PaddleState& Paddle = (Direction < 0.0f) ? LeftPlayer.GetPaddle() : RightPlayer.GetPaddle();
    const float HalfBall = BALL_SIZE/2.0f;

    const float PaddleFace = Paddle.X - Direction * PADDLE_WIDTH /2.0f;
    const float PreviousEdge = PreviousX + Direction * HalfBall;
    const float CurrentEdge = Ball.X + Direction * HalfBall;

    const bool bIsColliding = (PreviousEdge - PaddleFace) * Direction <= 0.0f
                       &&(CurrentEdge - PaddleFace) * Direction >= 0.0f;

    if (bIsColliding && OverlapsVertically(Ball.Y,Paddle.Y)){
        ReturnBall(Paddle,PaddleFace - Direction * HalfBall);
    }

}

//void Game::ReturnBall(const PaddleState &Paddle, float ContactX)
void Game::ReturnBall(const PaddleState &Paddle, float ContactX){
    Ball.X = ContactX;
    Ball.VelocityX = std::clamp(-Ball.VelocityX * BALL_SPEEDUP,
                                -BALL_MAX_SPEED_X,
                                BALL_MAX_SPEED_X);
    const float Offset = std::clamp((Ball.Y - Paddle.Y) / (PADDLE_HEIGHT /2.0f), -1.0f, 1.0f);
    Ball.VelocityY = Offset * BALL_MAX_SPEED_Y;

    ++Rally;

    const CourtSide Hitter = (Ball.VelocityX > 0.0f) ? CourtSide::Left : CourtSide::Right;
    const float Power = NormalisedSpeed(Ball.VelocityX,BALL_MAX_SPEED_X);
    Events.Publish({MatchEventType::PaddleHit, Hitter,Ball.X,Ball.Y,Power,Rally});
}

void Game::UpdateOverlayStatus(){
    char Text[48];

    std::snprintf(Text, sizeof(Text), "WEIGHT: %s (%s)",
                   Director.GetLastWeight(), 
                   Director.GetLastSound());
    Overlay.SetStatus(0,Text);

    const float Pan = Director.PanFromX(Ball.X);
    std::snprintf(Text, sizeof(Text), "PAN: %.2f %s", Pan, PanSide(Pan));
    Overlay.SetStatus(1, Text);

}

void Game::CheckAudioEvents(){
    for (const char* Path : AudioPaths::REQUIRED_EVENTS){
        if(!Audio.HasEvent(Path)){
           SDL_Log("Missing FMOD event: %s", Path);
           Overlay.AddLine("Missing Audio Event - Please check Log");
        }
    }
}

void Game::AssignController(int DeviceIndex)
{
    if (!SDL_IsGameController(DeviceIndex)) {return;} // Is the device connected a gamepad?
    const SDL_JoystickID ID = SDL_JoystickGetDeviceInstanceID(DeviceIndex); // What type is it?
                                                                           
    if (LeftPlayer.GetControllerID() == ID || RightPlayer.GetControllerID() == ID) {return;}
    
    Player *Target = !LeftPlayer.HasController() ? &LeftPlayer
                   : !RightPlayer.HasController() ? &RightPlayer
                   : nullptr;
    if (!Target) {return;} // Both players have game pads
    
    SDL_GameController* Pad = SDL_GameControllerOpen(DeviceIndex);
    if (!NullGuard(Pad, "SDL_GameControllerOpen")){return;}
    Target->AssignController(Pad);
    SDL_Log("Controller %s -> %s", SDL_GameControllerName(Pad), Target == &LeftPlayer ? "Left" : "Right");

}

void Game::RemoveController(SDL_JoystickID InstanceID)
{
    if(LeftPlayer.GetControllerID() == InstanceID){LeftPlayer.ReleaseController();}
    if(RightPlayer.GetControllerID() == InstanceID){RightPlayer.ReleaseController();}
}

// What each menu item does. The menu only reports the choice; Game owns the consequences.
void Game::ChooseMenuItem(){
    switch (Menu.GetSelected()){
        case MainMenu::Item::StartGame: StartMatch(); break;
        case MainMenu::Item::Settings:  break;                    // not built yet
        case MainMenu::Item::Exit:      bIsRunning = false; break;
    }
}

// A fresh match: clean scores, paddles home, new serve
void Game::StartMatch(){
    MenuMusic.Stop(true);
    LeftPlayer.ResetMatch();
    RightPlayer.ResetMatch();
    SpawnPlayers();
    Rally = 0;
    ServeBall(CoinFlip() ? 1.0f : -1.0f);
    State = GameState::Playing;
}

// SOUNDCHECK: Destructor, Initialize and Construction
//
// 1 Game Destructor //
// 2 Initialize

// PERFORMANCE: Run Loop
// 1. Calculate Things (read the score)
// 2. Update The State (play the note)
// 3. Output (Audience hears it/sees it)


/*
   extern DECLSPEC SDL_Window * SDLCALL SDL_CreateWindow(const char *title,int x, int y, int w, int h, Uint32 flags);
   extern DECLSPEC SDL_Renderer * SDLCALL SDL_CreateRenderer(SDL_Window * window, int index, Uint32 flags);
   extern DECLSPEC int SDLCALL SDL_RenderSetLogicalSize(SDL_Renderer * renderer, int w, int h);
   extern DECLSPEC Uint64 SDLCALL SDL_GetPerformanceCounter(void);
   extern DECLSPEC const Uint8 *SDLCALL SDL_GetKeyboardState(int *numkeys);
   Use flags ACCELERATED to draw on GPU, use PresentVSYNC so that SDL_RendererPresent will wait for monitor refresh to avoid screen tearing
*/ 


