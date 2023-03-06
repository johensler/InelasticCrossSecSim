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
    solidTarget = new G4Box("solidTarget", 5 * cm, 5 * cm, target_thickness / 2);
    solidDet = new G4Box("solidDet", 1024 * 29.24 / 2 * micrometer, 512 * 26.88 / 2 * micrometer, 50 / 2 * micrometer);
    solidOB = new G4Box("solidOB", 7 * 1024 * 29.24 / 2 * micrometer, 2 * 512 * 26.88 / 2 * micrometer, 50 / 2 * micrometer);

    G4Box *Box1 = new G4Box("Box1", 5 * cm, 5 * cm, target_thickness / 2);
    G4Box *Box2 = new G4Box("Box2", 5.2 * cm, 5.2 * cm, target_thickness / 2 + 2);
    solidVeto = new G4SubtractionSolid("solidVeto", Box2, Box1);

    // LOGIC VOLUMES ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    logicVeto = new G4LogicalVolume(solidVeto, worldMat, "logicVeto");
    logicTarget = new G4LogicalVolume(solidTarget, targetMat, "logicTarget");

    logicDet0 = new G4LogicalVolume(solidDet, detMat, "logicDet0");
    logicDet1 = new G4LogicalVolume(solidDet, detMat, "logicDet1");
    logicDet2 = new G4LogicalVolume(solidDet, detMat, "logicDet2");
    logicDet3 = new G4LogicalVolume(solidDet, detMat, "logicDet3");
    logicDet4 = new G4LogicalVolume(solidDet, detMat, "logicDet4");
    logicDet5 = new G4LogicalVolume(solidDet, detMat, "logicDet5");

    logicOB0 = new G4LogicalVolume(solidOB, detMat, "logicOB0");
    logicOB1 = new G4LogicalVolume(solidOB, detMat, "logicOB1");

    // WORLD POSITIONS ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    // G4double det0Zpos = target_thickness / 2 + d_target_det0;
    // G4double det1Zpos = det0Zpos + d_det0_det1;

    G4double det2Zpos = target_thickness / 2 - 1 * cm;
    G4double det1Zpos = det2Zpos - 2.3 * cm;
    G4double det0Zpos = det1Zpos - 2.3 * cm;

    G4double det3Zpos = target_thickness / 2 + 0.5 * cm;
    G4double det4Zpos = det3Zpos + 2.3 * cm;
    G4double det5Zpos = det4Zpos + 2.3 * cm;

    G4double OB0Zpos = det5Zpos + 3 * cm;
    G4double OB1Zpos = OB0Zpos + 3 * cm;

    // PHYSIC VOLUMES ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);
    physTarget = new G4PVPlacement(0, G4ThreeVector(0, 0., 0), logicTarget, "physTarget", logicWorld, false, 101, true);
    physVeto = new G4PVPlacement(0, G4ThreeVector(0., 0., 0), logicVeto, "physVeto", logicWorld, false, 100, true);

    physDet0 = new G4PVPlacement(0, G4ThreeVector(0., 0., det0Zpos), logicDet0, "physDet", logicWorld, false, 0, true);
    physDet1 = new G4PVPlacement(0, G4ThreeVector(0., 0., det1Zpos), logicDet1, "physDet2", logicWorld, false, 1, true);
    physDet2 = new G4PVPlacement(0, G4ThreeVector(0., 0., det2Zpos), logicDet2, "physDet2", logicWorld, false, 2, true);
    physDet3 = new G4PVPlacement(0, G4ThreeVector(0., 0., det3Zpos), logicDet3, "physDet3", logicWorld, false, 3, true);
    physDet4 = new G4PVPlacement(0, G4ThreeVector(0., 0., det4Zpos), logicDet4, "physDet4", logicWorld, false, 4, true);
    physDet5 = new G4PVPlacement(0, G4ThreeVector(0., 0., det5Zpos), logicDet5, "physDet5", logicWorld, false, 5, true);
    physOB0 = new G4PVPlacement(0, G4ThreeVector(0., 0., OB0Zpos), logicOB0, "physOB0", logicWorld, false, 6, true);
    physOB1 = new G4PVPlacement(0, G4ThreeVector(0., 0., OB1Zpos), logicOB1, "physOB1", logicWorld, false, 7, true);

    return physWorld;
}

void SimulationConstruction::ConstructSDandField()
{
    SensitiveDetector *SensDet0 = new SensitiveDetector("SensitiveDetector0");
    SensitiveDetector *SensDet1 = new SensitiveDetector("SensitiveDetector1");
    SensitiveDetector *SensDet2 = new SensitiveDetector("SensitiveDetector2");
    SensitiveDetector *SensDet3 = new SensitiveDetector("SensitiveDetector3");
    SensitiveDetector *SensDet4 = new SensitiveDetector("SensitiveDetector4");
    SensitiveDetector *SensDet5 = new SensitiveDetector("SensitiveDetector5");
    SensitiveDetector *SensOB0 = new SensitiveDetector("SensitiveOB0");
    SensitiveDetector *SensOB1 = new SensitiveDetector("SensitiveOB1");
    SensitiveDetector *SensVeto = new SensitiveDetector("SensitiveVeto");
    SensitiveDetector *SensTarget = new SensitiveDetector("SensitiveTarget");

    logicDet0->SetSensitiveDetector(SensDet0);
    logicDet1->SetSensitiveDetector(SensDet1);
    logicDet2->SetSensitiveDetector(SensDet2);
    logicDet3->SetSensitiveDetector(SensDet3);
    logicDet4->SetSensitiveDetector(SensDet4);
    logicDet5->SetSensitiveDetector(SensDet5);
    logicOB0->SetSensitiveDetector(SensOB0);
    logicOB1->SetSensitiveDetector(SensOB1);
    logicVeto->SetSensitiveDetector(SensVeto);
    logicTarget->SetSensitiveDetector(SensTarget);
}

void SimulationConstruction::DefineMaterials()
{
    // MATERIALS ......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........
    G4NistManager *nist = G4NistManager::Instance();
    worldMat = nist->FindOrBuildMaterial("G4_AIR");
    targetMat = nist->FindOrBuildMaterial("G4_Al");
    detMat = nist->FindOrBuildMaterial("G4_Si");
}
