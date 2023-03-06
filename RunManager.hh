#ifndef RUNMANAGER_HH
#define RUNMANAGER_HH

#include "G4RunManager.hh"
#include "EventAction.hh"
#include "RunAction.hh"

class RunManager : public G4RunManager
{
public:
    RunManager();
    ~RunManager();

    EventAction *eventAction = NULL;

private:
};
#endif