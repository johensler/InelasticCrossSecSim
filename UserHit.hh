#ifndef USERHIT_HH
#define USERHIT_HH

#include "G4VHit.hh"
#include "G4Event.hh" //Needed vor G4ThreeVector

class UserHit
{
    public:

        UserHit();
        ~UserHit();

        void SetTrackID(int ID);
        int GetTrackID();

        void SetHitPosition(G4ThreeVector Position);
        G4ThreeVector GetHitPosition();

    // private:
        int TrackID;
        G4ThreeVector HitPosition;
};

#endif