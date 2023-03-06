#ifndef ACTION_HH
#define ACTION_HH

#include "G4VUserActionInitialization.hh"
#include "generator.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "RunManager.hh"

class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization();
    ~ActionInitialization();

    virtual void Build() const;

    PrimaryGenerator *generator;
    EventAction *eventAction;
};

#endif