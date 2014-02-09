#ifndef PARTICLESELECTORS_H 
#define PARTICLESELECTORS_H 

#include <memory>
#include "TCMuon.h"
#include "TCElectron.h"
#include "TCPhoton.h"
#include "TCGenParticle.h"
#include "TCJet.h"
#include "LeptonScaleCorrections.h"
#include "TRandom3.h"
#include "TClonesArray.h"
#include "AnalysisParameters.h"
#include "ZGAngles.h"

class ParticleSelector {
  public:
    ParticleSelector(const Parameters& parameters, const Cuts& cuts, bool isRealData, int runNumber, const TRandom3& rEl);
    void SetPv(const TVector3&);
    void SetRho(float);
    void SetEventNumber(int);
    bool FindGoodZElectron(const vector<TCElectron>& electronList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& ZP4, float& eta1, float& eta2, int& int1, int& int2); 
    bool FindGoodZMuon(const vector<TCMuon>& muonList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& ZP4, int& int1, int& int2); 
    bool FindGoodZElectron(vector<TCElectron>& electronList, const vector<TCElectron>& uncorElectronList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& uncorLepton1, TLorentzVector& uncorLepton2, TLorentzVector& ZP4, float& eta1, float& eta2, int& int1, int& int2); 
    bool FindGoodZMuon(const vector<TCMuon>& muonList, const vector<TCMuon>& uncorMuonList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& uncorLepton1, TLorentzVector& uncorLepton2, TLorentzVector& ZP4, int& int1, int& int2); 
    bool FindGoodDiJets(const vector<TCJet>& jetList, const TCPhysObject& lepton1, const TCPhysObject& lepton2, const TLorentzVector& gamma, TCJet& jet1, TCJet& jet2); 
    bool FindGoodPhoton(const vector<TCPhoton>& photonList, TCPhoton& gamma, const TCPhysObject& lepton1, const TCPhysObject& lepton2, float& scEta, const vector<TCGenParticle>& vetoPhotons);

    static bool P4SortCondition(const TLorentzVector& p1, const TLorentzVector& p2) {return (p1.Pt() > p2.Pt());} 
    struct genHZGParticles{
      genHZGParticles():h(0),z(0),w(0),g(0),lp(0),lm(0){};
      TCGenParticle* h;
      TCGenParticle* z;
      TCGenParticle* w;
      TCGenParticle* g;
      TCGenParticle* lp;
      TCGenParticle* lm;
    }genHZG;

    void FindGenParticles(const TClonesArray& genParticles, vector<TCGenParticle>& vetoPhotons, genHZGParticles& _genHZG);
    void CleanUpGen(genHZGParticles& _genHZG);

    bool PassMuonID(const TCMuon& mu, const Cuts::muIDCuts& cutLevel);
    bool PassMuonIso(const TCMuon& mu, const Cuts::muIsoCuts& cutLevel);
    bool PassElectronID(const TCElectron& el, const Cuts::elIDCuts& cutLevel, const TClonesArray&);
    bool PassElectronIso(const TCElectron& el, const Cuts::elIsoCuts& cutLevel, float EAEle[7]);
    bool PassPhotonID(const TCPhoton& ph, const Cuts::phIDCuts& cutLevel);
    bool PassPhotonIso(const TCPhoton& ph, const Cuts::phIsoCuts& cutLevel, float EAPho[7][3]);
    bool PassJetID(const TCJet& jet, int nVtx, const Cuts::jetIDCuts& cutLevel);
    float   Zeppenfeld(const TLorentzVector& p, const TLorentzVector& pj1, const TLorentzVector& pj2);

  private:
    Parameters _parameters;
    Cuts _cuts;
    bool _isRealData;
    int _runNumber;
    TRandom3 _rEl;
    TVector3 _pv;
    float _rhoFactor;
    int _evtnum;
};




#endif

