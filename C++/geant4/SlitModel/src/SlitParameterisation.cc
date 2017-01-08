#include "SlitParameterisation.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"

SlitParameterisation::SlitParameterisation(
		G4int noBoxes)
: G4VPVParameterisation()
{
	fNoBoxes = noBoxes;

}

SlitParameterisation::~SlitParameterisation()
{

}

void SlitParameterisation::ComputeTransformation(const G4int copyNo,
		G4VPhysicalVolume *physVol) const
{
	G4ThreeVector pos(copyNo*3.*cm);
	physVol->SetTranslation(pos);
}

void SlitParameterisation::ComputeDimensions( G4Box& slitBox,
		const G4int copyNo, const G4VPhysicalVolume* physVol) const
{
	slitBox.SetXHalfLength(1.*cm/2);
	slitBox.SetYHalfLength(1.*cm/2);
	slitBox.SetZHalfLength(20.*cm/2);

}
