#include "MainMenu.h"
#include "Theme.h"
#include "PixelFont.h"

namespace {
    // Order here = order on screen = order of MainMenu::Item
    constexpr const char* LABELS[] = { "START GAME", "SETTINGS", "EXIT" };
    constexpr int ITEM_COUNT = sizeof(LABELS) / sizeof(LABELS[0]);

    constexpr float MENU_CELL    = 8.0f;     // text size: tune by eye
    constexpr float MARGIN       = 60.0f;    // distance from the right edge
    constexpr float FIRST_ITEM_Y = 0.30f;    // first item's top, as a fraction of screen height
    constexpr float ITEM_SPACING = 0.22f;    // gap between items, as a fraction of screen height
}

int MainMenu::ItemCount() const { return ITEM_COUNT; }

void MainMenu::Draw(SDL_Renderer* Renderer) const {
    int ScreenW = 0, ScreenH = 0;
    SDL_RenderGetLogicalSize(Renderer, &ScreenW, &ScreenH);

    for (int i = 0; i < ITEM_COUNT; ++i){
        Theme::SetDrawColor(Renderer, (i == Selected) ? Theme::Color::TEXT_HIGHLIGHT : Theme::Color::TEXT_NORMAL);
        const float X = ScreenW - MARGIN - PixelFont::TextWidth(LABELS[i], MENU_CELL);   // right-aligned
        const float Y = ScreenH * (FIRST_ITEM_Y + i * ITEM_SPACING);
        PixelFont::DrawText(Renderer, LABELS[i], X, Y, MENU_CELL);
    }
}
