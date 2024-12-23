#include "SteppingAction.hh"
#include "EventAction.hh"
#include "Geometry.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4EmSaturation.hh"
#include "G4LossTableManager.hh"
#include "G4OpticalPhoton.hh"
#include "Analysis.hh"

G4int I=0;
G4double NTRACK[10];
G4double COPYNUM[10];

SteppingAction::SteppingAction(EventAction* eventAction)
    : G4UserSteppingAction(),
    fEventAction(eventAction),
    light_scint_model(1),
    fScoringVol_PAD(0),
    fScoringVol_PIX(0),
    fScoringVol_SCINT(0)
{}


SteppingAction::~SteppingAction()
{}


void SteppingAction::UserSteppingAction(const G4Step* step)
{
    if (UseFoCalH && ( !fScoringVol_PAD || !fScoringVol_PIX || !fScoringVol_SCINT ) ) {
        const Geometry* geometry
            = static_cast<const Geometry*>
            (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

		fScoringVol_PAD   = geometry->GetScoringVol_PAD();
        fScoringVol_PIX   = geometry->GetScoringVol_PIX();
        fScoringVol_SCINT = geometry->GetScoringVol_SCINT();
    

    }
    else if (!UseFoCalH && (!fScoringVol_PAD || !fScoringVol_PIX) ) {
        const Geometry* geometry
            = static_cast<const Geometry*>
            (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

		fScoringVol_PAD   = geometry->GetScoringVol_PAD();
        fScoringVol_PIX   = geometry->GetScoringVol_PIX();
        fScoringVol_SCINT = geometry->GetScoringVol_SCINT();
    

    }
    // Get PreStepPoint and TouchableHandle objects
    G4StepPoint* preStepPoint=step->GetPreStepPoint();
    G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();
    G4LogicalVolume* volume = theTouchable->GetVolume()->GetLogicalVolume();
    G4String volumeName = theTouchable->GetVolume()->GetName();
    G4int copyNo = theTouchable->GetCopyNumber();

	auto pos = step->GetPreStepPoint()->GetPosition();

    if (volume != fScoringVol_PAD
		&& volume != fScoringVol_PIX
        && volume != fScoringVol_SCINT) return;

    //==========================================================================
    G4ThreeVector position_World = preStepPoint->GetPosition();
    G4ThreeVector position_Local = theTouchable->GetHistory()->GetTopTransform().TransformPoint(position_World);
	G4double edepStep = step->GetTotalEnergyDeposit();
    G4double eionStep = step->GetTotalEnergyDeposit() - step->GetNonIonizingEnergyDeposit();
    G4double lightYield = 0;
    //==========================================================================

	//auto analysisManager=G4AnalysisManager::Instance();
	if (edepStep > 0){
	    if ( volume == fScoringVol_PAD ) {
	        fEventAction->AddeDepPAD(copyNo-IDnumber_PAD_First, edepStep);
			//G4cout << copyNo-IDnumber_PAD_First << " " << pos.x() << "  " << pos.y() << "  " << edepStep << G4endl;
	    } else if ( volume == fScoringVol_PIX ) {
            G4ThreeVector local_pos = theTouchable->GetHistory()->GetTopTransform().TransformPoint(pos);
			G4int ix = (local_pos.x() + PIX_Alpide_X/2.)/(PIX_Alpide_X/NpixX);
			G4int iy = (local_pos.y() + PIX_Alpide_Y/2.)/(PIX_Alpide_Y/NpixY);
            G4int ialpide = copyNo-IDnumber_PIX_First;
            G4int id = ix + NpixX*iy;
            //G4cout << ialpide << "  (ix,iy)=(" << ix << "," << iy << ")  id=" << id << "  edep=" << edepStep << G4endl;
	        fEventAction->AddeDepPIX(ialpide, id, edepStep);
        } else if ( volume == fScoringVol_SCINT ) {
            copyNo -= IDnumber_SCINT_First;
            //G4ThreeVector local_pos = theTouchable->GetHistory()->GetTopTransform().TransformPoint(pos);
			//G4int ix = (local_pos.x() + Tower_dx/2.)/(Tower_dx/NtowerX);
			//G4int iy = (local_pos.y() + Tower_dy/2.)/(Tower_dy/NtowerY);
            G4int iModule = copyNo / (NtowerX*NtowerY) ;
            copyNo -= iModule*(NtowerX*NtowerY) ;
            G4int iTower = copyNo;
            G4int iCapX = copyNo/NtowerX;
            copyNo -= iCapX*NtowerX;
            G4int iCapY = NtowerY;
            if (light_scint_model) {
                G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
                lightYield = emSaturation->VisibleEnergyDepositionAtAStep(step);
            } else {
                lightYield = eionStep;
            }
            //G4cout << "lightYield : " << lightYield << "  pos : " << position_World << G4endl;
            fEventAction->AddeDepSCINT(iModule, iTower, lightYield); // fEventAction->AddeDepSCINT(itower, lightYield);

        }
	}

}
