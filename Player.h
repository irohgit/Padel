#pragma once
#include <SDL/SDL.h>

struct PaddleState{
    float X = 0.0f;
    float Y = 0.0f;
};


class Player{
public:
    Player() = default;
    ~Player(); // Safety

    Player(const Player&) = delete; // One player/owner per controller
    Player& operator=(const Player&) = delete; // Protect copy assignment operator
    
    void SpawnAt(float X, float Y);

    const PaddleState& GetPaddle() const { return Paddle;}
    int GetScore() const {return Score;}

    void AddPoint() { ++Score;}
    void ReleaseController();

private:
    PaddleState Paddle;
    int Score = 0;
    SDL_GameController* Controller = nullptr;
};

