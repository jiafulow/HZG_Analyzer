#ifndef PARTICLESELECTORS_H 
#define PARTICLESELECTORS_H 

#include <memory>
#include "TCMuon.h"
#include "TCElectron.h"
#include "TCPhoton.h"
#include "TCGenParticle.h"
#include "LeptonScaleCorrections.h"
#include "TRandom3.h"
#include "AnalysisParameters.h"

class ParticleSelector {
  public:
    ParticleSelector(){};
    ParticleSelector(Cuts* cuts, bool isRealData, int runNumber, TRandom3* rEl);
    bool FindGoodZElectron(vector<TCElectron*> electronList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* ZP4, float* eta1, float* eta2, int* int1, int* int2); 
    bool FindGoodZMuon(vector<TCMuon*> muonList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* ZP4, int* int1, int* int2); 
    bool FindGoodZElectron(vector<TCElectron*> electronList, vector<TCElectron*> uncorElectronList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* uncorLepton1, TLorentzVector* uncorLepton2, TLorentzVector* ZP4, float* eta1, float* eta2, int* int1, int* int2); 
    bool FindGoodZMuon(vector<TCMuon*> muonList, vector<TCMuon*> uncorMuonList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* uncorLepton1, TLorentzVector* uncorLepton2, TLorentzVector* ZP4, int* int1, int* int2); 

  private:
    Cuts* _cuts;
    bool _isRealData;
    int _runNumber;
    TRandom3* _rEl;
};




#endif

