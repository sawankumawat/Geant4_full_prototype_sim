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
/// \file persistency/P01/include/RootIO.hh
/// \brief Definition of the RootIO class
//
#ifndef INCLUDE_ROOTIO_HH
#define INCLUDE_ROOTIO_HH 1

// Include files
#include <vector>
#include <utility> // std::pair from here

#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "TClonesArray.h"

#include "EventAction.hh"
#include "constants.hh"

using namespace std;




/** @class RootIO
 *
 *
 *
 */

/// Root IO implementation for the persistency example
class RootIO
{
public:
    virtual ~RootIO();

    static RootIO* GetInstance();
    void WriteEvent(int e);
    void WritePad(int i, float a);
    void WriteAlpide(int ialpide, int i, float a);
    void WriteScint(int iModule, int itower, float a);   // void WriteScint(int i, float a);

    void WriteParticleKinematics(float px, float py, float pz, float en);
    void WriteVertex(float x, float y, float z);
    void Fill();
    void Clear();
    void Close();

protected:
    RootIO();

private:

    int event;
    float data_pad[NpadX*NpadY*NumberPAD];
    vector<pair<int, float>> data_alpide[NalpideLayer*NumberPixRow*NumberPixCol*NumberPIX];
    vector<pair<int, float>> data_scint[N_HCAL_Module_X*N_HCAL_Module_Y];  // vector<pair<int, float>> data_scint;

    float particle_px;
    float particle_py;
    float particle_pz;
    float particle_en;
    float vertex_x;
    float vertex_y;
    float vertex_z;
    TFile* fFile;
    TTree * fHitTree;
    int fNevents;

};
#endif // INCLUDE_ROOTIO_HH
