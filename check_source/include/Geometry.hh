//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Geometry.hh
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef Geometry_h
#define Geometry_h 1


#include "G4VUserDetectorConstruction.hh"
#include "G4AssemblyVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "constants.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
//------------------------------------------------------------------------------
  class Geometry : public G4VUserDetectorConstruction
//------------------------------------------------------------------------------
{
  public:
    Geometry();
   ~Geometry();

    G4VPhysicalVolume* Construct();

	//Active volumes
    G4LogicalVolume* GetScoringVol_PAD() const { return fScoringVol_PAD; }
    G4LogicalVolume* GetScoringVol_PIX() const { return fScoringVol_PIX; }
    G4LogicalVolume* GetScoringVol_SCINT() const { return fScoringVol_SCINT; }

  protected:

    G4LogicalVolume*  fScoringVol_PAD;
    G4LogicalVolume*  fScoringVol_PIX;
    G4LogicalVolume*  fScoringVol_SCINT;

    G4Material* material_tungsten;
    G4Material* material_Kapton;
    G4Material* material_PET;
    G4Material* material_scintillator;
    G4Material* material_Si;
    G4Material* material_Al;

    G4double ConstructFoCalE(G4LogicalVolume* envelope);
    G4double ConstructFoCalEmodified(G4LogicalVolume* envelope);
    G4int ConstructFoCalH(G4LogicalVolume* envelope, G4LogicalVolume* logVol_World, G4double TotalLayerThickness, G4RotationMatrix *rotMtrx_HCAL_envelope); // bool ConstructFoCalH(G4LogicalVolume* envelope); 

    void CreateMaterials(G4NistManager* manager);
    G4AssemblyVolume* ConstructPixelLayer();
    G4AssemblyVolume* ConstructPadLayer();
  G4AssemblyVolume* ConstructPadLayer1();
};
#endif
