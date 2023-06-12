#ifndef SENSITIVEDET_HH
#define SENSITIVEDET_HH

#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"
#include "G4Deuteron.hh"
#include "G4Proton.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>
#include "G4LogicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"

#include "EventAction.hh"

class SensitiveDetector : public G4VSensitiveDetector
{
public:
    SensitiveDetector(G4String);
    ~SensitiveDetector();

private:
    virtual G4bool ProcessHits(G4Step *, G4TouchableHistory *);

    RunManager *runManager = NULL;
    EventAction *eventAction = NULL;

    G4ParticleDefinition *PrimaryDefinition;

    bool ContainsTrackID(std::vector<UserHit> vec, int trackID);
};

#endif