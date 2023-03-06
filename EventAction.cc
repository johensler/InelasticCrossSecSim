#include "EventAction.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

EventAction::EventAction()
{
}

EventAction::~EventAction()
{
}

void EventAction::BeginOfEventAction(const G4Event *event)
{
    // Get AnalysisManager
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    // Reset count bools
    bIsPassed = false;
    bIsAbsorbed = false;
    bHitDet0 = false;
    bHitDet1 = false;
    bHitDet2 = false;
    bHitDet3 = false;
    bHitDet4 = false;
    bHitDet5 = false;
    bHitOB0 = false;
    bHitOB1 = false;

    // Reset tracks
    InTrack = G4ThreeVector(0, 0, 0);
    OutTrack = G4ThreeVector(0, 0, 0);

    // Store number of produced particles
    man->FillH1(0, 0);
    // G4cout << "Particle generated" << G4endl;
}

void EventAction::EndOfEventAction(const G4Event *event)
{
    // Get AnalysisManager
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    // Inelastic cross section data storage -------------------------------------
    // Store number of particles passed the target
    if (bIsPassed)
    {
        man->FillH1(0, 1);
        // G4cout << "Particle passed the target" << G4endl;
    }

    // Store number of particles passed the target and measured
    if (bHitDet0 && bHitDet1 && bHitDet2 && bHitDet3 && bHitDet4 && bHitDet5 && bHitOB0 && bHitOB1)
    {
        man->FillH1(0, 2);
        // G4cout << "Particle was measured" << G4endl;
    }
    if (bIsAbsorbed)
    {
        man->FillH1(0, 3);
        // G4cout << "Particle absorbed in target " << G4endl;
    }

    // Elastic Scattering Distribution data storage
    if (OutTrack != G4ThreeVector(0, 0, 0))
    {
        // Calculate scattering angle
        // Project the Tracks on XZ or YZ plane
        G4ThreeVector TrackBeforeX = G4ThreeVector(InTrack.getX(), 0, InTrack.getZ());
        G4ThreeVector TrackBeforeY = G4ThreeVector(0, InTrack.getY(), InTrack.getZ());

        G4ThreeVector TrackAfterX = G4ThreeVector(OutTrack.getX(), 0, OutTrack.getZ());
        G4ThreeVector TrackAfterY = G4ThreeVector(0, OutTrack.getY(), OutTrack.getZ());

        // Calculate scatter angles in X and Y
        G4double phi = TrackBeforeX.angle(TrackAfterX);
        G4double theta = TrackBeforeY.angle(TrackAfterY);

        // Acount for sign of the angle (counter clockwise = positive)
        if (TrackBeforeX.getX() > TrackAfterX.getX())
        {
            phi = -phi;
        }
        if (TrackBeforeY.getY() > TrackAfterX.getY())
        {
            theta = -theta;
        }

        // Store scatter angles [rad]
        man->FillNtupleDColumn(0, 0, phi);
        man->FillNtupleDColumn(0, 1, theta);
        man->AddNtupleRow(0);
    }
}
