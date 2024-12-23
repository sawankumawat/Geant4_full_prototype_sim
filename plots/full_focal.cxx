#include <iostream>
using namespace std;

void full_focal()
{
    // TFile *f = new TFile("../simData/hits_5_2.root", "read");
    TFile *f = new TFile("../hits.root", "read");
    if (f->IsZombie())
    {
        cout << "Error opening file" << endl;
    }
    else
    {
        cout << "File opened successfully" << endl;
    }
    TTree *tree = (TTree *)f->Get("DataTree");
    if (tree == NULL)
    {
        cout << "Error opening tree" << endl;
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
            // if (i % 2 == 0)
            //     continue;
            // if (0.000001 < energy_deposited[i] && energy_deposited[i] < 100000)
            if (energy_deposited[i] > 1e-5 && energy_deposited[i] < 1e3)
            cout << "event " << ientry << " pad no. " << i << " energy: " << energy_deposited[i] << endl;
        }
    }
}