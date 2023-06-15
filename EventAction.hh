#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4Event.hh"
#include "G4UImanager.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"

#include "UserHit.hh"
#include "construction.hh"
#include "RunManager.hh"

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

    std::map<int, std::vector<UserHit> *> detector_hitvector_map{
        {0, &HitTracksDet0},
        {1, &HitTracksDet1},
        {2, &HitTracksDet2},
        {3, &HitTracksDet3},
        {4, &HitTracksDet4},
        {5, &HitTracksDet5},
    };

    std::vector<UserHit> HitTracksDet0;
    std::vector<UserHit> HitTracksDet1;
    std::vector<UserHit> HitTracksDet2;
    std::vector<UserHit> HitTracksDet3;
    std::vector<UserHit> HitTracksDet4;
    std::vector<UserHit> HitTracksDet5;
    std::vector<UserHit> HitTracksOBM0;
    std::vector<UserHit> HitTracksOBM1;

    G4ThreeVector InTrack;
    G4ThreeVector OutTrack;
    G4double EnteringEnergy = 0 * MeV;

    std::vector<G4double> PostEnergy;

    // Messenger
    G4GenericMessenger *fMessenger;
    G4bool bIsRestrictedInTrack;

private:
    double hResX = 9.8 * mm;
    double hResY = 2.0 * mm;

    G4double CalculateMSCAngle(G4double E, int z, G4double X_0, G4double x, G4double m);

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