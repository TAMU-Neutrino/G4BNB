#include "G4ios.hh"
#include "NuBeamGeometryConstruction.hh"
#include "NuBeamGeometryConstructionMessenger.hh"
#include "NuBeamLocalField.hh"
#include "NuBeamSkinDepthField.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UniformMagField.hh"
#include "G4PropagatorInField.hh"
#include "G4ChordFinder.hh"
#include "G4UImanager.hh"
#include "G4UImessenger.hh"
#include "G4RunManager.hh"
#include "G4UserLimits.hh"
#include "G4UImanager.hh"
#include "G4GDMLParser.hh"

#include <fstream>
using namespace std;

NuBeamGeometryConstruction::NuBeamGeometryConstruction()
  :fMessenger(0)
{
  fMessenger = new NuBeamGeometryConstructionMessenger(this);

  G4UImanager* UI = G4UImanager::GetUIpointer();  

  UI->ApplyCommand("/nubeam/geometry/geometryfile");
}

NuBeamGeometryConstruction::~NuBeamGeometryConstruction(){
  delete fMessenger;
  fMessenger = 0;
}

// -----------------------

G4VPhysicalVolume* NuBeamGeometryConstruction::Construct()
{
  std::cout << "NuBeamGeometryConstruction::Construct GDML geometry file " << fGeometryFile << std::endl;  
  std::ifstream gdmlfile(fGeometryFile.c_str());
  if (!gdmlfile.is_open()) {
    std::string mess("GDML geometry file "); 
    mess += fGeometryFile + G4String(" could not be found \n");
    G4Exception("NuBeamGeometryConstruction::Construct", " ", 
		FatalErrorInArgument, mess.c_str());
    return NULL;
  } else {
    gdmlfile.close();
  }
  G4GDMLParser parser;
  parser.Read( fGeometryFile, true );
    
  _pv = parser.GetWorldVolume("SAND0x3f17340"); // ?? not sure this is most optimum... 
  std::cerr << " Got World Volume, I hope and it has the logical name  .. " << _pv->GetLogicalVolume()->GetName() << std::endl;
  
  //-------------------------------------------------------------------------
  // Magnetic field
  //-------------------------------------------------------------------------
  
  static G4bool GlobalFieldIsInitialized = false;
  if(!GlobalFieldIsInitialized) {
    G4MagneticField* GlobalMagField = new
      G4UniformMagField(G4ThreeVector(0.0,0.0,0.0));
    G4FieldManager* GlobalFieldMgr
      = G4TransportationManager::GetTransportationManager()
      ->GetFieldManager();
      
    GlobalFieldMgr->SetDetectorField(GlobalMagField);
    GlobalFieldMgr->CreateChordFinder(GlobalMagField);
    GlobalFieldIsInitialized = true;
  }

  static G4bool LocalFieldIsInitialized = false;
  if(!LocalFieldIsInitialized) {
    pMyField          = new NuBeamLocalField();
    G4FieldManager* LocalFieldMgr = new G4FieldManager();

    //  now find horn logical volume
    G4LogicalVolumeStore* pLVStore = G4LogicalVolumeStore::GetInstance();
    for (size_t i=0; i<pLVStore->size(); i++) {
      G4LogicalVolume* lv = (*pLVStore)[i];
      if ( lv->GetName() == "HRN1") {
	lv->SetFieldManager(LocalFieldMgr,false);
	LocalFieldMgr->SetDetectorField(pMyField);	      
	LocalFieldMgr->CreateChordFinder(pMyField);
	LocalFieldMgr->GetChordFinder()->SetDeltaChord( 0.1*CLHEP::mm );
	G4cout << "NuBeamGeometryConstruction::Construct field set for = " <<
	  lv->GetName() << G4endl;
	LocalFieldMgr->SetDeltaIntersection( 0.01*CLHEP::mm );
      }	  
    }
    LocalFieldIsInitialized = true;
  }
  
  static G4bool SkinDepthFieldIsInitialized = false;
  if(!SkinDepthFieldIsInitialized) {
    pMySkinDepthField = new NuBeamSkinDepthField();
    NuBeamSkinDepthField* mySkinDepthField = pMySkinDepthField;
    G4FieldManager* SkinDepthFieldMgr = new G4FieldManager();
    G4LogicalVolumeStore* pLVStore = G4LogicalVolumeStore::GetInstance();
    for (size_t i=0; i<pLVStore->size(); i++) {
      G4LogicalVolume* lv = (*pLVStore)[i];
      if ( lv->GetName() == "HWL1") {
	lv->SetFieldManager(SkinDepthFieldMgr,false);
	SkinDepthFieldMgr->SetDetectorField(mySkinDepthField);
	SkinDepthFieldMgr->CreateChordFinder(mySkinDepthField);
	SkinDepthFieldMgr->GetChordFinder()->SetDeltaChord( 0.1*CLHEP::mm );
	SkinDepthFieldMgr->SetDeltaIntersection( 0.01*CLHEP::mm );
	
	G4cout << "BooNEGeometryConstruction::Construct field set for = " 
	       <<  lv->GetName() << " using the skin depth model " << G4endl;
	G4cout << "BooNEGeometryConstruction::Horn Current is     " 
	       << mySkinDepthField->GetSkinDepthHornCurrent()/CLHEP::ampere << " A " << G4endl;
	G4cout << "BooNEGeometryConstruction::Skin depth Model is " 
	       << mySkinDepthField->GetSkinDepthModel()   <<  G4endl;
	G4cout << "BooNEGeometryConstruction::Skin depth is       " 
	       << mySkinDepthField->GetSkinDepth()/CLHEP::mm     << " mm" <<  G4endl;
      }
    }
    SkinDepthFieldIsInitialized = true;
  }
  return _pv;
}

