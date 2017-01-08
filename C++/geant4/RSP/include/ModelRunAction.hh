/// \file B4RunAction.hh
/// \brief Definition of the B4RunAction class

#ifndef ModelRunAction_h
#define ModelRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;
class DetectorConstruction;
class PrimaryGeneratorAction;

/// Run action class
///
/// It accumulates statistic and computes dispersion of the energy deposit 
/// and track lengths of charged particles with use of analysis tools:
/// H1D histograms are created in BeginOfRunAction() for the following 
/// physics quantities:
/// - Edep in absorber
/// - Edep in gap
/// - Track length in absorber
/// - Track length in gap
/// The same values are also saved in the ntuple.
/// The histograms and ntuple are saved in the output file in a format
/// accoring to a selected technology in B4Analysis.hh.
///
/// In EndOfRunAction(), the accumulated statistic and computed 
/// dispersion is printed.
///



class ModelRunAction : public G4UserRunAction
{
  public:
    ModelRunAction(DetectorConstruction*, PrimaryGeneratorAction*);
    virtual ~ModelRunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

    void GetCuts();
    void CriticalEnergy();

  private:
    DetectorConstruction* fDetector;
    PrimaryGeneratorAction* fPrimary;
    G4double  fRangeCut[3];
    G4double fEnergyCut[3];
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
