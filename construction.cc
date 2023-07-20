#include "construction.hh"
#include "SensitiveDet.hh"

SimulationConstruction::SimulationConstruction()
{
    // Define UserMessenges
    fMessenger = new G4GenericMessenger(this, "/construction/", "Simulation Construction");
    fMessenger->DeclarePropertyWithUnit("target_thickness", "mm", target_thickness, "Thickness of the target");
    fMessenger->DeclareProperty("bIsTargetIn", bIsTargetIn, "Include / exclude target");

    // Initial Values
    target_thickness = 1 * mm;
    bIsTargetIn = true;

    // Once define the materials
    DefineMaterials();
}

SimulationConstruction::~SimulationConstruction()
{
}

G4VPhysicalVolume *SimulationConstruction::Construct()
{
    // SOLID VOLUMES ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    solidWorld = new G4Box("solidWorld", world_halfsizeX, world_halfsizeY, world_halfsizeZ); // half of the length
    solidTarget = new G4Box("solidTarget", 7 / 2 * cm, 7 / 2 * cm, target_thickness / 2);
    solidDet = new G4Box("solidDet", 1024 * 29.24 / 2 * micrometer, 512 * 26.88 / 2 * micrometer, 50 / 2 * micrometer);
    solidDetOBM = new G4Box("solidDetOBM", 1024 * 29.24 / 2 * micrometer, 512 * 26.88 / 2 * micrometer, 100 / 2 * micrometer);
    solidSci = new G4Box("solidSci", 45 / 2 * mm, 25 / 2 * mm, 10 / 2 * mm);

    // LOGIC VOLUMES ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    logicTarget = new G4LogicalVolume(solidTarget, targetMat, "logicTarget");
    logicALPIDE = new G4LogicalVolume(solidDet, detMat, "logicALPIDE");
    logicALPIDEOBM = new G4LogicalVolume(solidDetOBM, detMat, "logicALPIDE");
    logicSci = new G4LogicalVolume(solidSci, sciMat, "logicSci");

    // WORLD POSITIONS ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    G4double det2Zpos = -target_thickness / 2 - 0.5 * cm;
    G4double det1Zpos = det2Zpos - 2.3 * cm;
    G4double det0Zpos = det1Zpos - 2.3 * cm;

    G4double det3Zpos = target_thickness / 2 + 0.5 * cm;
    G4double det4Zpos = det3Zpos + 2.3 * cm;
    G4double det5Zpos = det4Zpos + 2.3 * cm;

    G4double OBM1Zpos = det5Zpos + 3 * cm;
    G4double OBM2Zpos = OBM1Zpos + 3 * cm;

    G4double sciZpos = det0Zpos - 2 * cm - 10 / 2 * mm;

    // PHYSIC VOLUMES ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);
    physSci = new G4PVPlacement(0, G4ThreeVector(0, 0., sciZpos), logicSci, "physSci", logicWorld, false, 102, true);

    if (bIsTargetIn)
    {
        physTarget = new G4PVPlacement(0, G4ThreeVector(0, 0., 0), logicTarget, "physTarget", logicWorld, false, 101, true);
    }

    // Single ALPIDEs
    physALPIDE0 = new G4PVPlacement(0, G4ThreeVector(0., 0., det0Zpos), logicALPIDE, "physALPIDE0", logicWorld, false, 0, true);
    physALPIDE1 = new G4PVPlacement(0, G4ThreeVector(0., 0., det1Zpos), logicALPIDE, "physALPIDE1", logicWorld, false, 1, true);
    physALPIDE2 = new G4PVPlacement(0, G4ThreeVector(0., 0., det2Zpos), logicALPIDE, "physALPIDE2", logicWorld, false, 2, true);
    physALPIDE3 = new G4PVPlacement(0, G4ThreeVector(0., 0., det3Zpos), logicALPIDE, "physALPIDE3", logicWorld, false, 3, true);
    physALPIDE4 = new G4PVPlacement(0, G4ThreeVector(0., 0., det4Zpos), logicALPIDE, "physALPIDE4", logicWorld, false, 4, true);
    physALPIDE5 = new G4PVPlacement(0, G4ThreeVector(0., 0., det5Zpos), logicALPIDE, "physALPIDE5", logicWorld, false, 5, true);

    double ALPIDE_x = 1024 * 29.24 * micrometer;
    double ALPIDE_y = 512 * 26.88 * micrometer;
    double ALPIDE_distance = 150 * micrometer;
    // OBM 0
    G4int ID = 10;
    for (G4int i = 0; i < 7; i++)
    {
        for (G4int j = 0; j < 2; j++)
        {
            G4VPhysicalVolume *physOBM1 = new G4PVPlacement(0,
                                                            G4ThreeVector(-3 * (ALPIDE_x + ALPIDE_distance) + (i * (ALPIDE_x + ALPIDE_distance)), -0.5 * (ALPIDE_y + ALPIDE_distance) + (j * (ALPIDE_y + ALPIDE_distance)), OBM1Zpos),
                                                            logicALPIDEOBM,
                                                            "physOBM1",
                                                            logicWorld,
                                                            false,
                                                            ID,
                                                            true);

            ID++;
        }
    }

    // OBM 1
    ID = 30;
    for (G4int i = 0; i < 7; i++)
    {
        for (G4int j = 0; j < 2; j++)
        {
            G4VPhysicalVolume *physOBM2 = new G4PVPlacement(0,
                                                            G4ThreeVector(-3 * (ALPIDE_x + ALPIDE_distance) + (i * (ALPIDE_x + ALPIDE_distance)), -0.5 * (ALPIDE_y + ALPIDE_distance) + (j * (ALPIDE_y + ALPIDE_distance)), OBM2Zpos),
                                                            logicALPIDEOBM,
                                                            "physOBM0",
                                                            logicWorld,
                                                            false,
                                                            ID,
                                                            true);
            ID++;
        }
    }

    return physWorld;
}

void SimulationConstruction::ConstructSDandField()
{

    SensitiveDetector *SensTarget = new SensitiveDetector("SensitiveTarget");
    logicTarget->SetSensitiveDetector(SensTarget);

    SensitiveDetector *SensALPIDE = new SensitiveDetector("SensitiveALPIDE");
    logicALPIDE->SetSensitiveDetector(SensALPIDE);

    SensitiveDetector *SensSci = new SensitiveDetector("SensitiveScintillator");
    logicSci->SetSensitiveDetector(SensSci);
}

void SimulationConstruction::DefineMaterials()
{
    // MATERIALS ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    G4NistManager *nist = G4NistManager::Instance();
    worldMat = nist->FindOrBuildMaterial("G4_AIR");
    targetMat = nist->FindOrBuildMaterial("G4_Al");
    detMat = nist->FindOrBuildMaterial("G4_Si");
    sciMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
}
