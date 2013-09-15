#ifndef ANALYSISPARAMETERS_H
#define ANALYSISPARAMETERS_H

#include <stddef.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

namespace parameters
{
  string  selection;
  string  period;
  int     JC_LVL;
  string  abcd;
  string  suffix;

  bool VBFcuts         = false;
  bool DYGammaVeto    = true;
  bool customPhotoID  = false;
  bool spikeVeto      = true;

  bool R9switch       = false;

  bool doEleMVA       = true;

  bool doLooseMuIso   = true;
  bool doAnglesMVA = false;

  ///// debugging dumps /////
  bool dumps = false;
  bool dataDumps = false;
  int EVENTNUMBER = -999;

  //// energy corrections ////
  bool engCor = true;
  bool doR9Cor = true;
  bool doEleReg  = true;

  //// Scale Factors ////
  bool doScaleFactors = true;
  bool doLumiXS= false;
}

class Cuts{
  public:
    static Cuts* Instance(string);
    const float leadJetPt, trailJetPt, leadMuPt, trailMuPt, leadElePt, trailElePt, gPtOverMass, gPt,
      zMassLow, zMassHigh, metLow, metHigh, zgMassLow, zgMassHigh, mzPmzg, dR, ME ;
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
  private:
    Cuts();
    void InitEA(string);
    static Cuts* m_pInstance;
};





#endif
