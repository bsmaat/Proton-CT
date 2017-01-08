#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"

#include "ModelRunAction.hh"

ActionInitialization::ActionInitialization(G4int angleID) :
	G4VUserActionInitialization()
{
	this->angleID = angleID;
}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
	SetUserAction(new ModelRunAction(angleID));
}

void ActionInitialization::Build() const
{
	SetUserAction(new PrimaryGeneratorAction);
	SetUserAction(new ModelRunAction(angleID));
}
