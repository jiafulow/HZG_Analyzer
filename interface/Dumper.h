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
    void  ElectronDump(const TCElectron& el, const TClonesArray& recoMuons, int dnum);
    void  MuonDump(const TCMuon& mu, int dnum);
    void  PhotonDump(const TCPhoton& ph, int dnum);
    void  CloseDumps();
    
  private:
    ofstream lepDump1;
    ofstream lepDump2;
    ofstream phoDump1;
    ofstream phoDump2;
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

