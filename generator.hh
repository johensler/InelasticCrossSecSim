#ifndef GENERATOR_HH
#define GENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4particleGun.hh"
#include "G4SingleParticleSource.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include <cmath>

class PrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGenerator();
    ~PrimaryGenerator();

    virtual void GeneratePrimaries(G4Event *anEvent);

    // Defines the distance between the beam exit and the target position
    G4double d_BeamStart_target = 6 * cm;
    G4double BeamAngle = 0 * degree;
    G4double BeamRadius = 0 * cm;

private:
    G4GeneralParticleSource *fGenParSource;

    // Store world box
    G4Box *fWorldBox = nullptr;
};

#endif