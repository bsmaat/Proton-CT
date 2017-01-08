#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4UImanager.hh"
#include "QGSP_BERT.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4RunManager.hh"
//#include "G4MTRunManager.hh"
#include "G4ScoringManager.hh"

#include "Randomize.hh"

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
	runManager->SetUserInitialization(new DetectorConstruction());

	G4VModularPhysicsList* physicsList = new QGSP_BERT;
	physicsList->SetVerboseLevel(0);
	runManager->SetUserInitialization(physicsList);

	/*G4VUserPhysicsList* physics = new ExN02PhysicsList;
	runManager->SetUserInitialization(physics);
	*/
	runManager->SetUserInitialization(new ActionInitialization());
	
	// get pointer to user interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();
	
	if(!ui) {
	// batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
} else {
	UImanager->ApplyCommand("/control/execute init_vis.mac");
	ui->SessionStart();
}
	delete visManager;
	delete runManager;
	
}
