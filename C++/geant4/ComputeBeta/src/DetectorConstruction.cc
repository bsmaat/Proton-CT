#include "DetectorConstruction.hh"
#include "TrackerSD.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

//extra includes to make rotation possible
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4SDManager.hh"
#include "G4Tubs.hh"


DetectorConstruction::DetectorConstruction() :
G4VUserDetectorConstruction(),
fScoringVolume(0),
fPipeMatrix(0)
{
}

DetectorConstruction::~DetectorConstruction()
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	// Cleanup old geometry
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();

	G4NistManager* nist = G4NistManager::Instance(); //nist material manager

	G4bool checkOverlaps = true;

	//envelope parameters
	G4double env_sizeXY = 30*cm, env_sizeZ = 30*cm;
	G4Material* env_mat = nist->FindOrBuildMaterial("G4_WATER");

	//World
	G4double world_sizeXY = 1.2*env_sizeXY;
	G4double world_sizeZ = 1.2*env_sizeZ;
	G4Material* world_mat = nist->FindOrBuildMaterial("G4_WATER");

	G4Box* solidWorld =
			new G4Box("World",
					0.5*world_sizeXY,
					0.5*world_sizeXY,
					0.5*world_sizeZ);

	G4LogicalVolume* logicWorld =
			new G4LogicalVolume(solidWorld,
					world_mat,
					"World");


	G4VPhysicalVolume* physWorld =
		new G4PVPlacement(0, // no rotation
					G4ThreeVector(), // at (0,0,0)
					logicWorld, //its logical volume
					"World",
					0,
					false,
					0,
					checkOverlaps);

	// Envelope of water
	G4Box* solidEnv =
		new G4Box("Envelope",
				  0.5*env_sizeXY,
				  0.5*env_sizeXY,
				  0.5*env_sizeZ);

	G4LogicalVolume* logicEnv =
		new G4LogicalVolume(solidEnv,
							env_mat,
							"Envelope");

	new G4PVPlacement(0,
					  G4ThreeVector(),
					  logicEnv,
					  "Envelope",
					  logicWorld,
					  false,
					  0,
					  checkOverlaps);

	// need to repeated construct sensitive detector planes
	G4double SD_sizeXY = env_sizeXY, SD_sizeZ = 0.1*mm;


	G4Box* solidSD =
			new G4Box("TrackerSD",
					0.5*SD_sizeXY,
					0.5*SD_sizeXY,
					0.5*SD_sizeZ);

	G4LogicalVolume* logicSD =
			new G4LogicalVolume(solidSD,
					env_mat,
					"TrackerSDVol");

	G4VisAttributes * logicSDAtt = new G4VisAttributes();
	logicSDAtt->SetForceWireframe(true);
	logicSD->SetVisAttributes(logicSDAtt);

	//place sensitive detectors every 5 mm in the z direction up to the end
	for(int i = 0; i<41; i++) {
		new G4PVPlacement(0,
				G4ThreeVector(0.,0.,-100.0*mm + i*5.*mm),
				logicSD,
				"SDPlacement",
				logicEnv,
				false,
				i,
				checkOverlaps);
	}




	return physWorld;

}

void DetectorConstruction::ConstructSDandField()
{
	G4String detectorName = "/SenseDetector";
	TrackerSD* senseDetector = new TrackerSD(detectorName, "HitsCollection");
	SetSensitiveDetector("TrackerSDVol", senseDetector, true);
	//G4SDManager* SDman = G4SDManager::GetSDMpointer();
	//SDman->AddNewDetector(senseDetector);
}


