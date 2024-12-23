
#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "constants.hh"

/// Event action class
class EventAction : public G4UserEventAction
{
public:
    EventAction();
    virtual ~EventAction();

    virtual void    BeginOfEventAction(const G4Event* event);
    virtual void    EndOfEventAction(const G4Event* event);

    inline void AddeDepPAD(G4int i, G4double de) {
        sum_eDep_PAD[i] += de;
    }

    inline G4double GetSumPAD(G4int i){
        return sum_eDep_PAD[i];
    }

    inline void AddeDepPIX(G4int ialpide, G4int i, G4double de) {
        sum_eDep_PIX[ialpide][i] += de; 
    }

    inline G4double GetSumPIX(G4int ialpide, G4int i) {
        return sum_eDep_PIX[ialpide][i];
    }

    inline void AddeDepSCINT(G4int iModule, G4int itower, G4double de) { // inline void AddeDepSCINT(G4int itower, G4double de) {
        //std::cout << "Adding " << iModule << " " << itower << " deposited energy " << de << std::endl;
        sum_eDep_SCINT[iModule][itower] += de; // sum_eDep_SCINT[itower] += de;
    }

    inline G4double GetSumSCINT(G4int iModule, G4int itower) { //inline G4double GetSumSCINT(G4int itower) {
        return sum_eDep_SCINT[iModule][itower]; // return sum_eDep_SCINT[itower];
    }

private:
    G4double  sum_eDep_PAD[NpadX*NpadY*NumberPAD];
    G4double  sum_eDep_PIX[NalpideLayer*NumberPixRow*NumberPixCol*NumberPIX][NpixX*NpixY];
    G4double  sum_eDep_SCINT[N_HCAL_Module_X*N_HCAL_Module_Y][NtowerX*NtowerY]; // G4double  sum_eDep_SCINT[NtowerX*NtowerY];
};

#endif
