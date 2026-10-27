#include "Player.h"
#include <algorithm>
#include <cmath>

namespace{
    constexpr float STICK_DEADZONE = 0.2f;
}
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
void Player::AssignController(SDL_GameController *Pad){
    ReleaseController(); // Release old gamepads first
    Controller = Pad;
}
SDL_JoystickID Player::GetControllerID() const{
    if(!Controller) {return -1;}
    return SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(Controller));
}
float Player::ReadPadY() const{
    if(!Controller){return 0.0f;}

    const float Stick = std::clamp(
        SDL_GameControllerGetAxis(Controller, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f, -1.0f, 1.0f);
    if(std::abs(Stick) > STICK_DEADZONE){return Stick;}
        
    const int Down = SDL_GameControllerGetButton(Controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    const int Up = SDL_GameControllerGetButton(Controller, SDL_CONTROLLER_BUTTON_DPAD_UP);

    return static_cast<float>(Down-Up);
}
