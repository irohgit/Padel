//Include Guard: Only for Game.h to show understanding of #pragma once
#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include "Theme.h"
#include "Player.h"
#include "Ball.h"
#include "MatchEvents.h"
#include "LogListener.h"


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

    void CheckPaddleHits(float PreviousX);
    void GenerateOutput();
    void MoveBall(float DeltaTime);
    void MovePlayers(float DeltaTime);
    void ProcessInput();
    void ReturnBall(const PaddleState& Paddle, float ContactX);
    void ServeBall(float Direction);
    void ScorePoint(Player& Scorer, float ServeDirection);
    void SpawnPlayers();
    void UpdateGame(float DeltaTime);

    // PRODUCTION: What the show needs to run
    // What It Has
    SDL_Window* Window = nullptr;
    SDL_Renderer* Renderer = nullptr;

    Player LeftPlayer;
    Player RightPlayer;

    BallState Ball;

    LogListener Logger;
    MatchEvents Events;
    // What it Knows
    bool bIsRunning = false;
    bool bIsSdlStarted = false;
    Uint64 LastCounter = 0;
    int Rally = 0;
    SDL_Color CourtColour = Theme::Color::COURT_CLASSIC;
    

};

#endif // GAME_H
