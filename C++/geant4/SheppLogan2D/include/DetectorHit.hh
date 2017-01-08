#ifndef DetectorHit_h 
#define DetectorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"
#include "G4ParticleDefinition.hh"

class DetectorHit : public G4VHit
{
	
	public:
		DetectorHit();
		DetectorHit(const DetectorHit&);
		virtual ~DetectorHit();
	
		//operators
		const DetectorHit& operator=(const DetectorHit&);
		G4int operator==(const DetectorHit&) const;
	
		inline void* operator new(size_t);
		inline void operator delete(void*);
	
		//methods from base class
		virtual void Draw();
		virtual void Print();
	
		// Set methods
		void SetChamberNb(G4int chamb)      { fChamberNb = chamb; };
		void SetKE 		 (G4double ke) 		{ fKE = ke; };
		void SetPos      (G4ThreeVector xyz){ fPos = xyz; };
		void SetMom 	 (G4ThreeVector xyz){ fMom = xyz; };
		void SetVel	  	 (G4double vel) 	{ fVel = vel; };
		void SetTotalEnergy (G4double E) 	{ fTotE = E; 	};
		void SetMomDirection(G4ThreeVector xyz) { fMomDirection = xyz; }
		void SetMass (G4double mass) 	{ fMass = mass; };
		void SetBeta (G4double beta) 	{ fBeta = beta; };

		// Get methods
		G4int GetChamberNb() const   { return fChamberNb; };
		G4double GetKE() const 		 { return fKE; };
		G4ThreeVector GetPos() const { return fPos; };
		G4ThreeVector GetMom() const { return fMom; };
		G4double GetVel()	   const { return fVel; };
		G4double GetTotalEnergy() const { return fTotE; };
		G4ThreeVector GetMomDirection() const { return fMomDirection; }
		G4double GetMass() 	const 	{ return fMass; }
		G4double GetBeta() const 	{ return fBeta; }

	private:
		//some data members;
		G4int fChamberNb;
		G4double fKE, fVel, fTotE, fMass, fBeta;
		G4ThreeVector fPos, fMom, fMomDirection;
};

//not sure about this: collection of hits?
typedef G4THitsCollection<DetectorHit> DetectorHitsCollection;

extern G4ThreadLocal G4Allocator<DetectorHit>* DetectorHitAllocator;

inline void* DetectorHit::operator new(size_t) 
{
	if(!DetectorHitAllocator)
		DetectorHitAllocator = new G4Allocator<DetectorHit>;
	return (void *) DetectorHitAllocator->MallocSingle();
}

inline void DetectorHit::operator delete(void *hit) 
{
	DetectorHitAllocator->FreeSingle((DetectorHit*) hit);
}

#endif
