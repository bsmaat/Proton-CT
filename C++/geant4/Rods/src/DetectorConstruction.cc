#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "SensitiveDetector.hh"

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
#include "G4EllipticalTube.hh"
#include "G4SubtractionSolid.hh"



DetectorConstruction::DetectorConstruction():
G4VUserDetectorConstruction(),
fRotationMatrix(0)
{
	fRotationMatrix = new G4RotationMatrix();
	fRotationMatrix->rotateY(0.);

	fDetectorMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{
	delete fDetectorMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	// Cleanup old geometry
	G4GeometryManager::GetInstance()->OpenGeometry();
	G4PhysicalVolumeStore::GetInstance()->Clean();
	G4LogicalVolumeStore::GetInstance()->Clean();
	G4SolidStore::GetInstance()->Clean();

	// define materials
	G4NistManager* nist = G4NistManager::Instance(); //nist material manager
	G4Material* bone_mat = nist->FindOrBuildMaterial("G4_BONE_COMPACT_ICRU");
	G4Material* air_mat = nist->FindOrBuildMaterial("G4_AIR");
	G4Material* water_mat = nist->FindOrBuildMaterial("G4_WATER");
	G4Material* detector_mat = nist->FindOrBuildMaterial("G4_AIR");
	G4Material* aluminium_mat = nist->FindOrBuildMaterial("G4_Al");


	G4bool checkOverlaps = true;

	//envelope parameters
	G4double env_sizeXY = 30*cm, env_sizeZ = 50*cm;

	//World
	G4double world_sizeXY = 1.2*env_sizeXY;
	G4double world_sizeZ = 1.2*env_sizeZ;

	G4Box* solidWorld =
			new G4Box("World", //name
					0.5*world_sizeXY,
					0.5*world_sizeXY, //size
					0.5*world_sizeZ);

	G4LogicalVolume* logicWorld =
			new G4LogicalVolume(solidWorld, // its solid
					air_mat,  // its material
					"World");   // its name


	G4VPhysicalVolume* physWorld =
			new G4PVPlacement(0, // no rotation
					G4ThreeVector(), // at (0,0,0)
					logicWorld, //its logical volume
					"World",
					0,
					false,
					0,
					checkOverlaps);

	// Envelope
	G4Box* solidEnv =
			new G4Box("Envelope",
					0.5*env_sizeXY,
					0.5*env_sizeXY,
					0.5*env_sizeZ);

	G4LogicalVolume* logicEnv =
			new G4LogicalVolume(solidEnv,
					air_mat,//water_mat,
					"Envelope");

	new G4PVPlacement(0,
			G4ThreeVector(),
			logicEnv,
			"Envelope",
			logicWorld,
			false,
			0,
			checkOverlaps);


	G4double Dx = 10.*cm;
	G4double Dy = 10.*cm;
	G4double Dz = 5.*cm;
	G4EllipticalTube* tubWater = new G4EllipticalTube("tubWater", Dx, Dy, Dz);

	G4LogicalVolume* logicWaterCylinder = new G4LogicalVolume(tubWater, water_mat, "LogicWaterCylinder");

	G4double rodDiameter = 5.*mm;
	G4EllipticalTube* tubAluminium = new G4EllipticalTube("tubRod", rodDiameter/2.0, rodDiameter/2.0, Dz);

	G4LogicalVolume* logicAluminiumRod = new G4LogicalVolume(tubAluminium, aluminium_mat, "LogicAluminiumRod");



	G4RotationMatrix * fRot = new G4RotationMatrix();
	fRot->rotateX(90.*deg);


	G4VisAttributes* logicWaterCylinderAtt = new G4VisAttributes(G4Colour::Green());
	logicWaterCylinderAtt->SetForceWireframe(false);
	logicWaterCylinder->SetVisAttributes(logicWaterCylinderAtt);

	fRotationMatrix->rotateX(90.*deg);

	new G4PVPlacement(//fRot,
			fRotationMatrix,
			G4ThreeVector(),
			logicWaterCylinder,
			"TubCylinderPhantom",
			//logicBox,
			logicEnv,
			false,
			0,
			checkOverlaps);


	// place the 24 aluminium rods
	G4ThreeVector pos;
	new G4PVPlacement(0,
			pos,
			logicAluminiumRod,
			"TubAluminiumRod",
			logicWaterCylinder,
			false,
			0,
			checkOverlaps);

	for (int i = 1; i < 24; i++) {
		pos.setRhoPhiZ(3.*mm + i*4.*mm, i*1.2, 0.0);
		new G4PVPlacement(0,
			pos,
			logicAluminiumRod,
			"TubAluminiumRod",
			logicWaterCylinder,
			false,
			i,
			checkOverlaps);
	}


	// construct sensitive detectors!
	G4VisAttributes * senseDetectorVisAtt = new G4VisAttributes(G4Colour::Red());
	senseDetectorVisAtt->SetForceWireframe(true);	
	G4double senseDetector_X = env_sizeXY, senseDetector_Y = env_sizeXY, 
			senseDetector_Z = 1*um;

	G4Box* solidDetector = new G4Box("Detector", 0.5*senseDetector_X,
			0.5*senseDetector_Y,
			0.5*senseDetector_Z);

	G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector, 
			detector_mat,
			"Detector");

	logicDetector->SetVisAttributes(senseDetectorVisAtt);

	G4ThreeVector pos_detector_right = G4ThreeVector(0.*cm, 0.*cm, 23.*cm); // was 9 and -9 cm
	G4ThreeVector pos_detector_left = G4ThreeVector(0.*cm, 0.*cm, -23.*cm);

	new G4PVPlacement(0,
			pos_detector_left,
			logicDetector,
			"DetectorLeft",
			logicEnv,
			false,
			0,
			checkOverlaps);

	new G4PVPlacement(0,
			pos_detector_right,
			logicDetector,
			"DetectorRight",
			logicEnv,
			false,
			1,
			checkOverlaps);

	return physWorld;

}

void DetectorConstruction::ConstructSDandField() 
{
	G4String detectorName = "/SenseDetector";
	SensitiveDetector* senseDetector = new SensitiveDetector(detectorName, "HitsCollection");
	SetSensitiveDetector("Detector", senseDetector, true);
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
	SDman->AddNewDetector(senseDetector);
}

void DetectorConstruction::SetRotationAngle(G4double newValue) 
{
	*fRotationMatrix = G4RotationMatrix();  // make it unit vector
	fRotationMatrix->rotateY(newValue);
	fRotationMatrix->rotateX(90.*deg);

	// tell G4RunManager that we change the geometry
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

