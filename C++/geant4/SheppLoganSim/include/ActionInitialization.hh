#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

class ActionInitialization : public G4VUserActionInitialization
{
public:
	ActionInitialization(G4int a);
	virtual ~ActionInitialization();

	virtual void BuildForMaster() const;
	virtual void Build() const;

private:
	G4int angleID;
};

#endif
