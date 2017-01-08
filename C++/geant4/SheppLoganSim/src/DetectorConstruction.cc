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
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"



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

	//World
	G4double world_sizeXY = 1.2*env_sizeXY;
	G4double world_sizeZ = 1.2*env_sizeZ;

	// materials
	G4Material* water_mat = nist->FindOrBuildMaterial("G4_WATER");
	G4Material* air_mat = nist->FindOrBuildMaterial("G4_AIR");
	G4Material* bone_mat = nist->FindOrBuildMaterial("G4_BONE_COMPACT_ICRU");
	G4Material* brain_mat = nist->FindOrBuildMaterial("G4_BRAIN_ICRP");


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


	G4double Dz = 5.*cm;

	// major (x) and minor (y) axis for elliptical tubes in Shepp Logan phantom
	// see https://en.wikipedia.org/wiki/Shepp%E2%80%93Logan_phantom
	G4double scaler = 13.*cm;

	G4double xa = 0.69*scaler;
	G4double ya = 0.92*scaler;

	G4double xb = 0.6224*scaler;
	G4double yb = 0.874*scaler;

	G4double xC = 0.11*scaler;
	G4double yC = 0.31*scaler;

	G4double xd = 0.16*scaler;
	G4double yd  = 0.41*scaler;

	G4double xe = 0.21*scaler;
	G4double ye = 0.25*scaler;

	G4double xf = 0.046*scaler;
	G4double yf = 0.046*scaler;

	G4double xg = 0.046*scaler;
	G4double yg = 0.046*scaler;

	G4double xh = 0.046*scaler;
	G4double yh = 0.023*scaler;

	G4double xi = 0.023*scaler;
	G4double yi = 0.023*scaler;

	G4double xj = 0.023*scaler;
	G4double yj = 0.046*scaler;

	G4RotationMatrix* TubaRot = new G4RotationMatrix;
	G4RotationMatrix* TubbRot = new G4RotationMatrix;
	G4RotationMatrix* TubCRot = new G4RotationMatrix(0,0,-18.*deg);
	G4RotationMatrix* TubdRot = new G4RotationMatrix(0,0,18.*deg);
	G4RotationMatrix* TubeRot = new G4RotationMatrix;
	G4RotationMatrix* TubfRot = new G4RotationMatrix;
	G4RotationMatrix* TubgRot = new G4RotationMatrix;
	G4RotationMatrix* TubhRot = new G4RotationMatrix;
	G4RotationMatrix* TubiRot = new G4RotationMatrix;
	G4RotationMatrix* TubjRot = new G4RotationMatrix;


	G4EllipticalTube* tuba = new G4EllipticalTube("tuba", xa, ya, Dz);
	G4EllipticalTube* tubb = new G4EllipticalTube("tubb", xb, yb, Dz);
	G4EllipticalTube* tubC = new G4EllipticalTube("tubC", xC, yC, Dz);
	G4EllipticalTube* tubd = new G4EllipticalTube("tubd", xd, yd, Dz);
	G4EllipticalTube* tube = new G4EllipticalTube("tube", xe, ye, Dz);
	G4EllipticalTube* tubf = new G4EllipticalTube("tubf", xf, yf, Dz);
	G4EllipticalTube* tubg = new G4EllipticalTube("tubg", xg, yg, Dz);
	G4EllipticalTube* tubh = new G4EllipticalTube("tubh", xh, yh, Dz);
	G4EllipticalTube* tubi = new G4EllipticalTube("tubi", xi, yi, Dz);
	G4EllipticalTube* tubj = new G4EllipticalTube("tubj", xj, yj, Dz);

	// creating tubesubdf
	G4SubtractionSolid* tubesubd = new G4SubtractionSolid("tubesubd", tube, tubd, TubdRot, G4ThreeVector((-0.22+0)*scaler, (0-0.35)*scaler,0));
	G4SubtractionSolid* tubesubdf = new G4SubtractionSolid("tubesubdf", tubesubd, tubf, 0, G4ThreeVector((0-0)*scaler, (0.1-0.35)*scaler,0 ) );

	// creating tubgsubd
	G4SubtractionSolid* tubgsubd = new G4SubtractionSolid("tubgsubd", tubg, tubd, TubdRot, G4ThreeVector(-0.22*scaler, 0.1*scaler, 0));

	// creating tubfsube
	G4SubtractionSolid* tubfsube = new G4SubtractionSolid("tubfsube", tubf, tube, 0, G4ThreeVector(0, (0.35-0.1)*scaler, 0));

	// creating tubdsubeg
	G4RotationMatrix* invTubdRot = new G4RotationMatrix(0,0,-18.*deg);
	G4ThreeVector rotatedVecde  = G4ThreeVector(0.22*scaler*std::cos(18*deg) + 0.35*scaler*std::sin(18*deg), 0.22*scaler*-std::sin(18*deg) + 0.35*scaler*std::cos(18*deg),0);
	G4SubtractionSolid* tubdsube = new G4SubtractionSolid("tubdsube", tubd, tube, invTubdRot, rotatedVecde);
	G4ThreeVector rotatedVecdg  = G4ThreeVector(0.22*scaler*std::cos(18*deg) + -0.1*scaler*std::sin(18*deg), 0.22*scaler*-std::sin(18*deg) + -0.1*scaler*std::cos(18*deg),0);
	G4SubtractionSolid* tubdsubeg = new G4SubtractionSolid("tubdsubeg", tubdsube, tubg, 0, rotatedVecdg);

	// creating tubintersectef
	G4IntersectionSolid* tubintersectef = new G4IntersectionSolid("tubintersectef", tube, tubf, 0, G4ThreeVector((0-0)*scaler, (0.1-0.35)*scaler,0 ) );

	// creating tubintersectde - intersection of d and e
	G4IntersectionSolid* tubintersectde = new G4IntersectionSolid("tubintersectde", tube, tubd, TubdRot, G4ThreeVector((-0.22+0)*scaler, (0-0.35)*scaler,0));

	// creating tubintersectdg - intersection of d and g
	G4IntersectionSolid* tubintersectdg = new G4IntersectionSolid("tubintersectdg", tubg, tubd, TubdRot, G4ThreeVector(-0.22*scaler, 0.1*scaler, 0));


	G4LogicalVolume* logicTuba = new G4LogicalVolume(tuba, bone_mat, "LogicTuba");
	G4LogicalVolume* logicTubb = new G4LogicalVolume(tubb, water_mat, "LogicTubb");
	G4LogicalVolume* logicTubC = new G4LogicalVolume(tubC, brain_mat, "LogicTubC");
	G4LogicalVolume* logicTubd = new G4LogicalVolume(tubd, brain_mat, "LogicTubd");
	G4LogicalVolume* logicTube = new G4LogicalVolume(tube, bone_mat, "LogicTube");
	G4LogicalVolume* logicTubf = new G4LogicalVolume(tubf, air_mat, "LogicTubf");
	G4LogicalVolume* logicTubg = new G4LogicalVolume(tubg, air_mat, "LogicTubg");
	G4LogicalVolume* logicTubh = new G4LogicalVolume(tubh, air_mat, "LogicTubh");
	G4LogicalVolume* logicTubi = new G4LogicalVolume(tubi, air_mat, "LogicTubi");
	G4LogicalVolume* logicTubj = new G4LogicalVolume(tubj, air_mat, "LogicTubj");

	G4LogicalVolume* logicTubesubd = new G4LogicalVolume(tubesubd, water_mat, "LogicTubesubd");
	G4LogicalVolume* logicTubesubdf = new G4LogicalVolume(tubesubdf, bone_mat, "LogicTubefsubd");
	G4LogicalVolume* logicTubgsubd = new G4LogicalVolume(tubgsubd, air_mat, "LogicTubgsubd");

	//G4LogicalVolume* logicTubdsube = new G4LogicalVolume(tubdsube, water_mat, "LogicTubdsube");
	G4LogicalVolume* logicTubdsubeg = new G4LogicalVolume(tubdsubeg, brain_mat, "LogicTubesubeg");

	G4LogicalVolume* logicTubfsube = new G4LogicalVolume(tubfsube, air_mat, "LogicTubfsube");

	G4LogicalVolume* logicTubintersectde = new G4LogicalVolume(tubintersectde, air_mat, "LogicTubintersectde");
	G4LogicalVolume* logicTubintersectdg = new G4LogicalVolume(tubintersectdg, air_mat, "LogicTubintersectdg");
	G4LogicalVolume* logicTubintersectef = new G4LogicalVolume(tubintersectef, air_mat, "LogicTubintersectef");


	G4VisAttributes * tubaAtt = new G4VisAttributes(G4Colour::White());
	G4VisAttributes * tubbAtt = new G4VisAttributes(G4Colour::Gray());
	G4VisAttributes * tubCAtt = new G4VisAttributes(G4Colour::Black());
	G4VisAttributes * tubdAtt = new G4VisAttributes(G4Colour::Red());
	G4VisAttributes * tubeAtt = new G4VisAttributes(G4Colour::Green());
	G4VisAttributes * tubfAtt = new G4VisAttributes(G4Colour::Blue());
	G4VisAttributes * tubgAtt = new G4VisAttributes(G4Colour::Cyan());
	G4VisAttributes * tubhAtt = new G4VisAttributes(G4Colour::Magenta());
	G4VisAttributes * tubiAtt = new G4VisAttributes(G4Colour::Yellow());
	G4VisAttributes * tubjAtt = new G4VisAttributes(G4Colour::White());

	G4VisAttributes * tubesubdAtt = new G4VisAttributes(G4Colour::Green());
	G4VisAttributes * tubdsubegAtt = new G4VisAttributes(G4Colour::Red());
	G4VisAttributes * tubintersectAtt = new G4VisAttributes(G4Colour::Yellow());

	logicTuba->SetVisAttributes(tubaAtt);
	logicTubb->SetVisAttributes(tubbAtt);
	logicTubC->SetVisAttributes(tubCAtt);
	logicTubd->SetVisAttributes(tubdAtt);
	logicTube->SetVisAttributes(tubeAtt);
	logicTubf->SetVisAttributes(tubfAtt);
	logicTubg->SetVisAttributes(tubgAtt);
	logicTubh->SetVisAttributes(tubhAtt);
	logicTubi->SetVisAttributes(tubiAtt);
	logicTubj->SetVisAttributes(tubjAtt);

	logicTubesubd->SetVisAttributes(tubesubdAtt);
	logicTubesubdf->SetVisAttributes(tubeAtt);
	logicTubgsubd->SetVisAttributes(tubgAtt);
	logicTubdsubeg->SetVisAttributes(tubdsubegAtt);
	logicTubfsube->SetVisAttributes(tubfAtt);

	logicTubintersectde->SetVisAttributes(tubintersectAtt);
	logicTubintersectdg->SetVisAttributes(tubintersectAtt);
	logicTubintersectef->SetVisAttributes(tubintersectAtt);


	// rotation to get phantom in correct orientation
	fRotationMatrix->rotateX(90.*deg);
	fRotationMatrix->rotateZ(90.*deg);

	new G4PVPlacement(fRotationMatrix, G4ThreeVector(0, 0, 0), logicTuba, "TubaPhantom", logicEnv, false, 0, checkOverlaps);


	new G4PVPlacement(TubbRot, G4ThreeVector(0, -0.0184*scaler,0), logicTubb, "TubbPhantom", logicTuba, false, 0, checkOverlaps);
	new G4PVPlacement(TubCRot, G4ThreeVector(0.22*scaler, 0,0), logicTubC, "TubCPhantom", logicTubb, false, 0, checkOverlaps);
	//new G4PVPlacement(TubdRot, G4ThreeVector(-0.22*scaler, 0,0), logicTubd, "TubdPhantom", logicTuba, false, 0, checkOverlaps);
	new G4PVPlacement(TubdRot, G4ThreeVector(-0.22*scaler, 0, 0), logicTubdsubeg, "TubdsubegPhantom", logicTubb, false, 0, checkOverlaps);

	//new G4PVPlacement(TubeRot, G4ThreeVector(0, 0.35*scaler,0), logicTube, "TubePhantom", logicTuba, false, 0, checkOverlaps);
	new G4PVPlacement(TubeRot, G4ThreeVector(0, 0.35*scaler, 0), logicTubesubdf, "TubesubdfPhantom", logicTubb, false, 0, checkOverlaps);

	//new G4PVPlacement(TubfRot, G4ThreeVector(0, 0.1*scaler,0), logicTubf, "TubfPhantom", logicTuba, false, 0, checkOverlaps);
	new G4PVPlacement(TubfRot, G4ThreeVector(0, 0.1*scaler, 0), logicTubfsube, "TubfsubePhantom", logicTubb, false, 0, checkOverlaps);

	//new G4PVPlacement(TubgRot, G4ThreeVector(0, -0.1*scaler,0), logicTubg, "TubgPhantom", logicTuba, false, 0, checkOverlaps);
	new G4PVPlacement(TubgRot, G4ThreeVector(0, -0.1*scaler,0), logicTubgsubd, "TubgsubdPhantom", logicTubb, false, 0, checkOverlaps);

	new G4PVPlacement(TubhRot, G4ThreeVector(-0.08*scaler, -0.605*scaler,0), logicTubh, "TubhPhantom", logicTubb, false, 0, checkOverlaps);
	new G4PVPlacement(TubiRot, G4ThreeVector(0, -0.605*scaler,0), logicTubi, "TubiPhantom", logicTubb, false, 0, checkOverlaps);
	new G4PVPlacement(TubjRot, G4ThreeVector(0.06*scaler, -0.605*scaler,0), logicTubj, "TubjPhantom", logicTubb, false, 0, checkOverlaps);



	new G4PVPlacement(TubeRot, G4ThreeVector(0, 0.35*scaler, 0), logicTubintersectde, "TubintersectdePhantom", logicTubb, false, 0, checkOverlaps);
	new G4PVPlacement(TubgRot, G4ThreeVector(0, -0.1*scaler, 0), logicTubintersectdg, "TubintersectdgPhantom", logicTubb, false, 0, checkOverlaps);
	new G4PVPlacement(TubeRot, G4ThreeVector(0, 0.35*scaler, 0), logicTubintersectef, "TubesubdfPhantom", logicTubb, false, 0, checkOverlaps);




	// DETECTOR!!!

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
	fRotationMatrix->rotateX(90.*deg);
	fRotationMatrix->rotateZ(90.* deg + newValue);

	// tell G4RunManager that we change the geometry
	G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

