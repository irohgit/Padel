#include "Player.h"
#include <algorithm>
 
Player::~Player(){ReleaseController();}

//void Player::Move(float DeltaTime, float MinY, float MaxY)
void Player::Move(float DeltaTime, float MinY, float MaxY){
    Paddle.Y += MoveDirection * PlayerSpeed * DeltaTime;
    Paddle.Y = std::clamp(Paddle.Y, MinY, MaxY); // Bounds player to window
};
void Player::SetMoveDirection(float Direction){ MoveDirection = Direction;}
void Player::SpawnAt(float X, float Y){ Paddle = {X,Y}; }
void Player::ReleaseController(){ if(Controller){SDL_GameControllerClose(Controller); Controller = nullptr;}}
