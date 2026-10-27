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

    void AddPoint();
    void ReleaseController();
    void ResetScore();

    void AddSet();
    int GetSets() const;
    void ResetMatch();

    // Player Controls
    void SetMoveDirection(float Direction);
    void Move(float DeltaTime, float MinY, float MaxY);
    
    // Player Input
    void AssignController(SDL_GameController* Pad);
    bool HasController() const {return Controller != nullptr;}
    SDL_JoystickID GetControllerID() const;
    float ReadPadY() const;

private:
    PaddleState Paddle;
    int Score = 0;
    int Sets = 0;
    SDL_GameController* Controller = nullptr;
    float MoveDirection = 0.0f;
    float PlayerSpeed = 500.0f;

};

