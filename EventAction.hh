#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4Event.hh"
#include "G4UImanager.hh"

#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"

class EventAction : public G4UserEventAction
{
public:
    EventAction();
    ~EventAction();

    void BeginOfEventAction(const G4Event *event) override;
    void EndOfEventAction(const G4Event *event) override;

    // Monte Carlo Variables
    G4bool bIsEntered = false;
    G4bool bIsExited = false;
    G4bool bIsPassed = false;
    G4bool bIsInelastic = false;
    G4bool bIsElastic = false;
    G4bool bHitDet0 = false;
    G4bool bHitDet1 = false;
    G4bool bHitDet2 = false;
    G4bool bHitDet3 = false;
    G4bool bHitDet4 = false;
    G4bool bHitDet5 = false;
    G4bool bHitOB0 = false;
    G4bool bHitOB1 = false;
    G4bool bIsAbsorbedALP34 = false;

    std::vector<G4ThreeVector> OutTrackSecondaries;
    std::vector<std::string> ParticleTypeSecondaries;

    G4double BeamPosDet0X;
    G4double BeamPosDet0Y;

    // Measurement variables
    G4bool bIsTrigger = false;
    G4bool bIsInTrack = false;
    G4int NrInTrack = 0;

    G4int NrOutTrack = 0;
    G4bool bIsOutSingleTrack = false;
    G4bool bIsOutMultipleTrack = false;
    G4bool bIsNoOutTrack = false;

    std::map<int, std::vector<G4Track> *> detector_hitvector_map{
        {0, &HitTracksDet0},
        {1, &HitTracksDet1},
        {2, &HitTracksDet2},
        {3, &HitTracksDet3},
        {4, &HitTracksDet4},
        {5, &HitTracksDet5},
    };

    std::vector<G4Track> HitTracksDet0;
    std::vector<G4Track> HitTracksDet1;
    std::vector<G4Track> HitTracksDet2;
    std::vector<G4Track> HitTracksDet3;
    std::vector<G4Track> HitTracksDet4;
    std::vector<G4Track> HitTracksDet5;
    std::vector<G4Track> HitTracksOBM0;
    std::vector<G4Track> HitTracksOBM1;

    G4ThreeVector InTrack;
    G4ThreeVector OutTrack;

    std::vector<G4double> PostEnergy;

    // Messenger
    G4GenericMessenger *fMessenger;
    G4bool bIsRestrictedInTrack;

private:
    float hResX = 14.6 * mm;
    float hResY = 6.0 * mm;
    
    bool atLeastThree(bool a, bool b, bool c, bool d, bool e);
    bool atLeastFour(bool a, bool b, bool c, bool d, bool e);

    std::map<std::string, int> particle_map = {
        {"proton", 1},
        {"neutron", 2},
        {"gamma", 3},
        {"pi+", 4},
        {"pi-", 4},
        {"e+", 5},
        {"e-", 5},
        {"deuteron", 6},
        {"alpha", 7},
        {"He3", 8},
        {"triton", 9},
        {"mu+", 10},
        {"mu-", 10}};
};

#endif