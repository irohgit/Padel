#include "LogListener.h"
#include <SDL/SDL.h>
#include <cstdio>

namespace{
    const char* ToString(MatchEventType Type){
        switch (Type){
            case MatchEventType::PaddleHit: return "PaddleHit";
            case MatchEventType::WallBounce: return "WallBounce";
            case MatchEventType::PointScored: return "PointScored";
            case MatchEventType::SetWon: return "SetWon";
            case MatchEventType::MatchEnd: return "MatchEnd";
        }
        return "Unknown";
    }
    const char* ToString(CourtSide Side){
        switch (Side){
            case CourtSide::Left: return "Left";
            case CourtSide::Right: return "Right";
            case CourtSide::None: return "None";
        }
        return "Unknown";
    }
}

void LogListener::OnMatchEvent(const MatchEvent& Event){
    char Line[128];
    std::snprintf(Line, sizeof(Line), "[Match] %-11s side=%-5s x=%4.0f y=%4.0f power=%.2f rally=%d",
                  ToString(Event.Type), ToString(Event.Side), Event.X, Event.Y, Event.Power, Event.Rally);
    SDL_Log("%s", Line);
}


