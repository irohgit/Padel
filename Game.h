//Include Guard: Only for Game.h to show understanding of #pragma once
#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>

//PROGRAMME - Who's in tonight's show and what they can do. How they play the music is in Game.cpp

class Game{
public: // FRONT OF HOUSE (public): What the theatre manager (main) may ask for
    Game() = default; // use C++ Default Constructor
    ~Game(); // Create destructor when shutting down game
    Game(const Game&) = delete; // Block copy constructor 
    Game& operator=(const Game&) = delete; // Block Assignment Operator
    
    bool Initialize(); // Needs to return true for main to check 
    void RunLoop();
    void ShutDown();
private:
    // CUE SHEET: Conductor (RunLoop) calls, in order
    void ProcessInput();
    float ComputeDeltaTime();
    void UpdateGame(float dt);
    void GenerateOutput();

    // PRODUCTION: What the show needs to run
    // What It Has
    SDL_Window* Window = nullptr;
    SDL_Renderer* Renderer = nullptr;
    // What it Knows
    bool bIsSdlStarted;
    bool bIsRunning;
    Uint64 LastCounter;

};


// BACKSTAGE (private): Only production
// 1. The Cycle director
//   . Process Input - Take in the cues 
//   . Compute Delta Time - How long since the last beat?
//   . UpdateGame(dt): play the note
//   . Generate Output:
//   THE PRODUCTION (member variables): what the show needs to run
//     What it has:
//       - mWindow:      the stage
//       - mRenderer:    the camera filming the stage for the audience
//     What it knows:
//       - mSdlStarted:  is the venue unlocked?
//       - mIsRunning:   is the show still on?
//       - mLastCounter: the metronome's last tick

#endif // GAME_H
