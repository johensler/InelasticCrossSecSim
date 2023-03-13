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
    bIsPassed = true;
    bIsAbsorbed = false;
    bHitDet0 = false;
    bHitDet1 = false;
    bHitDet2 = false;
    bHitDet3 = false;
    bHitDet4 = false;
    bHitDet5 = false;
    bHitOB0 = false;
    bHitOB1 = false;

    bChargedHitDet0 = false;
    bChargedHitDet1 = false;
    bChargedHitDet2 = false;
    bChargedHitDet3 = false;
    bChargedHitDet4 = false;
    bChargedHitDet5 = false;
    bChargedHitOB0 = false;
    bChargedHitOB1 = false;

    // Reset tracks
    InTrack = G4ThreeVector(0, 0, 0);
    OutTrack = G4ThreeVector(0, 0, 0);

    OutTrackSecondaries.clear();
    ParticleTypeSecondaries.clear();

    // Store number of produced particles
    // For each event one particle is produced -> just enter one entry at the beginning of each event
    man->FillH1(0, 0);
}

void EventAction::EndOfEventAction(const G4Event *event)
{
    // Get AnalysisManager
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    // Inelastic cross section data storage ---------------------------------------------------------------------
    // Store number of particles passed the target
    if (bIsPassed)
    {
        man->FillH1(0, 1);
        // G4cout << "Particle passed the target" << G4endl;
    }

    // Store number of particles passed the target and measured
    if (bHitDet0 && bHitDet1 && bHitDet2 && bHitDet3 && bHitDet4 && bHitDet5 && bHitOB0 && bHitOB1)
    {
        man->FillH1(0, 4);
        // G4cout << "Particle was measured in 5 planes after target" << G4endl;
    }

    if (atLeastFour(bHitDet3, bHitDet4, bHitDet5, bHitOB0, bHitOB1))
    {
        man->FillH1(0, 5);
        // G4cout << "Particle was measured in at least 4 planes after target" << G4endl;
    }

    if (atLeastThree(bHitDet3, bHitDet4, bHitDet5, bHitOB0, bHitOB1))
    {
        man->FillH1(0, 6);
        // G4cout << "Particle was measured in at least 3 planes after target" << G4endl;
    }

    if (bIsAbsorbed)
    {
        man->FillH1(0, 3);
        // G4cout << "Particle absorbed in target " << G4endl;
    }

    // All charged particles
    if (bChargedHitDet0 && bChargedHitDet1 && bChargedHitDet2 && bChargedHitDet3 && bChargedHitDet4 && bChargedHitDet5 && bChargedHitOB0 && bChargedHitOB1)
    {
        man->FillH1(0, 7);
        // G4cout << "Charged particle was measured in 5 planes after target" << G4endl;
    }

    if (atLeastFour(bChargedHitDet3, bChargedHitDet4, bChargedHitDet5, bChargedHitOB0, bChargedHitOB1))
    {
        man->FillH1(0, 8);
        // G4cout << "Charged particle was measured in at least 4 planes after target" << G4endl;
    }

    if (atLeastThree(bChargedHitDet3, bChargedHitDet4, bChargedHitDet5, bChargedHitOB0, bChargedHitOB1))
    {
        man->FillH1(0, 9);
        // G4cout << "Charged particle measured in at least 3 planes after target" << G4endl;
    }

    // Elastic Scattering Distribution data storage --------------------------------------------------------------------------------
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
        if (TrackBeforeY.getY() > TrackAfterY.getY())
        {
            theta = -theta;
        }

        // Store scatter angles [rad]
        man->FillNtupleDColumn(0, 0, phi);
        man->FillNtupleDColumn(0, 1, theta);
        man->AddNtupleRow(0);
    }

    // Secondary production data storage --------------------------------------------------------------------
    if (!OutTrackSecondaries.empty())
    {
        int i = 0;
        // Iterate though all produced secondary particles
        for (auto it = OutTrackSecondaries.begin(); it != OutTrackSecondaries.end(); it++)
        {
            // Calculate the angles for the secondaries towards the Z axis
            // InTrack is the Z axis
            G4ThreeVector InTrackSecondary = G4ThreeVector(0, 0, 1);

            G4ThreeVector TrackBeforeX = G4ThreeVector(InTrackSecondary.getX(), 0, InTrackSecondary.getZ());
            G4ThreeVector TrackBeforeY = G4ThreeVector(0, InTrackSecondary.getY(), InTrackSecondary.getZ());

            G4ThreeVector TrackAfterX = G4ThreeVector((*it).getX(), 0, (*it).getZ());
            G4ThreeVector TrackAfterY = G4ThreeVector(0, (*it).getY(), (*it).getZ());

            // Calculate scatter angles in X and Y
            G4double phi = TrackBeforeX.angle(TrackAfterX);
            G4double theta = TrackBeforeY.angle(TrackAfterY);

            // Acount for sign of the angle (counter clockwise = positive)
            if (TrackBeforeX.getX() > TrackAfterX.getX())
            {
                phi = -phi;
            }
            if (TrackBeforeY.getY() > TrackAfterY.getY())
            {
                theta = -theta;
            }

            // Store production angles according to produced type
            if(particle_map.find(ParticleTypeSecondaries[i]) == particle_map.end())
            {   
                //Particle type not declared
                G4cout << ParticleTypeSecondaries[i] << G4endl;
                man->FillNtupleDColumn(9, 0, phi);
                man->FillNtupleDColumn(9, 1, theta);
                man->AddNtupleRow(9);
            }
            else
            {   
                int NtupleIndex = particle_map[ParticleTypeSecondaries[i]];
                man->FillNtupleDColumn(NtupleIndex, 0, phi);
                man->FillNtupleDColumn(NtupleIndex, 1, theta);
                man->AddNtupleRow(NtupleIndex);
            }

            i++;
        }
    }
}

bool EventAction::atLeastThree(bool a, bool b, bool c, bool d, bool e)
{
    int count = 0;
    if (a)
        count++;
    if (b)
        count++;
    if (c)
        count++;
    if (d)
        count++;
    if (e)
        count++;
    return count >= 3;
}

bool EventAction::atLeastFour(bool a, bool b, bool c, bool d, bool e)
{
    int count = 0;
    if (a)
        count++;
    if (b)
        count++;
    if (c)
        count++;
    if (d)
        count++;
    if (e)
        count++;
    return count >= 4;
}
