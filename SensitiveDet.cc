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

    // VetoDetector
    if (CopyNo == 100)
    {
        // Handle primary particles ------------------------------------------------------------------------------------------------------------------------------------
        //  Three needed properties to be counted as passed
        //(i) particle moved from veto to world volume (not double count incoming particles)
        //(ii) particle is of specified (produced) type
        //(iii) particle produced in world (reject secondary particles)
        if (PostStepVolume == "physWorld" && PreStepVolume == "physVeto" && track->GetParticleDefinition() == ParticleDefinition && ParticleOriginVolume == "physWorld")
        {
            // G4cout << "Primary particle left target" << G4endl;
            eventAction->OutTrack = preStepPoint->GetMomentumDirection();
        }

        // Three needed properties to be counted as entered particle
        //(i) particle moved from veto to target volume
        //(ii) particle is of specified (produced) type
        //(iii) particle produced in world (reject secondary particles)
        if (PostStepVolume == "physTarget" && PreStepVolume == "physVeto" && track->GetParticleDefinition() == ParticleDefinition && ParticleOriginVolume == "physWorld")
        {
            // G4cout << "Primary particle entered target" << G4endl;
            eventAction->InTrack = preStepPoint->GetMomentumDirection();
        }

        // Handle outgoing secondary particles ------------------------------------------------------------------------------------------------------------------------------------
        //  Two needed properties to be counted as passed secondary
        //(i) particle moved from veto to world volume (not double count incoming particles)
        //(ii) particle produced in target (reject primary particles)
        if (PostStepVolume == "physWorld" && PreStepVolume == "physVeto" && ParticleOriginVolume == "physTarget")
        {
            // G4cout << "Secondary " << track->GetParticleDefinition()->GetParticleName() << " left target" << G4endl;
            eventAction->OutTrackSecondaries.push_back(track->GetMomentumDirection());
            eventAction->ParticleTypeSecondaries.push_back(track->GetParticleDefinition()->GetParticleName());
        }
    }

    // Target
    else if (CopyNo == 101)
    {
        // Detect ingoing primary particle
        if (PreStepStatus == fGeomBoundary && track->GetParticleDefinition() == ParticleDefinition && ParticleOriginVolume == "physWorld")
        {
            // G4cout << "Particle entered the target" << G4endl;
            eventAction->bIsEntered = true;
        }

        // Track inelastic interaction
        if (preStepPoint->GetProcessDefinedStep())
        {
            G4String ProcessName = postStepPoint->GetProcessDefinedStep()->GetProcessName();
            if (track->GetParticleDefinition() == ParticleDefinition && ProcessName == InelasitcProcessName)
            {
                eventAction->bIsAbsorbed = true;
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
        if (PreStepStatus == fGeomBoundary && (track->GetParticleDefinition()->GetPDGCharge() == 1 || track->GetParticleDefinition()->GetPDGCharge() == -1))
        {
            // Hit in single ALPIDEs
            if (CopyNo < 10)
            {
                eventAction->detector_hitvector_map[CopyNo]->push_back(track);
            }
            // Hit in OBM0
            else if (10 <= CopyNo && CopyNo <= 23)
            {
                eventAction->HitTracksOBM0.push_back(track);
            }
            // Hit in OBM1
            else if (30 <= CopyNo && CopyNo <= 43)
            {
                eventAction->HitTracksOBM1.push_back(track);
            }
        }
    }
    return true;
}
