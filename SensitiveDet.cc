#include "SensitiveDet.hh"

SensitiveDetector::SensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{
    runManager = static_cast<RunManager *>(RunManager::GetRunManager());
    if (runManager && runManager->eventAction)
    {
        eventAction = runManager->eventAction;
    }
}

SensitiveDetector::~SensitiveDetector()
{
}

G4bool SensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *R0hist)
{
    // Acess the Track corresponding to the Step
    G4Track *track = aStep->GetTrack();

    // Create hit object
    UserHit hit = UserHit();
    hit.SetTrackID(track->GetTrackID());
    hit.SetHitPosition(track->GetPosition());

    // Acess the Step Points
    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    G4StepPoint *postStepPoint = aStep->GetPostStepPoint();

    // Acess the StepSatus of each StepPoint
    G4StepStatus PreStepStatus = preStepPoint->GetStepStatus();
    G4StepStatus PostStepStatus = postStepPoint->GetStepStatus();

    // Acess the Volumes of the StepPoints
    G4String PostStepVolume = postStepPoint->GetPhysicalVolume()->GetName();
    G4String PreStepVolume = preStepPoint->GetPhysicalVolume()->GetName();

    // Determine the Origin of the detected particle
    G4String ParticleOriginVolume = track->GetOriginTouchable()->GetVolume()->GetName();

    // Get the Copy Nr of the detector hit (to differentiate between the different detectors)
    G4int CopyNo = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();

    // Temporary particle definition and inelastic process (not dynamic by now)
    G4ParticleDefinition *ParticleDefinition = G4Proton::Definition();
    G4String InelasitcProcessName = "protonInelastic";

    // Target
    if (CopyNo == 101)
    {
        // Handle outgoing secondary particles --------------------------------------------------------------------------------------------
        if (PostStepVolume == "physWorld" && PreStepVolume == "physTarget" && ParticleOriginVolume == "physTarget")
        {
            // G4cout << "Secondary " << track->GetParticleDefinition()->GetParticleName() << " left target" << G4endl;
            eventAction->OutTrackSecondaries.push_back(track->GetMomentumDirection());
            eventAction->ParticleTypeSecondaries.push_back(track->GetParticleDefinition()->GetParticleName());
        }

        // Detect ingoing proton ---------------------------------------------------------------------------------------
        if (PreStepStatus == fGeomBoundary && track->GetParticleDefinition() == ParticleDefinition, ParticleOriginVolume == "physWorld")
        {
            // G4cout << "Particle entered the target" << G4endl;
            eventAction->bIsEntered = true;

            if (eventAction->InTrack == G4ThreeVector(0, 0, 0)) // Only save the Intrack wenn the particle first enters the target, not overwrite of backscattering happens
            {
                eventAction->EnteringEnergy = track->GetTotalEnergy();
                eventAction->InTrack = preStepPoint->GetMomentumDirection();
            }
        }

        // Detect outgoing proton ----------------------------------------------------------------------------------------------------
        if (PostStepStatus == fGeomBoundary && PreStepVolume == "physTarget" && track->GetParticleDefinition() == ParticleDefinition)
        {
            eventAction->bIsExited = true;

            if (eventAction->OutTrack == G4ThreeVector(0, 0, 0)) // Only save the Outtrack wenn the particle first exits the target, not overwrite of backscattering happens
            {
                // G4cout << "Primary particle left target" << postStepPoint->GetPosition() << G4endl;
                eventAction->OutTrack = postStepPoint->GetMomentumDirection();
            }
        }

        // Track inelastic interaction -------------------------------------------------------------------------
        if (postStepPoint->GetProcessDefinedStep())
        {
            G4String ProcessName = postStepPoint->GetProcessDefinedStep()->GetProcessName();
            if (ProcessName == InelasitcProcessName && track->GetParticleDefinition() == ParticleDefinition && ParticleOriginVolume == "physWorld")
            {
                eventAction->bIsInelastic = true;
            }
        }

        // Track elastic interactions (hadronic elastic and coulomb) CoulombScat hadElastic
        if (postStepPoint->GetProcessDefinedStep())
        {
            G4String ProcessName = postStepPoint->GetProcessDefinedStep()->GetProcessName();
            if (track->GetParticleDefinition() == ParticleDefinition && (ProcessName == "CoulombScat" || ProcessName == "hadElastic") && ParticleOriginVolume == "physWorld")
            {
                eventAction->bIsElastic = true;
            }
        }
    }

    // Scintillator
    else if (CopyNo == 102)
    {
        if (PreStepStatus == fGeomBoundary && track->GetParticleDefinition() == ParticleDefinition && ParticleOriginVolume == "physWorld")
        {
            eventAction->bIsTrigger = true;
        }
    }

    // ALPIDEs
    else if (0 <= CopyNo && CopyNo <= 43)
    {
        // Monte Carlo: Consider only the primary particles to study the "real" scenario
        //  Three needed properties to be counted as passed
        //(i) Step at Boundary (to not double count for multiple steps)
        //(ii) particle is of specified (produced) type
        //(iii) particle produced in world (reject secondary particles)
        if (PreStepStatus == fGeomBoundary && track->GetParticleDefinition() == ParticleDefinition && ParticleOriginVolume == "physWorld")
        {
            if (CopyNo == 0)
            {
                // G4cout << "Proton registered in Det 0" << G4endl;
                eventAction->bHitDet0 = true;

                // Beam Profile
                eventAction->BeamPosDet0X = track->GetPosition().getX();
                eventAction->BeamPosDet0Y = track->GetPosition().getY();
            }
            else if (CopyNo == 1)
            {
                // G4cout << "Proton registered in Det 1" << G4endl;
                eventAction->bHitDet1 = true;
            }
            else if (CopyNo == 2)
            {
                // G4cout << "Proton registered in Det 2" << G4endl;
                eventAction->bHitDet2 = true;
            }
            else if (CopyNo == 3)
            {
                // G4cout << "Proton registered in Det 3" << G4endl;
                eventAction->bHitDet3 = true;
            }
            else if (CopyNo == 4)
            {
                // G4cout << "Proton registered in Det 4" << G4endl;
                eventAction->bHitDet4 = true;
            }
            else if (CopyNo == 5)
            {
                // G4cout << "Proton registered in Det 5" << G4endl;
                eventAction->bHitDet5 = true;
            }
            else if (10 <= CopyNo && CopyNo <= 23)
            {
                // G4cout << "Proton registered in OBM1" << G4endl;
                eventAction->bHitOB0 = true;
            }
            else if (30 <= CopyNo && CopyNo <= 43)
            {
                // G4cout << "Proton registered in OBM2" << G4endl;
                eventAction->bHitOB1 = true;
            }
        }

        // Handle measurement: All Charged particles are measured
        G4double charge = track->GetParticleDefinition()->GetPDGCharge();
        if (PreStepStatus == fGeomBoundary && (charge == 1 || charge == -1) && !(track->GetParticleDefinition()->GetParticleName() == "e-" && track->GetKineticEnergy() < 1 * MeV))
        {

            // Hit in single ALPIDEs
            if (CopyNo < 10)
            {

                if (!(ContainsTrackID(*(eventAction->detector_hitvector_map[CopyNo]), hit.GetTrackID())))
                {
                    eventAction->detector_hitvector_map[CopyNo]->push_back(hit);

                    // // Set all variables the track saved track should contain
                    // eventAction->detector_hitvector_map[CopyNo]->back().SetTrackID((*track).GetTrackID()); // TrackID needs to be set since constructor sets it to zero when assigning
                    // eventAction->detector_hitvector_map[CopyNo]->back().SetPosition((*track).GetPosition());
                }
            }
            // Hit in OBM0
            else if (10 <= CopyNo && CopyNo <= 23)
            {
                if (!(ContainsTrackID(eventAction->HitTracksOBM0, hit.GetTrackID())))
                {
                    eventAction->HitTracksOBM0.push_back(hit);
                    // eventAction->HitTracksOBM0.back().SetTrackID((*track).GetTrackID());
                    // eventAction->HitTracksOBM0.back().SetPosition((*track).GetPosition());
                }
            }
            // Hit in OBM1
            else if (30 <= CopyNo && CopyNo <= 43)
            {
                if (!(ContainsTrackID(eventAction->HitTracksOBM1, hit.GetTrackID())))
                {
                    eventAction->HitTracksOBM1.push_back(hit);
                    // eventAction->HitTracksOBM1.back().SetTrackID((*track).GetTrackID());
                    // eventAction->HitTracksOBM1.back().SetPosition((*track).GetPosition());
                }
            }

            // Access energy of particles at position of ALP3
            if (CopyNo == 3)
            {
                eventAction->PostEnergy.push_back(track->GetKineticEnergy());
            }
        }
    }
    return true;
}

// Function to check if a trackID is already stored during the same event -> prevent multiple hits of same particle on one plane (delta electrons)
bool SensitiveDetector::ContainsTrackID(std::vector<UserHit> vec, int trackID)
{
    for (auto it = vec.begin(); it != vec.end(); it++)
    {
        if ((*it).GetTrackID() == trackID)
        {
            return true;
        }
    }
    return false;
}
