#pragma once
#include "MatchEvents.h"
#include <SDL/SDL.h>
#include <cstddef>
#include <deque>
#include <string>

// F1 Toggle Listener
class DebugOverlay : public IMatchListener {
public:
    void OnMatchEvent(const MatchEvent& Event) override;
    void AddLine(const std::string& Line);    
    void Toggle();
    void Draw(SDL_Renderer* Renderer) const;

private:
    static constexpr std::size_t MAX_LINES = 10;
    std::deque<std::string> Lines; // oldest at the front, newest at the back
    bool bIsVisible = true;
};
