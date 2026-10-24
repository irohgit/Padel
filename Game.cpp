#pragma once


namespace{
    const int WINDOW_WIDTH = 1920;
    const int WINDOW_HEIGHT = 1080;
    const float MAX_DELTA = 0.05f;
}
// SCORE: Namespace and Constants 
// 1,2 WINDOW_WIDTH,WINDOW_HEIGHT
// 3, MAX_DELTA

// SOUNDCHECK: Destructor, Initialize and Construction
// 1 Game Destructor //
// 2 Initialize

// PERFORMANCE: Run Loop
// 1. Calculate Things (read the score)
// 2. Update The State (play the note)
// 3. Output (Audience hears it/sees it)


// PACKDOWN: Pack down in reverse order, audience leaves first then band, then lights off
// 1. Destroy Renderer
// 2. Destroy Window
// 3. Destroy Game

