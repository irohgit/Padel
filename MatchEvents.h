#pragma once
#include <vector>

enum class MatchEventType{
    PaddleHit,
    WallBounce,
    PointScored,
    SetWon,
    MatchEnd,
};
enum class CourtSide{
    None,
    Left,
    Right
};

struct MatchEvent{
    MatchEventType Type;
    CourtSide Side = CourtSide::None;
    float X = 0.0f;
    float Y = 0.0f;
    float Power = 0.0f;
    int Rally = 0;
};

class IMatchListener{
public:
    virtual ~IMatchListener() = default;
    virtual void OnMatchEvent(const MatchEvent& Event) = 0;
};

class MatchEvents{
public:
    void Subscribe(IMatchListener* Listener);
    void Publish(const MatchEvent& Event);
private:
    std::vector<IMatchListener*> Listeners;
};


