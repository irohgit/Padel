#pragma once
#include <SDL/SDL.h>

// Opening Sequence

class TitleSequence{
public:
    TitleSequence() = default;
    ~TitleSequence();
    TitleSequence(const TitleSequence&) = delete;
    TitleSequence& operator=(const TitleSequence&) = delete;

    bool Load(SDL_Renderer* Renderer);
    void Release();
    void Update(float DeltaTime);
    void Draw(SDL_Renderer* Renderer) const;
    void Skip();
    bool IsFinished()const;
    bool ConsumeStingCue();

private:
    bool bStingDue = false;
    SDL_Texture* Court = nullptr;
    int CourtWidth = 0;
    int CourtHeight =0;
    float Time = 0.0f;

};
