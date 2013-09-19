#ifndef DUMPER_H
#define DUMPER_H

#include "AnalysisParameters.h"
#include "ParticleSelectors.h"

class Dumper{
  public:
    void  InitDumps();
    void  ElectronDump(TCElectron *el, Cuts::elIDCuts cutLevelID, Cuts::elIsoCuts cutLevelIso, float EAEle[7], ofstream & dump);
    void  MVADumper(TCElectron *ele, EGammaMvaEleEstimator* mvaMaker, double rhoFactor, Cuts::elIsoCuts cutLevelIso, float EAEle[7], ofstream & dump);
    void  MuonDump(TCMuon *mu, Cuts::muIDCuts cutLevelID, Cuts::muIsoCuts cutLevelIso, ofstream & dump);
    void  PhotonDump(TCPhoton *ph, Cuts::phIDCuts cutLevelID, Cuts::phIsoCuts cutLevelIso, float EAPho[7][3], ofstream & dump);
    void  PhotonDump2(TCPhoton *ph, Cuts::phIDCuts cutLevelID, Cuts::phIsoCuts cutLevelIso, float EAPho[7][3], TLorentzVector lepton1, TLorentzVector lepton2, ofstream & dump);
    void  DataDumper(TLorentzVector* lepton1, TLorentzVector* lepton2, TLorentzVector* gamma, float R9, float SCEta, ofstream & dump, float eta1, float eta2);
    void  DataDumper(TLorentzVector* lepton1, TLorentzVector* lepton2, TLorentzVector* gamma,
        TLorentzVector* uncorLepton1, TLorentzVector* uncorLepton2, TLorentzVector* uncorGamma, float R9, float SCEta, ofstream & dump, float eta1, float eta2);
    void  FinalDumper(TLorentzVector* lepton1, TLorentzVector* lepton2, TLorentzVector* gamma, int catNum, ofstream & dump);
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

}


#endif

