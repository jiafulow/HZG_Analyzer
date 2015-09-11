//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Mar 20 10:22:10 2014 by ROOT version 5.32/00
// from TTree eventTree/eventTree
// found on file: /tthome/bpollack/storage/nuTuples_v9.6_8TeV/Data/DoubleMu_Run2012A/nuTuple_100_1_tEx.root
//////////////////////////////////////////////////////////

#ifndef amumuAnalyzer_h
#define amumuAnalyzer_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH2.h>
#include <TStyle.h>

// Header file for the classes stored in the TTree if any.
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <unistd.h>

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TVector2.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TProfile.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

#include "../interface/TCPhysObject.h"
#include "../interface/TCEGamma.h"
#include "../interface/TCTrack.h"
#include "../interface/TCJet.h"
#include "../interface/TCMET.h"
#include "../interface/TCElectron.h"
#include "../interface/TCMuon.h"
#include "../interface/TCTau.h"
#include "../interface/TCPhoton.h"
#include "../interface/TCGenJet.h"
#include "../interface/TCGenParticle.h"
#include "../interface/TCPrimaryVtx.h"
#include "../interface/TCTriggerObject.h"
#include "../interface/HistManager.h"
#include "../interface/WeightUtils.h"
#include "../interface/TriggerSelector.h"
#include "../interface/ElectronFunctions.h"
#include "../interface/rochcor_2011.h"
#include "../interface/rochcor2012jan22.h"
#include "../interface/PhosphorCorrectorFunctor.hh"
#include "../interface/MuScleFitCorrector.h"
#include "../interface/LeptonScaleCorrections.h"
#include "../interface/EGammaMvaEleEstimator.h"
#include "../interface/ZGAngles.h"
#include "../interface/AnalysisParameters.h"
#include "../interface/ParticleSelectors.h"
#include "../interface/Dumper.h"

#ifdef __MAKECINT__
#pragma link C++ class vector<string>+;
#endif

// Fixed size dimensions of array or collections stored in the TTree if any.

class amumuAnalyzer : public TSelector {
  public :
    TTree          *fChain;   //!pointer to the analyzed TTree or TChain

    // Declaration of leaf types
    TClonesArray    *recoJets;
    TClonesArray    *recoElectrons;
    TClonesArray    *recoMuons;
    TClonesArray    *recoPhotons;
    TCMET           *recoMET;
    TClonesArray    *genJets;
    TClonesArray    *genParticles;
    TClonesArray    *triggerObjects;
    TClonesArray    *primaryVtx;
    TVector3        *beamSpot;
    Int_t           nPUVertices;
    Float_t         nPUVerticesTrue;
    Bool_t          isRealData;
    UInt_t          runNumber;
    ULong64_t       eventNumber;
    UInt_t          lumiSection;
    UInt_t          bunchCross;
    Float_t         ptHat;
    Float_t         qScale;
    Float_t         evtWeight;
    Int_t           lhe_nup;
    Float_t         rhoFactor;
    Float_t         rho25Factor;
    Float_t         rhoMuFactor;
    ULong64_t       triggerStatus;
    UInt_t          hltPrescale[64];
    Bool_t          NoiseFilters_isScraping;
    Bool_t          NoiseFilters_isNoiseHcalHBHE;
    Bool_t          NoiseFilters_isNoiseHcalLaser;
    Bool_t          NoiseFilters_isNoiseEcalTP;
    Bool_t          NoiseFilters_isNoiseEcalBE;
    Bool_t          NoiseFilters_isCSCTightHalo;
    Bool_t          NoiseFilters_isCSCLooseHalo;
    Bool_t          NoiseFilters_isNoiseTracking;
    Bool_t          NoiseFilters_isNoiseEEBadSc;
    Bool_t          NoiseFilters_isNoisetrkPOG1;
    Bool_t          NoiseFilters_isNoisetrkPOG2;
    Bool_t          NoiseFilters_isNoisetrkPOG3;

    // List of branches
    TBranch        *b_recoJets;   //!
    TBranch        *b_recoElectrons;   //!
    TBranch        *b_recoMuons;   //!
    TBranch        *b_recoPhotons;   //!
    TBranch        *b_recoMET;   //!
    TBranch        *b_genJets;   //!
    TBranch        *b_genParticles;   //!
    TBranch        *b_triggerObjects;   //!
    TBranch        *b_primaryVtx;   //!
    TBranch        *b_beamSpot;   //!
    TBranch        *b_nPUVertices;   //!
    TBranch        *b_nPUVerticesTrue;   //!
    TBranch        *b_isRealData;   //!
    TBranch        *b_runNumber;   //!
    TBranch        *b_eventNumber;   //!
    TBranch        *b_lumiSection;   //!
    TBranch        *b_bunchCross;   //!
    TBranch        *b_ptHat;   //!
    TBranch        *b_qScale;   //!
    TBranch        *b_evtWeight;   //!
    TBranch        *b_lhe_nup;   //!
    TBranch        *b_rhoFactor;   //!
    TBranch        *b_rho25Factor;   //!
    TBranch        *b_rhoMuFactor;   //!
    TBranch        *b_triggerStatus;   //!
    TBranch        *b_hltPrescale;   //!
    TBranch        *b_NoiseFilters;   //!

    // Defs that are user created, not templated

    // For file and event counting
    int fileCount;
    long int unskimmedEventsTotal;
    long int unskimmedEvents;
    TFile *file0;
    TH1F * h1_numOfEvents;
    TTree *thisTree;

    //Params and Cuts:
    auto_ptr<Parameters> params;
    auto_ptr<Cuts> cuts;
    auto_ptr<Dumper> dumper;

    auto_ptr<TFile> amumuFile;
    auto_ptr<TTree> amumuTree;
    auto_ptr<TTree> genTree;

    auto_ptr<TVector3> pvPosition;
    auto_ptr<HistManager> hm;

    // Random number generator
    auto_ptr<TRandom3> rnGenerator;
    auto_ptr<TRandom3> rEl;
    auto_ptr<TRandom3> rMuRun;

    // Selectors
    //GenParticleSelector genParticleSelector;
    auto_ptr<TriggerSelector> triggerSelector;
    auto_ptr<ParticleSelector> particleSelector;
    ParticleSelector::genHZGParticles genHZG;

    // Utilities
    auto_ptr<WeightUtils> weighter;
    auto_ptr<rochcor_2011> rmcor2011;
    auto_ptr<rochcor2012> rmcor2012;
    auto_ptr<zgamma::PhosphorCorrectionFunctor> phoCorrector;
    auto_ptr<MuScleFitCorrector> muscleFitCor;

    //ElectronMVA selection
    auto_ptr<TMVA::Reader> myTMVAReader;

    //the amumu branches
    TLorentzVector muonOne;
    TLorentzVector muonTwo;
    TLorentzVector bjet;
    TLorentzVector fjet;
    TLorentzVector met;
    TLorentzVector dimuon;
    TLorentzVector dijet;
    int njets;
    int ncjets;
    int nbjets;
    int nfjets;
    int njets_pt20;
    int njets_pt25;
    int njets_pt30;
    int njets_pt40;
    int njets_pt50;
    int ncjets_pt20;
    int ncjets_pt25;
    int ncjets_pt30;
    int ncjets_pt40;
    int ncjets_pt50;
    int nbjets_pt20;
    int nbjets_pt25;
    int nbjets_pt30;
    int nbjets_pt40;
    int nbjets_pt50;
    int nfjets_pt20;
    int nfjets_pt25;
    int nfjets_pt30;
    int nfjets_pt40;
    int nfjets_pt50;
    int njets_noclean_pt20;
    int njets_noclean_pt25;
    int njets_noclean_pt30;
    int njets_noclean_pt40;
    int njets_noclean_pt50;
    int ncjets_noclean_pt20;
    int ncjets_noclean_pt25;
    int ncjets_noclean_pt30;
    int ncjets_noclean_pt40;
    int ncjets_noclean_pt50;
    int nbjets_noclean_pt20;
    int nbjets_noclean_pt25;
    int nbjets_noclean_pt30;
    int nbjets_noclean_pt40;
    int nbjets_noclean_pt50;
    int nmuons;
    int nelectrons;
    int nphotons;
    bool passSasha;
    bool passStep5;
    bool passStep6;
    bool passStep7;
    int muonOneCharge;
    float muonOneIsPF;
    float muonOneIsGLB;
    float muonOneNormalizedChi2;
    float muonOneNumberOfMatchedStations;
    float muonOneNumberOfValidMuonHits;
    float muonOneNumberOfValidPixelHits;
    float muonOneTrackLayersWithMeasurement;
    float muonOneDxy;
    float muonOneDz;
    float muonOneTrkIso;
    float muonOneRelIso;
    int muonTwoCharge;
    float muonTwoIsPF;
    float muonTwoIsGLB;
    float muonTwoNormalizedChi2;
    float muonTwoNumberOfMatchedStations;
    float muonTwoNumberOfValidMuonHits;
    float muonTwoNumberOfValidPixelHits;
    float muonTwoTrackLayersWithMeasurement;
    float muonTwoDxy;
    float muonTwoDz;
    float muonTwoTrkIso;
    float muonTwoRelIso;
    float bjetCSV;
    float bjetCSVv1;
    float bjetCSVMVA;
    float bjetPUID;
    float fjetCSV;
    float fjetCSVv1;
    float fjetCSVMVA;
    float fjetPUID;
    float x; // for unbinned fit
    float w; // event weight

    TLorentzVector muonOneGen;
    TLorentzVector muonTwoGen;
    TLorentzVector bjetGen;
    TLorentzVector fjetGen;

    // Default functions

    amumuAnalyzer(TTree * /*tree*/ =0) :
      fChain(0),
      fileCount(0),
      unskimmedEventsTotal(0),
      unskimmedEvents(0),
      file0(0),
      h1_numOfEvents(0),
      thisTree(0) { }
    virtual ~amumuAnalyzer() { }
    virtual Int_t   Version() const { return 2; }
    virtual void    Begin(TTree *tree);
    virtual void    SlaveBegin(TTree *tree) {}
    virtual void    Init(TTree *tree);
    virtual Bool_t  Notify();
    virtual Bool_t  Process(Long64_t entry);
    virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
    virtual void    SetOption(const char *option) { fOption = option; }
    virtual void    SetObject(TObject *obj) { fObject = obj; }
    virtual void    SetInputList(TList *input) { fInput = input; }
    virtual TList  *GetOutputList() const { return fOutput; }
    virtual void    SlaveTerminate() {}
    virtual void    Terminate();

    //Custom Functions

    static bool   P4SortCondition(const TLorentzVector& p1, const TLorentzVector& p2) {return (p1.Pt() > p2.Pt());} 


    ClassDef(amumuAnalyzer,0);
};

#endif

#ifdef amumuAnalyzer_cxx
void amumuAnalyzer::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the branch addresses and branch
  // pointers of the tree will be set.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).

  // Set object pointer
  recoJets = 0;
  recoElectrons = 0;
  recoMuons = 0;
  recoPhotons = 0;
  recoMET = 0;
  genJets = 0;
  genParticles = 0;
  triggerObjects = 0;
  primaryVtx = 0;
  beamSpot = 0;
  // Set branch addresses and branch pointers
  if (!tree) return;
  thisTree = tree;
  fChain = tree;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("recoJets", &recoJets, &b_recoJets);
  fChain->SetBranchAddress("recoElectrons", &recoElectrons, &b_recoElectrons);
  fChain->SetBranchAddress("recoMuons", &recoMuons, &b_recoMuons);
  fChain->SetBranchAddress("recoPhotons", &recoPhotons, &b_recoPhotons);
  fChain->SetBranchAddress("recoMET", &recoMET, &b_recoMET);
  fChain->SetBranchAddress("genJets", &genJets, &b_genJets);
  fChain->SetBranchAddress("genParticles", &genParticles, &b_genParticles);
  //fChain->SetBranchAddress("triggerObjects", &triggerObjects, &b_triggerObjects);
  fChain->SetBranchAddress("primaryVtx", &primaryVtx, &b_primaryVtx);
  fChain->SetBranchAddress("beamSpot", &beamSpot, &b_beamSpot);
  fChain->SetBranchAddress("nPUVertices", &nPUVertices, &b_nPUVertices);
  fChain->SetBranchAddress("nPUVerticesTrue", &nPUVerticesTrue, &b_nPUVerticesTrue);
  fChain->SetBranchAddress("isRealData", &isRealData, &b_isRealData);
  fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
  fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
  fChain->SetBranchAddress("lumiSection", &lumiSection, &b_lumiSection);
  fChain->SetBranchAddress("bunchCross", &bunchCross, &b_bunchCross);
  fChain->SetBranchAddress("ptHat", &ptHat, &b_ptHat);
  fChain->SetBranchAddress("qScale", &qScale, &b_qScale);
  fChain->SetBranchAddress("evtWeight", &evtWeight, &b_evtWeight);
  fChain->SetBranchAddress("lhe_nup", &lhe_nup, &b_lhe_nup);
  fChain->SetBranchAddress("rhoFactor", &rhoFactor, &b_rhoFactor);
  fChain->SetBranchAddress("rho25Factor", &rho25Factor, &b_rho25Factor);
  fChain->SetBranchAddress("rhoMuFactor", &rhoMuFactor, &b_rhoMuFactor);
  fChain->SetBranchAddress("triggerStatus", &triggerStatus, &b_triggerStatus);
  fChain->SetBranchAddress("hltPrescale", hltPrescale, &b_hltPrescale);
  fChain->SetBranchAddress("NoiseFilters", &NoiseFilters_isScraping, &b_NoiseFilters);
}

Bool_t amumuAnalyzer::Notify()
{
  fileCount+= 1;
  file0 = thisTree->GetCurrentFile();
  h1_numOfEvents = (TH1F*) file0->Get("ntupleProducer/numOfEvents");
  unskimmedEvents = h1_numOfEvents->GetBinContent(1);
  cout<<"THIS IS FILE NUMBER: "<<fileCount<<" and it has this many events: "<<unskimmedEvents<<endl;
  unskimmedEventsTotal += unskimmedEvents;

  return kTRUE;
}

#endif // #ifdef amumuAnalyzer_cxx
