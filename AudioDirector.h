#pragma once
#include "MatchEvents.h"

class AudioSystem;

class AudioDirector : public IMatchListener{
public:
    AudioDirector(AudioSystem& Audio, float CourtWidth);
    //Destructor is inherited from IMatchListener virtual ~IMatchListener() = default;
    void OnMatchEvent(const MatchEvent& Event) override;
    float PanFromX(float X) const;
    const char* GetLastWeight() const {return LastWeight;}
    const char* GetLastSound() const {return LastSound;}
    
private:
    void PlayImpact(const char* EventPath, float Power, float X);
    AudioSystem& Audio;
    float CourtWidth;
    const char* LastWeight = "None";
    const char* LastSound = "-";
};
