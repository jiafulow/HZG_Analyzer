#ifndef DUMPER_H
#define DUMPER_H

#include <iostream>
#include <fstream>
#include "TClonesArray.h"
#include "TCMuon.h"
#include "TCElectron.h"
#include "TCPhoton.h"
#include "TLorentzVector.h"
#include "AnalysisParameters.h"
#include "ParticleSelectors.h"
#include "EGammaMvaEleEstimator.h"

class Dumper{
  public:
    Dumper(const Parameters& params, const Cuts& cuts, const ParticleSelector& psel);
    void  InitDumps();
    void  SetRho(float);
    void  SetRun(int);
    void  SetEvent(int);
    void  SetLumi(int);
    void  SetPv(const TVector3&);
    void  ElectronDump(const TCElectron& el, const TClonesArray& recoMuons, bool final);
    void  MVADumper(const TCElectron& ele, EGammaMvaEleEstimator* mvaMaker);
    void  MuonDump(const TCMuon& mu, bool final);
    void  PhotonDump(const TCPhoton& ph);
    void  PhotonDump2(const TCPhoton& ph, const TLorentzVector& lepton1, const TLorentzVector& lepton2);
    void  DataDumper(const TLorentzVector& lepton1, const TLorentzVector& lepton2, const TLorentzVector& gamma, float R9, float SCEta, float eta1, float eta2);
    void  DataDumper(const TLorentzVector& lepton1, const TLorentzVector& lepton2, const TLorentzVector& gamma, const TLorentzVector& uncorLepton1, const TLorentzVector& uncorLepton2, const TLorentzVector& uncorGamma, float R9, float SCEta, float eta1, float eta2);
    void  FinalDumper(const TLorentzVector& lepton1, const TLorentzVector& lepton2, const TLorentzVector& gamma, int catNum);
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
    Parameters _parameters;
    Cuts _cuts;
    ParticleSelector _psel;
    float _rhoFactor;
    int _runNumber;
    int _eventNumber;
    int _lumiSection;
    TVector3 _pv;

};


#endif

