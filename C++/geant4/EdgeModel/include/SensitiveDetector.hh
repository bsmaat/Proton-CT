#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1
#include "G4VSensitiveDetector.hh"
#include "DetectorHit.hh"
#include <vector>

class G4Step;
class G4HCofThisEvent;

class SensitiveDetector : public G4VSensitiveDetector
{
	public:
	SensitiveDetector(const G4String& name, const G4String& hitsCollectionName);
	virtual ~SensitiveDetector();
	
	//methods from base class
	virtual void Initialize(G4HCofThisEvent* hitCollection);
	virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
	virtual void EndOfEvent(G4HCofThisEvent* hitCollection);
	
	private:
	void CreateAnalysisManager();
	void WriteToFile(DetectorHit*);
	DetectorHitsCollection * fHitsCollection;
	G4int collectionID;
};

#endif
