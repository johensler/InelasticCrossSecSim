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

    bIsInTrack = false;
    NrInTrack = 0;

    NrOutTrack = 0;
    bIsOutSingleTrack = false;
    bIsOutMultipleTrack = false;
    bIsNoOutTrack = false;

    HitTracksDet0.clear();
    HitTracksDet1.clear();
    HitTracksDet2.clear();
    HitTracksDet3.clear();
    HitTracksDet4.clear();
    HitTracksDet5.clear();
    HitTracksOBM0.clear();
    HitTracksOBM1.clear();

    // Reset tracks
    InTrack = G4ThreeVector(0, 0, 0);
    OutTrack = G4ThreeVector(0, 0, 0);

    OutTrackSecondaries.clear();
    ParticleTypeSecondaries.clear();
}

void EventAction::EndOfEventAction(const G4Event *event)
{
    G4AnalysisManager *man = G4AnalysisManager::Instance(); // Get AnalysisManager

    // Inelastic cross section data storage ---------------------------------------------------------------------
    // Store number of particles entering the target
    if (bIsEntered)
    {
        man->FillH1(0, 0);
    }

    // Store number of particles passed the target
    if (bIsEntered && !bIsAbsorbed)
    {
        man->FillH1(0, 1);
    }

    // Store number of particles passed the target and measured
    if (bHitDet0 && bHitDet1 && bHitDet2 && bHitDet3 && bHitDet4 && bHitDet5 && bHitOB0 && bHitOB1)
    {
        man->FillH1(0, 4);
    }

    if (atLeastFour(bHitDet3, bHitDet4, bHitDet5, bHitOB0, bHitOB1))
    {
        man->FillH1(0, 5);
    }

    if (atLeastThree(bHitDet3, bHitDet4, bHitDet5, bHitOB0, bHitOB1))
    {
        man->FillH1(0, 6);
    }

    if (bIsAbsorbed)
    {
        man->FillH1(0, 3);
    }

    // Handle measurement signatures: All Charged particles are measured -------------------------------------------------------------------------------------------------
    // Distinguish three measurement signatures: i) track in track out (TITO), ii)track in no (track) out (TINO), iii) track in, multiple out (TIMO)
    // Track in: 3 planes infornt hit, Track out: the first plane after target is hit and min 3 planes total after target

    // Extract all "in" tracks (in general track through all first three planes, so also charged secondaries)
    std::unordered_map<int, int> freq_in; // Store the frequency of each TrackID in the first three planes
    for (int CopyNo = 0; CopyNo < 3; CopyNo++)
    {
        for (int i = 0; i < detector_hitvector_map[CopyNo]->size(); i++)
        {
            // Add one for that TrackID
            freq_in[(*(detector_hitvector_map[CopyNo]))[i]->GetTrackID()]++;
        }
    }
    for (auto it = freq_in.begin(); it != freq_in.end(); it++)
    {
        if (it->second >= 3)
        {
            bIsInTrack = true;
            NrInTrack++;
        }
        // G4cout << it->first << " appears " << it->second << " times." << G4endl;
    }
    if (bIsInTrack)
    {
        man->FillH1(1, 0);
    }

    // Extract all out tracks (differentiate between one out track and multiple out tracks)
    std::unordered_map<int, int> freq_out;          // Store the frequency of each TrackID in the 5 planes after the target
    std::unordered_map<int, bool> bHit3rdPlane_map; //  Store if the first plane after target was hit for each TrackID

    // Count frequecy of TrackIDs in single ALPIDEs after the target
    for (int CopyNo = 3; CopyNo < 6; CopyNo++)
    {
        for (int i = 0; i < detector_hitvector_map[CopyNo]->size(); i++)
        {
            freq_out[(*(detector_hitvector_map[CopyNo]))[i]->GetTrackID()]++; // Add one for that TrackID

            if (CopyNo == 3)
            {
                bHit3rdPlane_map[(*(detector_hitvector_map[CopyNo]))[i]->GetTrackID()] = true; // Mark this TrackID as hit in the 3rd plane
            }
        }
    }
    // Count frequency of TrackIDs in OBM
    for (int i = 0; i < HitTracksOBM0.size(); i++)
    {
        freq_out[(HitTracksOBM0[i])->GetTrackID()]++;
    }
    for (int i = 0; i < HitTracksOBM1.size(); i++)
    {
        freq_out[(HitTracksOBM1[i])->GetTrackID()]++;
    }

    for (auto it = freq_out.begin(); it != freq_out.end(); it++)
    {
        // OutTrack requirement: 3rd plane hit and total min 3 planes hit
        if (bHit3rdPlane_map[it->first] && it->second >= 3)
        {
            NrOutTrack++;
        }
        // G4cout << it->first << " appears " << it->second << " times." << G4endl;
    }

    if (NrOutTrack == 1)
    {
        bIsOutSingleTrack = true;
        man->FillH1(1, 2);
    }
    else if (NrOutTrack > 1)
    {
        bIsOutMultipleTrack = true;
        man->FillH1(1, 3);
    }
    else if (NrOutTrack == 0)
    {
        bIsNoOutTrack = true;
        man->FillH1(1, 1);
    }
    // Categorise proccesses ------------------------------------------------------------------------------
    // (I)TITO
    if (bIsOutSingleTrack)
    {
        // Two possible cases considerd:
        //(I.i) ElasP (Elastic interacted primary particle)
        if (!bIsAbsorbed)
        {
            man->FillH1(1, 4);
        }
        // (I.ii) InelasO (Inelastic interaction with one charde particle in acceptance)
        else
        {
            man->FillH1(1, 5);
        }
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
