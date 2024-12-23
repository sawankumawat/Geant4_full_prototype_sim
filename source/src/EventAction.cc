
#include "EventAction.hh"
#include "Analysis.hh"
#include "constants.hh"
#include "RootIO.hh"
#include "Randomize.hh"
#include <iomanip>
//#include "G4Version.hh"


#ifndef G4VERSION_NUMBER
    #include "G4AnalysisManager.hh"
#endif

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

EventAction::EventAction()
    : G4UserEventAction()
{}


EventAction::~EventAction()
{
    RootIO::GetInstance()->Close();
}

G4int eventNum=0;
void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
    eventNum++;
    G4cout << "Event = " << eventNum <<G4endl;

	//Zero out all the deposit in the beginning of the event
    for (G4int i = 0; i < NpadX*NpadY*NumberPAD; i++){
        sum_eDep_PAD[i] = 0;
    }

    for (G4int i = 0; i < NalpideLayer*NumberPixRow*NumberPixCol*NumberPIX; i++){
        for (G4int j = 0; j < NpixX*NpixY; j++){
    		sum_eDep_PIX[i][j] = 0;
    	}
    }

    for (G4int i = 0; i < N_HCAL_Module_X*N_HCAL_Module_Y; i++){  // for (G4int i = 0; i < NtowerX*NtowerY; i++){ sum_eDep_SCINT[i] = 0; }
        for (G4int j = 0; j < NtowerX*NtowerY; j++){
    		sum_eDep_SCINT[i][j] = 0;
    	}
    }

	RootIO::GetInstance()->Clear();
}

void EventAction::EndOfEventAction(const G4Event* event)
{
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	RootIO::GetInstance()->WriteEvent(eventNum);

    for (G4int ialpide = 0; ialpide < NalpideLayer*NumberPixCol*NumberPixRow*NumberPIX; ialpide++) {
		for (int iy = 0; iy < NpixY; iy++) {
			for (int ix = 0; ix < NpixX; ix++) {
				G4int i = ix + NpixX*iy;// + NpixX*NpixY*ilayer;
				analysisManager->FillH1(0, i, GetSumPIX(ialpide, i));
				if (GetSumPIX(ialpide, i)>0) {
					//G4cout << i << "  " << ialpide << "  " << ix << "  " << iy << "   " << GetSumPIX(ialpide, i) << G4endl;
			        RootIO::GetInstance()->WriteAlpide(ialpide, i, GetSumPIX(ialpide, i));
				}
			}
		}
	}
	for (G4int ilayer = 0; ilayer < NumberPAD; ilayer++){
		for (int iy = 0; iy < NpadY; iy++){
			for (int ix = 0; ix < NpadX; ix++){
				G4int i = ix + NpadX*iy + NpadX*NpadY*ilayer;
                if (GetSumPAD(i)) {
					//G4cout << i << "  " << ilayer << "  " << ix << "  " << iy << "   " << GetSumPAD(i) << G4endl;
			        RootIO::GetInstance()->WritePad(i, GetSumPAD(i));
				}
			}
		}
    }

    // Added: for (G4int iModule = 0; iModule < N_HCAL_Module_X*N_HCAL_Module_Y; iModule++) { }
    int nHCalTowersHit = 0;
    for (G4int iModule = 0; iModule < N_HCAL_Module_X*N_HCAL_Module_Y; iModule++) { 
        for (G4int ix = 0; ix < NtowerX; ix++) {
            for (G4int iy = 0; iy < NtowerY; iy++) {
                G4int itower = ix + NtowerX*iy; // + NtowerY*NtowerX*iModule; 
                G4double sumscint = GetSumSCINT(iModule, itower);
                analysisManager->FillH1(0, itower, sumscint);
                //G4cout <<"joke? " << iModule << " " << itower << "  " << ix << "  " << iy << "   " << sumscint << G4endl;
                if (sumscint > 0) {
                    //G4cout <<"joke? " << iModule << " " << itower << "  " << ix << "  " << iy << "   " << sumscint << G4endl;
                    RootIO::GetInstance()->WriteScint(iModule, itower, (float) sumscint); // RootIO::GetInstance()->WriteScint(i, GetSumSCINT(i));
                    nHCalTowersHit++;
                }
            }
        }
    }
    G4cout << "NHCalTowersHits: "<< nHCalTowersHit<< G4endl;

    G4PrimaryVertex* primaryVertex = event->GetPrimaryVertex();
	G4cout << primaryVertex->GetX0() << " " << primaryVertex->GetY0() << "  " << primaryVertex->GetZ0() << G4endl;
    G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
    G4double ke = primaryParticle->GetKineticEnergy();
	G4double x = primaryVertex->GetX0();
	G4double y = primaryVertex->GetY0();
	G4double z = primaryVertex->GetZ0();
    G4double px = primaryParticle->GetPx();
    G4double py = primaryParticle->GetPy();
    G4double pz = primaryParticle->GetPz();
    G4double en = primaryParticle->GetTotalEnergy();
	RootIO::GetInstance()->WriteParticleKinematics(px, py, pz, en);
	RootIO::GetInstance()->WriteVertex(x, y, z);
    RootIO::GetInstance()->Fill();
    //*****************************************
}
