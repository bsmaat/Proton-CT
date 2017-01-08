//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B4RunAction.cc 87359 2014-12-01 16:04:27Z gcosmo $
//
/// \file B4RunAction.cc
/// \brief Implementation of the B4RunAction class

#include "ModelRunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4EmCalculator.hh"

#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ModelRunAction::ModelRunAction(DetectorConstruction* det, PrimaryGeneratorAction* kin)
 : G4UserRunAction(), fDetector(det), fPrimary(kin)
{ 
	// set printing event number per each event
	G4RunManager::GetRunManager()->SetPrintProgress(1);

	// Create analysis manager
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	// Create directories
	analysisManager->SetVerboseLevel(1);
	analysisManager->CreateNtuple("Model", "Edep and TrackL");
	analysisManager->CreateNtupleDColumn("E");
	analysisManager->CreateNtupleDColumn("StoppingPower");
	analysisManager->FinishNtuple();

	G4String filename = "output";
	analysisManager->OpenFile(filename);

	std::cout << "CONSTRUCTOR" << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ModelRunAction::~ModelRunAction()
{
	std::cout << "DESTRUCTOR" << std::endl;
	G4AnalysisManager::Instance()->CloseFile();
	delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ModelRunAction::BeginOfRunAction(const G4Run* run)
{ 
    G4EmCalculator emCal;

    //get stopping power
    std::vector<G4double> dedx1;
    std::vector<G4double> dedx2;
    std::vector<G4String> emName;
    std::vector<G4double> enerCut;


    G4double energy = fPrimary->GetParticleGun()->GetParticleEnergy();;
    G4ParticleDefinition* particle = fPrimary->GetParticleGun()->GetParticleDefinition();
    G4Material* material = fDetector->GetMaterial();

    G4ProcessVector* plist = particle->GetProcessManager()->GetProcessList();
    G4double cut;
    G4String procName;
    size_t length = plist->size();

    GetCuts();

    for (size_t j=0; j<length; j++) {
    	procName = (*plist)[j]->GetProcessName();
    	cut = fEnergyCut[1];
    	if ((procName == "eBrem")||(procName == "muBrems")) cut = fEnergyCut[0];
    	if (((*plist)[j]->GetProcessType() == fElectromagnetic) &&
    			(procName != "msc")) {
    		emName.push_back(procName);
    		enerCut.push_back(cut);
    	}
    }

    //get stopping power
    G4double dedxtot = 0;
    G4double dedx;
    size_t nproc = emName.size();

    for(size_t j=0; j<nproc; j++) {
    	// restricted
    	//dedx = emCal.ComputeDEDX(energy, particle, emName[j], material, enerCut[j]);
    	// unrestricted
    	dedx = emCal.ComputeDEDX(energy, particle, emName[j], material, energy);

    	dedxtot += dedx;
    	dedx1.push_back(dedx);
    }

    dedx1.push_back(dedxtot);

    //std::cout << dedx1 << std::endl;
    for (size_t j = 0; j<=nproc; j++) {
    	std::cout << G4BestUnit(dedx1[j], "Energy/Length");
    }


	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	analysisManager->FillNtupleDColumn(0, energy/MeV);
	analysisManager->FillNtupleDColumn(1, dedx1[nproc]/(MeV/cm));
	analysisManager->AddNtupleRow();


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ModelRunAction::EndOfRunAction(const G4Run* /*run*/)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ModelRunAction::GetCuts()
{
  G4ProductionCutsTable* theCoupleTable =
        G4ProductionCutsTable::GetProductionCutsTable();

  size_t numOfCouples = theCoupleTable->GetTableSize();
  const G4MaterialCutsCouple* couple = 0;
  G4int index = 0;
  for (size_t i=0; i<numOfCouples; i++) {
     couple = theCoupleTable->GetMaterialCutsCouple(i);
     if (couple->GetMaterial() == fDetector->GetMaterial()) {index = i; break;}
  }

  fRangeCut[0] =
         (*(theCoupleTable->GetRangeCutsVector(idxG4GammaCut)))[index];
  fRangeCut[1] =
         (*(theCoupleTable->GetRangeCutsVector(idxG4ElectronCut)))[index];
  fRangeCut[2] =
         (*(theCoupleTable->GetRangeCutsVector(idxG4PositronCut)))[index];

  fEnergyCut[0] =
         (*(theCoupleTable->GetEnergyCutsVector(idxG4GammaCut)))[index];
  fEnergyCut[1] =
         (*(theCoupleTable->GetEnergyCutsVector(idxG4ElectronCut)))[index];
  fEnergyCut[2] =
         (*(theCoupleTable->GetEnergyCutsVector(idxG4PositronCut)))[index];

}
