#pragma once
#include "MatchEvents.h"

class LogListener : public IMatchListener{
public:
    void OnMatchEvent(const MatchEvent& Event) override;
};

