#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#include "../interface/EGammaMvaEleEstimator.h"

void mvaTester(){

  EGammaMvaEleEstimator myMVATrig;
  std::vector<std::string> myManualCatWeigthsTrig;

  myManualCatWeigthsTrig.push_back("../plugins/MVAWeights/Electrons_BDTG_TrigV0_Cat1.weights.xml");
  myManualCatWeigthsTrig.push_back("../plugins/MVAWeights/Electrons_BDTG_TrigV0_Cat2.weights.xml");
  myManualCatWeigthsTrig.push_back("../plugins/MVAWeights/Electrons_BDTG_TrigV0_Cat3.weights.xml");
  myManualCatWeigthsTrig.push_back("../plugins/MVAWeights/Electrons_BDTG_TrigV0_Cat4.weights.xml");
  myManualCatWeigthsTrig.push_back("../plugins/MVAWeights/Electrons_BDTG_TrigV0_Cat5.weights.xml");
  myManualCatWeigthsTrig.push_back("../plugins/MVAWeights/Electrons_BDTG_TrigV0_Cat6.weights.xml");

  myMVATrig = new EGammaMvaEleEstimator();
  myMVATrig->initialize("BDT",
      EGammaMvaEleEstimator::kTrig,
      true,
      myManualCatWeigthsTrig);

  delete myMVATrig;

  cout<<"that's it"<<endl;
}
