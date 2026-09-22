#include "BooNEHadronCrossSectionsMessenger.hh"
#include "BooNEHadronCrossSections.hh"

#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "G4UIdirectory.hh"
#include "G4Tokenizer.hh"

BooNEHadronCrossSectionsMessenger::BooNEHadronCrossSectionsMessenger(BooNEHadronCrossSections* m1)
{

  theBooNEHadronCrossSections = m1;

  //-------------------------Directories-------------------------//

  //  Define the (hadronic) cross sections directory
  booneCrossSectionsDirectory = new G4UIdirectory("/boone/crosssections/");
  booneCrossSectionsDirectory->SetGuidance( "BooNE cross sections control commands.");

  //  Define the proton cross sections directory
  booneProtonCrossSectionsDirectory = new G4UIdirectory("/boone/crosssections/proton/");
  booneProtonCrossSectionsDirectory->SetGuidance( "BooNE proton cross sections control commands.");

  //  Define the neutron cross sections directory
  booneNeutronCrossSectionsDirectory = new G4UIdirectory("/boone/crosssections/neutron/");
  booneNeutronCrossSectionsDirectory->SetGuidance( "BooNE proton cross sections control commands.");

  //  Define the piplus cross sections directory
  boonePionPlusCrossSectionsDirectory = new G4UIdirectory("/boone/crosssections/pionplus/");
  boonePionPlusCrossSectionsDirectory->SetGuidance( "BooNE piplus cross sections control commands.");

  //  Define the piplus cross sections directory
  boonePionMinusCrossSectionsDirectory = new G4UIdirectory("/boone/crosssections/pionminus/");
  boonePionMinusCrossSectionsDirectory->SetGuidance( "BooNE piminus cross sections control commands.");

  G4UIparameter* param;

  // specify valid momentum ranges
  proBeMomentumRangeCmd =  new G4UIcommand("/boone/crosssections/proton/proBeMomentumRange",this);
  proBeMomentumRangeCmd->SetGuidance("Set momentum range for BooNE Proton Be Cross section ");
  proBeMomentumRangeCmd->SetGuidance("Usage: /boone/crosssections/proton/proBeMomentumRange min max");
  param = new G4UIparameter("proBeMomentumRangeMin",'d',true);  param->SetDefaultValue(2.0); proBeMomentumRangeCmd->SetParameter(param);
  param = new G4UIparameter("proBeMomentumRangeMax",'d',true);  param->SetDefaultValue(9.0); proBeMomentumRangeCmd->SetParameter(param);

  proAlMomentumRangeCmd =  new G4UIcommand("/boone/crosssections/proton/proAlMomentumRange",this);
  proAlMomentumRangeCmd->SetGuidance("Set momentum range for BooNE Proton Be Cross section ");
  proAlMomentumRangeCmd->SetGuidance("Usage: /boone/crosssections/proton/proAlMomentumRange min max");
  param = new G4UIparameter("proAlMomentumRangeMin",'d',true);  param->SetDefaultValue(2.0); proAlMomentumRangeCmd->SetParameter(param);
  param = new G4UIparameter("proAlMomentumRangeMax",'d',true);  param->SetDefaultValue(9.0); proAlMomentumRangeCmd->SetParameter(param);

  neuBeMomentumRangeCmd =  new G4UIcommand("/boone/crosssections/neutron/neuBeMomentumRange",this);
  neuBeMomentumRangeCmd->SetGuidance("Set momentum range for BooNE Neutron Cross section ");
  neuBeMomentumRangeCmd->SetGuidance("Usage: /boone/crosssections/neutron/neuBeMomentumRange min max");
  param = new G4UIparameter("neuBeMomentumRangeMin",'d',true);  param->SetDefaultValue(2.0); neuBeMomentumRangeCmd->SetParameter(param);
  param = new G4UIparameter("neuBeMomentumRangeMax",'d',true);  param->SetDefaultValue(9.0); neuBeMomentumRangeCmd->SetParameter(param);

  neuAlMomentumRangeCmd =  new G4UIcommand("/boone/crosssections/neutron/neuAlMomentumRange",this);
  neuAlMomentumRangeCmd->SetGuidance("Set momentum range for BooNE Neutron Cross section ");
  neuAlMomentumRangeCmd->SetGuidance("Usage: /boone/crosssections/neutron/neuAlMomentumRange min max");
  param = new G4UIparameter("neuAlMomentumRangeMin",'d',true);  param->SetDefaultValue(2.0); neuAlMomentumRangeCmd->SetParameter(param);
  param = new G4UIparameter("neuAlMomentumRangeMax",'d',true);  param->SetDefaultValue(9.0); neuAlMomentumRangeCmd->SetParameter(param);

  pipBeMomentumRangeCmd =  new G4UIcommand("/boone/crosssections/pionplus/pipBeMomentumRange",this);
  pipBeMomentumRangeCmd->SetGuidance("Set momentum range for BooNE Pion plus Cross section ");
  pipBeMomentumRangeCmd->SetGuidance("Usage: /boone/crosssections/pionplus/pipBeMomentumRange min max");
  param = new G4UIparameter("pipBeMomentumRangeMin",'d',true);  param->SetDefaultValue(2.0); pipBeMomentumRangeCmd->SetParameter(param);
  param = new G4UIparameter("pipBEMomentumRangeMax",'d',true);  param->SetDefaultValue(9.0); pipBeMomentumRangeCmd->SetParameter(param);

  pipAlMomentumRangeCmd =  new G4UIcommand("/boone/crosssections/pionplus/pipAlMomentumRange",this);
  pipAlMomentumRangeCmd->SetGuidance("Set momentum range for BooNE Pion plus Cross section ");
  pipAlMomentumRangeCmd->SetGuidance("Usage: /boone/crosssections/pionplus/pipAlMomentumRange min max");
  param = new G4UIparameter("pipAlMomentumRangeMin",'d',true);  param->SetDefaultValue(2.0); pipAlMomentumRangeCmd->SetParameter(param);
  param = new G4UIparameter("pipAlMomentumRangeMax",'d',true);  param->SetDefaultValue(9.0); pipAlMomentumRangeCmd->SetParameter(param);

  pimBeMomentumRangeCmd =  new G4UIcommand("/boone/crosssections/pionminus/pimBeMomentumRange",this);
  pimBeMomentumRangeCmd->SetGuidance("Set momentum range for BooNE Pion plus Cross section ");
  pimBeMomentumRangeCmd->SetGuidance("Usage: /boone/crosssections/pionplus/pimBeMomentumRange min max");
  param = new G4UIparameter("pimBeMomentumRangeMin",'d',true);  param->SetDefaultValue(2.0); pimBeMomentumRangeCmd->SetParameter(param);
  param = new G4UIparameter("pimBeMomentumRangeMax",'d',true);  param->SetDefaultValue(9.0); pimBeMomentumRangeCmd->SetParameter(param);

  pimAlMomentumRangeCmd =  new G4UIcommand("/boone/crosssections/pionminus/pimAlMomentumRange",this);
  pimAlMomentumRangeCmd->SetGuidance("Set momentum range for BooNE Pion plus Cross section ");
  pimAlMomentumRangeCmd->SetGuidance("Usage: /boone/crosssections/pionminus/pimAlMomentumRange min max");
  param = new G4UIparameter("pimAlMomentumRangeMin",'d',true);  param->SetDefaultValue(2.0); pimAlMomentumRangeCmd->SetParameter(param);
  param = new G4UIparameter("pimAlMomentumRangeMax",'d',true);  param->SetDefaultValue(9.0); pimAlMomentumRangeCmd->SetParameter(param);


  // specify proton cross section parameters
  // Be total cross section
  totProBeXsecParCmd =  new G4UIcommand("/boone/crosssections/proton/totProBeXsecPar",this);
  totProBeXsecParCmd->SetGuidance("Set values of parameters for proton-Be tot xsec ");
  totProBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/proton/totProBeXsecPar c0 c1 c2 c3 c4");
  totProBeXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("totProBeXsecPar0",'d',true);  param->SetDefaultValue(307.825   ); totProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totProBeXsecPar1",'d',true);  param->SetDefaultValue(  0.897317); totProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totProBeXsecPar2",'d',true);  param->SetDefaultValue(  0.002670); totProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totProBeXsecPar3",'d',true);  param->SetDefaultValue( -2.59749 ); totProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totProBeXsecPar4",'d',true);  param->SetDefaultValue( -4.97272 ); totProBeXsecParCmd->SetParameter(param);

  // Be ine cross section
  ineProBeXsecParCmd =  new G4UIcommand("/boone/crosssections/proton/ineProBeXsecPar",this);
  ineProBeXsecParCmd->SetGuidance("Set values of parameters for proton-Be ine xsec ");
  ineProBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/proton/ineProBeXsecPar c0 c1 c2 c3 c4");
  ineProBeXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("ineProBeXsecPar0",'d',true);  param->SetDefaultValue(186.670   ); ineProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineProBeXsecPar1",'d',true);  param->SetDefaultValue(104.307   ); ineProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineProBeXsecPar2",'d',true);  param->SetDefaultValue( -1.03926 ); ineProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineProBeXsecPar3",'d',true);  param->SetDefaultValue( 10.3808  ); ineProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineProBeXsecPar4",'d',true);  param->SetDefaultValue(-15.8309  ); ineProBeXsecParCmd->SetParameter(param);

  // Be qel cross section
  qelProBeXsecParCmd =  new G4UIcommand("/boone/crosssections/proton/qelProBeXsecPar",this);
  qelProBeXsecParCmd->SetGuidance("Set values of parameters for proton-Be qel xsec ");
  qelProBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/proton/qelProBeXsecPar c0 c1 c2 c3 c4");
  qelProBeXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("qelProBeXsecPar0",'d',true);  param->SetDefaultValue(164.817   ); qelProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelProBeXsecPar1",'d',true);  param->SetDefaultValue( -4.00943 ); qelProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelProBeXsecPar2",'d',true);  param->SetDefaultValue(  0.408197); qelProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelProBeXsecPar3",'d',true);  param->SetDefaultValue( 21.4038  ); qelProBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelProBeXsecPar4",'d',true);  param->SetDefaultValue(-61.4512  ); qelProBeXsecParCmd->SetParameter(param);


  // Al total cross section
  totProAlXsecParCmd =  new G4UIcommand("/boone/crosssections/proton/totProAlXsecPar",this);
  totProAlXsecParCmd->SetGuidance("Set values of parameters for proton-Al tot xsec ");
  totProAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/proton/totProAlXsecPar c0 c1 c2 c3 c4");
  totProAlXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("totProAlXsecPar0",'d',true);  param->SetDefaultValue(760.290    ); totProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totProAlXsecPar1",'d',true);  param->SetDefaultValue( -0.0557496);	totProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totProAlXsecPar2",'d',true);  param->SetDefaultValue(  2.48478  );	totProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totProAlXsecPar3",'d',true);  param->SetDefaultValue(  6.17264  );	totProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totProAlXsecPar4",'d',true);  param->SetDefaultValue(-41.5967   );	totProAlXsecParCmd->SetParameter(param);

  // Al ine cross section
  ineProAlXsecParCmd =  new G4UIcommand("/boone/crosssections/proton/ineProAlXsecPar",this);
  ineProAlXsecParCmd->SetGuidance("Set values of parameters for proton-Al ine xsec ");
  ineProAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/proton/ineProAlXsecPar c0 c1 c2 c3 c4");
  ineProAlXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("ineProAlXsecPar0",'d',true);  param->SetDefaultValue(470.942    ); ineProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineProAlXsecPar1",'d',true);  param->SetDefaultValue( -0.258776 );	ineProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineProAlXsecPar2",'d',true);  param->SetDefaultValue(  2.42894  );	ineProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineProAlXsecPar3",'d',true);  param->SetDefaultValue( 48.8579   );	ineProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineProAlXsecPar4",'d',true);  param->SetDefaultValue(-87.1846   );	ineProAlXsecParCmd->SetParameter(param);

  // Al qel cross section
  qelProAlXsecParCmd =  new G4UIcommand("/boone/crosssections/proton/qelProAlXsecPar",this);
  qelProAlXsecParCmd->SetGuidance("Set values of parameters for proton-Al qel xsec ");
  qelProAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/proton/qelProAlXsecPar c0 c1 c2 c3 c4");
  qelProAlXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("qelProAlXsecPar0",'d',true);  param->SetDefaultValue( 255.728     ); qelProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelProAlXsecPar1",'d',true);  param->SetDefaultValue(   8.79205   ); qelProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelProAlXsecPar2",'d',true);  param->SetDefaultValue(   0.00239737); qelProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelProAlXsecPar3",'d',true);  param->SetDefaultValue(  32.2378    ); qelProAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelProAlXsecPar4",'d',true);  param->SetDefaultValue(-155.862     ); qelProAlXsecParCmd->SetParameter(param);


  // specify neutron cross section parameters
  // Be total cross section
  totNeuBeXsecParCmd =  new G4UIcommand("/boone/crosssections/neutron/totNeuBeXsecPar",this);
  totNeuBeXsecParCmd->SetGuidance("Set values of parameters for neutron-Be tot xsec ");
  totNeuBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/neutron/totNeuBeXsecPar c0 c1 c2 c3 c4");
  totNeuBeXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("totNeuBeXsecPar0",'d',true);  param->SetDefaultValue(307.825   ); totNeuBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totNeuBeXsecPar1",'d',true);  param->SetDefaultValue(  0.897317); totNeuBeXsecParCmd->SetParameter(param); 
  param = new G4UIparameter("totNeuBeXsecPar2",'d',true);  param->SetDefaultValue(  0.002670); totNeuBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totNeuBeXsecPar3",'d',true);  param->SetDefaultValue( -2.59749 ); totNeuBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totNeuBeXsecPar4",'d',true);  param->SetDefaultValue( -4.97272 ); totNeuBeXsecParCmd->SetParameter(param);

  // Be ine cross section
  ineNeuBeXsecParCmd =  new G4UIcommand("/boone/crosssections/neutron/ineNeuBeXsecPar",this);
  ineNeuBeXsecParCmd->SetGuidance("Set values of parameters for neutron-Be ine xsec ");
  ineNeuBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/neutron/ineNeuBeXsecPar c0 c1 c2 c3 c4");
  ineNeuBeXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("ineNeuBeXsecPar0",'d',true);  param->SetDefaultValue(186.670   ); ineNeuBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineNeuBeXsecPar1",'d',true);  param->SetDefaultValue(104.307   ); ineNeuBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineNeuBeXsecPar2",'d',true);  param->SetDefaultValue( -1.03926 ); ineNeuBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineNeuBeXsecPar3",'d',true);  param->SetDefaultValue( 10.3808  ); ineNeuBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineNeuBeXsecPar4",'d',true);  param->SetDefaultValue(-15.8309  ); ineNeuBeXsecParCmd->SetParameter(param);

  // Be qel cross section
  qelNeuBeXsecParCmd =  new G4UIcommand("/boone/crosssections/neutron/qelNeuBeXsecPar",this);
  qelNeuBeXsecParCmd->SetGuidance("Set values of parameters for neutron-Be qel xsec ");
  qelNeuBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/neutron/qelNeuBeXsecPar c0 c1 c2 c3 c4");
  qelNeuBeXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("qelNeuBeXsecPar0",'d',true);  param->SetDefaultValue(164.817   ); qelNeuBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelNeuBeXsecPar1",'d',true);  param->SetDefaultValue( -4.00943 ); qelNeuBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelNeuBeXsecPar2",'d',true);  param->SetDefaultValue(  0.408197); qelNeuBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelNeuBeXsecPar3",'d',true);  param->SetDefaultValue( 21.4038  ); qelNeuBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelNeuBeXsecPar4",'d',true);  param->SetDefaultValue(-61.4512  ); qelNeuBeXsecParCmd->SetParameter(param);


  // Al total cross section
  totNeuAlXsecParCmd =  new G4UIcommand("/boone/crosssections/neutron/totNeuAlXsecPar",this);
  totNeuAlXsecParCmd->SetGuidance("Set values of parameters for neutron-Al tot xsec ");
  totNeuAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/neutron/totNeuAlXsecPar c0 c1 c2 c3 c4");
  totNeuAlXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("totNeuAlXsecPar0",'d',true);  param->SetDefaultValue(760.290    ); totNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totNeuAlXsecPar1",'d',true);  param->SetDefaultValue( -0.0557496);	totNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totNeuAlXsecPar2",'d',true);  param->SetDefaultValue(  2.48478  );	totNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totNeuAlXsecPar3",'d',true);  param->SetDefaultValue(  6.17264  );	totNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totNeuAlXsecPar4",'d',true);  param->SetDefaultValue(-41.5967   );	totNeuAlXsecParCmd->SetParameter(param);

  // Al ine cross section
  ineNeuAlXsecParCmd =  new G4UIcommand("/boone/crosssections/neutron/ineNeuAlXsecPar",this);
  ineNeuAlXsecParCmd->SetGuidance("Set values of parameters for neutron-Al ine xsec ");
  ineNeuAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/neutron/ineNeuAlXsecPar c0 c1 c2 c3 c4");
  ineNeuAlXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("ineNeuAlXsecPar0",'d',true);  param->SetDefaultValue(470.942    ); ineNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineNeuAlXsecPar1",'d',true);  param->SetDefaultValue( -0.258776 );	ineNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineNeuAlXsecPar2",'d',true);  param->SetDefaultValue(  2.42894  );	ineNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineNeuAlXsecPar3",'d',true);  param->SetDefaultValue( 48.8579   );	ineNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("ineNeuAlXsecPar4",'d',true);  param->SetDefaultValue(-87.1846   );	ineNeuAlXsecParCmd->SetParameter(param);

  // Al qel cross section
  qelNeuAlXsecParCmd =  new G4UIcommand("/boone/crosssections/neutron/qelNeuAlXsecPar",this);
  qelNeuAlXsecParCmd->SetGuidance("Set values of parameters for neutron-Al qel xsec ");
  qelNeuAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/neutron/qelNeuAlXsecPar c0 c1 c2 c3 c4");
  qelNeuAlXsecParCmd->SetGuidance("Description: parametrization according to c0 + c1 p^c2 + c3 log^2 p + c4 log p");

  param = new G4UIparameter("qelNeuAlXsecPar0",'d',true);  param->SetDefaultValue( 255.728     ); qelNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelNeuAlXsecPar1",'d',true);  param->SetDefaultValue(   8.79205   ); qelNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelNeuAlXsecPar2",'d',true);  param->SetDefaultValue(   0.00239737); qelNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelNeuAlXsecPar3",'d',true);  param->SetDefaultValue(  32.2378    ); qelNeuAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelNeuAlXsecPar4",'d',true);  param->SetDefaultValue(-155.862     ); qelNeuAlXsecParCmd->SetParameter(param);


  // specify pion plus cross section parameters
  // Be total cross section
  totPipBeXsecParCmd =  new G4UIcommand("/boone/crosssections/pionplus/totPipBeXsecPar",this);
  totPipBeXsecParCmd->SetGuidance("Set values of parameters for pionplus-Be tot xsec ");
  totPipBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionplus/totPipBeXsecPar c0 c1 c2 c3 c4 c5 c6 c7");
  totPipBeXsecParCmd->SetGuidance("Description: parametrization according to");
  totPipBeXsecParCmd->SetGuidance("             CBW or 0.5 (1 + tanh( (p-c1)*c2 ) )* (c3 + c4 * p^c5 + c6* log^2 p + c7 log p)");
  totPipBeXsecParCmd->SetGuidance("             where CBW is the Carroll et al. BW function with Be)");
  totPipBeXsecParCmd->SetGuidance("             where c0  marks the transition between the two forms");

  param = new G4UIparameter("totPipBeXsecPar0",'d',true);  param->SetDefaultValue(  0.5     ); totPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipBeXsecPar1",'d',true);  param->SetDefaultValue(   0.81375);	totPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipBeXsecPar2",'d',true);  param->SetDefaultValue(   3.4176 );	totPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipBeXsecPar3",'d',true);  param->SetDefaultValue( 237.59   );	totPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipBeXsecPar4",'d',true);  param->SetDefaultValue( 111.29   );	totPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipBeXsecPar5",'d',true);  param->SetDefaultValue(  -4.1856 );	totPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipBeXsecPar6",'d',true);  param->SetDefaultValue(  -9.7922 );	totPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipBeXsecPar7",'d',true);  param->SetDefaultValue(   0.0    );	totPipBeXsecParCmd->SetParameter(param);

  // Be inelastic cross section
  inePipBeXsecParCmd =  new G4UIcommand("/boone/crosssections/pionplus/inePipBeXsecPar",this);
  inePipBeXsecParCmd->SetGuidance("Set values of parameters for pionplus-Be ine xsec ");
  inePipBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionplus/inePipBeXsecPar c0 c1 c2 c3 c4 c5 c6 c7 c8 c9");
  inePipBeXsecParCmd->SetGuidance("Description: parametrization according to");
  inePipBeXsecParCmd->SetGuidance("             0.5 * (1+tanh( (p-c3)*c4 ))* (c5 + c6 * p^c7 + c8* log^2 p + c9* log p) + c0 * BW(c1, c2)");
  inePipBeXsecParCmd->SetGuidance("             where BW is the Breit Wigner form with peak c1 and width c2)");

  param = new G4UIparameter("inePipBeXsecPar0",'d',true);  param->SetDefaultValue(  528.484    ); inePipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipBeXsecPar1",'d',true);  param->SetDefaultValue(    1.20055  ); inePipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipBeXsecPar2",'d',true);  param->SetDefaultValue(    0.232764 ); inePipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipBeXsecPar3",'d',true);  param->SetDefaultValue(    0.305002 ); inePipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipBeXsecPar4",'d',true);  param->SetDefaultValue(    4.58634  ); inePipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipBeXsecPar5",'d',true);  param->SetDefaultValue(  199.894    ); inePipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipBeXsecPar6",'d',true);  param->SetDefaultValue(  -31.0728   ); inePipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipBeXsecPar7",'d',true);  param->SetDefaultValue(   -2.06807  ); inePipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipBeXsecPar8",'d',true);  param->SetDefaultValue(  -17.2027   ); inePipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipBeXsecPar9",'d',true);  param->SetDefaultValue(    0.0      ); inePipBeXsecParCmd->SetParameter(param);


  // Be quasielastic cross section
  qelPipBeXsecParCmd =  new G4UIcommand("/boone/crosssections/pionplus/qelPipBeXsecPar",this);
  qelPipBeXsecParCmd->SetGuidance("Set values of parameters for pionplus-Be qel xsec ");
  qelPipBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionplus/qelPipBeXsecPar c0 c1 c2 c3 c4 c5 c6 c7 c8 c9");
  qelPipBeXsecParCmd->SetGuidance("Description: parametrization according to");
  qelPipBeXsecParCmd->SetGuidance("             0.5 * (1+ tanh( (p-c3)*c4 ))* (c5 + c6 * p^c7 + c8* log^2 p + c9 * log p) + c0 * BW(c1, c2)");
  qelPipBeXsecParCmd->SetGuidance("             where BW is the Breit Wigner form with peak c1 and width c2)");

  param = new G4UIparameter("qelPipBeXsecPar0",'d',true);  param->SetDefaultValue( 379.921   ); qelPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipBeXsecPar1",'d',true);  param->SetDefaultValue(   1.20055 ); qelPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipBeXsecPar2",'d',true);  param->SetDefaultValue(   0.5575  ); qelPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipBeXsecPar3",'d',true);  param->SetDefaultValue(   0.6345  ); qelPipBeXsecParCmd->SetParameter(param); 
  param = new G4UIparameter("qelPipBeXsecPar4",'d',true);  param->SetDefaultValue(   3.7844  ); qelPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipBeXsecPar5",'d',true);  param->SetDefaultValue(  -2.3773  ); qelPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipBeXsecPar6",'d',true);  param->SetDefaultValue( -81.8376  ); qelPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipBeXsecPar7",'d',true);  param->SetDefaultValue(  -2.7024  ); qelPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipBeXsecPar8",'d',true);  param->SetDefaultValue(   3.1726  ); qelPipBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipBeXsecPar9",'d',true);  param->SetDefaultValue(   0.0     ); qelPipBeXsecParCmd->SetParameter(param);

  // Al total cross section
  totPipAlXsecParCmd =  new G4UIcommand("/boone/crosssections/pionplus/totPipAlXsecPar",this);
  totPipAlXsecParCmd->SetGuidance("Set values of parameters for pionplus-Be tot xsec ");
  totPipAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionplus/totPipAlXsecPar c0 c1 c2 c3 c4 c5 c6 c7");
  totPipAlXsecParCmd->SetGuidance("Description: parametrization according to");
  totPipAlXsecParCmd->SetGuidance("             CBW or 0.5 * (1 + tanh( (p-c1)*c2 ))* (c3 + c4 * p^c5 + c6* log^2 p + c7 log p)");
  totPipAlXsecParCmd->SetGuidance("             where CBW is the Carroll et al. BW function, A = 27 for Al)");
  totPipAlXsecParCmd->SetGuidance("             where c0 marks the transition between the two forms");

  param = new G4UIparameter("totPipAlXsecPar0",'d',true);  param->SetDefaultValue(  0.5     ); totPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipAlXsecPar1",'d',true);  param->SetDefaultValue(   0.81375); totPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipAlXsecPar2",'d',true);  param->SetDefaultValue(   3.4176 ); totPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipAlXsecPar3",'d',true);  param->SetDefaultValue( 237.59   ); totPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipAlXsecPar4",'d',true);  param->SetDefaultValue( 111.29   ); totPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipAlXsecPar5",'d',true);  param->SetDefaultValue(  -4.1856 ); totPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipAlXsecPar6",'d',true);  param->SetDefaultValue(  -9.7922 ); totPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPipAlXsecPar7",'d',true);  param->SetDefaultValue(   0.0    ); totPipAlXsecParCmd->SetParameter(param);

  // Al inelastic cross section
  inePipAlXsecParCmd =  new G4UIcommand("/boone/crosssections/pionplus/inePipAlXsecPar",this);
  inePipAlXsecParCmd->SetGuidance("Set values of parameters for pionplus-Al ine xsec ");
  inePipAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionplus/inePipAlXsecPar c0 c1 c2 c3 c4 c5 c6 c7 c8 c9");
  inePipAlXsecParCmd->SetGuidance("Description: parametrization according to");
  inePipAlXsecParCmd->SetGuidance("             0.5 * ( 1 + tanh( (p-c3)*c4 ))* (c5 + c6 * p^c7 + c8* log^2 p + c9 * log p) + c0 * BW(c1, c2)");
  inePipAlXsecParCmd->SetGuidance("             where BW is the Breit Wigner form with peak c1 and width c2)");

  param = new G4UIparameter("inePipAlXsecPar0",'d',true);  param->SetDefaultValue(     510.66     ); inePipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipAlXsecPar1",'d',true);  param->SetDefaultValue(      1.1888    ); inePipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipAlXsecPar2",'d',true);  param->SetDefaultValue(      0.1851    ); inePipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipAlXsecPar3",'d',true);  param->SetDefaultValue(      0.295     ); inePipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipAlXsecPar4",'d',true);  param->SetDefaultValue(      2.3068    ); inePipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipAlXsecPar5",'d',true);  param->SetDefaultValue(   1537.40      ); inePipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipAlXsecPar6",'d',true);  param->SetDefaultValue(  -1109.43      ); inePipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipAlXsecPar7",'d',true);  param->SetDefaultValue(      0.056944  ); inePipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipAlXsecPar8",'d',true);  param->SetDefaultValue(     14.395     ); inePipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePipAlXsecPar9",'d',true);  param->SetDefaultValue(      0.0       ); inePipAlXsecParCmd->SetParameter(param);


  // Al quasielastic cross section
  qelPipAlXsecParCmd =  new G4UIcommand("/boone/crosssections/pionplus/qelPipAlXsecPar",this);
  qelPipAlXsecParCmd->SetGuidance("Set values of parameters for pionplus-Al qel xsec ");
  qelPipAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionplus/qelPipAlXsecPar c0 c1 c2 c3 c4 c5 c6 c7 c8 c9");
  qelPipAlXsecParCmd->SetGuidance("Description: parametrization according to");
  qelPipAlXsecParCmd->SetGuidance("             0.5 * (1 + tanh( (p-c3)*c4 ))* (c5 + c6 * p^c7 + c8* log^2 p + c9 * log p) + c0 * BW(c1, c2)");
  qelPipAlXsecParCmd->SetGuidance("             where BW is the Breit Wigner form with peak c1 and width c2)");


  param = new G4UIparameter("qelPipAlXsecPar0",'d',true);  param->SetDefaultValue( 229.399  ); qelPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipAlXsecPar1",'d',true);  param->SetDefaultValue(   1.1888 ); qelPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipAlXsecPar2",'d',true);  param->SetDefaultValue(   0.1887 ); qelPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipAlXsecPar3",'d',true);  param->SetDefaultValue(   0.6979 ); qelPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipAlXsecPar4",'d',true);  param->SetDefaultValue(   2.1343 ); qelPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipAlXsecPar5",'d',true);  param->SetDefaultValue(  40.3783 ); qelPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipAlXsecPar6",'d',true);  param->SetDefaultValue(  89.2019 ); qelPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipAlXsecPar7",'d',true);  param->SetDefaultValue(  -1.5745 ); qelPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipAlXsecPar8",'d',true);  param->SetDefaultValue(   0.3354 ); qelPipAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPipAlXsecPar9",'d',true);  param->SetDefaultValue(   0.0000 ); qelPipAlXsecParCmd->SetParameter(param);


  // specify pion minus cross section parameters
  // Be total cross section
  totPimBeXsecParCmd =  new G4UIcommand("/boone/crosssections/pionminus/totPimBeXsecPar",this);
  totPimBeXsecParCmd->SetGuidance("Set values of parameters for pionminus-Be tot xsec ");
  totPimBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionminus/totPimBeXsecPar c0 c1 c2 c3 c4 c5 c6 c7");
  totPimBeXsecParCmd->SetGuidance("Description: parametrization according to");
  totPimBeXsecParCmd->SetGuidance("             CBW or 0.5 * (1 + tanh( (p-c1)*c2 ))* (c3 + c4 * p^c5 + c6* log^2 p + c7 * log p)");
  totPimBeXsecParCmd->SetGuidance("             where CBW is the Carroll et al. BW function, A 9 for Be)");
  totPimBeXsecParCmd->SetGuidance("             where c0 marks the transition between the two forms");

  param = new G4UIparameter("totPimBeXsecPar0",'d',true);  param->SetDefaultValue(   0.600   ); totPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimBeXsecPar1",'d',true);  param->SetDefaultValue(   0.56733 );	totPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimBeXsecPar2",'d',true);  param->SetDefaultValue(   1.97133 );	totPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimBeXsecPar3",'d',true);  param->SetDefaultValue( 510.537   ) ;totPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimBeXsecPar4",'d',true);  param->SetDefaultValue(-212.193   );	totPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimBeXsecPar5",'d',true);  param->SetDefaultValue(   0.4417  );	totPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimBeXsecPar6",'d',true);  param->SetDefaultValue(  50.4741  );	totPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimBeXsecPar7",'d',true);  param->SetDefaultValue(   0.0     );	totPimBeXsecParCmd->SetParameter(param);

  // Be inelastic cross section
  inePimBeXsecParCmd =  new G4UIcommand("/boone/crosssections/pionminus/inePimBeXsecPar",this);
  inePimBeXsecParCmd->SetGuidance("Set values of parameters for pionminus-Be ine xsec ");
  inePimBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionminus/inePimBeXsecPar c0 c1 c2 c3 c4 c5 c6 c7 c8 c9");
  inePimBeXsecParCmd->SetGuidance("Description: parametrization according to");
  inePimBeXsecParCmd->SetGuidance("             0.5 * ( 1+ tanh( (p-c3)*c4 ))* (c5 + c6 * p^c7 + c8* log^2 p + c9 * log p) + c0 * BW(c1, c2)");
  inePimBeXsecParCmd->SetGuidance("             where BW is the Breit Wigner form with peak c1 and width c2)");

  param = new G4UIparameter("inePimBeXsecPar0",'d',true);  param->SetDefaultValue(  371.452   ); inePimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimBeXsecPar1",'d',true);  param->SetDefaultValue(    1.20055 ); inePimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimBeXsecPar2",'d',true);  param->SetDefaultValue(    0.2328  ); inePimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimBeXsecPar3",'d',true);  param->SetDefaultValue(    0.6000  ); inePimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimBeXsecPar4",'d',true);  param->SetDefaultValue(    2.8737  ); inePimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimBeXsecPar5",'d',true);  param->SetDefaultValue(   92.6620  ); inePimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimBeXsecPar6",'d',true);  param->SetDefaultValue(  112.213   ); inePimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimBeXsecPar7",'d',true);  param->SetDefaultValue( -0.486080  ); inePimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimBeXsecPar8",'d',true);  param->SetDefaultValue(    7.500   ); inePimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimBeXsecPar9",'d',true);  param->SetDefaultValue(    0.0     ); inePimBeXsecParCmd->SetParameter(param);


  // Be quasielastic cross section
  qelPimBeXsecParCmd =  new G4UIcommand("/boone/crosssections/pionminus/qelPimBeXsecPar",this);
  qelPimBeXsecParCmd->SetGuidance("Set values of parameters for pionminus-Be qel xsec ");
  qelPimBeXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionminus/qelPimBeXsecPar c0 c1 c2 c3 c4 c5 c6 c7 c8 c9");
  qelPimBeXsecParCmd->SetGuidance("Description: parametrization according to");
  qelPimBeXsecParCmd->SetGuidance("             0.5 * (1+tanh( (p-c3)*c4 ))* (c5 + c6 * p^c7 + c8* log^2 p + c9 * log p) + c0 * BW(c1, c2)");
  qelPimBeXsecParCmd->SetGuidance("             where BW is the Breit Wigner form with peak c1 and width c2)");

  param = new G4UIparameter("qelPimBeXsecPar0",'d',true);  param->SetDefaultValue( 188.971   ); qelPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimBeXsecPar1",'d',true);  param->SetDefaultValue(   1.20055 ); qelPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimBeXsecPar2",'d',true);  param->SetDefaultValue(   0.1851  ); qelPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimBeXsecPar3",'d',true);  param->SetDefaultValue(   0.6264  ); qelPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimBeXsecPar4",'d',true);  param->SetDefaultValue(   2.5043  ); qelPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimBeXsecPar5",'d',true);  param->SetDefaultValue(  -1.5594  ); qelPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimBeXsecPar6",'d',true);  param->SetDefaultValue(  46.4073  ); qelPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimBeXsecPar7",'d',true);  param->SetDefaultValue(  -0.6325  ); qelPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimBeXsecPar8",'d',true);  param->SetDefaultValue(   1.8743  ); qelPimBeXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimBeXsecPar9",'d',true);  param->SetDefaultValue(   0.0000  ); qelPimBeXsecParCmd->SetParameter(param);

  // Al total cross section
  totPimAlXsecParCmd =  new G4UIcommand("/boone/crosssections/pionminus/totPimAlXsecPar",this);
  totPimAlXsecParCmd->SetGuidance("Set values of parameters for pionminus-Be tot xsec ");
  totPimAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionminus/totPimAlXsecPar c0 c1 c2 c3 c4 c5 c6 c7");
  totPimAlXsecParCmd->SetGuidance("Description: parametrization according to");
  totPimAlXsecParCmd->SetGuidance("             CBW or 0.5 * (1+tanh( (p-c1)*c2 ))* (c3 + c4 * p^c5 + c6* log^2 p + c7*log p)");
  totPimAlXsecParCmd->SetGuidance("             where CBW is the Carroll et al. BW function, A =27 for Al)");
  totPimAlXsecParCmd->SetGuidance("             where c0 marks the transition between the two forms");

  param = new G4UIparameter("totPimAlXsecPar0",'d',true);  param->SetDefaultValue(  0.900   ); totPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimAlXsecPar1",'d',true);  param->SetDefaultValue(  0.9313  ); totPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimAlXsecPar2",'d',true);  param->SetDefaultValue(  3.1856  ); totPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimAlXsecPar3",'d',true);  param->SetDefaultValue(569.084   ); totPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimAlXsecPar4",'d',true);  param->SetDefaultValue(511.272   ); totPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimAlXsecPar5",'d',true);  param->SetDefaultValue( -3.791   ); totPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimAlXsecPar6",'d',true);  param->SetDefaultValue(-18.495   ); totPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("totPimAlXsecPar7",'d',true);  param->SetDefaultValue(  0.0     ); totPimAlXsecParCmd->SetParameter(param);

  // Al inelastic cross section
  inePimAlXsecParCmd =  new G4UIcommand("/boone/crosssections/pionminus/inePimAlXsecPar",this);
  inePimAlXsecParCmd->SetGuidance("Set values of parameters for pionminus-Al ine xsec ");
  inePimAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionminus/inePimAlXsecPar c0 c1 c2 c3 c4 c5 c6 c7 c8 c9");
  inePimAlXsecParCmd->SetGuidance("Description: parametrization according to");
  inePimAlXsecParCmd->SetGuidance("             0.5 * (1+tanh( (p-c3)*c4 ))* (c5 + c6 * p^c7 + c8* log^2 p + c9 * log p) + c0 * BW(c1, c2)");
  inePimAlXsecParCmd->SetGuidance("             where BW is the Breit Wigner form with peak c1 and width c2)");

  param = new G4UIparameter("inePimAlXsecPar0",'d',true);  param->SetDefaultValue(  346.568  ); inePimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimAlXsecPar1",'d',true);  param->SetDefaultValue(    1.1888 ); inePimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimAlXsecPar2",'d',true);  param->SetDefaultValue(    0.30478); inePimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimAlXsecPar3",'d',true);  param->SetDefaultValue(    0.8746 ); inePimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimAlXsecPar4",'d',true);  param->SetDefaultValue(    1.469  ); inePimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimAlXsecPar5",'d',true);  param->SetDefaultValue( 1532.00   ); inePimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimAlXsecPar6",'d',true);  param->SetDefaultValue( -848.623  ); inePimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimAlXsecPar7",'d',true);  param->SetDefaultValue(    0.7050 ); inePimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimAlXsecPar8",'d',true);  param->SetDefaultValue(  575.056  ); inePimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("inePimAlXsecPar9",'d',true);  param->SetDefaultValue(    0.000  ); inePimAlXsecParCmd->SetParameter(param);


  // Al quasielastic cross section
  qelPimAlXsecParCmd =  new G4UIcommand("/boone/crosssections/pionminus/qelPimAlXsecPar",this);
  qelPimAlXsecParCmd->SetGuidance("Set values of parameters for pionminus-Al qel xsec ");
  qelPimAlXsecParCmd->SetGuidance("Usage: /boone/crosssections/pionminus/qelPimAlXsecPar c0 c1 c2 c3 c4 c5 c6 c7 c8 c9");
  qelPimAlXsecParCmd->SetGuidance("Description: parametrization according to");
  qelPimAlXsecParCmd->SetGuidance("             0.5 * (1+tanh( (p-c3)*c4 ))* (c5 + c6 * p^c7 + c8* log^2 p + c9 * log p) + c0 * BW(c1, c2)");
  qelPimAlXsecParCmd->SetGuidance("             where BW is the Breit Wigner form with peak c1 and width c2)");

  param = new G4UIparameter("qelPimAlXsecPar0",'d',true);  param->SetDefaultValue( 129.119   ); qelPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimAlXsecPar1",'d',true);  param->SetDefaultValue(   1.1888  ); qelPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimAlXsecPar2",'d',true);  param->SetDefaultValue(   0.30478 ); qelPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimAlXsecPar3",'d',true);  param->SetDefaultValue(   0.633   ); qelPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimAlXsecPar4",'d',true);  param->SetDefaultValue(   2.199   ); qelPimAlXsecParCmd->SetParameter(param); 
  param = new G4UIparameter("qelPimAlXsecPar5",'d',true);  param->SetDefaultValue(  32.523   ); qelPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimAlXsecPar6",'d',true);  param->SetDefaultValue(  85.148   ); qelPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimAlXsecPar7",'d',true);  param->SetDefaultValue(  -1.225   ); qelPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimAlXsecPar8",'d',true);  param->SetDefaultValue(   1.383   ); qelPimAlXsecParCmd->SetParameter(param);
  param = new G4UIparameter("qelPimAlXsecPar9",'d',true);  param->SetDefaultValue(   0.000   ); qelPimAlXsecParCmd->SetParameter(param);

  // Additive cross-section offsets in mb for systematic variations
  // (/boone/crosssections/<species>/<tot|ine|qel><Pro|Neu|Pip|Pim><Be|Al>XsecOffset value)
  const char* offDirs[4]  = {"proton", "neutron", "pionplus", "pionminus"};
  const char* offTags[4]  = {"Pro", "Neu", "Pip", "Pim"};
  const char* offKinds[3] = {"tot", "ine", "qel"};
  const char* offMats[2]  = {"Be", "Al"};
  for (int i = 0; i < 4; ++i) for (int k = 0; k < 3; ++k) for (int m = 0; m < 2; ++m) {
    G4String key = G4String(offKinds[k]) + offTags[i] + offMats[m];
    G4String path = G4String("/boone/crosssections/") + offDirs[i] + "/" + key + "XsecOffset";
    G4UIcmdWithADouble* cmd = new G4UIcmdWithADouble(path, this);
    G4String guidance = "Additive offset (mb) applied to the parametrised " + key + " cross section";
    cmd->SetGuidance(guidance.c_str());
    cmd->SetParameterName("offset_mb", false);
    cmd->SetDefaultValue(0.);
    offsetCmds[cmd] = key;
  }
}

BooNEHadronCrossSectionsMessenger::~BooNEHadronCrossSectionsMessenger()
{
  for (std::map<G4UIcommand*, G4String>::iterator it = offsetCmds.begin(); it != offsetCmds.end(); ++it) delete it->first;

  delete booneCrossSectionsDirectory;
  delete booneProtonCrossSectionsDirectory;
  delete booneNeutronCrossSectionsDirectory;
  delete boonePionPlusCrossSectionsDirectory;
  delete boonePionMinusCrossSectionsDirectory;

  delete proBeMomentumRangeCmd;
  delete proAlMomentumRangeCmd;
  delete neuBeMomentumRangeCmd;
  delete neuAlMomentumRangeCmd;
  delete pipBeMomentumRangeCmd;
  delete pipAlMomentumRangeCmd;
  delete pimBeMomentumRangeCmd;
  delete pimAlMomentumRangeCmd;

  delete totProBeXsecParCmd;
  delete ineProBeXsecParCmd;
  delete qelProBeXsecParCmd;
  delete totProAlXsecParCmd;
  delete ineProAlXsecParCmd;
  delete qelProAlXsecParCmd;

  delete totNeuBeXsecParCmd;
  delete ineNeuBeXsecParCmd;
  delete qelNeuBeXsecParCmd;
  delete totNeuAlXsecParCmd;
  delete ineNeuAlXsecParCmd;
  delete qelNeuAlXsecParCmd;

  delete totPipBeXsecParCmd;
  delete inePipBeXsecParCmd;
  delete qelPipBeXsecParCmd;
  delete totPipAlXsecParCmd;
  delete inePipAlXsecParCmd;
  delete qelPipAlXsecParCmd;

  delete totPimBeXsecParCmd;
  delete inePimBeXsecParCmd;
  delete qelPimBeXsecParCmd;
  delete totPimAlXsecParCmd;
  delete inePimAlXsecParCmd;
  delete qelPimAlXsecParCmd;

}

void BooNEHadronCrossSectionsMessenger::SetNewValue(G4UIcommand * command,G4String newValues)
{

  G4Tokenizer next(newValues );

  std::map<G4UIcommand*, G4String>::iterator offset = offsetCmds.find(command);
  if (offset != offsetCmds.end()) {
    theBooNEHadronCrossSections->SetCrossSectionOffset(offset->second, G4UIcmdWithADouble::GetNewDoubleValue(newValues));
    return;
  }

  // momentum range
  if (command == proBeMomentumRangeCmd){
    G4double min = StoD(next());
    G4double max = StoD(next());
    theBooNEHadronCrossSections->SetProtonBeMomentumRange(min, max);
  }

  if (command == proAlMomentumRangeCmd){
    G4double min = StoD(next());
    G4double max = StoD(next());
    theBooNEHadronCrossSections->SetProtonAlMomentumRange(min, max);
  }

  if (command == neuBeMomentumRangeCmd){
    G4double min = StoD(next());
    G4double max = StoD(next());
    theBooNEHadronCrossSections->SetNeutronBeMomentumRange(min, max);
  }

  if (command == neuAlMomentumRangeCmd){
    G4double min = StoD(next());
    G4double max = StoD(next());
    theBooNEHadronCrossSections->SetNeutronAlMomentumRange(min, max);
  }

  if (command == pipBeMomentumRangeCmd){
    G4double min = StoD(next());
    G4double max = StoD(next());
    theBooNEHadronCrossSections->SetPionPlusBeMomentumRange(min, max);
  }

  if (command == pipAlMomentumRangeCmd){
    G4double min = StoD(next());
    G4double max = StoD(next());
    theBooNEHadronCrossSections->SetPionPlusAlMomentumRange(min, max);
  }


  if (command == pimBeMomentumRangeCmd){
    G4double min = StoD(next());
    G4double max = StoD(next());
    theBooNEHadronCrossSections->SetPionMinusBeMomentumRange(min, max);
  }

  if (command == pimAlMomentumRangeCmd){
    G4double min = StoD(next());
    G4double max = StoD(next());
    theBooNEHadronCrossSections->SetPionMinusAlMomentumRange(min, max);
  }


  // protons
  if (command == totProBeXsecParCmd){
    for(int i = 0; i < 5; i++)totProBeXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetpBeTotParameter(totProBeXsecPar, 5);
  }

  if (command == ineProBeXsecParCmd){
    for(int i = 0; i < 5; i++)ineProBeXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetpBeIneParameter(ineProBeXsecPar, 5);
  }

  if (command == qelProBeXsecParCmd){
    for(int i = 0; i < 5; i++)qelProBeXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetpBeQelParameter(qelProBeXsecPar, 5);
  }

  if (command == totProAlXsecParCmd){
    for(int i = 0; i < 5; i++)totProAlXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetpAlTotParameter(totProAlXsecPar, 5);
  }

  if (command == ineProAlXsecParCmd){
    for(int i = 0; i < 5; i++)ineProAlXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetpAlIneParameter(ineProAlXsecPar, 5);
  }

  if (command == qelProAlXsecParCmd){
    for(int i = 0; i < 5; i++)qelProAlXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetpAlQelParameter(qelProAlXsecPar, 5);
  }


  // neutrons
  if (command == totNeuBeXsecParCmd){
    for(int i = 0; i < 5; i++)totNeuBeXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetnBeTotParameter(totNeuBeXsecPar, 5);
  }

  if (command == ineNeuBeXsecParCmd){
    for(int i = 0; i < 5; i++)ineNeuBeXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetnBeIneParameter(ineNeuBeXsecPar, 5);
  }

  if (command == qelNeuBeXsecParCmd){
    for(int i = 0; i < 5; i++)qelNeuBeXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetnBeQelParameter(qelNeuBeXsecPar, 5);
  }

  if (command == totNeuAlXsecParCmd){
    for(int i = 0; i < 5; i++)totNeuAlXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetnAlTotParameter(totNeuAlXsecPar, 5);
  }

  if (command == ineNeuAlXsecParCmd){
    for(int i = 0; i < 5; i++)ineNeuAlXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetnAlIneParameter(ineNeuAlXsecPar, 5);
  }

  if (command == qelNeuAlXsecParCmd){
    for(int i = 0; i < 5; i++)qelNeuAlXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetnAlQelParameter(qelNeuAlXsecPar, 5);
  }



  // Pion plus
  if (command == totPipBeXsecParCmd){ 
    for(int i = 0; i < 8; i++)totPipBeXsecPar[i] = StoD(next()); 
    theBooNEHadronCrossSections->SetPipBeTotParameter(totPipBeXsecPar, 8);
  }

  if (command == inePipBeXsecParCmd){
    for(int i = 0; i < 10; i++)inePipBeXsecPar[i] = StoD(next()); 
    theBooNEHadronCrossSections->SetPipBeIneParameter(inePipBeXsecPar, 10);
  }

  if (command == qelPipBeXsecParCmd){
    for(int i = 0; i < 10; i++)qelPipBeXsecPar[i] = StoD(next()); 
    theBooNEHadronCrossSections->SetPipBeQelParameter(qelPipBeXsecPar, 10);
  }

  if (command == totPipAlXsecParCmd){
    for(int i = 0; i < 8; i++)totPipAlXsecPar[i] = StoD(next()); 
    theBooNEHadronCrossSections->SetPipAlTotParameter(totPipAlXsecPar, 8);
  }

  if (command == inePipAlXsecParCmd){
    for(int i = 0; i < 10; i++)inePipAlXsecPar[i] = StoD(next()); 
    theBooNEHadronCrossSections->SetPipAlIneParameter(inePipAlXsecPar, 10);
  }

  if (command == qelPipAlXsecParCmd){
    for(int i = 0; i < 10; i++)qelPipAlXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetPipAlQelParameter(qelPipAlXsecPar, 10);
  }


  // Pion minus
  if (command == totPimBeXsecParCmd){
    for(int i = 0; i < 8; i++)totPimBeXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetPimBeTotParameter(totPimBeXsecPar, 8);
  }

  if (command == inePimBeXsecParCmd){
    for(int i = 0; i < 10; i++)inePimBeXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetPimBeIneParameter(inePimBeXsecPar, 10);
  }

  if (command == qelPimBeXsecParCmd){
    for(int i = 0; i < 10; i++)qelPimBeXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetPimBeQelParameter(qelPimBeXsecPar, 10);
  }

  if (command == totPimAlXsecParCmd){
    for(int i = 0; i < 8; i++)totPimAlXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetPimAlTotParameter(totPimAlXsecPar, 8);
  }

  if (command == inePimAlXsecParCmd){
    for(int i = 0; i < 10; i++)inePimAlXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetPimAlIneParameter(inePimAlXsecPar, 10);
  }

  if (command == qelPimAlXsecParCmd){
    for(int i = 0; i < 10; i++)qelPimAlXsecPar[i] = StoD(next());
    theBooNEHadronCrossSections->SetPimAlQelParameter(qelPimAlXsecPar, 10);
  }

}
