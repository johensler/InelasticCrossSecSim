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

    G4bool bChargedHitDet0 = false;
    G4bool bChargedHitDet1 = false;
    G4bool bChargedHitDet2 = false;
    G4bool bChargedHitDet3 = false;
    G4bool bChargedHitDet4 = false;
    G4bool bChargedHitDet5 = false;
    G4bool bChargedHitOB0 = false;
    G4bool bChargedHitOB1 = false;

    G4ThreeVector InTrack;
    G4ThreeVector OutTrack;

    std::vector<G4ThreeVector> OutTrackSecondaries;
    std::vector<std::string> ParticleTypeSecondaries;

private:
    bool atLeastThree(bool a, bool b, bool c, bool d, bool e);
    bool atLeastFour(bool a, bool b, bool c, bool d, bool e);
};

#endif