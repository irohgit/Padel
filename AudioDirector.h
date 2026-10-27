#pragma once
#include "MatchEvents.h"
#include "SoundEvent.h"

class AudioSystem;

class AudioDirector : public IMatchListener{
public:
    AudioDirector(AudioSystem& Audio, float CourtWidth);
    //Destructor is inherited from IMatchListener virtual ~IMatchListener() = default;
    void OnMatchEvent(const MatchEvent& Event) override;
    float PanFromX(float X) const;
    const char* GetLastWeight() const {return LastWeight;}
    const char* GetLastSound() const {return LastSound;}
    void Update();    
private:
    void PlayImpact(const char* EventPath, float Power, float X);
    void CallScore(const MatchEvent& Event);
    void PlaySideCall(const char* EventPath, CourtSide Side);
    AudioSystem& Audio;
    SoundEvent MatchMusic;
    SoundEvent WinStinger;
    float CourtWidth;
    const char* LastWeight = "None";
    const char* LastSound = "-";
};
