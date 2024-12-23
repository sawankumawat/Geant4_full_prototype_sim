#include "TTree.h"
#include "TFile.h"
#include "TInterpreter.h"
#include "TH1D.h"
#include "TCanvas.h"

#include <vector>
#include <iostream>

const int NModX = 3;
const int NModY = 3;

const int NTowerX = 24;
const int NTowerY = 28;


float prom =0.;
int towch[9][672];
int countercentral = 0;


float FiberCharges[NTowerX*NModX][NTowerY*NModY];
float FiberCentral[24][28];

float value[6048];
float valuecentral[672];
float promcentral = 0.;

std::vector<std::pair<int,float>> *tower[NModX*NModY];


void clearEvent(){
  for (int i = 0;i<NTowerX*NModX;i++){
    for (int j = 0;j<NTowerY*NModY;j++) {
      FiberCharges[i][j] = 0;
    }
  }

  for (int i = 0; i<6048; i++)
    { value [i] = 0;}

}


void ReadHCALData()
{
    gInterpreter->GenerateDictionary("vector<pair<int,float>>", "vector");

    TFile *fin = TFile::Open("150GeV-pi+25k.root", "READ");
    if (!fin) return;

    TTree *tree = (TTree*)fin->Get("DataTree");

    TFile *fout = TFile::Open("Output-350GeV-pi+20k.root", "RECREATE");
    fout->cd();
    
    for (int i=0; i<NModX; i++) {
        for (int j=0; j<NModY; j++) {
          tree->SetBranchAddress(Form("HCAL_ModuleX%d_ModuleY%d", i, j), &tower[i*NModX + j]);
        }
    }

    TCanvas *c = new TCanvas();
    TH1D *hCharge = new TH1D("hCharge", "hCharge", 800,0,10000);
    TH1F *fibCharge = new TH1F ("fibCharge", "Fiber Charge", 100, 0.0, 10);
    TH2F *fiberChargeMap = new TH2F ("fiberChargeMap", "Cumulative charge", NModX*NTowerX, 0, NModX*NTowerX, NModY*NTowerY, 0, NModY*NTowerY);
    TH2F *fiberChargeCenter = new TH2F ("fiberChargeCenter", "Fiber Charge Map in the central module", 24, 0, 24, 28, 0, 28);
    TH1D *fibCentralCharge = new TH1D ("fibCentralCharge", "Total charge in the central module", 800, 0, 10000);


  
    int counter = 0;

    
    for (int i=0; i<tree->GetEntries(); i++) {
       std::cout << "\nEvent " << i+1 << std::endl;
       clearEvent();
        tree->GetEntry(i);

        float charge = 0;
	float centraltotalcharge = 0.;
	for (int i=0; i<NModX*NModY; i++) {
	  // std::cout << "alpide " << i << "  size : " << tower[i]->size() << std::endl;
	  for (int j=0; j<tower[i]->size(); j++) {
	    std::pair<int,float> data = tower[i]->at(j);
	    // std::cout << "\tid : " << data.first << "   value : " <<data.second << std::endl;
	    charge+=data.second;
	    if (i==4){
	      centraltotalcharge += data.second;
	     
	      int ifibrecentral = data.first; //assign fiber index 
	      float fibchcentral = data.second; //take fiber charge value 
	      valuecentral[ifibrecentral] += fibchcentral; // assign the fiber charge value with it's index  
	      
	    }
	    int ifibre = data.first+(i*671); //fiber index 
	    int fibretow = data.first;     //fiber index in given tower 
	    float fibCh  = data.second;   // fiber charge 
	    value[ifibre] += fibCh;
	    fibCharge->Fill(fibCh);
	    towch[i][fibretow] = fibCh;

	  }

        }

	for (int i = 0; i< 24; i++){
	  for (int j = 0; j < 28; j++) {

	    countercentral = j + 28*i;
	    promcentral = valuecentral[countercentral];
	    FiberCentral[i][j] = promcentral;
	    fiberChargeCenter->Fill(i,j,FiberCentral[i][j]);
	    
	    
	  }
	}

	//   for (int i = 0; i<6048; i++) {
	     //    std::cout << i << " " << value [i] << " " << std::endl; 
	//   }


	   
	   int x = 72;  //row
	   int y = 84;  //coloumn
	   for (int i=0; i<x; i++){
	     for (int j=0; j<y; j++) {
	       
	        counter = j + NTowerX*NModY*i;
	        prom = value[counter];
		FiberCharges[i][j] = prom;
	   
	       //	FiberCharges[i][j] += data.second;
	       fiberChargeMap->Fill(i, j, FiberCharges[i][j] );
	     }
	   }
	   

	   
	   //   std::cout << i << "\t" << charge << std::endl;
	   fibCentralCharge->Fill(centraltotalcharge);
	   hCharge->Fill(charge);
    }
    hCharge->Draw();
    hCharge->SetTitle("pi+ 350GeV");
    hCharge->GetXaxis()->SetTitle("Light yield in all fibers");
    c->SaveAs("pi_350GeV.png");

    fout->Write();
    fout->Close();
    
}
