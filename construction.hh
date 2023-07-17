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
#include "G4GenericMessenger.hh"
#include "G4SubtractionSolid.hh"
#include <cmath>

class SensitiveDet;

class SimulationConstruction : public G4VUserDetectorConstruction
{
public:
    SimulationConstruction();
    ~SimulationConstruction();

    virtual G4VPhysicalVolume *Construct();

    G4double target_thickness;
    G4bool bIsTargetIn;

private:
    G4GenericMessenger *fMessenger;

    virtual void ConstructSDandField();
    void DefineMaterials();

    G4double world_halfsizeX = 0.12 * m;
    G4double world_halfsizeY = 0.12 * m;
    G4double world_halfsizeZ = 0.2 * m;

    // Solids-------------------------------------------------------------
    G4Box *solidWorld;
    G4Box *solidTarget;
    G4Box *solidDet;
    G4Box *solidDetOBM;
    G4Box *solidSci;
    // Logics-------------------------------------------------------------
    G4LogicalVolume *logicWorld;
    G4LogicalVolume *logicTarget;
    G4LogicalVolume *logicALPIDE;
    G4LogicalVolume *logicALPIDEOBM;
    G4LogicalVolume *logicSci;

    // Physics-------------------------------------------------------------
    G4VPhysicalVolume *physWorld;
    G4VPhysicalVolume *physTarget;
    G4VPhysicalVolume *physSci;

    // Signle ALPIDEs
    G4VPhysicalVolume *physALPIDE0;
    G4VPhysicalVolume *physALPIDE1;
    G4VPhysicalVolume *physALPIDE2;
    G4VPhysicalVolume *physALPIDE3;
    G4VPhysicalVolume *physALPIDE4;
    G4VPhysicalVolume *physALPIDE5;

    // Mats-------------------------------------------------------------
    G4Material *worldMat;
    G4Material *targetMat;
    G4Material *detMat;
    G4Material *sciMat;
};

#endif
