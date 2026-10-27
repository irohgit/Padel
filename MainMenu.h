#pragma once
#include "MenuScreen.h"

// The title menu. Knows its items and how to draw them; Game decides what each item does.
class MainMenu : public MenuScreen {
public:
    enum class Item { StartGame, Settings, Exit };   // order = LABELS order in the .cpp

    MainMenu() { bCloseOnCancel = false; }           // nothing beneath the main menu to return to
    Item GetSelected() const { return static_cast<Item>(Selected); }
    void Draw(SDL_Renderer* Renderer) const override;

protected:
    int ItemCount() const override;
};
