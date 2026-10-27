#pragma once
#include <SDL/SDL.h>

// Ported from my Flapjack (Godot) MenuScreen: one base class for every menu screen.
// Screens receive ACTIONS, never raw keys (Godot's ui_accept / ui_cancel), so keyboard
// and pad behave identically, and only the screen on top ever hears input.
enum class MenuAction { Up, Down, Accept, Cancel };

class MenuScreen {
public:
    virtual ~MenuScreen() = default;

    // first_focus: something is always highlighted when a screen opens
    virtual void OnOpen() { Selected = 0; }

    // Navigation is the same for every screen, so it lives here, once.
    // Returns true when the player accepts the highlighted item: the owner decides what that means.
    bool HandleAction(MenuAction Action){
        const int Count = ItemCount();
        switch (Action){
            case MenuAction::Up:     Selected = (Selected - 1 + Count) % Count; return false;
            case MenuAction::Down:   Selected = (Selected + 1) % Count;         return false;
            case MenuAction::Accept: return true;
            case MenuAction::Cancel: return false;   // the owner checks CanCloseOnCancel()
        }
        return false;
    }

    bool CanCloseOnCancel() const { return bCloseOnCancel; }   // close_on_cancel
    virtual void Draw(SDL_Renderer* Renderer) const = 0;

protected:
    virtual int ItemCount() const = 0;    // each screen says how many items it has
    int Selected = 0;
    bool bCloseOnCancel = true;
};
