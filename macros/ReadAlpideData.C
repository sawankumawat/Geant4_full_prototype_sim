#include <vector>
#include <iostream>

#include "TInterpreter.h"
#include "TChain.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TFile.h"

const int npix = 2;
const int nalpide = 18;

std::vector<std::pair<int,float>> *data_pix0[nalpide];
std::vector<std::pair<int,float>> *data_pix1[nalpide];

int NMax = 3072;
int NBins = 3072/8;
int NMin = 0;

std::pair<short, short> PixelSimulationLocal2Global(int chipid, int localcoords){

        int offsetY = 0;
        unsigned short globalY = 0;
        unsigned short globalX = 0;

        if(chipid>=9) {
            offsetY=512;
            globalY=((chipid-9)/3)*1024+localcoords/1024+offsetY;
            //if( (globalY % 512) <=111) continue;
        } else {
            globalY=(chipid/3)*1024+localcoords/1024;
        }
        globalX=(chipid%3)*1024+localcoords%1024;
        
        return {globalX, globalY};

}


void ReadAlpideData()
{
    gInterpreter->GenerateDictionary("vector<pair<int,float>>", "vector");

    //TFile *fin = TFile::Open("/home/mxr/data/focal/TBsimulation/mu+/100GeV/hits_pi+_100GeV_2022_10_23_23-18-07.root", "READ");
    //if (!fin) return;

    //TTree *tree = (TTree*)fin->Get("DataTree");

    TChain *tree = new TChain("DataTree");
    //tree->Add("/home/mxr/data/focal/TBsimulation/proton/300GeV/hits_proton_300GeV_2022_10_23_23-18-07.root");
    
    tree->Add("~/data/focal/TBSimulation_20221207/e-/20GeV/Merged.root");
    //tree->Add("/home/mxr/data/focal/TBsimulation/pi+/100GeV/hits_pi+_100GeV_2022_10_23_23-17-43.root");

    tree->Print();
    int n5 = 0;
    int n10=0;
    int q5 = 0;
    int q10=0;

    for(unsigned int i=0;i<nalpide;i++){
        tree->SetBranchAddress(Form("data_pix%d_alpide%d", 0, i), &data_pix0[i]);
        tree->SetBranchAddress(Form("data_pix%d_alpide%d", 1, i), &data_pix1[i]);
    }

    //TH2D *hitmapLayer5 = new TH2D("hitmapLayer5", "hitmapLayer5", NBins, NMin, NMax, NBins, NMin, NMax );
    //TH2D *hitmapLayer10 = new TH2D("hitmapLayer10", "hitmapLayer10", NBins, NMin, NMax, NBins, NMin, NMax );
    TH2D *hitmapLayer5 = new TH2D("hitmapLayer5", "hitmapLayer5", 18, 0, 18, 512*1024, 0, 512*1024 );
    TH2D *hitmapLayer10 = new TH2D("hitmapLayer10", "hitmapLayer10", 18, 0, 18, 512*1024, 0, 512*1024 );

    
    TH1D *hQ5 = new TH1D("hQ5", "hQ5", 1000, 0, 1);
    TH1D *hQ10 = new TH1D("hQ10", "hQ10", 1000, 0, 1);
    TH1D *hN5 = new TH1D("hN5", "hN5", 800, 0, 4000);
    TH1D *hN10 = new TH1D("hN10", "hN10", 800, 0, 4000);

    TCanvas *cQ = new TCanvas("cQ", "cQ", 1024*1.1, 512);
    cQ->Divide(2,1);
    TCanvas *cN = new TCanvas("cN", "cN", 1024*1.1, 512);
    cN->Divide(2,1);

    TCanvas *cH = new TCanvas("cH", "cH", 1024*1.1, 512);
    cH->Divide(2,1);



    for (int i=0; i<tree->GetEntries(); i++) {
        if(!(i%100) ) std::cout << "\nEvent " << i << std::endl;
        tree->GetEntry(i);

	    n5=0;
	    n10=0;

        for (int i=0; i<nalpide; i++) {
	    //n5+=data_pix[i]->size();
            //cout << "alpide " << i << "  size : " << data_pix[i]->size() << endl;
	        int lastfilled = -42;
            for (int j=0; j<data_pix0[i]->size(); j++) {
                
                //calculate the x and the y position
                //std::cout << i << "\t" <<data_pix0[i]->at(j).first<<"\t"  << data_pix0[i]->at(j).first % 1024 << "\t" << data_pix0[i]->at(j).first / 1024 << std::endl;
                auto globalCoords = PixelSimulationLocal2Global(i, data_pix0[i]->at(j).first);
                //std::cout << globalCoords.first << "\t" << globalCoords.second << std::endl;
                //hitmapLayer5->Fill(globalCoords.first, globalCoords.second);
                hitmapLayer5->Fill(i, data_pix0[i]->at(j).first);

            }

            for (int j=0; j<data_pix1[i]->size(); j++) {
                
                //calculate the x and the y position
                //std::cout << data_pix1[i]->at(j).first % 1024 << "\t" << data_pix1[i]->at(j).first / 1024 << std::endl;
                auto globalCoords = PixelSimulationLocal2Global(i, data_pix1[i]->at(j).first);
                //std::cout << globalCoords.first << "\t" << globalCoords.second << std::endl;
                //hitmapLayer10->Fill(globalCoords.first, globalCoords.second);
                hitmapLayer10->Fill(i, data_pix1[i]->at(j).first);

            }

            //std::cout << std::endl;
        }
	hN5->Fill(n5);
	hN10->Fill(n10);
    }

    cN->cd(1);
    hN5->Draw();
    gPad->SetLogy();
    gPad->Modified();
    gPad->Update();

    cN->cd(2);
    hN10->Draw();
    gPad->SetLogy();
    gPad->Modified();
    gPad->Update();

    cQ->cd(1);
    hQ5->Draw();
    gPad->SetLogy();
    gPad->Modified();
    gPad->Update();

    cQ->cd(2);
    hQ10->Draw();
    gPad->SetLogy();
    gPad->Modified();
    gPad->Update();

    cH->cd(1);
    hitmapLayer5->Draw("COLZ");
    gPad->Modified();
    gPad->Update();

    cH->cd(2);
    hitmapLayer10->Draw("COLZ");
    gPad->Modified();
    gPad->Update();

    //TFile *out = new TFile("../../focal_testbeamanalysis/test/simu_e+_100GeV.root", "RECREATE");
    //out->cd();
    //hN5->Write();
    //hN10->Write();

    //out->Close();
}
