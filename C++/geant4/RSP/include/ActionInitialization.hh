#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class DetectorConstruction;
class PrimaryGeneratorAction;

class ActionInitialization : public G4VUserActionInitialization
{
public:
	ActionInitialization(DetectorConstruction*, PrimaryGeneratorAction*);
	virtual ~ActionInitialization();

	virtual void BuildForMaster() const;
	virtual void Build() const;

private:
	DetectorConstruction* fDet;
	PrimaryGeneratorAction* fPrimary;
};

#endif
