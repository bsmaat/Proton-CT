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
	G4double env_sizeXY = 30*cm, env_sizeZ = 30*cm;
	G4Material* env_mat = nist->FindOrBuildMaterial("G4_WATER");
	
	//World
	G4double world_sizeXY = 1.2*env_sizeXY;
	G4double world_sizeZ = 1.2*env_sizeZ;
	G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

	G4Box* solidWorld =
			new G4Box("World", //name
					0.5*world_sizeXY,
					0.5*world_sizeXY, //size
					0.5*world_sizeZ);

	G4LogicalVolume* logicWorld =
			new G4LogicalVolume(solidWorld, // its solid
					world_mat,  // its material
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
							world_mat,//env_mat,
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
	G4ThreeVector pos_box = G4ThreeVector(0, 0, 0);

	G4double box_x = 18.*cm, box_y = 18.*cm, box_z = 18.*cm;
	G4Box* boxBone = new G4Box("BoxBone",
							   0.5*box_x,
							   0.5*box_y,
							   0.5*box_z);

	G4double innerRadius = 0.*cm;
	G4double outerRadius = 1*cm;
	G4double hz = 10.*cm;
	G4double startAngle = 0.*deg;
	G4double spanningAngle = 360.*deg;

	//G4Tubs* tubBone = new G4Tubs("BoneTub", innerRadius, outerRadius, hz/2, startAngle, spanningAngle);

	G4double Dx = 8.*cm;
	G4double Dy = 7.*cm;
	G4double Dz = 5.*cm;
	G4EllipticalTube* tubBone = new G4EllipticalTube("tubBone", Dx, Dy, Dz);
	G4EllipticalTube* tubWater = new G4EllipticalTube("tubWater", Dx-1*cm, Dy-1*cm, Dz); // not sure why we need that 1mm but okay...
	G4Tubs* cyl = new G4Tubs("Cylinder", innerRadius, outerRadius, hz, startAngle, spanningAngle);

	G4double slitBox_x = 1.*cm, slitBox_y = 1.*cm, slitBox_z = 10*cm;

	G4Box* slitBox = new G4Box("SlitBox", slitBox_x/2,slitBox_y/2, slitBox_z/2); //mother volume of slits, material should be water

	//G4Box* slit = new G4Box("Slit", 1.*cm, 0.5*cm, slitBox_z ); // daugher of slit box, material should be bone or air


	G4RotationMatrix* yRot = new G4RotationMatrix;
	G4ThreeVector trans(4.*cm,0.*cm,0);

	//G4SubtractionSolid* subtraction = new G4SubtractionSolid("Cut", tubBone, cyl, yRot, trans);
	//G4SubtractionSolid* subtraction = new G4SubtractionSolid("BoneShell", tubBone, tubWater); // is this needed this way?
	//G4SubtractionSolid* subtraction2 = new G4SubtractionSolid("WaterBody", tubWater, slitBox);
	G4LogicalVolume* logicOuterTub =
		new G4LogicalVolume(tubBone,
							bone_mat,
							"LogicOuterTub");

	G4LogicalVolume* logicInnerTub =
			new G4LogicalVolume(tubWater, env_mat, "LogicInnerTub");

	//G4LogicalVolume* logicSlitBox =
	//		new G4LogicalVolume(slitBox, env_mat, "LogicSlitBox");

	G4LogicalVolume* logicBox =
		new G4LogicalVolume(boxBone,
							env_mat,
							"BoxBone");



	G4VisAttributes * boxAtt = new G4VisAttributes(G4Colour::Green());
	boxAtt->SetForceWireframe(true);
	logicBox->SetVisAttributes(boxAtt);

	G4VisAttributes * innerTubAtt = new G4VisAttributes(G4Colour::Yellow());
	//innerTubAtt->SetForceWireframe(true);
	logicInnerTub->SetVisAttributes(innerTubAtt);



	G4RotationMatrix * fRot = new G4RotationMatrix();
		fRot->rotateX(90.*deg);

// making boxes and placing slices of air in there.
	for (int i = 0; i < 5; i++) {
		G4ThreeVector pos((i-2)*2.*cm,2.5*cm,0); // position of each box
		G4double lines = (i+1)*2; //number of lines
		G4double widthOfLine = 1.*cm/lines; //width of each line

		 // daughter of slit box, material should be bone or air, slitbox is water
		G4LogicalVolume* logicSlitBox =
				new G4LogicalVolume(slitBox, env_mat, "LogicSlitBox");

		G4VisAttributes * slitBoxAtt = new G4VisAttributes(G4Colour::White());
		//slitBoxAtt->SetForceWireframe(true);
		logicSlitBox->SetVisAttributes(slitBoxAtt);

		G4Box* slit = new G4Box("Slit", 1.*cm/2, widthOfLine/2, slitBox_z/2 );

		G4LogicalVolume* logicSlitBone =
				new G4LogicalVolume(slit, bone_mat, "SlitBone");
		G4LogicalVolume* logicSlitAir =
				new G4LogicalVolume(slit, world_mat, "SlitAir");
		G4VisAttributes * slitAtt = new G4VisAttributes(G4Colour::Green());
		logicSlitAir->SetVisAttributes(slitAtt);
		new G4PVPlacement(0,
				pos,
				logicSlitBox,
				"SlitBoxPhantom",
				logicInnerTub,
				false,
				i,
				checkOverlaps);
		G4int k = 0;
		for (int j = 0; j < lines; j++) {
			if (j%2 == 0) {
				new G4PVPlacement(0,
						G4ThreeVector(0, -slitBox_y/2 + widthOfLine/2 + j*widthOfLine, 0),
						logicSlitAir,
						"SlitPhantom",
						logicSlitBox,
						false,
						k,
						checkOverlaps);
				k++;
			}

		}
	}

	// making boxes and placing slices of bone in there.
		for (int i = 0; i < 5; i++) {
			G4ThreeVector pos(-(i-2)*2.*cm,-2.5*cm,0); // position of each box
			G4double lines = (i+1)*2; //number of lines
			G4double widthOfLine = 1.*cm/lines; //width of each line

			 // daughter of slit box, material should be bone or air, slitbox is water
			G4LogicalVolume* logicSlitBox =
					new G4LogicalVolume(slitBox, env_mat, "LogicSlitBox");

			G4VisAttributes * slitBoxAtt = new G4VisAttributes(G4Colour::White());
			slitBoxAtt->SetForceWireframe(true);
			logicSlitBox->SetVisAttributes(slitBoxAtt);

			G4Box* slit = new G4Box("Slit", 1.*cm/2, widthOfLine/2, slitBox_z/2 );

			G4LogicalVolume* logicSlitBone =
					new G4LogicalVolume(slit, bone_mat, "SlitBone");
			G4VisAttributes * slitAtt = new G4VisAttributes(G4Colour::Black());
			logicSlitBone->SetVisAttributes(slitAtt);
			new G4PVPlacement(0,
					pos,
					logicSlitBox,
					"SlitBoxPhantom",
					logicInnerTub,
					false,
					i,
					checkOverlaps);

			G4int k = 0;
			for (int j = 0; j < lines; j++) {
				if (j%2 == 0) {
					new G4PVPlacement(0,
							G4ThreeVector(0, -slitBox_y/2 + widthOfLine/2 + j*widthOfLine, 0),
							logicSlitBone,
							"SlitPhantom",
							logicSlitBox,
							false,
							k,
							checkOverlaps);
					k++;
				}

			}
		}
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

	//box into environment
/*
	new G4PVPlacement(fRotationMatrix,
					  G4ThreeVector(),
					  logicBox,
					  "PhantomBox",
					  logicEnv,
					  false,
					  0,
					  checkOverlaps);

*/
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
	
	G4ThreeVector pos_detector_right = G4ThreeVector(0.*cm, 0.*cm, 13.*cm); // was 9 and -9 cm
	G4ThreeVector pos_detector_left = G4ThreeVector(0.*cm, 0.*cm, -13.*cm);
	
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
	
