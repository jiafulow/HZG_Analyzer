#ifndef DUMPER_H
#define DUMPER_H

#include "AnalysisParameters.h"
#include "ParticleSelectors.h"
#include "EGammaMvaEleEstimator.h"

class Dumper{
  public:
    Dumper();
    void  InitDumps();
    void  SetCuts(Cuts* cuts);
    void  SetPSelect(ParticleSelector*);
    void  SetRho(float);
    void  SetRun(int);
    void  SetEvent(int);
    void  SetLumi(int);
    void  SetPv(TVector3*);
    void  ElectronDump(TCElectron *el, TClonesArray* recoMuons, bool final);
    void  MVADumper(TCElectron *ele, EGammaMvaEleEstimator* mvaMaker);
    void  MuonDump(TCMuon *mu, bool final);
    void  PhotonDump(TCPhoton *ph);
    void  PhotonDump2(TCPhoton *ph, TLorentzVector lepton1, TLorentzVector lepton2);
    void  DataDumper(TLorentzVector* lepton1, TLorentzVector* lepton2, TLorentzVector* gamma, float R9, float SCEta, float eta1, float eta2);
    void  DataDumper(TLorentzVector* lepton1, TLorentzVector* lepton2, TLorentzVector* gamma, TLorentzVector* uncorLepton1, TLorentzVector* uncorLepton2, TLorentzVector* uncorGamma, float R9, float SCEta, float eta1, float eta2);
    void  FinalDumper(TLorentzVector* lepton1, TLorentzVector* lepton2, TLorentzVector* gamma, int catNum);
    void  CloseDumps();
  private:
    ofstream muDump1;
    ofstream elDump2;
    ofstream elDumpMVA;
    ofstream phDump1;
    ofstream phDump2;

    ofstream muDumpFinal;
    ofstream elDumpFinal;

    ofstream dataDump;

    ofstream finalDump;

    bool electronDump;
    bool muonDump;
    Cuts* _cuts;
    ParticleSelector* _psel;
    float _rhoFactor;
    int _runNumber;
    int _eventNumber;
    int _lumiSection;
    TVector3* _pv;

};


#endif

