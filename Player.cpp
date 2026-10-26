#include "Player.h"
 
Player::~Player(){ReleaseController();}

void Player::SpawnAt(float X, float Y){ Paddle = {X,Y}; }

void Player::ReleaseController(){
 if(Controller){SDL_GameControllerClose(Controller); Controller = nullptr;}
}
