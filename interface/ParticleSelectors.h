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
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#include "AnalysisParameters.h"
#include "ZGAngles.h"

class ParticleSelector {
  public:
    ParticleSelector(const Parameters& parameters, const Cuts& cuts, bool isRealData, int runNumber);
    void SetPv(const TVector3&);
    void SetRho(float);
    void SetEventNumber(int);
    bool FindGoodZElectron(const vector<TCElectron>& electronList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& ZP4, float& eta1, float& eta2, int& int1, int& int2); 
    bool FindGoodZMuon(const vector<TCMuon>& muonList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& ZP4, int& int1, int& int2); 
    bool FindGoodZMuon(const vector<TCMuon>& muonList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& ZP4, int& int1, int& int2, float diLepMass); 
    bool FindGoodZMuon(const vector<TCMuon>& muonList1, const vector<TCMuon>& muonList2, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& ZP4, int& int1, int& int2, float diLepMass); 
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

    void FindGenParticles(const TClonesArray& genParticles, const TClonesArray& _recoPhotons, vector<TCGenParticle>& vetoPhotons, genHZGParticles& _genHZG,bool vetoDY);
    void CleanUpGen(genHZGParticles& _genHZG);

    bool PassMuonID(const TCMuon& mu, const Cuts::muIDCuts& cutLevel);
    bool PassMuonIso(const TCMuon& mu, const Cuts::muIsoCuts& cutLevel);
    bool PassElectronID(const TCElectron& el, const Cuts::elIDCuts& cutLevel, const TClonesArray&);
    bool PassElectronIso(const TCElectron& el, const Cuts::elIsoCuts& cutLevel, float EAEle[7]);
    bool PassPhotonID(const TCPhoton& ph, const Cuts::phIDCuts& cutLevel);
    bool PassPhotonIso(const TCPhoton& ph, const Cuts::phIsoCuts& cutLevel, float EAPho[7][3]);
    bool PassPhotonMVA(const TCPhoton& ph, const Cuts::phMVACuts& cutLevel, const bool isGoodLeptons);
    bool PassJetID(const TCJet& jet, int nVtx, const Cuts::jetIDCuts& cutLevel);
    float   Zeppenfeld(const TLorentzVector& p, const TLorentzVector& pj1, const TLorentzVector& pj2);

  private:
    Parameters _parameters;
    Cuts _cuts;
    bool _isRealData;
    int _runNumber;
    TVector3 _pv;
    float _rhoFactor;
    int _evtnum;
};




#endif

