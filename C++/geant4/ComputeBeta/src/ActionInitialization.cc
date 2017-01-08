#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "ModelRunAction.hh"
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

ActionInitialization::ActionInitialization(DetectorConstruction* detConstruction) :
	G4VUserActionInitialization(),
	fDetConstruction(detConstruction)
{
}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
	SetUserAction(new ModelRunAction);

}

void ActionInitialization::Build() const
{
	SetUserAction(new PrimaryGeneratorAction);
	SetUserAction(new ModelRunAction);
	SetUserAction(new SteppingAction(fDetConstruction));

}
