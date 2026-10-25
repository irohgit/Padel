#include "Game.h"

int main(int argc, char **argv){
    
    // Job 1: Construct
    Game Padel;

    // Job 2: Setup
    if(!Padel.Initialize())
    {
        Padel.ShutDown();
        return 1;
    }
    // Job 3: Doors Open and Perform
    Padel.RunLoop();
    // Job 4: Packdown
    Padel.ShutDown();
    // Job 5: Get Paid
    return 0;
}
