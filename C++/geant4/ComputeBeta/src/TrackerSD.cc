#include "G4SDManager.hh"

#include "TrackerSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"
#include "Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName):
G4VSensitiveDetector(name),
fHitsCollection(NULL)
{
		collectionName.insert(hitsCollectionName);
}

TrackerSD::~TrackerSD()
{}


void TrackerSD::Initialize(G4HCofThisEvent* HCE)
{

	//create hits collection
	fHitsCollection =
		new DetectorHitsCollection(SensitiveDetectorName, collectionName[0]);

	//add this collection in HCE
	G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	HCE->AddHitsCollection(hcID, fHitsCollection);


}

//create, fill and store the Hit objects at each step
G4bool TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist)
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
				aHit->SetVel(aStep->GetPostStepPoint()->GetVelocity());
				aHit->SetMass(aStep->GetPostStepPoint()->GetMass());
				aHit->SetTotalEnergy(aStep->GetPostStepPoint()->GetTotalEnergy());
				aHit->SetMomDirection(aStep->GetPostStepPoint()->GetMomentumDirection());
				aHit->SetBeta(aStep->GetPostStepPoint()->GetBeta());
				fHitsCollection->insert(aHit);
			}
		}
	}
	return true;
}

void TrackerSD::EndOfEvent(G4HCofThisEvent* HCE)
{
	G4int nofHits = fHitsCollection->entries();
	std::cout << nofHits << std::endl;
	G4cout << G4endl
		   << "-------->Hits Collection: in this event they are " << nofHits
		   << " hits in the tracker chambers: " << G4endl;

	if (nofHits == 41) // check to see made it to end of cube
	{
		for ( G4int i=0; i<nofHits; i++ )
		{
			//(*fHitsCollection)[i]->Print();
			WriteToFile((*fHitsCollection)[i]);
		}
	}
}


void TrackerSD::WriteToFile(DetectorHit* aHit)
{
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	G4String filename = "output";
	//analysisManager->OpenFile(fileName);

	// I need to store initial energy, final energy, position and momentum
	// maybe there is an easier way to do this for G4ThreeVector()
	analysisManager->FillNtupleDColumn(0, aHit->GetKE()/(MeV));
	analysisManager->FillNtupleDColumn(1, aHit->GetPos().getX()/m);
	analysisManager->FillNtupleDColumn(2, aHit->GetPos().getY()/m);
	analysisManager->FillNtupleDColumn(3, aHit->GetPos().getZ()/m);

	analysisManager->FillNtupleDColumn(4, aHit->GetMom().getX());
	analysisManager->FillNtupleDColumn(5, aHit->GetMom().getY());
	analysisManager->FillNtupleDColumn(6, aHit->GetMom().getZ());
	analysisManager->FillNtupleDColumn(7, aHit->GetMom().mag()/(MeV));
	analysisManager->FillNtupleDColumn(8, aHit->GetVel()/(m/s));
	analysisManager->FillNtupleDColumn(9, aHit->GetBeta());
	analysisManager->FillNtupleIColumn(10, aHit->GetChamberNb());
	analysisManager->FillNtupleDColumn(11, aHit->GetTotalEnergy()/(MeV));///(MeV));
	analysisManager->AddNtupleRow();

}


