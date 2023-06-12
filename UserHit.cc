#include "UserHit.hh"

UserHit::UserHit()
{
    TrackID = 0;
    // HitPosition = G4ThreeVector(0, 0, 0);
}

UserHit::~UserHit()
{
    
}

void UserHit::SetTrackID(int ID)
{
    TrackID = ID;
}

int UserHit::GetTrackID()
{
    return TrackID;
}

void UserHit::SetHitPosition(G4ThreeVector Position)
{
    HitPosition = Position;
}

G4ThreeVector UserHit::GetHitPosition()
{
    return HitPosition;
}
