#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4UImanager.hh"
#include "QGSP_BERT.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4RunManager.hh"
//#include "G4MTRunManager.hh"
#include "G4ScoringManager.hh"

#include "Randomize.hh"

#include "Analysis.hh"

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

	// Construct default run manager
/**#ifdef G4MULTITHREADED
	std::cout << "multithread" << std::endl;
	G4MTRunManager* runManager = new G4MTRunManager;
#else
**/
	G4RunManager* runManager = new G4RunManager;
//#endif

	

	//set mandatory initialization classes

	DetectorConstruction* det = new DetectorConstruction;
	runManager->SetUserInitialization(det);

	G4VModularPhysicsList* physicsList = new QGSP_BERT;
	physicsList->SetVerboseLevel(0);
	runManager->SetUserInitialization(physicsList);

	PrimaryGeneratorAction* prim = new PrimaryGeneratorAction;
	runManager->SetUserInitialization(new ActionInitialization(det, prim));
	
	// get pointer to user interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();



	if(!ui) {
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UImanager->ApplyCommand(command+fileName);
	} else {


	}

	delete visManager;
	delete runManager;
	
}
