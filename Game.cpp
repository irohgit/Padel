#include "Game.h"

// SCORE: Namespace and Constants 
namespace{
    const int WINDOW_WIDTH = 1920;
    const int WINDOW_HEIGHT = 1080;
    const float MAX_DELTA = 0.05f;
}

bool Game::Initialize(){
    // Ask , Check, Record
    // 1. Start SDL: Ask, check, record pattern
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0){ 
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    } // Success is zero as per SDL2 SDL_Init Documentation

    // TODO (1) : record SDL start success bool
    // TODO (2) : Create the window
    // TODO (3) : Create the renderer
    // TODO (4) : Start Performance Counter, set game to Is running True

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

