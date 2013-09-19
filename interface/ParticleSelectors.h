#ifndef PARTICLESELECTORS_H 
#define PARTICLESELECTORS_H 

#include <memory>
#include "TCMuon.h"
#include "TCElectron.h"
#include "TCPhoton.h"
#include "TCGenParticle.h"
#include "LeptonScaleCorrections.h"
#include "TRandom3.h"
#include "TClonesArray.h"
#include "AnalysisParameters.h"
#include "ZGAngles.h"

class ParticleSelector {
  public:
    ParticleSelector(){};
    ParticleSelector(Cuts* cuts, bool isRealData, int runNumber, TRandom3* rEl);
    void SetPv(TVector3*);
    void SetRho(float);
    bool FindGoodZElectron(vector<TCElectron*> electronList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* ZP4, float* eta1, float* eta2, int* int1, int* int2); 
    bool FindGoodZMuon(vector<TCMuon*> muonList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* ZP4, int* int1, int* int2); 
    bool FindGoodZElectron(vector<TCElectron*> electronList, vector<TCElectron*> uncorElectronList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* uncorLepton1, TLorentzVector* uncorLepton2, TLorentzVector* ZP4, float* eta1, float* eta2, int* int1, int* int2); 
    bool FindGoodZMuon(vector<TCMuon*> muonList, vector<TCMuon*> uncorMuonList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* uncorLepton1, TLorentzVector* uncorLepton2, TLorentzVector* ZP4, int* int1, int* int2); 

    static bool P4SortCondition(const TLorentzVector* p1, const TLorentzVector* p2) {return (p1->Pt() > p2->Pt());} 
    struct genHZGParticles{
      TCGenParticle* h;
      TCGenParticle* z;
      TCGenParticle* w;
      TCGenParticle* g;
      TCGenParticle* lp;
      TCGenParticle* lm;
    }genHZG;

    void FindGenParticles(TClonesArray* genParticles, string selection, vector<TCGenParticle*>& vetoPhotons, genHZGParticles& _genHZG);
    void CleanUpGen(genHZGParticles& _genHZG);

    bool PassMuonID(TCMuon *mu, Cuts::muIDCuts cutLevel);
    bool PassMuonIso(TCMuon *mu, Cuts::muIsoCuts cutLevel);
    bool PassElectronID(TCElectron *el, Cuts::elIDCuts cutLevel, TClonesArray*);
    bool PassElectronIso(TCElectron *el, Cuts::elIsoCuts cutLevel, float EAEle[7]);
    bool PassPhotonID(TCPhoton *ph, Cuts::phIDCuts cutLevel);
    bool PassPhotonIso(TCPhoton *ph, Cuts::phIsoCuts cutLevel, float EAPho[7][3]);

  private:
    Cuts* _cuts;
    bool _isRealData;
    int _runNumber;
    TRandom3* _rEl;
    TVector3* _pv;
    float _rhoFactor;
};




#endif

