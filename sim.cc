#include <iostream>
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4SystemOfUnits.hh"
#include "QBBC.hh"

#include "RunManager.hh"
#include "construction.hh"
#include "physics.hh"
#include "action.hh"

int main(int argc, char **argv)
{
    RunManager *runManager = new RunManager();

    // Construction
    runManager->SetUserInitialization(new SimulationConstruction());

    // Physics
    G4VModularPhysicsList *physicsList = new QBBC;
    // runManager->SetUserInitialization(new PhysicsList()); // own physics list
    runManager->SetUserInitialization(physicsList); // Geant4 physics list

    // Action
    ActionInitialization *Action = new ActionInitialization();
    Action->generator->d_BeamStart_target = 10 * cm;
    Action->generator->BeamAngle = 0 * degree;
    runManager->SetUserInitialization(Action);

    runManager->Initialize();

    // Only create ui visulization if no argument given (run macro)
    G4UIExecutive *ui = 0;
    if (argc == 1)
    {
        ui = new G4UIExecutive(argc, argv);
    }

    G4VisManager *visManager = new G4VisExecutive();
    visManager->Initialize();

    G4UImanager *uiManager = G4UImanager::GetUIpointer();
    if (ui)
    {
        // No macro file -> Use UI
        uiManager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
    }
    else
    {
        // Run with macro file
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        uiManager->ApplyCommand(command + fileName);
        G4cout << "Geant4 started with run macro: " << fileName << G4endl;
    }

    return 0;
}