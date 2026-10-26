#include "Game.h"
#include "Theme.h" // Pragma once in Theme.h will skip this include. Added here for readability 
#include "Player.h"
#include <string>
#include <random>
#include <algorithm>
#include <cmath>
#include "PixelFont.h"

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
    constexpr float BALL_SPEED_X = 450.0f;
    constexpr float BALL_SPEED_Y = 250.0f;
    constexpr float BALL_MIN_Y = BALL_SIZE / 2.0f;
    constexpr float BALL_MAX_Y = WINDOW_HEIGHT - BALL_SIZE / 2.0f;
    constexpr float BALL_SPEEDUP = 1.25f; // Each return will speed up ball by 25%
    constexpr float BALL_MAX_SPEED_X = 1000.0f;
    constexpr float BALL_MAX_SPEED_Y = 350.0f;
    constexpr float HIT_ZONE = PADDLE_MARGIN + PADDLE_WIDTH / 2.0f + BALL_SIZE + 20.0f;
    constexpr int POINTS_TO_WIN_SET = 5;
    constexpr int BEST_OF_SETS = 3;
    constexpr int SETS_TO_WIN_MATCH = BEST_OF_SETS / 2 + 1;
    constexpr float SCORE_OFFSET = 160.0f;
    constexpr float SCORE_Y = 50.0f;
    constexpr float SCORE_CELL = 10.0f;
    constexpr float SETS_GAP = 16.0f;
    constexpr float SET_BOX_SIZE = BALL_SIZE/2.0f;
    constexpr float SET_BOX_GAP = BALL_SIZE/3.0f;

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
    void DrawPlayerScore(SDL_Renderer* Renderer, const Player& Player, float CentreX){
        const std::string Points = std::to_string(Player.GetScore());
        const float BoxesY = SCORE_Y + PixelFont::TextHeight(SCORE_CELL) + SETS_GAP;

        PixelFont::DrawTextCentred(Renderer, Points.c_str(), CentreX, SCORE_Y,SCORE_CELL);
        DrawSetBoxes(Renderer, Player.GetSets(), CentreX, BoxesY);
    }
    
    bool OverlapsVertically(float BallY, float PaddleY){
        return std::abs(BallY - PaddleY) <= PADDLE_HEIGHT/2.0f + BALL_SIZE /2.0f;
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

    // Debug and Development Helpers
    void checkSDLVersion(){SDL_version Version; SDL_GetVersion(&Version);SDL_Log("SDL: %d.%d.%d", Version.major, Version.minor, Version.patch);}

}

bool Game::Initialize(){
    // 1. Start SDL: Ask, check, record pattern
    if (!SDLGuard(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER), "SDL_Init")) return false;// Success is zero as per SDL2 SDL_Init Documentation
    bIsSdlStarted = true; // Successful Initiation  : record SDL start success bool
                          
    Window = SDL_CreateWindow(WINDOW_TITLE,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WINDOW_WIDTH, WINDOW_HEIGHT,SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!NullGuard(Window, "SDL_CreateWindow")) return false;

    Renderer = SDL_CreateRenderer(Window,GRAPHICS_DRIVER,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!NullGuard(Renderer, "SDL_CreateRenderer")) return false;
    if (!SDLGuard(SDL_RenderSetLogicalSize(Renderer, WINDOW_WIDTH,WINDOW_HEIGHT), "SDL_RenderSetLogicalSize")) return false;
        
    LastCounter = SDL_GetPerformanceCounter(); 
    SpawnPlayers();
    Events.Subscribe(&Logger);
    Events.Subscribe(&Overlay);
    ServeBall(CoinFlip() ? 1.0f : -1.0f);
    bIsRunning = true;

    // Temporary Helper for upgrade - Delete for production
    checkSDLVersion();

    return true;
}
void Game::ProcessInput(){
    // 1. Events
    SDL_Event Event;
    while (SDL_PollEvent(&Event)){
        if(Event.type == SDL_QUIT){bIsRunning = false;}
        else if(Event.type == SDL_KEYDOWN && Event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){bIsRunning = false;}// Check if Escape is pressed then quit game
        else if(Event.type == SDL_KEYDOWN && Event.key.keysym.scancode == SDL_SCANCODE_F1){Overlay.Toggle();}
    }

    //Create the array of the current state of the keyboard, 1 is pressed and 0 is not
    const Uint8* Keys = SDL_GetKeyboardState(nullptr);
    LeftPlayer.SetMoveDirection(static_cast<float>(Keys[SDL_SCANCODE_S] - Keys[SDL_SCANCODE_W]));
    RightPlayer.SetMoveDirection(static_cast<float>(Keys[SDL_SCANCODE_DOWN] - Keys[SDL_SCANCODE_UP]));
}

void Game::UpdateGame(float DeltaTime){
    MovePlayers(DeltaTime);
    MoveBall(DeltaTime);
}
void Game::GenerateOutput(){
    //Ensure blank canvas
    Theme::SetDrawColor(Renderer, CourtColour);
    SDL_RenderClear(Renderer);

    // Draw Points and Sets Order The Scoring first so that ball renders over
    const float Middle = WINDOW_WIDTH/2.0f;
    Theme::SetDrawColor(Renderer, Theme::Color::TEXT_NORMAL);
    DrawPlayerScore(Renderer, LeftPlayer, Middle - SCORE_OFFSET);
    DrawPlayerScore(Renderer, RightPlayer, Middle + SCORE_OFFSET);

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
    Release(Renderer, SDL_DestroyRenderer);//1. Renderer: If It exists,destroy the Renderer
    Release(Window, SDL_DestroyWindow);//2. Window: If it exists, destroy the Window
    Release(bIsSdlStarted,SDL_Quit);//3. If started, run SDL_Quit and reset bIsSdlStarted to false
}

Game::~Game(){ShutDown();} // Safety call. Destructor will ShutDown


// HELPER FUNCTIONS
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
        Events.Publish({MatchEventType::WallBounce,CourtSide::None, Ball.X,Ball.Y});
    }
    else if(Ball.Y > BALL_MAX_Y){ Ball.Y = BALL_MAX_Y; Ball.VelocityY = -Ball.VelocityY;
        Events.Publish({MatchEventType::WallBounce,CourtSide::None, Ball.X,Ball.Y});
    }

    //Start checking for collisions only when Ball is in the HitZone
    if(Ball.X < HIT_ZONE || Ball.X > WINDOW_WIDTH - HIT_ZONE){ CheckPaddleHits(PreviousX);}
    if(Ball.X < 0.0f){ScorePoint(RightPlayer, -1.0f);}
    else if(Ball.X > WINDOW_WIDTH){ScorePoint(LeftPlayer, 1.0f);}
}

//void Game::ScorePoint(Player &Scorer, float ServeDirection)
void Game::ScorePoint(Player &Scorer, float ServeDirection){
    const char* Name = (&Scorer == &LeftPlayer) ? "Left" : "Right";
    
    Scorer.AddPoint();
    SDL_Log("Point %s: %d - %d (rally %d)", Name, LeftPlayer.GetScore(),RightPlayer.GetScore(), Rally);
    const CourtSide ScorerSide =  (&Scorer == &LeftPlayer) ? CourtSide::Left : CourtSide::Right;
    Events.Publish({MatchEventType::PointScored, ScorerSide, Ball.X,Ball.Y, 0.0f, Rally});
    Rally = 0;

    if(Scorer.GetScore() >= POINTS_TO_WIN_SET){
        Scorer.AddSet();
        Events.Publish({MatchEventType::SetWon, ScorerSide});
        LeftPlayer.ResetScore(); RightPlayer.ResetScore();
        SDL_Log("Set to %s, Sets %d - %d", Name, LeftPlayer.GetSets(), RightPlayer.GetSets());
        if(Scorer.GetSets() >= SETS_TO_WIN_MATCH){
            Events.Publish({MatchEventType::MatchEnd, ScorerSide});
            SDL_Log("%s player wins the match", Name);
            LeftPlayer.ResetMatch();
            RightPlayer.ResetMatch();
        }
    }
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
    const float Power = std::abs(Ball.VelocityX) / BALL_MAX_SPEED_X;
    Events.Publish({MatchEventType::PaddleHit, Hitter,Ball.X,Ball.Y,Power,Rally});
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


