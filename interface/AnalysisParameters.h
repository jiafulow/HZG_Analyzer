#ifndef ANALYSISPARAMETERS_H
#define ANALYSISPARAMETERS_H 

#include <stddef.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

namespace parameters
{
  string  selection("lol");
  string  period("lol");
  int     JC_LVL(0);
  string  abcd("lol");
  string  suffix("lol");
  string  dataname("lol");
  string  jobCount("lol");

  const bool VBFcuts                 = false;
  const bool DYGammaVeto             = true;
  const bool customPhotoID           = false;
  const bool spikeVeto               = true;

  const bool R9switch                = false;

  const bool doEleMVA                = true;

  const bool doLooseMuIso            = true;
  const bool doAnglesMVA             = false;

  const bool doPhotonPurityStudy     = false;

  ///// debugging dumps /////
  const bool dumps                   = true;
  const bool dataDumps               = false;
  const int EVENTNUMBER              = -999;

  //// energy corrections ////
  const bool engCor                  = true;
  const bool doR9Cor                 = true;
  const bool doEleReg                = true;

  //// Scale Factors ////
  const bool doScaleFactors          = true;
  const bool doLumiXS                = false;
}

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
      string cutName;
    } vetoElID,looseElID;

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
      string cutName;
    } loosePhID, mediumPhID;

    struct phIsoCuts{
      float chIso03[2];
      float nhIso03[2];
      float phIso03[2];
      float relCombIso03[2];
      string cutName;
    } loosePhIso, mediumPhIso;

    struct jetIDCuts{
      float betaStarC[2];
      float dR2Mean[4];
      string cutName;
    } vbfJetID;

    ~Cuts();
    void InitEA(string);
};





#endif
