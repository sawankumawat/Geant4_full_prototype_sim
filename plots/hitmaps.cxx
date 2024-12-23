#include <iostream>
#include "../style.h"
using namespace std;

float energy[9][8];
int status[9][8];
int rcCluster[9][8];
int maxr, maxc, secondmaxr, secondmaxc;
void findMaxE(float e[9][8]);
void findCluster(float e[9][8]);
void findNN(int rm, int cm, float e[9][8]);

void hitmaps()
{
    TH2F *HhitMap[20];
    for (int i = 0; i < 20; i++)
    {
        HhitMap[i] = new TH2F(Form("HitMap_%d", i), "", 8, 0.5, 8.5, 9, 0.5, 9.5);
        HhitMap[i]->GetXaxis()->SetLabelSize(0);
        HhitMap[i]->GetYaxis()->SetLabelSize(0);
        HhitMap[i]->GetZaxis()->SetLabelSize(0.05);
        SetHistostyle3(HhitMap[i]);
        HhitMap[i]->SetTitle(0);
        HhitMap[i]->SetStats(0);
        HhitMap[i]->GetXaxis()->SetTitle("Pads");
        HhitMap[i]->GetYaxis()->SetTitle("Pads");
        HhitMap[i]->GetZaxis()->SetTitle("Number of hits");
        HhitMap[i]->GetXaxis()->SetTitleOffset(0.9);
        HhitMap[i]->GetYaxis()->SetTitleOffset(0.8);
        HhitMap[i]->GetZaxis()->SetTitleOffset(1.15);
        HhitMap[i]->GetXaxis()->SetTickLength(0);
        HhitMap[i]->GetYaxis()->SetTickLength(0);
        HhitMap[i]->GetXaxis()->SetNdivisions(115);
        HhitMap[i]->GetYaxis()->SetNdivisions(115);
        HhitMap[i]->GetZaxis()->CenterTitle();
    }

    TFile *f = TFile::Open("../hits.root", "read");
    if (!f || f->IsZombie())
    {
        std::cerr << "Error opening file" << std::endl;
        return;
    }

    TTree *tree = (TTree *)f->Get("DataTree");
    if (!tree)
    {
        std::cerr << "Error opening tree" << std::endl;
        return;
    }

    float entries = tree->GetEntries();
    cout << "Number of entries: " << entries << endl;
    int totalpads = 72 * 20;
    Float_t energy_deposited[totalpads];
    tree->SetBranchAddress("data_pad", energy_deposited);

    for (int ientry = 0; ientry < entries; ientry++)
    {
        tree->GetEntry(ientry);

        for (int i = 0; i < totalpads; i++)
        {
            if (energy_deposited[i] > 1e-9 && energy_deposited[i] < 1000)
            {
                int j = i % 72;
                int layer = i / 72;
                // int row = j / 8;
                // int col = j % 8;
                int row = j % 9;
                int col = j / 9;
                energy[row][col] = energy_deposited[i];
                status[row][col] = 1;
                HhitMap[layer]->Fill(col + 1, row + 1, energy_deposited[i]);
            }
        }
        findCluster(energy);
    }

    TFile *fhitmaps = new TFile("hitmaps.root", "recreate");
    TCanvas *c0 = new TCanvas("c0", "c0", 1440, 720);
    SetCanvasStyle(c0, 0.09, 0.17, 0.02, 0.1);
    c0->Divide(5, 4);

    // Draw histograms on c0
    for (int i = 0; i < 20; i++)
    {
        c0->cd(i + 1);
        HhitMap[i]->SetTitle(Form("Layer %d", i + 1));
        HhitMap[i]->Draw("colz text");

        for (int ix = 1; ix <= HhitMap[i]->GetNbinsX(); ix++)
        {
            for (int j = 1; j <= HhitMap[i]->GetNbinsY(); j++)
            {
                TBox *box = new TBox(HhitMap[i]->GetXaxis()->GetBinLowEdge(ix),
                                     HhitMap[i]->GetYaxis()->GetBinLowEdge(j),
                                     HhitMap[i]->GetXaxis()->GetBinUpEdge(ix),
                                     HhitMap[i]->GetYaxis()->GetBinUpEdge(j));
                box->SetLineStyle(1);
                box->SetFillStyle(0);
                box->Draw("same");
            }
        }
    }
    c0->Write("all_layers");

    // Create and write individual canvases
    for (int i = 0; i < 20; i++)
    {
        TCanvas *c = new TCanvas(Form("c%d", i + 1), Form("Canvas %d", i + 1), 720, 720);
        SetCanvasStyle(c, 0.09, 0.17, 0.08, 0.1);
        HhitMap[i]->Draw("colz text");

        for (int ix = 1; ix <= HhitMap[i]->GetNbinsX(); ix++)
        {
            for (int j = 1; j <= HhitMap[i]->GetNbinsY(); j++)
            {
                TBox *box = new TBox(HhitMap[i]->GetXaxis()->GetBinLowEdge(ix),
                                     HhitMap[i]->GetYaxis()->GetBinLowEdge(j),
                                     HhitMap[i]->GetXaxis()->GetBinUpEdge(ix),
                                     HhitMap[i]->GetYaxis()->GetBinUpEdge(j));
                box->SetLineStyle(1);
                box->SetFillStyle(0);
                box->Draw("same");
            }
        }
        c->Write(Form("HitMap_%d", i));
        c->Close(); // Close individual canvases
    }

    // Close the output file but leave c0 open
    fhitmaps->Close();
}

void findCluster(float e[9][8])
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
    findMaxE(e);

    if (maxr != -1 && maxc != -1)
    {
        // cout << "cluster ::::::::::::::::::::::: maxr/c " << maxr << "  " << maxc << "  " << e[maxr][maxc] << endl;
        findNN(maxr, maxc, e);
    }
}

void findMaxE(float e[9][8])
{

    float maxe = 0;
    // maxr;
    // maxc;

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (maxe < e[i][j] && status[i][j] == 1)
            { // used cell excluded
                maxe = e[i][j];
                maxr = i;
                maxc = j;
            }
        }
    }

    status[maxr][maxc] = 2;    //  seed is set to 2
    rcCluster[maxr][maxc] = 1; // max cell included in cluster
}

void findNN(int rm, int cm, float e[9][8])
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

                for (int i = rm - 1; i <= rm + 1; i++)
                { // finding NN 3x3
                    for (int j = cm - 1; j <= cm + 1; j++)
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
        findNN(0, 0, e);
}