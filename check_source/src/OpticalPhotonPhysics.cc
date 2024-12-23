#include "OpticalPhotonPhysics.hh"

#include "G4ProcessManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpWLS.hh"
#include "G4PhotoElectricEffect.hh"

OpticalPhotonPhysics::OpticalPhotonPhysics(const G4String& name)
  :  G4VPhysicsConstructor(name)
{ }

OpticalPhotonPhysics::~OpticalPhotonPhysics()
{ }

void OpticalPhotonPhysics::ConstructProcess()
{
    G4ProcessManager *pmanager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();
    pmanager->AddDiscreteProcess(new G4OpAbsorption());
    pmanager->AddDiscreteProcess(new G4OpRayleigh());
    pmanager->AddDiscreteProcess(new G4OpMieHG());
    pmanager->AddDiscreteProcess(new G4OpBoundaryProcess());
    pmanager->AddDiscreteProcess(new G4OpWLS());
    pmanager->AddDiscreteProcess(new G4PhotoElectricEffect());
}
