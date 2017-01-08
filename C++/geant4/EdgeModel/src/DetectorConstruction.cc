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

	G4NistManager* nist = G4NistManager::Instance(); //nist material manager

	G4bool checkOverlaps = true;

	//envelope parameters
	G4double env_sizeXY = 30*cm, env_sizeZ = 50*cm;
	G4Material* water_mat = nist->FindOrBuildMaterial("G4_WATER");

	//World
	G4double world_sizeXY = 1.2*env_sizeXY;
	G4double world_sizeZ = 1.2*env_sizeZ;
	G4Material* air_mat = nist->FindOrBuildMaterial("G4_AIR");

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

	// cubic bone shape

	//compact bone = 80% of bone in body
	G4Material* bone_mat = nist->FindOrBuildMaterial("G4_BONE_COMPACT_ICRU");

	G4double Dx = 8.*cm;
	G4double Dy = 7.*cm;
	G4double Dz = 5.*cm;
	G4EllipticalTube* tubBone = new G4EllipticalTube("tubBone", Dx, Dy, Dz);
	G4EllipticalTube* tubWater = new G4EllipticalTube("tubWater", Dx-1.*cm, Dy-1.*cm, Dz);

	G4double boxBone_x = 5.*cm;
	G4double boxBone_y = 4.*cm;
	G4double boxBone_z = 10.*cm;
	G4double boxAir_x = 5.*cm;
	G4double boxAir_y = 4.*cm;
	G4double boxAir_z = 10.*cm;


	G4Box* boxBone = new G4Box("boxBone",0.5*boxBone_x, 0.5*boxBone_y, 0.5*boxBone_z);
	G4LogicalVolume* logicBoxBone =
			new G4LogicalVolume(boxBone, bone_mat, "LogicBoxBone");


	G4Box* boxAir = new G4Box("boxAir", .5*boxAir_x, .5*boxAir_y, .5*boxAir_z);
	G4LogicalVolume* logicBoxAir =
			new G4LogicalVolume(boxAir, air_mat, "LogicBoxAir");

	G4LogicalVolume* logicOuterTub =
			new G4LogicalVolume(tubBone,
					bone_mat,
					"LogicOuterTub");

	G4LogicalVolume* logicInnerTub =
			new G4LogicalVolume(tubWater, water_mat, "LogicInnerTub");



	G4VisAttributes * boxAtt = new G4VisAttributes(G4Colour::Green());
	boxAtt->SetForceWireframe(false);
	logicBoxBone->SetVisAttributes(boxAtt);

	G4VisAttributes * innerTubAtt = new G4VisAttributes(G4Colour::Yellow());
	//innerTubAtt->SetForceWireframe(true);
	logicInnerTub->SetVisAttributes(innerTubAtt);



	G4RotationMatrix * fRot = new G4RotationMatrix();
	fRot->rotateX(90.*deg);


	new G4PVPlacement(0, G4ThreeVector(3.2*cm, 0, 0), logicBoxBone, "BoxBonePhantom", logicInnerTub, false, 0, checkOverlaps);
	new G4PVPlacement(0, G4ThreeVector(-3.2*cm, 0, 0), logicBoxAir, "BoxAirPhantom", logicInnerTub, false, 0, checkOverlaps);

	// place bone into box
	fRotationMatrix->rotateX(90.*deg);
	new G4PVPlacement(//fRot,
			fRotationMatrix,
			G4ThreeVector(),
			logicOuterTub,
			"TubBonePhantom",
			//logicBox,
			logicEnv,
			false,
			0,
			checkOverlaps);
	//water into bone
	new G4PVPlacement(0,
			G4ThreeVector(),
			logicInnerTub,
			"TubWaterPhantom",
			logicOuterTub,
			false,
			0,
			checkOverlaps);

	// construct sensitive detectors!

	G4VisAttributes * senseDetectorVisAtt = new G4VisAttributes(G4Colour::Red());
	senseDetectorVisAtt->SetForceWireframe(true);	
	G4double senseDetector_X = env_sizeXY, senseDetector_Y = env_sizeXY, 
			senseDetector_Z = 1*um;

	G4Material* detector_mat = nist->FindOrBuildMaterial("G4_AIR");

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
	//G4SDManager* SDman = G4SDManager::GetSDMpointer();
	//SDman->AddNewDetector(senseDetector);
}

void DetectorConstruction::SetRotationAngle(G4double newValue) 
{
	*fRotationMatrix = G4RotationMatrix();  // make it unit vector
	fRotationMatrix->rotateY(newValue);
	fRotationMatrix->rotateX(90.*deg);

	// tell G4RunManager that we change the geometry
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

