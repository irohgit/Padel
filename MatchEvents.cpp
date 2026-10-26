#pragma once
#include "MatchEvents.h"

void MatchEvents::Publish(const MatchEvent &Event){
    for (IMatchListener* Listener : Listeners)
    {Listener->OnMatchEvent(Event);}
}

void MatchEvents::Subscribe(IMatchListener* Listener){
    if(Listener){Listeners.push_back(Listener);}
}

