#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "globals.hh"
#include "G4UserSteppingAction.hh"
#include "G4AssemblyVolume.hh"

class EventAction;
//class Geometry;
class G4LogicalVolume;

/// Stepping action class
///

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(EventAction* eventAction);
    virtual ~SteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);

  private:
    EventAction*  fEventAction;

    G4bool light_scint_model;

	  G4LogicalVolume*  fScoringVol_EMCal1;
	  G4LogicalVolume*  fScoringVol_EMCal2;

    G4LogicalVolume*  fScoringVol_PAD;
    G4LogicalVolume*  fScoringVol_PIX;
    G4LogicalVolume*  fScoringVol_SCINT;
};



#endif
