/*
 * G4ThreeVectorNew.hh
 *
 *  Created on: 20 Jul 2016
 *      Author: billy
 */

#ifndef G4THREEVECTORNEW_HH_
#define G4THREEVECTORNEW_HH_

#include "G4ThreeVector.hh"

class G4ThreeVectorNew : public G4ThreeVector {
public:
	G4ThreeVectorNew();
	G4ThreeVectorNew(G4double&, G4double&);
	double const Test();
	virtual ~G4ThreeVectorNew();
};

#endif /* G4THREEVECTORNEW_HH_ */
