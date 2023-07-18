#include "EventAction.hh"

EventAction::EventAction()
{
    fMessenger = new G4GenericMessenger(this, "/eventaction/", "Event Action");
    fMessenger->DeclareProperty("RestrictInTracks", bIsRestrictedInTrack, "Should the incoming tracks be restricted by area?");
    fMessenger->DeclarePropertyWithUnit("SetResX", "mm", hResX, "Half-height of restriction in x");
    fMessenger->DeclarePropertyWithUnit("SetResY", "mm", hResY, "Half-height of restriction in y");

    // Initial Values
    bIsRestrictedInTrack = false;
    hResX = 9.8 * mm;   //for 1mm
    hResY = 1.7 * mm;   //for 1mm
}

EventAction::~EventAction()
{
}

void EventAction::BeginOfEventAction(const G4Event *event)
{
    // Get AnalysisManager
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    // Reset count bools and other data storage objects
    bIsEntered = false;
    bIsExited = false;
    bIsPassed = false;
    bIsInelastic = false;
    bIsElastic = false;
    bHitDet0 = false;
    bHitDet1 = false;
    bHitDet2 = false;
    bHitDet3 = false;
    bHitDet4 = false;
    bHitDet5 = false;
    bHitOB0 = false;
    bHitOB1 = false;

    G4double BeamPosDet0X = 0;
    G4double BeamPosDet0Y = 0;

    bIsTrigger = false;
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
    EnteringEnergy = 0 * MeV;

    OutTrackSecondaries.clear();
    ParticleTypeSecondaries.clear();

    PostEnergy.clear();
}

void EventAction::EndOfEventAction(const G4Event *event)
{
    G4AnalysisManager *man = G4AnalysisManager::Instance(); // Get AnalysisManager

    // Elastic Scattering Distribution data storage --------------------------------------------------------------------------------
    G4double phi = 0;
    G4double theta = 0;
    if (OutTrack != G4ThreeVector(0, 0, 0))
    {
        // Calculate scattering angle in phi and theta (spherical coordinates, phi from (-pi, pi) and theta from (-pi/2, pi/2)
        // where phi, theta = (0,0 is the forwards unaffected direction))
        // G4cout << "InTrack: " << InTrack << " OutTrack: " << OutTrack << G4endl;
        // To calculate phi, project incoming track and outgoing track in XZ plane
        G4ThreeVector TrackBeforeX = G4ThreeVector(InTrack.getX(), 0, InTrack.getZ());
        G4ThreeVector TrackAfterX = G4ThreeVector(OutTrack.getX(), 0, OutTrack.getZ());
        phi = TrackBeforeX.angle(TrackAfterX);

        // To calculate theta, the angle between the outoing track in XZ plane and the outoing track needs to be considered
        //  Note: The incoming particle is lie in the XZ plane. To correct, maybe substract the theta of incoming track.
        G4ThreeVector TrackBeforeY = G4ThreeVector(0, InTrack.getY(), InTrack.getZ());
        G4ThreeVector TrackAfterY = G4ThreeVector(0, OutTrack.getY(), OutTrack.getZ());
        theta = TrackBeforeY.angle(TrackAfterY);

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

    // Inelastic cross section data storage ---------------------------------------------------------------------
    // Store number of particles entering the target
    if (bIsEntered)
    {
        man->FillH1(0, 0);
    }

    // Store number of particles passed the target
    if (bIsEntered && !bIsInelastic)
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

    if (bIsInelastic)
    {
        man->FillH1(0, 3);
    }

    // Handle measurement signatures: All Charged particles are measured -------------------------------------------------------------------------------------------------
    // Distinguish three measurement signatures: i) track in track out (TITO), ii)track in no (track) out (TINO), iii) track in, multiple out (TIMO)
    // Track in: 3 planes infront hit + scintillator, Track out: the first plane after target is hit and min 3 planes total after target

    // Determine the number of triggers
    if (bIsTrigger)
    {
        man->FillH1(1, 0);
    }

    // Extract all "in" tracks (in general track through all first three planes, so also charged secondaries)
    std::unordered_map<int, int> freq_in; // Store the frequency of each TrackID in the first three planes
    for (int CopyNo = 0; CopyNo < 3; CopyNo++)
    {
        for (int i = 0; i < detector_hitvector_map[CopyNo]->size(); i++)
        {
            if (bIsRestrictedInTrack)
            {
                // Only register these hits if in specified area on ALPIDEs
                G4ThreeVector HitPosition = (*(detector_hitvector_map[CopyNo]))[i].GetHitPosition();
                if (abs(HitPosition.getX()) < hResX && abs(HitPosition.getY()) < hResY)
                {
                    freq_in[(*(detector_hitvector_map[CopyNo]))[i].GetTrackID()]++;
                }
            }
            else
            {
                freq_in[(*(detector_hitvector_map[CopyNo]))[i].GetTrackID()]++;
            }
        }
    }
    for (auto it = freq_in.begin(); it != freq_in.end(); it++)
    {
        // Definition of incoming tracks: Hit all three planes + scintillator
        if (it->second >= 3 && bIsTrigger)
        {
            bIsInTrack = true;
            NrInTrack++;
        }
    }

    if (bIsInTrack)
    {
        man->FillH1(1, 1);
    }

    // Extract all out tracks (differentiate between one out track and multiple out tracks)
    std::unordered_map<int, int> freq_out;          // Store the frequency of each TrackID in the 5 planes after the target
    std::unordered_map<int, bool> bHit3rdPlane_map; //  Store if the first plane after target was hit for each TrackID

    // Count frequecy of TrackIDs in single ALPIDEs after the target
    for (int CopyNo = 3; CopyNo < 6; CopyNo++)
    {
        for (int i = 0; i < (detector_hitvector_map[CopyNo])->size(); i++)
        {
            G4int TrackID = (*(detector_hitvector_map[CopyNo]))[i].GetTrackID();
            freq_out[TrackID]++; // Add one for that TrackID

            if (CopyNo == 3)
            {
                bHit3rdPlane_map[(*(detector_hitvector_map[CopyNo]))[i].GetTrackID()] = true; // Mark this TrackID as hit in the 3rd plane
            }
        }
    }
    // Count frequency of TrackIDs in OBM
    // G4cout << "OBM0" << G4endl;
    for (int i = 0; i < HitTracksOBM0.size(); i++)
    {
        freq_out[(HitTracksOBM0[i]).GetTrackID()]++;
        // G4cout << (HitTracksOBM0[i]).GetTrackID() << G4endl;
    }
    // G4cout << "OBM1" << G4endl;
    for (int i = 0; i < HitTracksOBM1.size(); i++)
    {
        freq_out[(HitTracksOBM1[i]).GetTrackID()]++;
        // G4cout << (HitTracksOBM1[i]).GetTrackID() << G4endl;
    }

    for (auto it = freq_out.begin(); it != freq_out.end(); it++)
    {
        // OutTrack requirement: 3rd plane hit and total min 3 planes hit
        if (bHit3rdPlane_map[it->first] && it->second >= 3)
        {
            NrOutTrack++;
        }
    }

    // Fill number of out tracks
    for (int i = 0; i < NrOutTrack; i++)
    {
        if (bIsInTrack) // Only register out-tracks if intrack was registered, avoid no track in - track out events
            man->FillH1(1, 2);
    }

    if (NrOutTrack == 1)
    {
        bIsOutSingleTrack = true;
    }
    else if (NrOutTrack > 1)
    {
        bIsOutMultipleTrack = true;
    }
    else if (NrOutTrack == 0)
    {
        bIsNoOutTrack = true;
    }

    // Categorise proccesses ------------------------------------------------------------------------------
    // (I)TITO
    if (bIsInTrack && bIsOutSingleTrack)
    {
        man->FillH1(1, 3);

        // Two possible cases considerd:
        //(I.i) TITO.ElasIn (Elastic interacted primary particle)
        if (!bIsInelastic)
        {
            man->FillH1(1, 4);
        }

        // (I.ii) TITO.InelasTO (Inelastic interaction in target with one charged particle in acceptance)
        else if (bIsInelastic)
        {
            man->FillH1(1, 5);
        }
    }
    //(II) TINO
    // Calculate MSC angle to determin ElasOut cut
    G4RunManager *runManager = (G4RunManager::GetRunManager());
    if (!runManager)
        return;

    const SimulationConstruction *construction = static_cast<const SimulationConstruction *>(runManager->GetUserDetectorConstruction());

    // For proton z = 1 and m = 938 MeV, and for Al X_0 = 8.897 cm
    G4double msc_cut = 3 * CalculateMSCAngle(EnteringEnergy, 1, 8.897 * cm, construction->target_thickness, 938 * MeV);

    if (bIsInTrack && bIsNoOutTrack)
    {
        man->FillH1(1, 6);

        // Three possible cases considered
        G4bool bElasticCondition = !bIsInelastic && (bIsElastic || abs(theta) > msc_cut || abs(phi) > msc_cut);
        //(II.i) TINO.InelasTN (Inelastic interaction with no charged particle in acceptance)
        if (bIsInelastic)
        {
            man->FillH1(1, 7);
        }

        //(II.ii) TINO.ElasOut (Single elastic scattering in target out of acceptance)
        else if (bElasticCondition)
        {
            man->FillH1(1, 8);

            // if (!bIsElastic)
            // {
                // Debug:
                // Display one current event
                // G4UImanager *uiManager = G4UImanager::GetUIpointer();
                // uiManager->ApplyCommand("/vis/enable");
                // G4EventManager *eventManager = G4EventManager::GetEventManager();
                // eventManager->KeepTheCurrentEvent();
                // G4RunManager::GetRunManager()->AbortRun();
            // }
 
        }
        //(II.iii) TINO.Bg (background, like scattering in ALPIDEs / divergence of beam / inelastic in ALPIDE)
        else
        {
            man->FillH1(1, 9);
        }

        //(II.iv) TINO.Absorb(primary particl enetered the target but did not exit. No inelastic interaction and no elastic interaction occured.)
        if (bIsEntered && !bIsExited && !bIsInelastic && !bElasticCondition)
        {

            man->FillH1(1, 13);
        }
    }

    //(III) TIMO
    if (bIsInTrack && bIsOutMultipleTrack)
    {

        // (III.i) TIMO.InelasTM (inelastic interaction in target with multiple charged secondaries in acceptance)
        if (bIsInelastic)
        {
            man->FillH1(1, 11);
        }
        // (III.ii) TIMO.Bg (i.e. delta electrons, whill be filtered out by tracking)
        if (!bIsInelastic)
        {
            man->FillH1(1, 12);
        }
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
    // Handle beam profile data storage
    if (bHitDet0)
    {
        man->FillNtupleDColumn(12, 0, BeamPosDet0X);
        man->FillNtupleDColumn(12, 1, BeamPosDet0Y);
        man->AddNtupleRow(12);
    }

    // Handle post energy data storage
    // Calculate total energy deposited in the event:
    if (bIsInTrack)
    {
        G4double E_tot = 0;
        for (int i = 0; i < PostEnergy.size(); i++)
        {
            E_tot = E_tot + PostEnergy[i];
        }
        man->FillNtupleDColumn(13, 0, E_tot);
        man->AddNtupleRow(13);
    }

    // Store amout of outgoing tracks
    if (NrOutTrack > 1)
    {
        man->FillNtupleDColumn(14, 0, NrOutTrack);
        man->AddNtupleRow(14);
    }
}

// Calculates the mean multiple scattering angle. Input energy in MeV
G4double EventAction::CalculateMSCAngle(G4double E, int z, G4double X_0, G4double x, G4double m)
{
    G4double p = sqrt(pow(E, 2) - pow(m, 2));
    G4double beta = p / E;

    return 13.6 * MeV / (beta * p) * z * sqrt(x / X_0) * (1 + 0.038 * log(x * pow(z, 2) / (X_0 * pow(beta, 2))));
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
