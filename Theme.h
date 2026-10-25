#pragma once

#include <SDL/SDL.h>

// THEME: Padel's design  
namespace Theme
{
    namespace Color
    {
        constexpr SDL_Color MENU_BACKGROUND   = {  82,  31,  35, 255 };   // #521f23
        constexpr SDL_Color SELECT_BACKGROUND = {  29,  48,  38, 255 };   // #1d3026
        constexpr SDL_Color COURT_SUPER       = {  53, 115, 188, 255 };   // #3573bc
        constexpr SDL_Color COURT_CLASSIC     = { 110, 188, 102, 255 };   // #6ebc66
        constexpr SDL_Color TEXT_HIGHLIGHT    = { 255, 204,  90, 255 };   // #ffcc5a
        constexpr SDL_Color TEXT_NORMAL       = { 255, 255, 255, 255 };   // #ffffff
    }

    inline void SetDrawColor(SDL_Renderer* Renderer, SDL_Color Colour)
    {
        SDL_SetRenderDrawColor(Renderer, Colour.r, Colour.g, Colour.b, Colour.a);
    }
}
