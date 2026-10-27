#include "AudioDirector.h"
#include "AudioSystem.h"
#include "AudioPaths.h"
#include <algorithm>

namespace{
    enum class HitWeight    : int {Light = 0, Medium = 1, Heavy = 2};
    enum class UmpireSide   : int {Left = 0, Right = 1};
    enum class UmpireNumber : int {Love = 0, Fifteen = 1, Thirty = 2, Forty = 3, All = 4 };

    static_assert(static_cast<int>(UmpireNumber::Love) == 0); 
    static_assert(static_cast<int>(UmpireNumber::Fifteen) == 1); 
    static_assert(static_cast<int>(UmpireNumber::Thirty) == 2); 
    static_assert(static_cast<int>(UmpireNumber::Forty) == 3); 
    
    UmpireNumber ToNumber(int Points){
        return static_cast<UmpireNumber>(std::clamp(Points,0,3));
    }

    UmpireSide ToSide(CourtSide Side){
        return (Side == CourtSide::Right) ? UmpireSide::Right : UmpireSide::Left;
    }
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
        case MatchEventType::PaddleHit : LastSound = "HIT" ; PlayImpact(AudioPaths::HIT_EVENT, Event.Power, Event.X); break;
        case MatchEventType::WallBounce: LastSound = "WALL"; PlayImpact(AudioPaths::WALL_EVENT, Event.Power, Event.X); break;
        case MatchEventType::PointScored : CallScore(Event) ; break;
        case MatchEventType::MatchEnd :
             MatchMusic.Stop(true);
             WinStinger = Audio.PlayEvent(AudioPaths::WIN_STING_EVENT);
        default : break;                                   
    }
}

void AudioDirector::PlayImpact(const char *EventPath, float Power, float X)
{
    const HitWeight Weight = ClassifyWeight(Power);
    const float Pan = PanFromX(X);

    SoundEvent Sound = Audio.CreateEvent(EventPath);
    Sound.SetParameter(AudioPaths::WEIGHT_PARAM, static_cast<float>(Weight));
    Sound.SetParameter(AudioPaths::PAN_PARAM, Pan);
    Sound.Start();

    LastWeight = ToString(Weight);
}

void AudioDirector::CallScore(const MatchEvent &Event){
    switch (Event.Outcome){
        case PointOutcome::Match:
            PlaySideCall(AudioPaths::UMPIRE_GAME_SET_MATCH_EVENT, Event.Side);
            return;
        case PointOutcome::Set:
        case PointOutcome::Game:
             PlaySideCall(AudioPaths::UMPIRE_GAME_EVENT, Event.Side);
             return;
        case PointOutcome::Point:
             break;
    }

    const int Left = Event.LeftPoints;
    const int Right = Event.RightPoints;

    if (Left>= 3 && Right >= 3){
        if(Left == Right){
            Audio.PlayEvent(AudioPaths::UMPIRE_DEUCE_EVENT);
        }
        else{
            PlaySideCall(AudioPaths::UMPIRE_ADVANTAGE_EVENT,Left>Right?CourtSide::Left : CourtSide::Right);
        }
        return; 
    }

    const UmpireNumber First = ToNumber(Left);
    const UmpireNumber Second = (Left == Right) ? UmpireNumber::All : ToNumber(Right);

    SoundEvent Call = Audio.CreateEvent(AudioPaths::UMPIRE_SCORE_EVENT);
    Call.SetParameter(AudioPaths::FIRST_PARAM, static_cast<float>(First));
    Call.SetParameter(AudioPaths::SECOND_PARAM, static_cast<float>(Second));
    Call.Start();
}

void AudioDirector::PlaySideCall(const char *EventPath, CourtSide Side){
    SoundEvent Call = Audio.CreateEvent(EventPath);
    Call.SetParameter(AudioPaths::SIDE_PARAM, static_cast<float>(ToSide(Side)));
    Call.Start();
}
void AudioDirector::Update(){
    if(!MatchMusic.IsValid() && !WinStinger.IsValid()){
        MatchMusic = Audio.PlayEvent(AudioPaths::MATCH_MUSIC_EVENT);
    }
}
