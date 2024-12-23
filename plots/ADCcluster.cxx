#include <iostream>
#include <fstream>
#include <vector>
#include "style.h"

using namespace std;

float energy[9][8];
int status[9][8];
float energy1[9][8];
int status1[9][8];
int rcCluster[9][8];
int maxr, maxc, secondmaxr, secondmaxc;
int ADCcut = 0;
int sigmacut = 3;
string wafercopy;
bool testing = 0;
int padsfired;
int npion = 0;
int maxhit_channel;
int second_maxhit_channel;

TH2F *before_cluster;
TH2F *after_cluster;

void findMaxE(float e[9][8], int padsok);
void findCluster(float e[9][8], int padsok);
void findNN(int rm, int cm, float e[9][8], int padsok);

void fitped(TH1F *hist, int lowrange, int highrange, float &fitmean, float &fitsigma)
{
    TF1 *fit = new TF1("fit", "gaus", lowrange, highrange);
    float mean1 = hist->GetMean();
    float rms1 = hist->GetRMS();
    fit->SetParameter(1, mean1);
    fit->SetParameter(2, rms1);
    hist->Fit("fit", "RQ"); // R : range, 0 : do not show fitting lines, Q : suppress the fit results in terminal.
    fitmean = fit->GetParameter(1);
    fitsigma = fit->GetParameter(2);
    // fitmean =  hist->GetMean();
    // fitsigma = hist->GetRMS();
    // float chindf = fit->GetChisquare() / fit->GetNDF();
    // if (chindf > 1000 || chindf < 0.01)
    // {
    //     fitmean = hist->GetMean();
    // }
}
const int mapping[72] = {32, 34, 35, 27, 63, 71, 70, 69,
                         30, 28, 33, 31, 67, 65, 64, 68,
                         29, 26, 25, 23, 57, 55, 60, 66,
                         20, 24, 21, 19, 61, 59, 62, 54,
                         18, 22, 17, 16, 52, 50, 56, 58,
                         13, 15, 10, 14, 48, 46, 51, 49,
                         12, 9, 11, 0, 36, 45, 47, 53,
                         5, 3, 6, 4, 40, 42, 37, 41,
                         7, 1, 2, 8, 44, 38, 39, 43};

const int mapxy[9][8] = {{32, 34, 35, 27, 63, 71, 70, 69}, {30, 28, 33, 31, 67, 65, 64, 68}, {29, 26, 25, 23, 57, 55, 60, 66}, {20, 24, 21, 19, 61, 59, 62, 54}, {18, 22, 17, 16, 52, 50, 56, 58}, {13, 15, 10, 14, 48, 46, 51, 49}, {12, 9, 11, 0, 36, 45, 47, 53}, {5, 3, 6, 4, 40, 42, 37, 41}, {7, 1, 2, 8, 44, 38, 39, 43}};

void GetXY(int ich, int &chx, int &chy, int map[9][8])
{
    for (int ix = 0; ix < 9; ix++)
    {
        for (int iy = 0; iy < 8; iy++)
        {
            // cout<<map[ix][iy];
            if (mapxy[ix][iy] == ich)
            {
                chx = ix;
                chy = iy;
            }
        }
    }
}

void ADCcluster(int wafer, bool gain, string particle)
{
    wafercopy = wafer;
    if (particle != "electron" && particle != "pion")
    {
        std::cerr << "particle not found " << endl;
        return;
    }
    cout << "Particle used is " << particle << endl;
    int rebin;
    rebin = (particle == "electron") ? 3 : 1;

    before_cluster = new TH2F("", "bc", 9, 0, 9, 8, 0, 8);
    after_cluster = new TH2F("", "ac", 9, 0, 9, 8, 0, 8);

    TFile *file = new TFile("source.root", "read");
    TFile *filebkg = new TFile("bkg.root", "read");
    TFile *file2 = new TFile("analyze.root", "recreate");

    TTree *tree = (TTree *)file->Get("PadData");
    TTree *treebkg = (TTree *)filebkg->Get("PadData");

    Double_t ADC[76] = {0};
    Double_t ADCbkg[76] = {0};
    Double_t ADCraw[72] = {0};
    Double_t ADC1[72] = {0};
    Double_t ADCped_sig[72] = {0};
    Double_t totalIntegral[72] = {0};
    float fitmean[72] = {0};
    float fitsigma[72] = {0};
    Double_t pedmean[72] = {0};
    Double_t pedrms[72] = {0};
    int nchx = 9;
    int nchy = 8;
    int chx, chy;

    fstream filetxt1;
    fstream filetxt2;
    if (wafer == 26)
    {
        filetxt1.open("position_scan_pi/maximum/gain_cali_maxi_wafer_26.txt", ios::in); // changed here
        filetxt2.open("position_scan_pi/gain_callibration_26_offset_v2.txt", ios::in);
    }
    else if (wafer == 10)
    {
        filetxt1.open("position_scan_pi/maximum/gain_cali_maxi_wafer_10.txt", ios::in); // changed here
        filetxt2.open("position_scan_pi/gain_callibration_10_v2.txt", ios::in);
    }
    else
    {
        std::cerr << "wafer not found " << endl;
        return;
    }

    if (!filetxt1 || !filetxt2)
    {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    int chh1, chh2;
    float gainf1, gainf2;
    float gainval[72] = {0};

    if (gain == 1)
    {

        for (int i = 0; i < 25; i++)
        {
            filetxt1 >> gainf1 >> chh1;
            filetxt2 >> gainf2 >> chh2;
            gainval[chh1] = gainf1; // changed here
        }
    }

    // reading the root file
    tree->SetBranchAddress("ADC", ADC);
    treebkg->SetBranchAddress("ADC", ADCbkg);
    int totalentries1 = tree->GetEntriesFast();
    int totalentries_bkg_temp = treebkg->GetEntriesFast();
    int totalentries_bkg = (totalentries_bkg_temp > 150000) ? 150000 : totalentries_bkg;

    int totalentries = totalentries1;
    // int totalentries = 100;
    cout << "\nTotal entries used :  " << totalentries << "\n";

    // Histograms initilization for hitmap, adcsum and pedestal

    TH2F *HhitMap = new TH2F("HhitMap", "HitMap", 8, 0.5, 8.5, 9, 0.5, 9.5);
    HhitMap->GetXaxis()->SetLabelSize(0);    // Remove x-axis range labels
    HhitMap->GetYaxis()->SetLabelSize(0);    // Remove y-axis range labels
    HhitMap->GetZaxis()->SetLabelSize(0.05); // Remove y-axis range labels

    // mapping file for hitmap
    ifstream iMap;
    ifstream iMap2;
    iMap.open("MAP2.txt");
    iMap2.open("MAP2.txt");
    int array1[72];
    int array2[72];

    //+++ fit the 2D histogram for hitmap
    int maxhit = 0;
    int maxch = 0;
    for (int ich = 0; ich < 72; ich++)
    {
        Int_t chan = 0;
        Int_t xpos = 0;
        Int_t ypos = 0;
        Int_t zzz = 0;

        iMap >> chan >> xpos >> ypos >> zzz;
        array1[ich] = xpos;
        array2[ich] = ypos;
    }
    // cout << "The channel with the maximum hits is : Channel " << maxch << "\n\n";

    TH1F *h_ADCraw[72];
    TH1F *h_ADCped_sig[72];
    TH1F *h_ADCped[72];
    TH1F *h_ADCbkg[72];
    TH1F *h_singlepad[72];
    TH1F *h_singlepad_pedsig[72];

    for (int ich = 0; ich < 72; ich++)
    {
        h_ADCraw[ich] = new TH1F(Form("raw%d", ich), Form("adc_raw_%d ", ich), 1050, -26, 1024);
        h_ADCbkg[ich] = new TH1F(Form("bkg%d", ich), Form("bkg_%d ", ich), 1050, -26, 1024);
        h_ADCped_sig[ich] = new TH1F(Form("pedsig%d", ich), Form("adc_ped_sig%d ", ich), 1050, -26, 1024);
        // h_singlepad[ich] = new TH1F(Form("sp%d", ich), Form("ch_%d ", ich), 1050, -26, 1024);
        h_singlepad_pedsig[ich] = new TH1F(Form("sps%d", ich), Form("ch%d ", ich), 1050, -26, 1024);

        h_ADCraw[ich]->Rebin(rebin);
        h_ADCbkg[ich]->Rebin(rebin);
        // h_ADCped_sig[ich]->Rebin(rebin);
        // h_singlepad[ich]->Rebin(rebin);
        // h_singlepad_pedsig[ich]->Rebin(rebin);
    }
    TH1F *h_ADCsum = new TH1F("h_ADCsum", "ADC sum", 1010, -100, 10000);
    TH1F *h_PadsFired = new TH1F("pads", "pads fired", 72, 0, 72);
    TH1F *h_pad_distribution = new TH1F("", "", 72, 0, 72);
    TH1F *transverse = new TH1F("transverse", "", 60, 0, 12);
    TH1F *h_2pad_1 = new TH1F("", "", 100, 0, 100);
    TH1F *h_3pad_1 = new TH1F("", "", 100, 0, 100);
    TH1F *h_3pad_2 = new TH1F("", "", 100, 0, 100);
    TH1F *h_3pad_3 = new TH1F("", "", 100, 0, 100);

    for (int i = 0; i < totalentries_bkg; i++)
    {

        treebkg->GetEntry(i + 1);
        for (int iv = 0; iv < 72; iv++)
        {
            ADC1[iv] = (double)ADCbkg[iv];
            h_ADCbkg[iv]->Fill(ADC1[iv]);
        }
    }

    for (int i = 0; i < 72; i++)
    {
        pedmean[i] = h_ADCbkg[i]->GetMean();
        pedrms[i] = h_ADCbkg[i]->GetRMS();
        fitped(h_ADCbkg[i], pedmean[i] - 50, pedmean[i] + 50, fitmean[i], fitsigma[i]);
    }

    int ixx = 0;
    int iyy = 0;

    for (int ientries = 0; ientries < totalentries; ientries++)
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                energy[i][j] = 0;
                energy1[i][j] = 0;
                status[i][j] = 0;
                status1[i][j] = 0;
            }
        }
        if (ientries % 5000 == 0)
            cout << "Event " << ientries << "/" << totalentries << endl;
        tree->GetEntry(ientries + 1);

        // if (testing == 1) cout<<"entry "<<i<<"************before clustering*************"<<endl;
        int posch = 0;
        double energysum_temp = 0;

        for (int ich = 0; ich < 72; ich++)
        {
            ADCraw[ich] = (double)ADC[ich] + gainval[ich];
            ADCped_sig[ich] = ADCraw[ich] - fitmean[ich] - sigmacut * fitsigma[ich];
            h_ADCraw[ich]->Fill(ADCraw[ich]);

            if (ADCped_sig[ich] > 0)
            {
                h_ADCped_sig[ich]->Fill(ADCped_sig[ich]);
                // HhitMap->Fill(array1[ich] + 8, array2[ich] + 9, 1); //hitmap filled before clustering (for testing)
                energysum_temp += ADCped_sig[ich];
            }

            for (int ix = 0; ix < 9; ix++)
            {
                for (int iy = 0; iy < 8; iy++)
                {
                    if (mapxy[ix][iy] == ich)
                    {
                        ixx = ix;
                        iyy = iy;
                        break;
                    }
                }
            }
            if (ADCped_sig[ich] > 0)
            {
                energy[ixx][iyy] = ADCped_sig[ich];
                energy1[ixx][iyy] = ADCped_sig[ich];
                status[ixx][iyy] = 1;
                status1[ixx][iyy] = 1;
                posch++;
            }
        }
        // h_ADCsum->Fill(energysum_temp); //for ADC sum before clustering

        findCluster(energy, 0);

        padsfired = 0;
        int padsfired1 = 0;
        double sumofadc = 0;
        double sumofadc1 = 0;

        // cout << "**************after clustering******************* " << endl;

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (rcCluster[i][j] == 1)
                {
                    int chno = mapxy[i][j];
                    HhitMap->Fill(array1[chno] + 8, array2[chno] + 9, 1); // hitmap filled after clustering
                    sumofadc += energy[i][j];
                    padsfired++;
                }
            }
        }
        if (padsfired > 1)
        {
            h_pad_distribution->Fill(second_maxhit_channel);
            // cout << "maxhit channel " << mapxy[maxr][maxc] << " second maxhit channel " << mapxy[secondmaxr][secondmaxc] << endl;
        }
        if (padsfired == 1)
        {
            h_singlepad_pedsig[maxhit_channel]->Fill(energy[maxr][maxc]);
        }

        // // to find the percentage of energy in maxhit, second maxhit and third maxhit channel
        if (padsfired == 2 && particle == "pion")
        {
            h_2pad_1->Fill(energy[maxr][maxc] * 100 / sumofadc);
        }
        // if (padsfired == 3 && particle == "pion")
        // {
        //     h_3pad_1->Fill(energy[maxr][maxc] * 100 / sumofadc);
        //     h_3pad_2->Fill(energy[secondmaxr][secondmaxc] * 100 / sumofadc);
        //     h_3pad_3->Fill((sumofadc - energy[maxr][maxc] - energy[secondmaxr][secondmaxc])*100/sumofadc);
        // }

        // if (padsfired == 1)
        // {
        //     for (int i = 0; i < 9; i++)
        //     {
        //         for (int j = 0; j < 8; j++)
        //         {
        //             if (rcCluster[i][j] == 1)
        //             {
        //                 int chno = mapxy[i][j];
        //                 HhitMap->Fill(array1[chno] + 7, array2[chno] + 8, 1);
        //             }
        //         }
        //     }
        // } // for testing purpose of pion hitmap

        sumofadc1 = sumofadc;
        padsfired1 = padsfired;
        int tempstat = 0;

        if (testing == 1 && posch > 2 && particle == "electron")
        {
            cout << "entry " << ientries << "************before clustering************* posch" << posch << endl;

            for (int ich = 0; ich < 72; ich++)
            {
                if (ADCped_sig[ich] > 0)
                    cout << "channel " << ich << " energy " << ADCped_sig[ich] << endl;
            }
            cout << "Total number of pads in the cluster " << padsfired << endl;
            tempstat = 1;
        }

        if (posch > 2 && particle == "electron")
        {
            padsfired = 0;
            sumofadc = 0;
            for (int ix = 0; ix < 9; ix++)
            {
                for (int iy = 0; iy < 8; iy++)
                {
                    energy[ix][iy] = energy1[ix][iy];
                    status[ix][iy] = status1[ix][iy];
                }
            }
            findCluster(energy, 1);
            for (int i = 0; i < 9; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    if (rcCluster[i][j] == 1)
                    {
                        sumofadc += energy[i][j];
                        padsfired++;
                        float radius = (TMath::Sqrt((maxr - i) * (maxr - i) + (maxc - j) * (maxc - j)));
                        transverse->Fill(radius, energy[i][j] / sumofadc);
                    }
                }
            }

            if (padsfired <= padsfired1)
            {
                sumofadc = sumofadc1;
                padsfired = padsfired1;
            }
        }

        // cout << "pads fired " << padsfired << endl;

        if (padsfired > 0)
        {
            h_PadsFired->Fill(padsfired);
        }
        if (sumofadc > 0)
        {
            h_ADCsum->Fill(sumofadc);
        }

        if (tempstat == 1 && particle == "electron")
        {
            cout << "**************after clustering******************* " << endl;
            cout << "Total number of pads in the cluster " << padsfired << endl;
        }

        if (particle == "pion" && padsfired == 3 && testing == 1)
        {
            npion++;
            cout << "number of pads in the cluster is " << padsfired << endl;
            for (int i = 0; i < 9; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    if (rcCluster[i][j] == 1)
                    {
                        cout << "event " << ientries << " pad number " << mapxy[i][j] << " energy " << energy[i][j] << endl;
                    }
                }
            }
        }
    } // event loop ended here
    // cout << "number of pions with more than 1 pad " << npion << endl;

    TCanvas *ct = new TCanvas("", "", 1200, 1000);
    SetCanvasStyle(ct, 0.15, 0.03, 0.04, 0.15);
    SetHistostyle3(transverse);
    transverse->GetXaxis()->SetTitle("Pad distance (cm)");
    transverse->GetYaxis()->SetTitle("Probability");
    // transverse->Scale(1. / transverse->Integral());
    transverse->Scale(1. / transverse->GetEntries());
    transverse->SetLineWidth(3);
    transverse->SetMarkerStyle(71);
    transverse->SetMarkerSize(2);
    transverse->Draw("elp");
    transverse->Write("tprofile");
    ct->Close();

    int nBinsX = HhitMap->GetNbinsX();
    int nBinsY = HhitMap->GetNbinsY();

    for (int i = 1; i <= nBinsX; i++)
    {
        for (int j = 1; j <= nBinsY; j++)
        {
            if (HhitMap->GetBinContent(i, j) == 0)
            {
                HhitMap->SetBinContent(i, j, 1);
            }
        }
    }
    // gStyle->SetPalette(kRainbow);
    TCanvas *c0 = new TCanvas("", "", 720, 720);
    SetCanvasStyle(c0, 0.09, 0.17, 0.02, 0.1);
    gPad->SetLogz(1);
    SetHistostyle3(HhitMap);
    HhitMap->SetTitle(0);
    HhitMap->SetStats(0);
    HhitMap->GetXaxis()->SetTitle("Pads");
    HhitMap->GetYaxis()->SetTitle("Pads");
    HhitMap->GetZaxis()->SetTitle("Number of hits");
    HhitMap->GetXaxis()->SetTitleOffset(0.9);
    HhitMap->GetYaxis()->SetTitleOffset(0.8);
    HhitMap->GetZaxis()->SetTitleOffset(1.15);
    HhitMap->GetXaxis()->SetTickLength(0);
    HhitMap->GetYaxis()->SetTickLength(0);
    HhitMap->GetXaxis()->SetNdivisions(115);
    HhitMap->GetYaxis()->SetNdivisions(115);
    HhitMap->GetZaxis()->CenterTitle();

    HhitMap->Draw("colz text");
    TLatex *texn[72];
    c0->Close();

    TCanvas *hpadsdist = new TCanvas("", "", 720, 720);
    SetCanvasStyle(hpadsdist, 0.15, 0.03, 0.03, 0.15);
    // gPad->SetLogy(1);
    SetHistoQA(h_pad_distribution);
    // THStack *hs = new THStack("hs", "Stacked 1D histograms");
    h_pad_distribution->GetXaxis()->SetTitle("Pad number");
    h_pad_distribution->GetXaxis()->SetNdivisions(515);
    h_pad_distribution->GetYaxis()->SetTitleOffset(1.5);
    h_pad_distribution->GetYaxis()->SetTitle("Normalized events");
    h_pad_distribution->SetLineWidth(2);
    h_pad_distribution->Scale(1 / h_pad_distribution->Integral());
    h_pad_distribution->Draw("HIST");
    // gStyle->SetOptStat(0);
    hpadsdist->Close();

    TCanvas *energy_distrbutuion = new TCanvas("", "", 1200, 1000);
    SetCanvasStyle(energy_distrbutuion, 0.15, 0.03, 0.03, 0.15);
    SetHistoQA(h_2pad_1);
    h_2pad_1->GetXaxis()->SetTitle("Percentage of energy deposited");
    h_2pad_1->GetYaxis()->SetTitle("Events");
    h_2pad_1->SetLineColor(2);
    h_2pad_1->SetMarkerStyle(53);
    h_2pad_1->SetMarkerSize(2);
    h_2pad_1->SetMarkerColor(2);
    h_2pad_1->SetMaximum(h_2pad_1->GetMaximum() * 1.5);
    h_2pad_1->Draw();

    TLegend *energy_leg = new TLegend(0.65, 0.78, 0.90, 0.9);
    energy_leg->AddEntry(h_2pad_1, "maxhit pad");
    energy_leg->SetFillStyle(0);
    energy_leg->SetBorderSize(0);
    energy_leg->Draw();
    energy_distrbutuion->Close();

    // TCanvas *energy_distrbutuion1 = new TCanvas("", "", 1200, 1000);
    // SetCanvasStyle(energy_distrbutuion, 0.15, 0.03, 0.03, 0.15);
    // SetHistoQA(h_3pad_1);
    // SetHistoQA(h_3pad_2);
    // SetHistoQA(h_3pad_3);
    // h_3pad_1->GetXaxis()->SetTitle("Percentage of energy deposited");
    // h_3pad_1->GetYaxis()->SetTitle("Events");
    // h_3pad_1->SetLineColor(2);
    // h_3pad_2->SetLineColor(4);
    // h_3pad_3->SetLineColor(28);
    // h_3pad_1->SetMarkerStyle(53);
    // h_3pad_2->SetMarkerStyle(55);
    // h_3pad_3->SetMarkerStyle(58);
    // h_3pad_1->SetMarkerSize(2);
    // h_3pad_2->SetMarkerSize(2);
    // h_3pad_3->SetMarkerSize(2);
    // h_3pad_1->SetMarkerColor(2);
    // h_3pad_2->SetMarkerColor(4);
    // h_3pad_3->SetMarkerColor(28);

    // h_3pad_1->SetMaximum(50);
    // h_3pad_1->Draw("p");
    // h_3pad_2->Draw("psame");
    // h_3pad_3->Draw("psame");

    // TLegend *energy_leg1 = new TLegend(0.65, 0.78, 0.90, 0.9);
    // energy_leg1->AddEntry(h_3pad_1, "maxhit");
    // energy_leg1->AddEntry(h_3pad_2, "second hit");
    // energy_leg1->AddEntry(h_3pad_3, "third hit");
    // energy_leg1->SetFillStyle(0);
    // energy_leg1->SetBorderSize(0);
    // energy_leg1->Draw();

    // for (int i = 0; i < 72; i++)
    // {
    //     TH1F *h = new TH1F(Form("h%d", i), "", 72, 0, 72);
    //     h->SetBinContent(i + 1, h_pad_distribution->GetBinContent(i + 1));
    //     h->SetFillColor(i + 11);
    //     hs->Add(h);
    // }
    // hs->Draw("hist");

    // ... fill h_pad_distribution ...

    // Draw the stack of histograms
    // hs->Draw("hist");

    // cout << " Checkpoint 1"<<endl;

    // add channel numbers to the hitmap
    // for (int ich = 0; ich < 72; ich++)
    // {
    //     Int_t chan = 0;
    //     Int_t xpos = 0;
    //     Int_t ypos = 0;
    //     Int_t zzz = 0;
    //     iMap2 >> chan >> xpos >> ypos >> zzz;

    //     texn[ich] = new TLatex(xpos + 7.3, ypos + 8.3, Form("%d", ich));
    //     texn[ich]->SetLineWidth(2);
    // }
    // for (int ich = 0; ich < 72; ich++)
    // {
    //     texn[ich]->Draw();
    // }
    c0->Write("Hitmap");
    // c0->SaveAs("Hitmap.png");s

    // ADC raw mapping plot
    TCanvas *craw = new TCanvas("craw", "raw adc with bkg", 1800, 820);
    craw->Divide(8, 9, 0, 0);
    for (int i = 0; i < 72; i++)
    {
        craw->cd(i + 1);
        gPad->SetLogy(1);
        float mean = h_ADCraw[mapping[i]]->GetMean();
        float mean_bkg = h_ADCbkg[mapping[i]]->GetMean();
        h_ADCraw[mapping[i]]->GetXaxis()->SetRangeUser(mean_bkg - 100, mean + 200);
        h_ADCraw[mapping[i]]->SetLineColor(48);
        h_ADCraw[mapping[i]]->SetFillColor(48);
        h_ADCbkg[mapping[i]]->SetLineColor(4);
        h_ADCbkg[mapping[i]]->SetFillColor(4);
        h_ADCbkg[mapping[i]]->SetFillStyle(3944);
        h_ADCraw[mapping[i]]->SetTitle(0);
        h_ADCraw[mapping[i]]->Draw();
        h_ADCbkg[mapping[i]]->Draw("same");
        TLegend *leg = new TLegend(0.65, 0.3, 0.90, 0.57);
        leg->AddEntry(h_ADCbkg[mapping[i]], "bkg");
        leg->AddEntry(h_ADCraw[mapping[i]], "beam");
        // leg->Draw();  // comment it to hide the legend of bkg and source

        // h_ADCraw[i]->Write(Form("ch%d", i));
        TLatex *lat = new TLatex(0.70, 0.72, Form("ch_%d", mapping[i]));
        lat->SetNDC();
        lat->SetTextSize(0.15);
        lat->Draw(); // comment it to hide channel number from plots
    }
    craw->Write("raw_ADC_map");
    craw->Close();

    // ADC raw plots of all channels
    TCanvas *craw1 = new TCanvas("craw1", "raw adc with bkg", 1800, 820);
    for (int i = 0; i < 72; i++)
    {
        float mean = h_ADCraw[mapping[i]]->GetMean();
        float mean_bkg = h_ADCbkg[mapping[i]]->GetMean();

        h_ADCraw[mapping[i]]->GetXaxis()->SetRangeUser(mean_bkg - 100, mean + 200);
        h_ADCraw[mapping[i]]->SetLineColor(48);
        h_ADCraw[mapping[i]]->SetFillColor(48);
        h_ADCraw[mapping[i]]->GetXaxis()->SetTitle("ADC values");
        h_ADCraw[mapping[i]]->GetYaxis()->SetTitle("Counts");
        h_ADCbkg[mapping[i]]->SetLineColor(4);
        h_ADCbkg[mapping[i]]->SetFillColor(4);
        h_ADCbkg[mapping[i]]->SetFillStyle(3016);
        h_ADCraw[mapping[i]]->Draw();
        h_ADCbkg[mapping[i]]->Draw("same");
        TLegend *leg = new TLegend(0.65, 0.3, 0.90, 0.57);
        leg->AddEntry(h_ADCbkg[mapping[i]], "bkg");
        leg->AddEntry(h_ADCraw[mapping[i]], "beam");
        leg->Draw(); // comment it to hide the legend of bkg and source

        TLatex *lat = new TLatex(0.69, 0.68, Form("ch_%d", mapping[i]));
        lat->SetNDC();
        lat->SetTextSize(0.1);
        lat->Draw(); // comment it to hide channel number from plots
        craw1->Write(Form("ch_%d", mapping[i]));
    }
    craw1->Close();

    TCanvas *cped = new TCanvas(); // comment the below to turn off showing the pedestal and 3 sigma subtracted graphs.
    cped->Divide(8, 9);
    cped->SetTitle("Noise subtracted graphs");

    for (int i = 0; i < 72; i++)
    {
        cped->cd(i + 1);
        float mean4 = h_ADCped_sig[mapping[i]]->GetMean();
        // h_ADCped_sig[mapping[i]]->GetXaxis()->SetRangeUser(0, mean4 + 350);
        h_ADCped_sig[mapping[i]]->GetXaxis()->SetTitle("ADC");
        h_ADCped_sig[mapping[i]]->GetYaxis()->SetTitle("Counts");
        h_ADCped_sig[mapping[i]]->GetXaxis()->SetTitleSize(0.05);
        h_ADCped_sig[mapping[i]]->GetYaxis()->SetTitleSize(0.05);
        h_ADCped_sig[mapping[i]]->Draw();
        int integral = h_ADCped_sig[mapping[i]]->Integral(h_ADCped_sig[i]->GetXaxis()->FindBin(1), h_ADCped_sig[i]->GetNbinsX());
        TLatex *lat = new TLatex(0.65, 0.6, Form("ch_%d", mapping[i]));
        TLatex *lat2 = new TLatex(0.65, 0.45, Form("ev~%d", integral));
        lat->SetNDC();
        lat->SetTextSize(0.15);
        lat2->SetNDC();
        lat2->SetTextSize(0.15);
        lat->Draw();
        lat2->Draw();
        h_ADCped_sig[i]->Write(Form("pad_noise_rm%d", i));
        h_singlepad_pedsig[i]->Write(Form("singlepad_noise_rm%d", i));
    }
    cped->Write("noise_subtracted");
    cped->Close();

    TCanvas *csum = new TCanvas();
    csum->SetGrid(1, 1);
    h_ADCsum->GetXaxis()->SetTitle("ADC");
    h_ADCsum->GetYaxis()->SetTitle("counts");
    h_ADCsum->SetTitle("ADC sum around maxhit channel");
    h_ADCsum->Draw();
    h_ADCsum->Write("adcsum");
    // csum->Close();

    // TCanvas *cevents = new TCanvas("cevents", "Channel vs. ADC - Event Distribution", 900, 10, 800, 500);
    // SetCanvasStyle2(cevents, 0.12, 0.09, 0.06, 0.15); // left, right , top , bottom
    // cevents->SetTitle("Channel vs. ADC - Event Distribution");
    // TH2D *hevents = (TH2D *)file->Get("hADCChannel");
    // gPad->SetLogz(1);
    // gPad->SetGrid(1, 0);
    // hevents->GetXaxis()->SetTitleOffset(1);
    // hevents->GetXaxis()->SetRangeUser(0, 72);
    // hevents->GetYaxis()->SetRangeUser(0, 1024);
    // hevents->GetXaxis()->SetNdivisions(72 / 4, 4);
    // hevents->GetYaxis()->SetNdivisions(1024 / 100, 5);
    // hevents->GetYaxis()->SetTitleOffset(0.9);
    // hevents->GetXaxis()->SetAxisColor(17);
    // hevents->GetYaxis()->SetAxisColor(17);
    // cevents->RedrawAxis();
    // hevents->Draw("colz");
    // cevents->Write("event_distribution");
    // cevents->Close();

    TCanvas *c_fired = new TCanvas("", "", 1200, 1000);
    SetCanvasStyle2(c_fired, 0.15, 0.03, 0.03, 0.15);
    SetHistoQA(h_PadsFired);
    h_PadsFired->GetXaxis()->SetTitle("No. of Pads fired");
    h_PadsFired->GetYaxis()->SetTitleOffset(1.5);
    h_PadsFired->GetYaxis()->SetTitle("Events");
    h_PadsFired->Draw("HIST");
    h_PadsFired->Scale(1. / h_PadsFired->Integral());
    h_PadsFired->Write("fired_pads");
    c_fired->Close();

    // TCanvas *c_temp = new TCanvas("", "", 720, 720);
    // SetCanvasStyle(c_temp, 0.15, 0.03, 0.01, 0.15);
    // c_temp->Divide(2,2);
    // SetHistostyle2(h_ADCraw[16]);
    // SetHistostyle2(h_ADCraw[52]);
    // SetHistostyle2(h_ADCraw[14]);
    // SetHistostyle2(h_ADCraw[48]);
    // h_ADCraw[16]->GetYaxis()->SetTitleOffset(1.1);
    // h_ADCraw[52]->GetYaxis()->SetTitleOffset(1.1);
    // h_ADCraw[14]->GetYaxis()->SetTitleOffset(1.1);
    // h_ADCraw[48]->GetYaxis()->SetTitleOffset(1.1);
    // c_temp->cd(1);
    // gPad->SetBottomMargin(0.15);
    // gPad->SetTopMargin(0.05);
    // gPad->SetLogy(1);
    // h_ADCraw[16]->GetXaxis()->SetRangeUser(0, 1050);
    // h_ADCraw[16]->Draw();
    // c_temp->cd(2);
    //   gPad->SetBottomMargin(0.15);
    // gPad->SetTopMargin(0.05);
    // gPad->SetLogy(1);
    // h_ADCraw[52]->GetXaxis()->SetRangeUser(0, 1050);
    // h_ADCraw[52]->Draw();
    // c_temp->cd(3);
    //   gPad->SetBottomMargin(0.15);
    // gPad->SetTopMargin(0.05);
    // gPad->SetLogy(1);
    // h_ADCraw[14]->GetXaxis()->SetRangeUser(0, 1050);
    // h_ADCraw[14]->Draw();
    // c_temp->cd(4);
    // gPad->SetLogy(1);
    //   gPad->SetBottomMargin(0.15);
    // gPad->SetTopMargin(0.05);
    // h_ADCraw[48]->GetXaxis()->SetRangeUser(0, 1050);
    // h_ADCraw[48]->Draw();
    // c_temp->SaveAs("paper_plots/editorcomments/ADCsaturation.png");
}

void findCluster(float e[9][8], int padsok)
{

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            rcCluster[i][j] = -1;
        }
    }

    maxr = -1;
    maxc = -1;
    findMaxE(e, padsok);

    if (maxr != -1 && maxc != -1)
    {
        // cout << "cluster ::::::::::::::::::::::: maxr/c " << maxr << "  " << maxc << "  " << e[maxr][maxc] << endl;
        findNN(maxr, maxc, e, padsok);
    }
}

void findMaxE(float e[9][8], int padsok)
{

    float maxe = 0;
    float secondMaxe = 0;
    // maxr;
    // maxc;

    // int ilow, ihigh, jlow, jhigh;
    // if (wafercopy == "26")
    // {
    //     ilow = 3;  // 3
    //     ihigh = 6; // 6
    //     jlow = 2;  // 2
    //     jhigh = 5; // 5
    // }
    // else
    // {
    //     ilow = 2;
    //     ihigh = 7;
    //     jlow = 2;
    //     jhigh = 5;
    // }
    // for (int i = ilow; i < ihigh; i++)
    // {
    //     for (int j = jlow; j < jhigh; j++)
    //     {
    //         if (maxe < e[i][j] && status[i][j] == 1)
    //         { // used cell excluded
    //             maxe = e[i][j];
    //             maxr = i;
    //             maxc = j;
    //         }
    //     }
    // }

    secondmaxr = 0;
    secondmaxc = 0;

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (maxe < e[i][j] && status[i][j] == 1)
            { // used cell excluded
                secondMaxe = maxe;
                secondmaxr = maxr;
                secondmaxc = maxc;
                maxe = e[i][j];
                maxr = i;
                maxc = j;
            }
            else if (e[i][j] > secondMaxe && status[i][j] == 1)
            {
                secondMaxe = e[i][j];
                secondmaxr = i;
                secondmaxc = j;
            }
        }
    }

    maxhit_channel = mapxy[maxr][maxc];

    if (padsok == 1)
    {
        maxe = secondMaxe;
        maxr = secondmaxr;
        maxc = secondmaxc;
    }

    second_maxhit_channel = mapxy[secondmaxr][secondmaxc];
    status[maxr][maxc] = 2;    //  seed is set to 2
    rcCluster[maxr][maxc] = 1; // max cell included in cluster
}

void findNN(int rm, int cm, float e[9][8], int padsok)
{
    int nNN = 0;
    // cout << "  ===================================================================================== " << endl;
    for (int ii = 0; ii < 9; ii++)
    {
        for (int jj = 0; jj < 8; jj++)
        {

            if (status[ii][jj] == 2)
            {
                rm = ii;
                cm = jj;
                status[ii][jj] = 3;
                rcCluster[ii][jj] = 1; // max cell included in cluster

                // cout << "SEED"
                //  << "  " << rm << "  " << cm << "  " << e[rm][cm] << endl;
                int lowrr = (padsok == 1) ? 1 : 1;
                for (int i = rm - lowrr; i <= rm + lowrr; i++)
                { // finding NN 3x3
                    for (int j = cm - lowrr; j <= cm + lowrr; j++)
                    {
                        // if(e[i][j]>0)  cout<<"NN check--> "<<i<<"  "<<j<<"  "<<e[i][j]<<"  "<<status[i][j]<<endl;
                        if (i < 0 || j < 0 || i > 8 || j > 7)
                            continue;
                        if (i == rm && j == cm)
                        {
                            // cout << j << "  -matched skip  seed-  " << j << endl;
                            status[i][j] = 3;    // used cll made zero
                            rcCluster[i][j] = 1; // max cell included in cluster
                                                 // continue;
                        }

                        if (!(i == rm && j == cm))
                        {
                            //	cout<<"No- NN"<<"  "<<i<<"  "<<j<<"  "<<e[i][i]<<"  "<< status[i][j]<<endl;
                            if (status[i][j] == 1)
                            {                        // used cell excluded
                                                     // if(e[i][j]!=0){  // used cell excluded
                                                     //	findNN1(i,j, e);
                                status[i][j] = 2;    // used cll made zero
                                rcCluster[i][j] = 1; // max cell included in cluster

                                //	cout<<"NN"<<"  "<<i<<"  "<<j<<"  "<<e[i][i]<<endl;
                                //	nNN++;
                                // cout << "included : " << i << "    " << j << "  " << e[i][j] << "  " << status[i][j] << endl;
                            }
                        }
                    }
                }
            }
        }
    } //

    int counter = 0;

    for (int ii = 0; ii < 9; ii++)
    {
        for (int jj = 0; jj < 8; jj++)
        {
            if (status[ii][jj] == 2)
            {
                // cout << "SEED to be included : " << ii << " " << jj << endl;
                counter++;
            }
        }
    }

    if (counter > 0)
        findNN(0, 0, e, padsok);
}
