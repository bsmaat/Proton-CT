#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

SteppingAction::SteppingAction(const DetectorConstruction* detectorConstruction) :
G4UserSteppingAction(),
fDetConstruction(detectorConstruction)
{
}

SteppingAction::~SteppingAction()
{

}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
	G4VPhysicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
	if (step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) {
		G4cout << volume->GetName() << G4endl;
	}
}
