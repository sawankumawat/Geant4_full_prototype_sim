//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file persistency/P01/src/RootIO.cc
/// \brief Implementation of the RootIO class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <sstream>
#include <vector>

#include "RootIO.hh"
//
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
//
#include "usercommandline.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

static RootIO* instance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RootIO::RootIO() : fNevents(0)
{
    // initialize ROOT
    TSystem ts;
    gSystem->Load("libFoCal_MainClassesDict");
    gInterpreter->GenerateDictionary("vector<pair<int,float>>", "vector");

    //    gDebug = 1;
    char helper[4000];
    string hitfilename = "hits.root";
    if(UserCommandLine::Input) hitfilename = Form("./data/%s/%dGeV/hits_%s_%dGeV_%s.root", UserCommandLine::Particle.c_str(), UserCommandLine::Energy , UserCommandLine::Particle.c_str(), UserCommandLine::Energy, UserCommandLine::Timestamp.c_str());
    else std::cout << "Creating file with " << Particle.c_str() << " at " << Energy << " GeV" << std::endl; 
    
    fFile = new TFile(hitfilename.c_str(),"RECREATE");
    fHitTree = new TTree("DataTree", "Data", 0);
    fHitTree->SetDirectory(fFile);
    fHitTree->Branch("event", &event, "event/I");

    sprintf(helper, "data_pad[%d]/F", NpadX*NpadY*NumberPAD);
    fHitTree->Branch("data_pad", &data_pad, helper );

    for (int ipix=0; ipix<NumberPIX; ipix++) {
        for (int ialpide=0; ialpide<NalpideLayer*NumberPixRow*NumberPixCol; ialpide++) {
            fHitTree->Branch(Form("data_pix%d_alpide%d", ipix, ialpide), &data_alpide[ipix*NalpideLayer*NumberPixRow*NumberPixCol + ialpide]);
        }
    }

    for (int ihcalx = 0; ihcalx < N_HCAL_Module_X; ihcalx++) {
        for (int ihcaly = 0; ihcaly < N_HCAL_Module_Y; ihcaly++) {
            fHitTree->Branch(Form("HCAL_ModuleX%d_ModuleY%d", ihcalx, ihcaly), &data_scint[ihcalx*N_HCAL_Module_Y + ihcaly]);
        }
    }  // fHitTree->Branch("data_scint", &data_scint); 

    fHitTree->Branch("particle_px", &particle_px, "particle_px/F");
    fHitTree->Branch("particle_py", &particle_py, "particle_py/F");
    fHitTree->Branch("particle_pz", &particle_pz, "particle_pz/F");
    fHitTree->Branch("particle_en", &particle_en, "particle_en/F");

    fHitTree->Branch("vertex_x", &vertex_x, "vertex_x/F");
    fHitTree->Branch("vertex_y", &vertex_y, "vertex_y/F");
    fHitTree->Branch("vertex_z", &vertex_z, "vertex_z/F");


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RootIO::~RootIO()
{
	delete fFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RootIO* RootIO::GetInstance()
{
    if (instance == 0 )
    {
        instance = new RootIO();
    }
    return instance;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RootIO::WriteEvent(int e)
{
    event = e;
}

//....oooOO0OOooo........   oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RootIO::WritePad(int i, float a)
{
    data_pad[i] = a;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RootIO::WriteAlpide(int ialpide, int i, float a)
{
    data_alpide[ialpide].push_back(std::make_pair(i, a));
}

//....oooOO0OOooo........   oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RootIO::WriteScint(int iModule, int itower, float a) // void RootIO::WriteScint(int i, float a)
{
    data_scint[iModule].push_back(std::make_pair(itower, a)); // data_scint.push_back(std::make_pair(i, a));

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RootIO::WriteVertex(float x, float y, float z)
{
    vertex_x = x;
    vertex_y = y;
    vertex_z = z;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RootIO::WriteParticleKinematics(float px, float py, float pz, float en)
{
    particle_px = px;
    particle_py = py;
    particle_pz = pz;
    particle_en = en;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RootIO::Clear(){
	for (int i = 0; i < NpadX*NpadY*NumberPAD; i++) data_pad[i] = 0;
    for (int i = 0; i < NalpideLayer*NumberPixCol*NumberPixRow*NumberPIX; i++) {
        data_alpide[i].clear();
    }
    for (int i = 0; i < N_HCAL_Module_X*N_HCAL_Module_Y; i++) {
        data_scint[i].clear();
    } // data_scint.clear();

	particle_px = 0;
	particle_py = 0;
	particle_pz = 0;
	particle_en = 0;
    vertex_x = 0;
    vertex_y = 0;
    vertex_z = 0;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RootIO::Fill(){
	fHitTree->Fill();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RootIO::Close()
{
    fFile->Write("", TObject::kOverwrite);
    fFile->Close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
