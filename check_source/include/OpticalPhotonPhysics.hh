#ifndef OpticalPhotonPhysics_h
#define OpticalPhotonPhysics_h

#include "G4VPhysicsConstructor.hh"

class OpticalPhotonPhysics : public G4VPhysicsConstructor
{
    public:
        OpticalPhotonPhysics(const G4String& name = "optical");
        ~OpticalPhotonPhysics();

    // This method is dummy for physics
    void ConstructParticle() {};

    // Add all processes for optical photons
    void ConstructProcess();
};

#endif
