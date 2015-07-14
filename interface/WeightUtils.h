/*
   Utilities for retrieving weights for PU,etc.
 */

#ifndef _WeightUtils_H
#define _WeightUtils_H

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "TROOT.h"
#include "TObject.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TGraphErrors.h"
#include "AnalysisParameters.h"
//#include "../../src/TCJet.h"

using namespace std;

class WeightUtils: public TObject {
    public:
        virtual ~WeightUtils() {};
        WeightUtils(const Parameters& params, bool isRealData, int runNumber);
        void  Initialize();
        void  SetRunNumber(int runNumber);
        void  SetIsRealData(bool isRealData);

        float PUWeight(float nPU);
        float MuonSelectionWeight(TLorentzVector l1);
        float ElectronSelectionWeight(TLorentzVector l1);
        float GammaSelectionWeight(TLorentzVector l1, float SCEta);
        float GammaSelectionWeight(TLorentzVector l1, int cat);
        float HqtWeight(TLorentzVector l1);

        float ElectronTriggerWeight(TLorentzVector l1, TLorentzVector l2, bool approx);
        float MuonTriggerWeight(TLorentzVector l1, TLorentzVector l2);
        float MuonTriggerWeightV2(TLorentzVector l1, TLorentzVector l2);
        float PhotonFakeWeight(float eta, float pt);



    private:
        //input parameters
        Parameters _params;
        bool   _isRealData;
        int _runNumber;

        //sources
        TFile * _inFileS6to2011;
        TFile * _inFileS6to2012;       
        TFile * _inFileS7to2011;
        TFile * _inFileS7to2012;
        TFile * _inFileS6to2011obs;
        TFile * _inFileS7to2012obs;
        TFile * _inFileS7to2012obsV2;
        TFile * _inFileS10to2012ABTrue;
        TFile * _inFileS10to2012CDTrue;
        TFile * _inFileS10to2012ABCDTrue;
        TFile * _inFileS10to2012ABCDTrue_73500;
        TFile * _inFileSomeTuneto2012ABCDTrue;

        TFile * _inFileRD1to2012ABCDTrue;
        TFile * _inFileRD1to2012ABCDTrue_6_40;

        TFile * _inFileRD1to2012ABTrue;
        TFile * _inFileRD1to2012CTrue;
        TFile * _inFileRD1to2012DTrue;

        TFile * _inFileRD1to2012ABCDMuMuTrue;
        TFile * _inFileRD1to2012ABCDEETrue;

        TFile *_MuTrig2011;

        TFile *_kFactors;

        TFile *_PhoMedWP2012;
        TFile *_PhoMedWPveto2012;

        TFile *_PhoJan22RD1_2012;

        TFile *_EleMoriondWP2012;
        TFile *_EleLegacyWP2012;
        TFile *_EleTightMVAWP;

        TFile *_inFileMuonID2012;
        TFile *_inFileMuonISO2012;
        TFile *_inFileMuonTrig2012;

        TFile *_inFileEl_ID_Data;
        TFile *_inFileEl_ID_MC;
        TFile *_inFileEl_Iso_Data;
        TFile *_inFileEl_Iso_MC;

        TFile *_inFileEl_ID_Data_v2;
        TFile *_inFileEl_ID_MC_v2;
        TFile *_inFileEl_Iso_Data_v2;
        TFile *_inFileEl_Iso_MC_v2;
        TFile *_inFileEl_ID_Data_Low_v2;
        TFile *_inFileEl_ID_MC_Low_v2;
        TFile *_inFileEl_Iso_Data_Low_v2;
        TFile *_inFileEl_Iso_MC_Low_v2;

        TFile *_inFileEl_LeadingTrigger_Data_v2;
        TFile *_inFileEl_LeadingTrigger_MC_v2;
        TFile *_inFileEl_TrailingTrigger_Data_v2;
        TFile *_inFileEl_TrailingTrigger_MC_v2;
        TFile *_inFileEl_LeadingTrigger_Data_Low_v2;
        TFile *_inFileEl_LeadingTrigger_MC_Low_v2;
        TFile *_inFileEl_TrailingTrigger_Data_Low_v2;
        TFile *_inFileEl_TrailingTrigger_MC_Low_v2;


        TH2D * _HLTMu17Mu8_2011;

        TH1F * h1_S6to2011;
        TH1F * h1_S6to2012; 
        TH1F * h1_S7to2011; 
        TH1F * h1_S7to2012; 
        TH1F * h1_S7to2012obs; 
        TH1F * h1_S7to2012obsV2; 
        TH1F * h1_S6to2011obs;
        TH1F * h1_S10to2012ABTrue;
        TH1F * h1_S10to2012CDTrue;
        TH1F * h1_S10to2012ABCDTrue;
        TH1F * h1_S10to2012ABCDTrue_73500;
        TH1F * h1_SomeTuneto2012ABCDTrue;
        TH1F * h1_RD1to2012ABCDTrue;
        TH1F * h1_RD1to2012ABCDTrue_6_40;

        TH1F * h1_RD1to2012ABTrue;
        TH1F * h1_RD1to2012CTrue;
        TH1F * h1_RD1to2012DTrue;

        TH1F * h1_RD1to2012ABMuMuTrue;
        TH1F * h1_RD1to2012CMuMuTrue;
        TH1F * h1_RD1to2012DMuMuTrue;

        TH1F * h1_RD1to2012ABEETrue;
        TH1F * h1_RD1to2012CEETrue;
        TH1F * h1_RD1to2012DEETrue;

        TH1D * kfact120_0;
        TH1D * kfact125_0;
        TH1D * kfact130_0;
        TH1D * kfact135_0;
        TH1D * kfact140_0;
        TH1D * kfact145_0;
        TH1D * kfact150_0;

        TH2F * h1_PhoMedWP2012;
        TH2F * h1_PhoMedWPveto2012;

        TH2F * h1_EleMoriondWP2012;
        TH2F * h1_EleLegacyWP2012;
        TH2F * h1_EleTightMVAWP;

        TH1F * h1_PhoMedWPJan22RD1_2012;
        TH1F * h1_PhoMedWPvetoJan22RD1_2012;

        TGraphErrors * ge_MuonID2012_Tight[4];
        TGraphErrors * ge_MuonISO2012_Tight[4];
        TGraphErrors * ge_MuonISO2012_Loose[4];

        TH2F * h2_MuonTrig2012[2];

        TH2F * h2_el_ID_Data;
        TH2F * h2_el_ID_MC;
        TH2F * h2_el_Iso_Data;
        TH2F * h2_el_Iso_MC;

        TH2F * h2_el_ID_Data_v2;
        TH2F * h2_el_ID_MC_v2;
        TH2F * h2_el_Iso_Data_v2;
        TH2F * h2_el_Iso_MC_v2;
        TH2F * h2_el_ID_Data_Low_v2;
        TH2F * h2_el_ID_MC_Low_v2;
        TH2F * h2_el_Iso_Data_Low_v2;
        TH2F * h2_el_Iso_MC_Low_v2;

        TH2F * h2_el_LeadingTrigger_Data_v2;
        TH2F * h2_el_LeadingTrigger_MC_v2;
        TH2F * h2_el_TrailingTrigger_Data_v2;
        TH2F * h2_el_TrailingTrigger_MC_v2;
        TH2F * h2_el_LeadingTrigger_Data_Low_v2;
        TH2F * h2_el_LeadingTrigger_MC_Low_v2;
        TH2F * h2_el_TrailingTrigger_Data_Low_v2;
        TH2F * h2_el_TrailingTrigger_MC_Low_v2;

        //weights
        float _puWeight;
        float _zzWeight;
        float _glugluWeight;
        float _vbfWeight;
        float _recoWeight;
        float _triggerWeight;
        float _recoilLongWeight;
        float _recoilTransWeight;
        float _gammaPtPuWeight;

        ClassDef(WeightUtils, 1);
};

#endif

