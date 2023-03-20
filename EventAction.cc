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
    bIsEntered = false;
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

}

void EventAction::EndOfEventAction(const G4Event *event)
{
    // Get AnalysisManager
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    // Inelastic cross section data storage ---------------------------------------------------------------------

    //
    // Store number of particles entering the target
    if (bIsEntered)
    {
        // G4cout << "Particle entered the target" << G4endl;
        man->FillH1(0, 0);
    }

    // Store number of particles passed the target
    if (bIsEntered && !bIsAbsorbed)
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

    // Handle measurement data: All Charged particles are measured


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
        // Calculate scattering angle in phi and theta (spherical coordinates, phi from (-pi, pi) and theta from (-pi/2, pi/2)
        // where phi, theta = (0,0 is the forwards unaffected direction))
        // G4cout << "InTrack: " << InTrack << " OutTrack: " << OutTrack << G4endl;
        // To calculate phi, project incoming track and outgoing track in XZ plane
        InTrack = G4ThreeVector(0, 0, 1);
        G4ThreeVector TrackBeforeX = G4ThreeVector(InTrack.getX(), 0, InTrack.getZ());
        G4ThreeVector TrackAfterX = G4ThreeVector(OutTrack.getX(), 0, OutTrack.getZ());
        G4double phi = TrackBeforeX.angle(TrackAfterX);

        // To calculate theta, the angle between the outoing track in XZ plane and the outoing track needs to be considered
        //  Note: The incoming particle is lie in the XZ plane. To correct, maybe substract the theta of incoming track.
        G4ThreeVector TrackBeforeY = G4ThreeVector(0, InTrack.getY(), InTrack.getZ());
        G4ThreeVector TrackAfterY = G4ThreeVector(0, OutTrack.getY(), OutTrack.getZ());
        G4double theta = TrackAfterX.angle(OutTrack);

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

            // To calculate phi, project incoming track and outgoing track in XZ plane
            G4ThreeVector TrackBeforeX = G4ThreeVector(InTrackSecondary.getX(), 0, InTrackSecondary.getZ());
            G4ThreeVector TrackAfterX = G4ThreeVector((*it).getX(), 0, (*it).getZ());
            G4double phi = TrackBeforeX.angle(TrackAfterX);

            // To calculate theta, the angle between the outoing track in XZ plane and the outoing track needs to be considered
            G4ThreeVector TrackBeforeY = G4ThreeVector(0, InTrackSecondary.getY(), InTrackSecondary.getZ());
            G4ThreeVector TrackAfterY = G4ThreeVector(0, (*it).getY(), (*it).getZ());
            G4double theta = TrackAfterX.angle((*it));

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
            if (particle_map.find(ParticleTypeSecondaries[i]) == particle_map.end())
            {
                // Particle type not declared
                G4cout << ParticleTypeSecondaries[i] << G4endl;
                man->FillNtupleDColumn(11, 0, phi);
                man->FillNtupleDColumn(11, 1, theta);
                man->AddNtupleRow(11);
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
