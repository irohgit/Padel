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
enum class PointOutcome{
    Point,
    Game,
    Set,
    Match
};
static_assert(PointOutcome::Point < PointOutcome::Game &&
              PointOutcome::Game < PointOutcome::Set &&
              PointOutcome::Set  < PointOutcome::Match,
              "PointOutcome must stay ordered from smallest to biggest. Point,Game,Set,Match. ScorePoint relies on >= comparison");
static_assert(static_cast<int>(PointOutcome::Game)  == static_cast<int>(PointOutcome::Point) + 1 &&
              static_cast<int>(PointOutcome::Set)   == static_cast<int>(PointOutcome::Game)  + 1 &&
              static_cast<int>(PointOutcome::Match) == static_cast<int>(PointOutcome::Set)   + 1,
              "PointOutcome values must be consecutive. CloseLevels loops through them");
struct MatchEvent{
    MatchEventType Type;
    CourtSide Side = CourtSide::None;
    float X = 0.0f;
    float Y = 0.0f;
    float Power = 0.0f;
    int Rally = 0;

    PointOutcome Outcome = PointOutcome::Point;
    int LeftPoints = 0;
    int RightPoints = 0;
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


