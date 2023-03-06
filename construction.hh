#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4SystemOfUnits.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include <cmath>
#include "G4GenericMessenger.hh"
#include "G4SubtractionSolid.hh"

#include "SensitiveDet.hh"

class SimulationConstruction : public G4VUserDetectorConstruction
{
public:
    SimulationConstruction();
    ~SimulationConstruction();

    virtual G4VPhysicalVolume *Construct();

    G4double d_target_det0;
    G4double d_det0_det1;
    G4double target_thickness;

private:
    G4GenericMessenger *fMessenger;

    virtual void ConstructSDandField();
    void DefineMaterials();

    G4double world_halfsizeX = 0.12 * m;
    G4double world_halfsizeY = 0.12 * m;
    G4double world_halfsizeZ = 0.2 * m;

    // Solids
    G4Box *solidWorld;
    G4Box *solidTarget;
    G4Box *solidDet;
    G4Box *solidOB;
    G4SubtractionSolid *solidVeto;

    // Logics
    G4LogicalVolume *logicWorld;
    G4LogicalVolume *logicTarget;
    G4LogicalVolume *logicVeto;

    G4LogicalVolume *logicDet0;
    G4LogicalVolume *logicDet1;
    G4LogicalVolume *logicDet2;
    G4LogicalVolume *logicDet3;
    G4LogicalVolume *logicDet4;
    G4LogicalVolume *logicDet5;

    G4LogicalVolume *logicOB0;
    G4LogicalVolume *logicOB1;

    // Physics
    G4VPhysicalVolume *physWorld;
    G4VPhysicalVolume *physTarget;
    G4VPhysicalVolume *physVeto;

    G4VPhysicalVolume *physDet0;
    G4VPhysicalVolume *physDet1;
    G4VPhysicalVolume *physDet2;
    G4VPhysicalVolume *physDet3;
    G4VPhysicalVolume *physDet4;
    G4VPhysicalVolume *physDet5;

    G4VPhysicalVolume *physOB0;
    G4VPhysicalVolume *physOB1;

    // Mats
    G4Material *worldMat;
    G4Material *targetMat;
    G4Material *detMat;
};

#endif
