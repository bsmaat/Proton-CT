#include "ModelRunAction.hh"
#include "Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <vector>


ModelRunAction::ModelRunAction()
 : G4UserRunAction()
{ 
	std::cout << "ModelRunAction()" << std::endl;
	// set printing event number per each event
	G4RunManager::GetRunManager()->SetPrintProgress(1);

	// Create analysis manager
	// The choice of analysis technology is done via selectin of a namespace
	// in B4Analysis.hh
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	G4cout << "Using " << analysisManager->GetType() << G4endl;

	// Create directories
	//analysisManager->SetHistoDirectoryName("histograms");
	//analysisManager->SetNtupleDirectoryName("ntuple");
	analysisManager->SetVerboseLevel(1);
	analysisManager->SetFirstHistoId(1);

	analysisManager->CreateNtuple("Model", "Edep and TrackL");
	//initial values
	analysisManager->CreateNtupleDColumn("KE");
	analysisManager->CreateNtupleDColumn("PosX");
	analysisManager->CreateNtupleDColumn("PosY");
	analysisManager->CreateNtupleDColumn("PosZ");
	analysisManager->CreateNtupleDColumn("MomX");
	analysisManager->CreateNtupleDColumn("MomY");
	analysisManager->CreateNtupleDColumn("MomZ");
	analysisManager->CreateNtupleDColumn("MomMag");
	analysisManager->CreateNtupleDColumn("Vel");
	analysisManager->CreateNtupleDColumn("Beta");
	analysisManager->CreateNtupleIColumn("ChamberNumber");
	analysisManager->CreateNtupleDColumn("TotE");


	analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ModelRunAction::~ModelRunAction()
{
  delete G4AnalysisManager::Instance();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ModelRunAction::BeginOfRunAction(const G4Run* run)
{ 
  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
    std::cout<< "BeginOfRunAction() :: RunID: " << run->GetRunID() <<  std::endl;

  // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  std::stringstream ss;
  G4String fileName = "output";// << run->GetRunID();
  ss << fileName << run->GetRunID();
  fileName = ss.str();
  analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ModelRunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // print histogram statistics
  //
  std::cout<< "EndOfRunAction()" << std::endl;
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
