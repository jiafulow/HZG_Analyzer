#ifndef ANALYSISPARAMETERS_H
#define ANALYSISPARAMETERS_H 

#include <stddef.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

class Parameters{
  public:
    string  selection;
    string  period;
    int     JC_LVL;
    string  abcd;
    string  suffix;
    string  dataname;
    string  jobCount;
    string  PU;

    bool VBFcuts;                 
    bool DYGammaVeto;            
    bool customPhotoID;         
    bool spikeVeto;            

    bool R9switch;            

    bool doEleMVA;           
    bool doHWWMVA;
    bool doHZZMVA;

    bool doLooseMuIso;      
    bool doAnglesMVA;      
    bool doAltMVA;      

    bool doPhotonPurityStudy;     
    bool doPhoMVA;

    bool doLeptonPrune;
    bool doVBF;
    
    bool doHighMass;

    bool doCT10;
    

    ///// debugging /////
    bool dumps;                  
    bool dataDumps;             
    int EVENTNUMBER;           
    bool doSync;

    //// energy corrections ////
    bool engCor;              
    bool doR9Cor;            
    bool doEleReg;          

    //// Scale Factors ////
    bool doScaleFactors;          
    bool doLumiXS;

    Parameters();
};

class Cuts{
  public:
    Cuts();
    float leadJetPt, trailJetPt, leadMuPt, trailMuPt, leadElePt, trailElePt, gPtOverMass, gPt,
      zMassLow, zMassHigh, metLow, metHigh, zgMassLow, zgMassHigh, mzPmzg, dR, ME, dRJet, dEtaJet, zepp, mjj, dPhiJet ;
    float EAMu[6]; 
    float EAEle[7]; 
    float EAPho[7][3];

    struct muIDCuts{
      float IsPF;
      float IsGLB;
      float IsTRK;
      float NormalizedChi2;
      float NumberOfValidMuonHits;
      float NumberOfMatchedStations;
      float NumberOfValidPixelHits;
      float TrackLayersWithMeasurement;
      float dxy;
      float dz;
      string cutName;
    } tightMuID, dalitzMuID;

    struct muIsoCuts{
      float chIso04;
      float nhIso04;
      float phIso04;
      float relCombIso04;
      string cutName;
    } tightMuIso, looseMuIso;

    struct elIDCuts{
      //broken into [0] barrel and [1] endcap
      float dEtaIn[2];
      float dPhiIn[2];
      float sigmaIetaIeta[2];
      float HadOverEm[2];
      float dxy[2];
      float dz[2];
      float fabsEPDiff[2];
      float ConversionMissHits[2];
      float PassedConversionProb[2];
      float dr03TkSumPt[2];
      float dr03EcalRecHitSumEt[2];
      float dr03HcalTowerSumEt[2];
      int   numberOfLostHits[2];
      string cutName;
    } vetoElID,looseElID, mediumElID, mvaPreElID;

    struct elIsoCuts{
      float chIso04;
      float nhIso04;
      float phIso04;
      float relCombIso04;
      float chIso03;
      float nhIso03;
      float phIso03;
      float relCombIso03;
      string cutName;
    } looseElIso, mediumElIso;

    struct phIDCuts{
      //broken into [0] barrel and [1] endcap
      float PassedEleSafeVeto[2];
      float HadOverEm[2];
      float sigmaIetaIeta[2];
      float HcalIso[2];
      float TrkIso[2];
      float ChPfIso[2];
      string cutName;
    } loosePhID, mediumPhID, preSelPhID;

    struct phIsoCuts{
      float chIso03[2];
      float nhIso03[2];
      float phIso03[2];
      float relCombIso03[2];
      string cutName;
    } loosePhIso, mediumPhIso;

    struct phMVACuts{
      float mvaValCat1;
      float mvaValCat2;
      float mvaValCat3;
      float mvaValCat4;
      string cutName;
    } catPhMVAID, noCatPhMVAID;

    struct elMVACuts{
      float mvaVal[6];
      float pt[2];
      float eta[3];
      int missHits[6];
      float sip[6];
      int conversionVeto[6];
      string cutName;
    } hzzMVAID, hwwMVAID, hzgMVAID;

    struct jetIDCuts{
      float betaStarC[2];
      float dR2Mean[4];
      string cutName;
    } vbfJetID;


    ~Cuts();
    void InitEA(string);
};





#endif
