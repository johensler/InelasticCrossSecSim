#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4Event.hh"

class EventAction : public G4UserEventAction
{
public:
    EventAction();
    ~EventAction();

    void BeginOfEventAction(const G4Event *event) override;
    void EndOfEventAction(const G4Event *event) override;

    G4bool bIsPassed = false;
    G4bool bIsAbsorbed = false;
    G4bool bHitDet0 = false;
    G4bool bHitDet1 = false;
    G4bool bHitDet2 = false;
    G4bool bHitDet3 = false;
    G4bool bHitDet4 = false;
    G4bool bHitDet5 = false;
    G4bool bHitOB0 = false;
    G4bool bHitOB1 = false;

    G4ThreeVector InTrack;
    G4ThreeVector OutTrack;

private:
};

#endif