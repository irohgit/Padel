#include "Game.h"
#include "Theme.h" // Pragma once in Theme.h will skip this include. Added here for readability 
#include "Player.h"
#include <random>

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
    constexpr float BALL_SIZE = 20.0f;
    constexpr float BALL_SPEED_X = 250.0f;
    constexpr float BALL_SPEED_Y = 250.0f;
    constexpr float BALL_MIN_Y = BALL_SIZE / 2.0f;
    constexpr float BALL_MAX_Y = WINDOW_HEIGHT - BALL_SIZE / 2.0f;
    
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
    ServeBall();
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
        else if(Event.type == SDL_KEYDOWN && Event.key.keysym.scancode == SDL_SCANCODE_ESCAPE  ){bIsRunning = false;}// Check if Escape is pressed then quit game
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

    Theme::SetDrawColor(Renderer, Theme::Color::PADDLE);
    DrawCentred(Renderer, LeftPlayer.GetPaddle(), PADDLE_WIDTH,PADDLE_HEIGHT);
    DrawCentred(Renderer, RightPlayer.GetPaddle(), PADDLE_WIDTH,PADDLE_HEIGHT);

    Theme::SetDrawColor(Renderer, Theme::Color::BALL);
    DrawCentred(Renderer, Ball, BALL_SIZE, BALL_SIZE); 

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
void Game::ServeBall(){
 // Start ball in the middle of the court and randomize X Direction and Y Velocityat 
 const float DirectionX = CoinFlip() ? 1.0f : -1.0f;
 const float VelocityY = RandomRange(-BALL_SPEED_Y, BALL_SPEED_Y);
 Ball = {WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f, DirectionX * BALL_SPEED_X, VelocityY};  
}

void Game::MoveBall(float DeltaTime){
    //Place updated Velocities into Ball.X and Y
    Ball.X += Ball.VelocityX * DeltaTime;
    Ball.Y += Ball.VelocityY * DeltaTime;
    //When the ball moves beyond the window top and bottom edges, place it back in and then reverse the y direction
    if(Ball.Y < BALL_MIN_Y){ Ball.Y = BALL_MIN_Y; Ball.VelocityY = -Ball.VelocityY;}
    else if(Ball.Y > BALL_MAX_Y){ Ball.Y = BALL_MAX_Y; Ball.VelocityY = -Ball.VelocityY;}

    // When ball leaves left or right side of window serve again (temporary)
    if(Ball.X < 0.0f || Ball.X > WINDOW_WIDTH){ ServeBall();}
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


