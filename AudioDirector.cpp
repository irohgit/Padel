#include "AudioDirector.h"
#include "AudioSystem.h"
#include <algorithm>

namespace{
    enum class HitWeight : int {Light = 0, Medium = 1, Heavy = 2};

    /* Design consideration of this function
     * Initial Decision: Continuous FMOD parameter (Power 0 to 1)
     *     Each Layer triggered by a range set in Studio (0-0.33,0.33-0.67,0.67-1)
     * Problem:
     *     The ranges share edges Light_Max (0.33) == Medium_Min (0.33)
     *     This could trigger two layers and a gapped range (0.33 to 0.34) would play nothing
     *     Confirming Problem: FMOD's edge rule not explicit and I could not test yet
     * Solution:
     *     A labeled FMOD Parameter (Weight: Light 0, Medium 1, Heavy 2).
     *     The game classifies with half-open ranges, [0,0.33), [0.33,0.67),[0.67,1],
     *     HalfOpen = Include start, exclude End
     *     Thus every value maps to exactly one weight.
     */
    HitWeight ClassifyWeight(float Power){
        if (Power < 0.33f) return HitWeight::Light;
        if (Power < 0.67f) return HitWeight::Medium;
        return HitWeight::Heavy;
    }
    const char* ToString(HitWeight Weight){
        switch (Weight){
            case HitWeight::Light: return "LIGHT";
            case HitWeight::Medium: return "MEDIUM";
            case HitWeight::Heavy: return "HEAVY";
        }
        return "?";
    }
}

AudioDirector::AudioDirector(AudioSystem& InAudio, float InCourtWidth)
              : Audio(InAudio), CourtWidth(InCourtWidth)
{}// End Constructor AudioDirector
float AudioDirector::PanFromX(float X) const { return std::clamp(X / CourtWidth * 2.0f - 1.0f, -1.0f, 1.0f); }
    
void AudioDirector::OnMatchEvent(const MatchEvent& Event)
{
    switch(Event.Type){
        case MatchEventType::PaddleHit  : LastSound = "HIT" ; PlayImpact("event:/SFX/Hit",Event.Power,Event.X);  break;
        case MatchEventType::WallBounce : LastSound = "WALL"; PlayImpact("event:/SFX/Wall",Event.Power,Event.X); break;
        default : break;
    }
}

void AudioDirector::PlayImpact(const char *EventPath, float Power, float X)
{   
    // Set up Labels and Parameter Values for FMOD
    const HitWeight Weight = ClassifyWeight(Power);
    const float Pan = PanFromX(X); 

    SoundEvent Sound = Audio.CreateEvent(EventPath);
    Sound.SetParameter("Weight",static_cast<float>(Weight));
    Sound.SetParameter("Pan",Pan);
    Sound.Start();

    LastWeight = ToString(Weight);
}


