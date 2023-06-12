#ifndef RUNMANAGER_HH
#define RUNMANAGER_HH

#include "G4RunManager.hh"
#include "RunAction.hh"

class EventAction;

class RunManager : public G4RunManager
{
public:
    RunManager();
    ~RunManager();

    EventAction *eventAction = NULL;

private:
};
#endif