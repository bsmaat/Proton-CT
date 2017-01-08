#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"

#include "ModelRunAction.hh"

ActionInitialization::ActionInitialization() :
	G4VUserActionInitialization()
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
}
