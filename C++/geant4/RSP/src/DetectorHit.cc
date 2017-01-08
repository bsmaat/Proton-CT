#include "DetectorHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>
#include <iostream>

G4ThreadLocal G4Allocator<DetectorHit>* DetectorHitAllocator=0;

DetectorHit::DetectorHit():
G4VHit(),
fChamberNb(-1),
fKE(0),
fPos(G4ThreeVector()),
fMom(G4ThreeVector()),
fVel(0),
fTotE(0),
fMomDirection(G4ThreeVector()),
fMass(0)
{
}
  
DetectorHit::~DetectorHit() {}
 
DetectorHit::DetectorHit(const DetectorHit& right)
: G4VHit()
{
	fChamberNb = right.fChamberNb;
	fPos = right.fPos;
	fMom = right.fMom;
	fKE = right.fKE;
	fVel = right.fVel;
	fTotE = right.fTotE;
	fMomDirection = right.fMomDirection;
	fMass = right.fMass;
} 

const DetectorHit& DetectorHit::operator=(const DetectorHit& right)
{
	fChamberNb = right.fChamberNb;
	fPos = right.fPos;
	fMom = right.fMom;
	fKE = right.fKE;
	fVel = right.fVel;
	fTotE = right.fTotE;
	fMomDirection = right.fMomDirection;
	fMass = right.fMass;
	return *this;
}

G4int DetectorHit::operator==(const DetectorHit& right) const
{
	return ( this == &right ) ? 1 : 0;
}


void DetectorHit::Draw() 
{

	G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
	if(pVVisManager)
	{
		G4Circle circle(fPos);
		circle.SetScreenSize(10.);
		circle.SetFillStyle(G4Circle::filled);
		G4Colour colour(1.,0.,0.);
		G4VisAttributes attribs(colour);
		circle.SetVisAttributes(attribs);
		pVVisManager->Draw(circle);
	}
}

void DetectorHit::Print()
{
	 G4cout
     << " Position: "
     << std::setw(7) << G4BestUnit( fPos,"Length")
     << " Momentum: "
     << std::setw(7) << G4BestUnit( fMom,"Length")
     << G4endl;
}
