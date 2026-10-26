#include "DebugOverlay.h"
#include "PixelFont.h"
#include "Theme.h"
#include <cstdio>

namespace {
    constexpr float PANEL_X     = 10.0f;
    constexpr float PANEL_Y     = 10.0f;
    constexpr float PANEL_WIDTH = 300.0f;
    constexpr float PADDING     = 6.0f;
    constexpr float CELL        = 2.0f;    
    constexpr float LINE_HEIGHT = 20.0f;

    const char* Name(MatchEventType Type){
        switch (Type){
            case MatchEventType::PaddleHit:   return "HIT";
            case MatchEventType::WallBounce:  return "WALL";
            case MatchEventType::PointScored: return "POINT";
            case MatchEventType::SetWon:      return "SET";
            case MatchEventType::MatchEnd:    return "MATCH";
        }
        return "?";
    }
    const char* Name(CourtSide Side){
        switch (Side){
            case CourtSide::Left:  return "LEFT";
            case CourtSide::Right: return "RIGHT";
            case CourtSide::None:  return "-";
        }
        return "?";
    }
}

void DebugOverlay::OnMatchEvent(const MatchEvent& Event){
    char Line[96];
    std::snprintf(Line, sizeof(Line), "%s %s X:%.0f Y:%.0f P:%.2f R:%d",
                  Name(Event.Type), Name(Event.Side), Event.X, Event.Y, Event.Power, Event.Rally);
    AddLine(Line);
}

void DebugOverlay::AddLine(const std::string& Line){
    Lines.push_back(Line);
    if (Lines.size() > MAX_LINES) { Lines.pop_front(); }   // the oldest line falls off
}

void DebugOverlay::Toggle(){ bIsVisible = !bIsVisible; }

void DebugOverlay::Draw(SDL_Renderer* Renderer) const {
    if (!bIsVisible || Lines.empty()) { return; }

    const SDL_FRect Panel = { PANEL_X, PANEL_Y, PANEL_WIDTH, PADDING * 2.0f + MAX_LINES * LINE_HEIGHT };
    SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
    Theme::SetDrawColor(Renderer, Theme::Color::DEBUG_PANEL);
    SDL_RenderFillRectF(Renderer, &Panel);
    SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);

    float Y = PANEL_Y + PADDING;
    for (std::size_t i = 0; i < Lines.size(); ++i){
        const bool bIsNewest = (i + 1 == Lines.size());
        Theme::SetDrawColor(Renderer, bIsNewest ? Theme::Color::TEXT_HIGHLIGHT : Theme::Color::TEXT_NORMAL);
        PixelFont::DrawText(Renderer, Lines[i].c_str(), PANEL_X + PADDING, Y, CELL, PixelFont::Weight::Regular);
        Y += LINE_HEIGHT;
    }
}
