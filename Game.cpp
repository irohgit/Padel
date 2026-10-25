#include "Game.h"

// SCORE: Namespace and Constants 
namespace{
    constexpr int WINDOW_WIDTH = 1920;
    constexpr int WINDOW_HEIGHT = 1080;
    constexpr float MAX_DELTA = 0.05f;
    constexpr const char *WINDOW_TITLE = "Padel";
    constexpr const int GRAPHICS_DRIVER = -1; // -1 Makes SDL choose
}

bool Game::Initialize(){
    // Ask , Check, Record
    // 1. Start SDL: Ask, check, record pattern
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0){ 
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    } // Success is zero as per SDL2 SDL_Init Documentation

    // Successful Initiation  : record SDL start success bool
    bIsSdlStarted = true; 
    
/*
extern DECLSPEC SDL_Window * SDLCALL SDL_CreateWindow(const char *title,int x, int y, int w, int h, Uint32 flags);
extern DECLSPEC SDL_Renderer * SDLCALL SDL_CreateRenderer(SDL_Window * window, int index, Uint32 flags);
*/ 
    Window = SDL_CreateWindow(WINDOW_TITLE,
                                SDL_WINDOW_FULLSCREEN_DESKTOP,
                                SDL_WINDOW_FULLSCREEN_DESKTOP,
                                WINDOW_WIDTH,
                                WINDOW_HEIGHT,
                                SDL_WINDOW_FULLSCREEN);
    
    // Use flags ACCELERATED to draw on GPU, use PresentVSYNC so that SDL_RendererPresent will wait for monitor refresh to avoid screen tearing
    Renderer = SDL_CreateRenderer(Window,GRAPHICS_DRIVER,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);

    // TODO (4) : Start Performance Counter, set game to Is running TrueACCELERA

    return true;
}

// SOUNDCHECK: Destructor, Initialize and Construction
// 1 Game Destructor //
// 2 Initialize

// PERFORMANCE: Run Loop
// 1. Calculate Things (read the score)
// 2. Update The State (play the note)
// 3. Output (Audience hears it/sees it)


// PACKDOWN: Pack down in reverse order, audience leaves first then band, then lights off
// 1. Destroy Renderer
// 2. Destroy Window
// 3. Destroy Game

