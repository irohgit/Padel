#include "TitleSequence.h"
#include "Theme.h"
#include "PixelFont.h"
#include <algorithm>

namespace {
    constexpr const char* COURT_PATH = "Assets/Textures/T_Court_Title.bmp";

    // TIMELINE (seconds)
    constexpr float FADE_START = 1.5f;     // black starts fading
    constexpr float FADE_END   = 4.0f;     // black fully gone
    constexpr float SLIDE_START = FADE_END;
    constexpr float SLIDE_DURATION = 1.5f;
    constexpr float SLIDE_END = SLIDE_START + SLIDE_DURATION;
    constexpr float MENU_APPEAR = SLIDE_END - 1.0f + SLIDE_DURATION * 0.6f;
    constexpr float END_TIME = MENU_APPEAR - 1.0f;
    constexpr float COURT_MENU_CENTRE_X = 500.0f;
    // LOGO: .PADEL waves in letter by letter, then fades out
    constexpr const char* LOGO_TEXT      = ".PADEL";
    constexpr float LOGO_CELL            = 18.0f;   // size: tune by eye
    constexpr float WAVE_START           = 0.15f;   // first letter starts
    constexpr float LETTER_DURATION      = 0.45f;   // each letter's rise
    constexpr float LETTER_STAGGER       = 0.08f;   // delay between letters: the "wave"
    constexpr float LETTER_RISE          = 60.0f;   // pixels each letter rises
    constexpr float LOGO_FADE_START      = FADE_END - FADE_END/2;    // logo fades out
    constexpr float LOGO_FADE_END        = FADE_END;
    constexpr float SMALL_LOGO_CELL = LOGO_CELL /2.5f;
    constexpr float STING_TIME = WAVE_START - 0.05f; 
    
    // Gradient for main menu
    constexpr float GRADIENT_HEIGHT = 0.4f; // fraction of screen height
    constexpr Uint8 GRADIENT_MAX_ALPHA = 255;
    // 0 before Start, 1 after End, a straight line in between
    float Progress(float Time, float Start, float End){
        return std::clamp((Time - Start) / (End - Start), 0.0f, 1.0f);
    }

    float Lerp(float A, float B, float Amount){ return A + (B - A) * Amount; }

    // Fast start, gentle landing
    float EaseOutCubic(float X){ const float Inverse = 1.0f - X; return 1.0f - Inverse * Inverse * Inverse; }

    // .PADEL, centred, each letter on its own slightly delayed clock
    void DrawLogo(SDL_Renderer* Renderer, float Time, int ScreenW, int ScreenH){
        const float FadeOut = 1.0f - Progress(Time, LOGO_FADE_START, LOGO_FADE_END);   // 1 → 0
        if (FadeOut <= 0.0f) { return; }

        const float Width = PixelFont::TextWidth(LOGO_TEXT, LOGO_CELL);
        const float BaseY = (ScreenH - PixelFont::TextHeight(LOGO_CELL)) / 2.0f;
        float X = (ScreenW - Width) / 2.0f;

        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
        for (int i = 0; LOGO_TEXT[i] != '\0'; ++i){
            const char Letter[2] = { LOGO_TEXT[i], '\0' };              // one letter as a tiny string

            const float Start = WAVE_START + i * LETTER_STAGGER;
            const float In    = EaseOutCubic(Progress(Time, Start, Start + LETTER_DURATION));   // 0 → 1
            const float Y     = BaseY + (1.0f - In) * LETTER_RISE;      // starts low, rises into place
            const Uint8 Alpha = static_cast<Uint8>(255.0f * In * FadeOut);

            SDL_SetRenderDrawColor(Renderer, 255, 255, 255, Alpha);
            PixelFont::DrawText(Renderer, Letter, X, Y, LOGO_CELL);
            X += PixelFont::TextWidth(Letter, LOGO_CELL) + LOGO_CELL;  // letter width + the 1-cell gap
        }
        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);
    }
    // A full-width band: transparent at its top, black at its bottom. Rises from below the screen.
    void DrawGradient(SDL_Renderer* Renderer, float Time, int ScreenW, int ScreenH){
        const float In = EaseOutCubic(Progress(Time, SLIDE_START, SLIDE_END));   // 0 → 1, with the court slide
        if (In <= 0.0f) { return; }

        const float Height = ScreenH * GRADIENT_HEIGHT;
        const float Top    = Lerp(static_cast<float>(ScreenH), ScreenH - Height, In);  // starts just below the screen
        const float Bottom = Top + Height;
        const float Right  = static_cast<float>(ScreenW);

        const SDL_Color Clear = {0, 0, 0, 0};
        const SDL_Color Black = {0, 0, 0, GRADIENT_MAX_ALPHA};

        // Four corners: position, colour, texture coordinate (unused: no texture)
        const SDL_Vertex Corners[4] = {
            { {0.0f,  Top},    Clear, {0.0f, 0.0f} },   // 0 top-left
            { {Right, Top},    Clear, {0.0f, 0.0f} },   // 1 top-right
            { {0.0f,  Bottom}, Black, {0.0f, 0.0f} },   // 2 bottom-left
            { {Right, Bottom}, Black, {0.0f, 0.0f} },   // 3 bottom-right
        };
        const int Triangles[6] = { 0, 1, 2,   2, 1, 3 };   // two triangles make the rectangle

        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderGeometry(Renderer, nullptr, Corners, 4, Triangles, 6);
        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);
    }
    // .PADEL in the bottom-left corner, one margin from the left and bottom. Rises from below the screen.
    void DrawSmallLogo(SDL_Renderer* Renderer, float Time, int ScreenH){
        const float In = EaseOutCubic(Progress(Time, SLIDE_START, SLIDE_END));   // 0 → 1, with the court slide
        if (In <= 0.0f) { return; }

        const float Height = PixelFont::TextHeight(SMALL_LOGO_CELL);
        const float RestY  = ScreenH - Theme::SCREEN_MARGIN - Height;             // final spot: margin above the bottom
        const float Y      = Lerp(static_cast<float>(ScreenH), RestY, In);        // starts just below the screen

        Theme::SetDrawColor(Renderer, Theme::Color::TEXT_NORMAL);
        PixelFont::DrawText(Renderer, LOGO_TEXT, Theme::SCREEN_MARGIN, Y, SMALL_LOGO_CELL);
    }
}

TitleSequence::~TitleSequence(){ Release(); }

bool TitleSequence::Load(SDL_Renderer* Renderer){
    SDL_Surface* Surface = SDL_LoadBMP(COURT_PATH);         // pixels in CPU memory
    if (!Surface){ SDL_Log("SDL_LoadBMP failed (%s): %s", COURT_PATH, SDL_GetError()); return false; }

    Court = SDL_CreateTextureFromSurface(Renderer, Surface); // copy to the GPU
    CourtWidth  = Surface->w;
    CourtHeight = Surface->h;
    SDL_FreeSurface(Surface);                                // the CPU copy is no longer needed

    if (!Court){ SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError()); return false; }
    return true;
}

void TitleSequence::Release(){
    if (Court){ SDL_DestroyTexture(Court); Court = nullptr; }
}

void TitleSequence::Update(float DeltaTime){
    const float Previous = Time;
    Time += DeltaTime;

    if(Previous < STING_TIME && Time >= STING_TIME)
    { bStingDue = true;}
}
bool TitleSequence::ConsumeStingCue(){
    const bool bDue = bStingDue;
    bStingDue = false;
    return bDue;
}
void TitleSequence::Skip(){ Time = END_TIME; }
bool TitleSequence::IsFinished() const { return Time >= END_TIME; }

void TitleSequence::Draw(SDL_Renderer* Renderer) const {
    int ScreenW = 0, ScreenH = 0;
    SDL_RenderGetLogicalSize(Renderer, &ScreenW, &ScreenH);

    // 1. Background: the maroon the court sits on (same colour as the image edges, so no seam)
    Theme::SetDrawColor(Renderer, Theme::Color::MENU_BACKGROUND);
    SDL_RenderClear(Renderer);

    // 2. Court: full height, true shape; centred, then slides left to its menu position
    const float Scale = static_cast<float>(ScreenH) / CourtHeight;
    const float W = CourtWidth * Scale;
    const float Slide   = EaseOutCubic(Progress(Time, SLIDE_START, SLIDE_END));
    const float CentreX = Lerp(ScreenW / 2.0f, COURT_MENU_CENTRE_X, Slide);
    const SDL_FRect CourtRect = { CentreX - W / 2.0f, 0.0f, W, static_cast<float>(ScreenH) };
    SDL_RenderCopyF(Renderer, Court, nullptr, &CourtRect);
    DrawGradient(Renderer, Time, ScreenW, ScreenH);
    // 3. Black overlay: opaque, then fades out between FADE_START and FADE_END
    const Uint8 Alpha = static_cast<Uint8>(255.0f * (1.0f - Progress(Time, FADE_START, FADE_END)));
    if (Alpha > 0){
        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, Alpha);
        const SDL_FRect Full = {0.0f, 0.0f, static_cast<float>(ScreenW), static_cast<float>(ScreenH)};
        SDL_RenderFillRectF(Renderer, &Full);
        SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_NONE);
    }
    DrawLogo(Renderer, Time, ScreenW, ScreenH);
    DrawSmallLogo(Renderer,Time,ScreenH);
}
