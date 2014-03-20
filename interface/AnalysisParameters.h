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

    const bool VBFcuts;                 
    const bool DYGammaVeto;            
    const bool customPhotoID;         
    const bool spikeVeto;            

    const bool R9switch;            

    const bool doEleMVA;           

    const bool doLooseMuIso;      
    const bool doAnglesMVA;      

    const bool doPhotonPurityStudy;     

    ///// debugging dumps /////
    const bool dumps;                  
    const bool dataDumps;             
    const int EVENTNUMBER;           

    //// energy corrections ////
    const bool engCor;              
    const bool doR9Cor;            
    const bool doEleReg;          

    //// Scale Factors ////
    const bool doScaleFactors;          
    const bool doLumiXS;

    Parameters();
};

class Cuts{
  public:
    Cuts();
    const float leadJetPt, trailJetPt, leadMuPt, trailMuPt, leadElePt, trailElePt, gPtOverMass, gPt,
      zMassLow, zMassHigh, metLow, metHigh, zgMassLow, zgMassHigh, mzPmzg, dR, ME, dRJet, dEtaJet, zepp, mjj, dPhiJet ;
    float EAMu[6]; 
    float EAEle[7]; 
    float EAPho[7][3];

    struct muIDCuts{
      float IsPF;
      float IsGLB;
      float NormalizedChi2;
      float NumberOfValidMuonHits;
      float NumberOfMatchedStations;
      float NumberOfValidPixelHits;
      float TrackLayersWithMeasurement;
      float dxy;
      float dz;
      string cutName;
    } tightMuID;

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
    } vetoElID,looseElID,mvaPreElID;

    struct elIsoCuts{
      float chIso04;
      float nhIso04;
      float phIso04;
      float relCombIso04;
      string cutName;
    } looseElIso;

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
    } catPhMVAID;

    struct jetIDCuts{
      float betaStarC[2];
      float dR2Mean[4];
      string cutName;
    } vbfJetID;


    ~Cuts();
    void InitEA(string);
};





#endif
