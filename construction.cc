#include "construction.hh"

SimulationConstruction::SimulationConstruction()
{
    // Define UserMessenges
    fMessenger = new G4GenericMessenger(this, "/construction/", "Simulation Construction");
    fMessenger->DeclarePropertyWithUnit("target_thickness", "mm", target_thickness, "Thickness of the target");
    fMessenger->DeclarePropertyWithUnit("d_target_det0", "mm", d_target_det0, "Distance from target to first detector");
    fMessenger->DeclarePropertyWithUnit("d_det0_det1", "mm", d_det0_det1, "Distance from first detector to last");

    // Initial Values
    target_thickness = 5 * mm;
    d_target_det0 = 4 * mm;
    d_det0_det1 = 6.9 * cm;

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
    // solidTarget = new G4Box("solidTarget", 5 / 2 * cm, 3 / 2 * cm, target_thickness / 2);
    solidTarget = new G4Box("solidTarget", 5 / 2 * mm, 5 / 2 * mm, target_thickness / 2);
    solidDet = new G4Box("solidDet", 1024 * 29.24 / 2 * micrometer, 512 * 26.88 / 2 * micrometer, 50 / 2 * micrometer);

    // G4Box *Box1 = new G4Box("Box1", 5 / 2 * cm, 3 / 2 * cm, target_thickness / 2);
    // G4Box *Box2 = new G4Box("Box2", 5.2 / 2 * cm, 3.2 / 2 * cm, target_thickness / 2 + 2);
    G4Box *Box1 = new G4Box("Box1", 5 / 2 * mm, 5 / 2 * mm, target_thickness / 2);
    G4Box *Box2 = new G4Box("Box2", 5.2 / 2 * mm, 5.2 / 2 * mm, target_thickness / 2 + 2);
    solidVeto = new G4SubtractionSolid("solidVeto", Box2, Box1);

    // solidVeto = new G4Sphere("solidVeto", 2.7 * cm, 2.9 * cm, 0, 360 * degree, 0, 180 * degree);

    // LOGIC VOLUMES ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    logicVeto = new G4LogicalVolume(solidVeto, worldMat, "logicVeto");
    logicTarget = new G4LogicalVolume(solidTarget, targetMat, "logicTarget");
    logicALPIDE = new G4LogicalVolume(solidDet, detMat, "logicALPIDE");

    // WORLD POSITIONS ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    G4double det2Zpos = target_thickness / 2 - 1 * cm;
    G4double det1Zpos = det2Zpos - 2.3 * cm;
    G4double det0Zpos = det1Zpos - 2.3 * cm;

    G4double det3Zpos = target_thickness / 2 + 0.5 * cm;
    G4double det4Zpos = det3Zpos + 2.3 * cm;
    G4double det5Zpos = det4Zpos + 2.3 * cm;

    G4double OBM1Zpos = det5Zpos + 3 * cm;
    G4double OBM2Zpos = OBM1Zpos + 3 * cm;

    // PHYSIC VOLUMES ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);
    physTarget = new G4PVPlacement(0, G4ThreeVector(0, 0., 0), logicTarget, "physTarget", logicWorld, false, 101, true);
    physVeto = new G4PVPlacement(0, G4ThreeVector(0., 0., 0), logicVeto, "physVeto", logicWorld, false, 100, true);

    // Single ALPIDEs
    physALPIDE0 = new G4PVPlacement(0, G4ThreeVector(0., 0., det0Zpos), logicALPIDE, "physALPIDE0", logicWorld, false, 0, true);
    physALPIDE1 = new G4PVPlacement(0, G4ThreeVector(0., 0., det1Zpos), logicALPIDE, "physALPIDE1", logicWorld, false, 1, true);
    physALPIDE2 = new G4PVPlacement(0, G4ThreeVector(0., 0., det2Zpos), logicALPIDE, "physALPIDE2", logicWorld, false, 2, true);
    physALPIDE3 = new G4PVPlacement(0, G4ThreeVector(0., 0., det3Zpos), logicALPIDE, "physALPIDE3", logicWorld, false, 3, true);
    physALPIDE4 = new G4PVPlacement(0, G4ThreeVector(0., 0., det4Zpos), logicALPIDE, "physALPIDE4", logicWorld, false, 4, true);
    physALPIDE5 = new G4PVPlacement(0, G4ThreeVector(0., 0., det5Zpos), logicALPIDE, "physALPIDE5", logicWorld, false, 5, true);

    // OBM 0
    G4int ID = 10;
    for (G4int i = 0; i < 7; i++)
    {
        for (G4int j = 0; j < 2; j++)
        {
            std::stringstream strOBM_ID;
            strOBM_ID << i << j;
            double ALPIDE_x = 1024 * 29.24 * micrometer;
            double ALPIDE_y = 512 * 26.88 * micrometer;
            double ALPIDE_distance = 100 * micrometer;
            G4VPhysicalVolume *physOBM1 = new G4PVPlacement(0,
                                                            G4ThreeVector(-3 * (ALPIDE_x + ALPIDE_distance) + (i * (ALPIDE_x + ALPIDE_distance)), -0.5 * (ALPIDE_y + ALPIDE_distance) + (j * (ALPIDE_y + ALPIDE_distance)), OBM1Zpos),
                                                            logicALPIDE,
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
            std::stringstream strOBM_ID;
            strOBM_ID << i << j;
            double ALPIDE_x = 1024 * 29.24 * micrometer;
            double ALPIDE_y = 512 * 26.88 * micrometer;
            double ALPIDE_distance = 100 * micrometer;
            G4VPhysicalVolume *physOBM2 = new G4PVPlacement(0,
                                                            G4ThreeVector(-3 * (ALPIDE_x + ALPIDE_distance) + (i * (ALPIDE_x + ALPIDE_distance)), -0.5 * (ALPIDE_y + ALPIDE_distance) + (j * (ALPIDE_y + ALPIDE_distance)), OBM2Zpos),
                                                            logicALPIDE,
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
    SensitiveDetector *SensVeto = new SensitiveDetector("SensitiveVeto");
    logicVeto->SetSensitiveDetector(SensVeto);

    SensitiveDetector *SensTarget = new SensitiveDetector("SensitiveTarget");
    logicTarget->SetSensitiveDetector(SensTarget);

    SensitiveDetector *SensALPIDE = new SensitiveDetector("SensitiveALPIDE");
    logicALPIDE->SetSensitiveDetector(SensALPIDE);
}

void SimulationConstruction::DefineMaterials()
{
    // MATERIALS ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    G4NistManager *nist = G4NistManager::Instance();
    worldMat = nist->FindOrBuildMaterial("G4_AIR");
    targetMat = nist->FindOrBuildMaterial("G4_Al");
    detMat = nist->FindOrBuildMaterial("G4_Si");
}
