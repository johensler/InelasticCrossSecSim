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

    // Acess the primary particle type
    // if (runManager && runManager->GetCurrentEvent())
    // {
    //     PrimaryDefinition = runManager->GetCurrentEvent()->GetPrimaryVertex()->GetPrimary()->GetParticleDefinition();
    //     G4cout << "Primary Particle " << PrimaryDefinition->GetParticleName() << G4endl;
    // }

    // Temporary particle definition and inelastic process (not dynamic by now)
    G4ParticleDefinition *ParticleDefinition = G4Proton::Definition();
    G4String InelasitcProcessName = "protonInelastic";

    // VetoDetector
    if (CopyNo == 100)
    {
        // Three needed properties to be counted as passed
        //(i) particle moved from veto to world volume (not double count incoming particles)
        //(ii) particle is of specified (produced) type
        //(iii) particle produced in world (reject secondary particles)
        if (PostStepVolume == "physWorld" && PreStepVolume == "physVeto" && track->GetParticleDefinition() == ParticleDefinition && ParticleOriginVolume == "physWorld")
        {
            // G4cout << "Primary particle left target" << G4endl;
            eventAction->OutTrack = track->GetMomentumDirection();
            eventAction->bIsPassed = true;
        }

        // Three needed properties to be counted as entered particle
        //(i) particle moved from veto to target volume
        //(ii) particle is of specified (produced) type
        //(iii) particle produced in world (reject secondary particles)
        if (PostStepVolume == "physTarget" && PreStepVolume == "physVeto" && track->GetParticleDefinition() == ParticleDefinition && ParticleOriginVolume == "physWorld")
        {
            // G4cout << "Primary particle entered target" << G4endl;
            eventAction->InTrack = track->GetMomentumDirection();
        }
    }

    else if (CopyNo == 101)
    {
        if (preStepPoint->GetProcessDefinedStep())
        {
            G4String ProcessName = postStepPoint->GetProcessDefinedStep()->GetProcessName();
            if (track->GetParticleDefinition() == ParticleDefinition && ProcessName == InelasitcProcessName)
            {
                // G4cout << "Inelastic Interaction" << G4endl;
                eventAction->bIsAbsorbed = true;
            }
            // G4String ProcessName = preStepPoint->GetProcessDefinedStep()->GetProcessName();
            // G4String ProcessName2 = postStepPoint->GetProcessDefinedStep()->GetProcessName();
            // G4cout << "preStep " << track->GetParticleDefinition()->GetParticleName() << " " << ProcessName << G4endl;
            // G4cout << "postStep " << track->GetParticleDefinition()->GetParticleName() << " " << ProcessName2 << G4endl;
        }
    }

    // Telescope Detectors
    else if (0 <= CopyNo && CopyNo <= 7)
    {
        // Three needed properties to be counted as passed
        //(i) Step at Boundary (to not double count for multiple steps)
        //(ii) particle is of specified (produced) type
        //(iii) particle produced in world (reject secondary particles)
        if (PreStepStatus == fGeomBoundary && track->GetParticleDefinition() == ParticleDefinition && ParticleOriginVolume == "physWorld")
        {
            if (CopyNo == 0)
            {
                // G4cout << "Proton registered in Det 0" << G4endl;
                eventAction->bHitDet0 = true;
            }
            else if (CopyNo == 1)
            {
                // G4cout << "Proton registered in Det 1" << G4endl;
                eventAction->bHitDet1 = true;
            }
        }
    }
    return true;
}

