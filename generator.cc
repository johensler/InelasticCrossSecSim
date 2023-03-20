#include "generator.hh"

PrimaryGenerator::PrimaryGenerator()
{
    fParticleGun = new G4ParticleGun(1); // 1 primary vertex per event

    // Define particle ID
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName = "proton";
    G4ParticleDefinition *particle = particleTable->FindParticle(particleName);
    fParticleGun->SetParticleDefinition(particle);

    // Define beam position
    G4double posX = 0; // tan(BeamAngle) * d_BeamStart_target;
    G4double posY = 0;
    G4double posZ = -d_BeamStart_target;

    G4ThreeVector BeamPosition(posX, posY, posZ);
    G4TwoVector XYParticle = RandPointInCircle(BeamPosition, BeamRadius);
    G4ThreeVector ParticlePosition(XYParticle.x(), XYParticle.y(), BeamPosition.getZ());
    fParticleGun->SetParticlePosition(ParticlePosition);

    // Define particles momentum direction
    G4double momY = 0;
    G4double momZ = 1;
    G4double momX = 0; //-tan(BeamAngle) * momZ;

    G4ThreeVector momentum(momX, momY, momZ);
    fParticleGun->SetParticleMomentumDirection(momentum);

    // Define particle momentum magnitude
    fParticleGun->SetParticleMomentum(1 * GeV);
}

PrimaryGenerator::~PrimaryGenerator()
{
    delete fParticleGun;
}

void PrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{
    // G4cout << fParticleGun->GetParticleDefinition()->GetParticleName() << " produced with kin. energy: " << fParticleGun->GetParticleEnergy() << " MeV" << G4endl;
    // G4cout << fParticleGun->GetParticleDefinition()->GetParticleName() << " produced with momentum: " << fParticleGun->GetParticleMomentum() << " MeV" << G4endl;
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

G4TwoVector PrimaryGenerator::RandPointInCircle(G4ThreeVector position, G4double radius)
{
    // Generate two random values between 0 and 1
    G4double rand0 = (double)rand() / RAND_MAX;
    G4double rand1 = (double)rand() / RAND_MAX;

    // Calculate random theta and radius
    G4double theta = 2 * 3.14159265358979323846264 * rand0;
    G4double r = sqrt(rand1);

    // Convert into cartesian coordiantes
    G4double x = position.getX() + r * radius * cos(theta);
    G4double y = position.getY() + r * radius * sin(theta);

    return G4TwoVector(x, y);
}
