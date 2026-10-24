//Include Guard: Only for Game.h to show understanding of #pragma once
#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>

//PROGRAMME - Who's in tonight's show and what they can do. How they play the music is in Game.cpp

// FRONT OF HOUSE (public): What the theatre manager (main) may ask for
// 1.The production: use C++ default constructor
//   . Destructor:  Packdown, organized by the venue. Conductor or TheatreManager is not worried.
//   . Overload destructor to ensure no copies of it
// 2.Theatre Manager Commands
//  .Initialize: Soundcheck, get everything ready
//  .Runloop: "Begin the show"
//  .Shutdown: packdown curtain call


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
