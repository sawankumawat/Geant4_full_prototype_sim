#include "TTree.h"
#include "TFile.h"
#include "TInterpreter.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TChain.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TMath.h"
#include "TLatex.h"


#include "PathFinder.cpp"

#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#define GAMMA

const int NModX = 3;
const int NModY = 3;

const int NTowerX = 24;
const int NTowerY = 28;


std::ofstream layermax_txt;

Float_t paddata[1296];


std::vector<std::pair<int,float>> *tower[NModX*NModY];

std::string simudir = "/home/mxr/data/focal/TBSimulation_20221207/";
int energy = 500;
float energy_c = 0.61 / 75.24;
std::string particle = "e-";
float threshold = 0;

TFile *fileOut ;

std::pair<float, float> padSigmaOverE;
TGraphErrors *gResolution;
TGraphErrors *gLinearity;

void ReadPadData()
{


    gInterpreter->GenerateDictionary("vector<pair<int,float>>", "vector");

    std::vector<std::string> filenames ;
    std::string basedir =  Form("%s/%s/%dGeV/", simudir.c_str(), particle.c_str(),  energy );
    PathFinder(basedir, "Merged", ".root", filenames);

    TChain *chain = new TChain("DataTree");
    for(const std::string filename : filenames){
	std::cout << filename << std::endl;
	chain->Add(filename.c_str());
    }

    //TFile *fin = TFile::Open(filenames.c_str(), "READ");
    //if (!fin) return;

    //TTree *tree = (TTree*)fin->Get("DataTree");

    //for (int i=0; i<NModX; i++) {
    //    for (int j=0; j<NModY; j++) {
          //chain->SetBranchAddress(Form("HCAL_ModuleX%d_ModuleY%d", i, j), &tower[i*NModX + j]);
    //    }
    //}

    chain->SetBranchAddress("data_pad", paddata);

    std::cout << chain->GetEntries() << std::endl;

    fileOut->cd();


    std::vector<TH1D*> hPadLayers;
    for(unsigned int l= 1;l<=20;l++){
        if(l==5) continue;
        if(l==10) continue;
        const char* _hname = Form("layer%d", l);
        hPadLayers.push_back(new TH1D(_hname, _hname, 1000, 0, 1000));
    }

    const char* cshowerprofilename = Form("cLayer_%s_%dGeV", particle.c_str(), energy);
    const char* cchargecountname = Form("cChargeCount_%s_%dGeV", particle.c_str(), energy);

    TCanvas *cChargeCount = new TCanvas(cchargecountname, cchargecountname, 500, 500);
    TH1D *hChargeCount = new TH1D(Form("hChargeCount_%dGeV", energy), Form("hChargeCount_%dGeV", energy), 15000 , 0, 7500);
    
    // user for muons
    //TH1D *hChargeCount = new TH1D(Form("hChargeCount_%dGeV", energy), Form("hChargeCount_%dGeV", energy), 100 , 0, 20);
    cChargeCount->cd();
    for (int i=0; i<chain->GetEntries(); i++) {

        if((i%100)==0) std::cout << i << " / " << chain->GetEntries() << std::endl;
        //std::cout << "\nEvent " << i+1 << std::endl;
        chain->GetEntry(i);
        int lp = 0;
        Float_t qdep_total = 0;
	
	    bool selected=true;;
        for(unsigned int l= 0;l<18;l++){
                
            Float_t qdep = 0;
            for(int p=0;p<72;p++){
                qdep += paddata[lp];
                lp++;
            }

                //std::cout <<qdep << std::endl;
            //if(l==0&&qdep<threshold){
            //	    selected = false;
            //	    break;
            //}
                
            //std::cout << qdep << std::endl;
            qdep_total += qdep;
            hPadLayers.at(l)->Fill(qdep);
        }
	
	//if(!selected) break;

        hChargeCount->Fill(qdep_total);
        
    }
    hChargeCount->Draw();

    //TGraphErrors *gShowerProfile = new TGraphErrors();
    TCanvas *cShowerProfile = new TCanvas(cshowerprofilename, cshowerprofilename, 1000, 800);
    cShowerProfile->Divide(5,4);
    TH1D *hShowerProfile = new TH1D(Form("hShowerProfile_%dGeV", energy), Form("hShowerProfile_%dGeV", energy), 20, 0.5, 20.5);
    hShowerProfile->GetXaxis()->SetRangeUser(0,20);

    float layermax = 0;

    for(unsigned int l=1,h=0;l<=20;l++){
        if(l==5) continue;
        if(l==10) continue;
        cShowerProfile->cd(l);
        gPad->SetMargin(0.01,0.01,0.1,0.01);
        hPadLayers.at(h)->SetLineColor(kGray+3);
        hPadLayers.at(h)->SetTitleSize(0.07);

        hPadLayers.at(h)->Draw();

        hShowerProfile->SetBinContent(l, hPadLayers.at(h)->GetMean());
        hShowerProfile->SetBinError(l, hPadLayers.at(h)->GetRMS());

        float meandepenergy = hPadLayers.at(h)->GetMean();
        float rmsdepenergy = hPadLayers.at(h)->GetRMS();

        TLatex latex;
        latex.SetNDC();
        latex.SetTextColor(kRed);
        latex.SetTextSize(0.06);
        latex.DrawLatex(0.05,0.87,Form("%.2f #pm %.2f MeV", meandepenergy, rmsdepenergy));

        h++;

        if(layermax<meandepenergy) {
            layermax = meandepenergy;
        }


    }

    std::cout << energy <<" GeV\t" << layermax << " MeV"<<std::endl;
    layermax_txt << energy <<" GeV\t" << layermax << " MeV"<<std::endl;


    cShowerProfile->cd(5);
    TLatex latexE;
    latexE.SetNDC();
    latexE.SetTextColor(kBlack);
    latexE.SetTextSize(0.1);
    latexE.DrawLatex(0.1,0.5,Form("%s @ %d GeV", particle.c_str(), energy));


    cShowerProfile->SaveAs(Form("%s.png", cShowerProfile->GetName()));
    cChargeCount->SaveAs(Form("%s.png", cChargeCount->GetName()));


    TCanvas *cShowerProfiles = new TCanvas("cShowerProfile", "cShowerProfile", 1024, 512);
    cShowerProfiles->Divide(2,1);
    cShowerProfiles->cd(1);
    hShowerProfile->SetMarkerStyle(kFullSquare);
    hShowerProfile->Draw("P");
    hShowerProfile->GetYaxis()->SetRangeUser(0,hShowerProfile->GetBinContent(hShowerProfile->GetMaximumBin())*1.33);
    hShowerProfile->GetXaxis()->SetTitle("FoCal-E Layer");



#ifdef GAMMA

    float b = 0.5;
    float a = (TMath::Log(energy/energy_c)-0.5)*b +1 ;

    TF1 *fGammaDist = new TF1("fGammaDist", "[0]*TMath::GammaDist(x/0.97,[1],[2],[3])", 0.1, 20);
    fGammaDist->SetParameters(1,a,0,1./b);
    fGammaDist->FixParameter(2,0);

    std::cout << a << std::endl;

    fGammaDist->SetLineColor(kBlue);
    fGammaDist->SetLineStyle(kSolid);
    hShowerProfile->Fit(fGammaDist, "R");

    double a_fitted = fGammaDist->GetParameter(1);
    double b_fitted = 1./fGammaDist->GetParameter(3);
    double tmax_fitted = (a_fitted-1)/b;
    Double_t scale = fGammaDist->GetParameter(0);
    std::cout << "tmax_fitted = " << tmax_fitted << std::endl;
    fGammaDist->SetParameters(scale,a,0,1./b);

    fGammaDist->SetLineColor(kBlue);
    fGammaDist->SetLineStyle(kDashed);
    fGammaDist->DrawCopy("sames");
    //chain->Print();

    TLatex latex_tmax;
    latex_tmax.SetNDC();
    latex_tmax.SetTextColor(kBlue);
    latex_tmax.SetTextSize(0.04);
    latex_tmax.DrawLatex(0.15,0.85,Form("t_{max, fitted} = ln(E_{0} / E_{c}) - 0.5 = %.1lf", TMath::Log(energy/energy_c)-0.5));    

#endif

    padSigmaOverE = {};

    cShowerProfiles->cd(2);
    hChargeCount->Draw();
    TLatex latex;
    latex.SetNDC();
    latex.SetTextColor(kBlue);
    latex.SetTextSize(0.04);
    latex.DrawLatex(0.15,0.85,Form("E_{dep, pads} = %.1f #pm %.1f MeV", hChargeCount->GetMean(), hChargeCount->GetRMS()));    

    cShowerProfile->SaveAs("PadSimulation_20_to_500GeV.pdf");
    cShowerProfiles->SaveAs("PadSimulation_20_to_500GeV.pdf");


    cShowerProfile->Close();
    cShowerProfiles->Close();
    cChargeCount->Close();

    hChargeCount->Write();

    //padSigmaOverE = {hChargeCount->GetMean(), hChargeCount->GetRMS()};

    double mean = hChargeCount->GetMean();
    double meanerror = hChargeCount->GetMeanError();
    double sigma = hChargeCount->GetRMS();
    double sigmaerror = hChargeCount->GetRMSError();

    gLinearity->SetPoint(gLinearity->GetN(), energy, mean);
    gLinearity->SetPointError(gLinearity->GetN()-1, 0, meanerror);
    

    gResolution->SetPoint(gResolution->GetN(), energy, sigma/mean);
    gResolution->SetPointError(gResolution->GetN()-1, 0, TMath::Sqrt(meanerror/mean*meanerror/mean+sigmaerror/sigma*sigmaerror/sigma)*sigma/mean);



}

void ReadAllPadData(){

    fileOut = new TFile(Form("%s_out.root", particle.c_str()), "READ");

    gResolution = new TGraphErrors();
    gLinearity = new TGraphErrors;

    TCanvas *c0 = new TCanvas();

    c0->SaveAs("PadSimulation_20_to_500GeV.pdf[");

    layermax_txt.open("layermax.txt");

    threshold = 0.25;
    energy = 20;
    ReadPadData();


    energy = 40;
    ReadPadData();

    energy = 60;
    ReadPadData();

    energy = 80;
    ReadPadData();

    energy = 100;
    ReadPadData();

    //energy = 120;
    //ReadPadData();

    energy = 150;
    ReadPadData();

    energy = 200;
    ReadPadData();

    energy = 250;
    ReadPadData();

    energy = 300;
    ReadPadData();

    //energy = 350;
    //ReadPadData();

    //energy = 500;
    //ReadPadData();

    c0->SaveAs("PadSimulation_20_to_500GeV.pdf]");
    
    c0->Close();

    layermax_txt.close();


    TCanvas *cPerformance = new TCanvas("cPerformance", "cPerformance", 1024*1.1, 512);
    cPerformance->Divide(2,1);
    cPerformance->cd(1);
    gLinearity->Draw("AL*");
    gLinearity->Fit("pol1");
    gLinearity->GetXaxis()->SetRangeUser(0,600);   
    gLinearity->GetHistogram()->SetMaximum(5000);
    gLinearity->GetHistogram()->SetMinimum(0);

    gPad->Modified();
    gPad->Update();
    cPerformance->cd(2);
    gResolution->Draw("AL*");


    //fileOut->Close();

}
