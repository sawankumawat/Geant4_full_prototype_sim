
#include "RunAction.hh"
#include "Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "constants.hh"
//#include "G4Version.hh"


#ifndef G4VERSION_NUMBER
    #include "G4AnalysisManager.hh"
#endif

#include <stdio.h>
#include "RootIO.hh"

RunAction::RunAction()
	: G4UserRunAction()
{
	// Create analysis manager

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	G4cout << "Using " << analysisManager->GetType() << G4endl;

	analysisManager->SetVerboseLevel(1);
	//  analysisManager->SetFirstHistoId(1);        // Histo Id starts from 1 not from0
	// Creating histograms
	//-----------------------------------------------------------------
	//char Hist_Name[525];
	//char file_Name[4000];

	analysisManager->CreateNtuple("Pixel","file_Name");
	//-----------------------------------------------------------------

	analysisManager->CreateH1("pixelsdata","pixelsdata",53954,0,53954);

	//for(G4int i=1; i<=3840; i++){
	//	std::sprintf(file_Name,"data%d",i);
	//	analysisManager->CreateNtupleDColumn(file_Name);
	//
	//}
	//////////////////////////////////////////////
    /*analysisManager->CreateNtupleDColumn("px");
    analysisManager->CreateNtupleDColumn("py");
    analysisManager->CreateNtupleDColumn("pz");
    analysisManager->CreateNtupleDColumn("photon");//not used
	analysisManager->CreateNtupleDColumn("Scinti1");
	analysisManager->CreateNtupleDColumn("Scinti2");*/
	analysisManager->FinishNtuple();

}

RunAction::~RunAction()
{
	delete G4AnalysisManager::Instance();
}

void RunAction::BeginOfRunAction(const G4Run* /*run*/)

{
	// Get analysis manager and open an output file
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	G4String fileName = "diagnostic.root";

	analysisManager->OpenFile(fileName);
}

void RunAction::EndOfRunAction(const G4Run* /*run*/)

{
	//save histograms & ntuple
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->Write();
	analysisManager->CloseFile();

}
