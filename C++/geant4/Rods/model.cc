#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4UImanager.hh"
//#include "QBBC.hh"
#include "QGSP_BERT.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4ScoringManager.hh"

#include "Randomize.hh"

#include "G4SystemOfUnits.hh" // for deg

#include <cstdlib>


int main(int argc, char** argv)
{
	// detect interactive mode (if no arguments) and define UI session
	G4UIExecutive* ui = 0;
	if (argc == 1)
	{
		ui = new G4UIExecutive(argc, argv);
	}

	// Choose random engine
	G4Random::setTheEngine(new CLHEP::RanecuEngine);

#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif

	

	//set mandatory initialization classes
  	DetectorConstruction* fDet = new DetectorConstruction();
	runManager->SetUserInitialization(fDet);

	G4VModularPhysicsList* physicsList = new QGSP_BERT;
	physicsList->SetVerboseLevel(0);
	runManager->SetUserInitialization(physicsList);

	G4int angleID = atoi(argv[1]);
	G4int numOfParticles = atoi(argv[2]);
    G4double ang = 2 * angleID * CLHEP::pi / 720.0 ;
	//G4double ang = 10.0;
	ActionInitialization* fActionInitialization = new ActionInitialization(angleID);
	runManager->SetUserInitialization(fActionInitialization);
	
	// get pointer to user interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();
	
	if(!ui) {
    runManager->SetNumberOfThreads(4);
    runManager->Initialize(); // /run/initialize
    fDet->SetRotationAngle(ang);
    runManager->BeamOn(numOfParticles);

    //UImanager->ApplyCommand(command+fileName);
} else {
	UImanager->ApplyCommand("/control/execute init_vis.mac");
	ui->SessionStart();
}
	delete visManager;
	delete runManager;
	
}
