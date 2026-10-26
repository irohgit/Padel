#include "Player.h"
#include <algorithm>
 
Player::~Player(){ReleaseController();}
void Player::AddPoint(){++Score;}
void Player::AddSet(){++Sets;}
int Player::GetSets() const{return Sets;}
void Player::ResetMatch(){Score = 0; Sets =0;}
void Player::Move(float DeltaTime, float MinY, float MaxY){
    Paddle.Y += MoveDirection * PlayerSpeed * DeltaTime;
    Paddle.Y = std::clamp(Paddle.Y, MinY, MaxY); // Bounds player to window
};
void Player::SetMoveDirection(float Direction){ MoveDirection = Direction;}
void Player::SpawnAt(float X, float Y){ Paddle = {X,Y}; }
void Player::ReleaseController(){ if(Controller){SDL_GameControllerClose(Controller); Controller = nullptr;}}
void Player::ResetScore(){Score = 0;}
