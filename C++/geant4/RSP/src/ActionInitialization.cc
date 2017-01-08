#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"

#include "ModelRunAction.hh"

ActionInitialization::ActionInitialization(DetectorConstruction* det, PrimaryGeneratorAction* kin) :
	G4VUserActionInitialization(), fDet(det), fPrimary(kin)
{
}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
	SetUserAction(new ModelRunAction(fDet, fPrimary));

}

void ActionInitialization::Build() const
{
	SetUserAction(new PrimaryGeneratorAction);
	SetUserAction(new ModelRunAction(fDet, fPrimary));
}
