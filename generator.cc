#include "generator.hh"

PrimaryGenerator::PrimaryGenerator()
{
    fGenParSource = new G4GeneralParticleSource(); // Construct a GeneralParticleSource (already includes one SingleParticleSource)

    // Define particle
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName = "proton";
    G4ParticleDefinition *particle = particleTable->FindParticle(particleName);
    fGenParSource->GetCurrentSource()->SetParticleDefinition(particle);

    // Define particles momentum direction
    G4ThreeVector momentum(0., 0., 1.);
    fGenParSource->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(momentum);

    // Define particle energy distribution and default value
    fGenParSource->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
    fGenParSource->GetCurrentSource()->GetEneDist()->SetMonoEnergy(232 * MeV);

    // Define beam position distribution
    G4double FWHM = 1 * cm;
    G4double sigma = (1 / 2.35482004503) * FWHM;

    fGenParSource->GetCurrentSource()->GetPosDist()->SetPosDisType("Beam");                                       // Type beam: gaussian distrbution
    fGenParSource->GetCurrentSource()->GetPosDist()->SetBeamSigmaInR(sigma);                                      // Set sigma of beam profile
    fGenParSource->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0., 0., -d_BeamStart_target)); // Center of beam
}

PrimaryGenerator::~PrimaryGenerator()
{
    delete fGenParSource;
}

void PrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{
    fGenParSource->GetCurrentSource()->GeneratePrimaryVertex(anEvent);
}
