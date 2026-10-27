//Include Guard: Only for Game.h to show understanding of #pragma once
#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include "Theme.h"
#include "Player.h"
#include "Ball.h"
#include "MatchEvents.h"
#include "LogListener.h"
#include "DebugOverlay.h"
#include "AudioSystem.h"
#include "AudioDirector.h"



//PROGRAMME - Who's in tonight's show and what they can do. How they play the music is in Game.cpp

class Game{
public: // FRONT OF HOUSE (public): What the theatre manager (main) may ask for
    Game() = default; // use C++ Default Constructor
    ~Game(); // Safety net: Calls ShutDown when the Game's lifetime ends. 
    Game(const Game&) = delete; // Block copy constructor 
    Game& operator=(const Game&) = delete; // Block Assignment Operator
    
    bool Initialize(); // Needs to return true for main to check 
    void RunLoop();
    void ShutDown();

private:
    // CUE SHEET: Conductor (RunLoop) calls, in order
    float ComputeDeltaTime();

    void AssignController(int DeviceIndex);
    void CheckPaddleHits(float PreviousX);
    void CheckAudioEvents();
    void GenerateOutput();
    void MoveBall(float DeltaTime);
    void MovePlayers(float DeltaTime);
    void ProcessInput();
    void RemoveController(SDL_JoystickID InstanceID);
    void ReturnBall(const PaddleState& Paddle, float ContactX);
    void ServeBall(float Direction);
    void ScorePoint(Player& Scorer, float ServeDirection);
    void SpawnPlayers();
    void UpdateGame(float DeltaTime);
    void UpdateOverlayStatus();

    // PRODUCTION: What the show needs to run
    // What It Has
    AudioSystem Audio;
    AudioDirector Director{Audio,1920.0f}; //C++ Builds members top to bottom and destroys them bottom to top.
    SDL_Window* Window = nullptr;
    SDL_Renderer* Renderer = nullptr;

    Player LeftPlayer;
    Player RightPlayer;

    BallState Ball;

    LogListener Logger;
    DebugOverlay Overlay;
    MatchEvents Events;

    // What it Knows
    bool bIsRunning = false;
    bool bIsSdlStarted = false;
    Uint64 LastCounter = 0;
    int Rally = 0;
    SDL_Color CourtColour = Theme::Color::COURT_CLASSIC;
    

};

#endif // GAME_H
