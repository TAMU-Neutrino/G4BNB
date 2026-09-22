#ifndef BooNEHadronCrossSections_h
#define BooNEHadronCrossSections_h 1
 
#include "globals.hh"
#include "G4Element.hh"
#include "G4VProcess.hh"
#include "G4DynamicParticle.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4Proton.hh"
#include "G4Neutron.hh"
#include "G4UImanager.hh"
#include "BooNEHadronCrossSectionsMessenger.hh"
#include <map>

class BooNEHadronCrossSections
{
public:

  BooNEHadronCrossSections();

 
  ~BooNEHadronCrossSections()
  {
   delete theCrossSectionsMessenger;
   theCrossSectionsMessenger = 0;
  }  

  static BooNEHadronCrossSections* Instance()
  {
    if (!theInstance) theInstance = new BooNEHadronCrossSections();
    return theInstance;
  }

  BooNEHadronCrossSectionsMessenger* theCrossSectionsMessenger;
  
  G4bool IsApplicable(const G4DynamicParticle* aParticle,
		      G4int Z, G4int A);
  
  G4double GetTotalCrossSection(const G4DynamicParticle* aParticle,
				G4int Z, G4int A);

  G4double GetElasticCrossSection(const G4DynamicParticle* aParticle,
				  G4int Z, G4int A);
  
  G4double GetQuasiElasticCrossSection(const G4DynamicParticle* aParticle,
				       G4int Z, G4int A);
  
  G4double GetInelasticCrossSection(const G4DynamicParticle* aParticle,
				    G4int Z, G4int A);
  
  // Additive offsets in mb applied on top of the parametrised total, inelastic and
  // quasi-elastic cross sections (systematic variations, PRD 79 072002 Table XIII).
  // key = <tot|ine|qel><Pro|Neu|Pip|Pim><Be|Al>, e.g. "qelPipBe"
  void SetCrossSectionOffset(const G4String& key, G4double mb) { fOffsets[key] = mb; }
  G4double CrossSectionOffset(const G4DynamicParticle* aParticle, G4int Z, G4int A,
                              const char* kind) const;

  // accessor for momentum ranges
  G4double GetProtonBeMomentumMin()     { return pBeMin;   }
  G4double GetNeutronBeMomentumMin()    { return nBeMin;   }
  G4double GetPionPlusBeMomentumMin()   { return pipBeMin; }
  G4double GetPionMinusBeMomentumMin()  { return pimBeMin; }

  G4double GetProtonBeMomentumMax()     { return pBeMax;   }
  G4double GetNeutronBeMomentumMax()    { return nBeMax;   }
  G4double GetPionPlusBeMomentumMax()   { return pipBeMax; }
  G4double GetPionMinusBeMomentumMax()  { return pimBeMax; }

  G4double GetProtonAlMomentumMin()     { return pAlMin;   }
  G4double GetNeutronAlMomentumMin()    { return nAlMin;   }
  G4double GetPionPlusAlMomentumMin()   { return pipAlMin; }
  G4double GetPionMinusAlMomentumMin()  { return pimAlMin; }

  G4double GetProtonAlMomentumMax()     { return pAlMax;   }
  G4double GetNeutronAlMomentumMax()    { return nAlMax;   }
  G4double GetPionPlusAlMomentumMax()   { return pipAlMax; }
  G4double GetPionMinusAlMomentumMax()  { return pimAlMax; }


  // modifier for momentum ranges
  void SetProtonBeMomentumRange(G4double min, G4double max)      { pBeMin = min;   pBeMax = max;   }
  void SetNeutronBeMomentumRange(G4double min, G4double max)     { nBeMin = min;   nBeMax = max;   }
  void SetPionPlusBeMomentumRange(G4double min, G4double max)    { pipBeMin = min; pipBeMax = max; }
  void SetPionMinusBeMomentumRange(G4double min, G4double max)   { pimBeMin = min; pimBeMax = max; }

  void SetProtonAlMomentumRange(G4double min, G4double max)       { pAlMin = min;   pAlMax = max;   }
  void SetNeutronAlMomentumRange(G4double min, G4double max)	  { nAlMin = min;   nAlMax = max;   }
  void SetPionPlusAlMomentumRange(G4double min, G4double max)	  { pipAlMin = min; pipAlMax = max; }
  void SetPionMinusAlMomentumRange(G4double min, G4double max)	  { pimAlMin = min; pimAlMax = max; }

  // modifiers for the cross sections
  // proton-Be
  void SetpBeTotParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pBeTotParameters.push_back(values[i]); }
  void SetpBeQelParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pBeQelParameters.push_back(values[i]); }
  void SetpBeIneParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pBeIneParameters.push_back(values[i]); }

  // proton-Al
  void SetpAlTotParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pAlTotParameters.push_back(values[i]); }
  void SetpAlQelParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pAlQelParameters.push_back(values[i]); }
  void SetpAlIneParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pAlIneParameters.push_back(values[i]); }

  // neutron-Be
  void SetnBeTotParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)nBeTotParameters.push_back(values[i]); }
  void SetnBeQelParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)nBeQelParameters.push_back(values[i]); }
  void SetnBeIneParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)nBeIneParameters.push_back(values[i]); }

  // neutron-Al
  void SetnAlTotParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)nAlTotParameters.push_back(values[i]); }
  void SetnAlQelParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)nAlQelParameters.push_back(values[i]); }
  void SetnAlIneParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)nAlIneParameters.push_back(values[i]); }

  // pip-Be
  void SetPipBeTotParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pipBeTotParameters.push_back(values[i]); }
  void SetPipBeQelParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pipBeQelParameters.push_back(values[i]); }
  void SetPipBeIneParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pipBeIneParameters.push_back(values[i]); }

  // pip-Al
  void SetPipAlTotParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pipAlTotParameters.push_back(values[i]); }
  void SetPipAlQelParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pipAlQelParameters.push_back(values[i]); }
  void SetPipAlIneParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pipAlIneParameters.push_back(values[i]); }

  // pim-Be
  void SetPimBeTotParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pimBeTotParameters.push_back(values[i]); }
  void SetPimBeQelParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pimBeQelParameters.push_back(values[i]); }
  void SetPimBeIneParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pimBeIneParameters.push_back(values[i]); }

  // neutron-Al
  void SetPimAlTotParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pimAlTotParameters.push_back(values[i]); }
  void SetPimAlQelParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pimAlQelParameters.push_back(values[i]); }
  void SetPimAlIneParameter(G4double* values, G4int nvars){ for(int i = 0; i < nvars; i++)pimAlIneParameters.push_back(values[i]); }



  // functions for constructing cross section parametrization

  // Breit Wigner functions
  G4double BreitWigner(G4double p, G4double mHadron, G4double mTarget, 
		       G4double N, G4double M0,      G4double Gamma);

  G4double CarrollBreitWigner(G4double p, G4double A, G4double mHadron);


  // Regge parametrization of high energy behavior
  G4double Regge(G4double p, G4double A, G4double B, G4double n, G4double C, G4double D);

  G4double ReggeWithThreshold(G4double p, G4double p0, G4double s, 
			      G4double A, G4double B, G4double n, G4double C, G4double D);



  void SetVerboseLevel(G4int value)
  {
    verboseLevel = value;
  }
  
  G4int GetVerboseLevel()
  {
    return verboseLevel;
  }
  
private:
  std::map<G4String, G4double> fOffsets;

  // momentum validity ranges 
  G4double pBeMin,   pBeMax,   pAlMin,   pAlMax; 
  G4double nBeMin,   nBeMax,   nAlMin,   nAlMax; 
  G4double pipBeMin, pipBeMax, pipAlMin, pipAlMax; 
  G4double pimBeMin, pimBeMax, pimAlMin, pimAlMax; 

  // proton cross section parameters
  std::vector<G4double>  pBeTotParameters;
  std::vector<G4double>  pBeQelParameters;
  std::vector<G4double>  pBeIneParameters;

  std::vector<G4double>  pAlTotParameters;
  std::vector<G4double>  pAlQelParameters;
  std::vector<G4double>  pAlIneParameters;

 
  // neutron cross section parameters
  std::vector<G4double>  nBeTotParameters;
  std::vector<G4double>  nBeQelParameters;
  std::vector<G4double>  nBeIneParameters;

  std::vector<G4double>  nAlTotParameters;
  std::vector<G4double>  nAlQelParameters;
  std::vector<G4double>  nAlIneParameters;


  // piplus
  std::vector<G4double>  pipBeTotParameters;
  std::vector<G4double>  pipBeQelParameters;
  std::vector<G4double>  pipBeIneParameters;

  std::vector<G4double>  pipAlTotParameters;
  std::vector<G4double>  pipAlQelParameters;
  std::vector<G4double>  pipAlIneParameters;


  // piminus cross section parameters
  std::vector<G4double>  pimBeTotParameters;
  std::vector<G4double>  pimBeQelParameters;
  std::vector<G4double>  pimBeIneParameters;

  std::vector<G4double>  pimAlTotParameters;
  std::vector<G4double>  pimAlQelParameters;
  std::vector<G4double>  pimAlIneParameters;

  static BooNEHadronCrossSections* theInstance;
  
  G4int verboseLevel;
  

};
#endif
