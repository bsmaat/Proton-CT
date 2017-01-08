#include "G4SDManager.hh"

#include "SensitiveDetector.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"

#include "ModelRunAction.hh"
#include "Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

SensitiveDetector::SensitiveDetector(const G4String& name, const G4String& hitsCollectionName) 
	: G4VSensitiveDetector(name), fHitsCollection(NULL)
{
		collectionName.insert(hitsCollectionName);
}

SensitiveDetector::~SensitiveDetector() {}

	
void SensitiveDetector::Initialize(G4HCofThisEvent* HCE) 
{
	
	//create hits collection
	fHitsCollection = 
		new DetectorHitsCollection(SensitiveDetectorName, collectionName[0]);
		
	//add this collection in HCE
	G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	HCE->AddHitsCollection(hcID, fHitsCollection);
	
	
}

//create, fill and store the Hit objects at each step
G4bool SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist)
{
	if(aStep->GetTrack()->GetParentID() == 0) // check that it's a primary particle
	{
		if(aStep->GetTrack()->GetDefinition()->GetParticleName() == "proton")
		{
			if(aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary) // check particle has just entered SD
			{
				DetectorHit* aHit = new DetectorHit();
				aHit->SetChamberNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
				aHit->SetKE(aStep->GetTrack()->GetKineticEnergy());
				aHit->SetPos(aStep->GetPostStepPoint()->GetPosition());
				aHit->SetMom(aStep->GetPostStepPoint()->GetMomentum());
				//aHit->SetVel(aStep->GetPostStepPoint()->GetVelocity());
				//aHit->SetMass(aStep->GetPostStepPoint()->GetMass());
				//aHit->SetTotalEnergy(aStep->GetPostStepPoint()->GetTotalEnergy());
				//aHit->SetMomDirection(aStep->GetPostStepPoint()->GetMomentumDirection());
				//aHit->SetBeta(aStep->GetPostStepPoint()->GetBeta());
				fHitsCollection->insert(aHit);
			}
		}
	}
	return true;
}


void SensitiveDetector::EndOfEvent(G4HCofThisEvent* HCE) 
{
	//if(verboseLevel > 1) 
	{
		G4int nofHits = fHitsCollection->entries();

		/**
		G4cout << G4endl 
		       << "-------->Hits Collection: in this event they are " << nofHits 
               << " hits in the tracker chambers: " << G4endl;
               **/
		if (nofHits == 2) {
		for ( G4int i=0; i<nofHits; i++ )
		{
			//(*fHitsCollection)[i]->Print();
			WriteToFile((*fHitsCollection)[i]);
		}
		}
		
	}
}


void SensitiveDetector::WriteToFile(DetectorHit* aHit)
{
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	G4String filename = "outputs";

	analysisManager->FillNtupleDColumn(0, aHit->GetKE()/(MeV));
	analysisManager->FillNtupleDColumn(1, aHit->GetPos().getX()/m);
	analysisManager->FillNtupleDColumn(2, aHit->GetPos().getY()/m);
	analysisManager->FillNtupleDColumn(3, aHit->GetPos().getZ()/m);
	analysisManager->FillNtupleDColumn(4, aHit->GetMom().getX());
	analysisManager->FillNtupleDColumn(5, aHit->GetMom().getY());
	analysisManager->FillNtupleDColumn(6, aHit->GetMom().getZ());
	analysisManager->FillNtupleIColumn(7, aHit->GetChamberNb());
	analysisManager->AddNtupleRow();
}

	
