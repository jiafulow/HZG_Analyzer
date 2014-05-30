#ifndef higgsAnalyzer_h
#define higgsAnalyzer_h

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

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH2.h>
#include <TStyle.h>

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
#include "../interface/TCTrack.h"
#include "../interface/TCEGamma.h"
#include "../interface/TCJet.h"
#include "../interface/TCMET.h"
#include "../interface/TCElectron.h"
#include "../interface/TCMuon.h"
#include "../interface/TCTau.h"
#include "../interface/TCPhoton.h"
#include "../interface/TCGenJet.h"
#include "../interface/TCPrimaryVtx.h"
#include "../interface/TCTriggerObject.h"
#include "../interface/TCGenParticle.h"
#include "../interface/HistManager.h"
#include "../interface/TriggerSelector.h"
#include "../interface/WeightUtils.h"
#include "../interface/ElectronFunctions.h"
#include "../interface/rochcor_2011.h"
#include "../interface/rochcor2012jan22.h"
#include "../interface/PhosphorCorrectorFunctor.hh"
#include "../interface/LeptonScaleCorrections.h"
#include "../interface/EGammaMvaEleEstimator.h"
#include "../interface/ZGAngles.h"
#include "../interface/AnalysisParameters.h"
#include "../interface/ParticleSelectors.h"
#include "../interface/Dumper.h"
#include "/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/hzgammaME/TVar.hh"
#include "/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/hzgammaME/TEvtProb.cc"

#ifdef __MAKECINT__
#pragma link C++ class vector<string>+;
#endif

class higgsAnalyzer : public TSelector {

	private:

    //Params and Cuts:
    auto_ptr<Parameters> params;
    auto_ptr<Cuts> cuts;
    auto_ptr<Dumper> dumper;

    auto_ptr<TFile> histoFile;
    auto_ptr<TFile> trainingFile;
    auto_ptr<TFile> sampleFile;
    auto_ptr<TFile> eleSmearFile;
    auto_ptr<TFile> eleIDISOFile;
    auto_ptr<TFile> m_llgFile;
    auto_ptr<HistManager> hm;

    int unskimmedEventsTotal;
    int fileCount;

    auto_ptr<TVector3> pvPosition;

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

    //Unskimmed events
    TTree *thisTree;
    TFile *file0;
    TH1F * h1_numOfEvents;

    //ElectronMVA selection
    auto_ptr<TMVA::Reader> myTMVAReader;

    //ZGAngles ME shit

    auto_ptr<TEvtProb> Xcal2;

    //category stuff
    int catNum;

  public :
    TTree          *fChain;   //!pointer to the analyzed TTree or TChain
    auto_ptr<TTree> sampleChain; 
    TTree* trainingChain; 
    auto_ptr<TTree> m_llgChain;

    //MVA Branches
    float          diffZGscalar;
    float          diffZGvector;
    float          threeBodyMass;
    float          threeBodyPt;
    float          divPt;
    float          cosZ;
    float          cosG;
    float          METdivQt;
    float          GPt;
    float          ZPt;
    float          DPhi;
    float          diffPlaneMVA;
    float          vtxVariable;
    float          dr1;
    float          dr2;
    float          M12;
    float          medisc;
    float          smallTheta;
    float          bigTheta;
    float          comPhi; 
    float          GPtOM;
    float          diffZGvectorOM;
    float          threeBodyPtOM;
    float          ZPtOM;
    float          GEta;
    float          ZEta;
    float          l1Eta;
    float          l2Eta;
    float          threeBodyEta;
    float          GPtOHPt;
    float          R9var;
    float          sieip;
    float          sipip;
    float          SCRawE;
    float          SCPSE;
    float          SCPSEOPt;
    float          SCRawEOPt;
    float          e5x5;
    float          e2x2;
    float          e2x2O5x5;
    float          scaleFactor;

    //M_llg branches
    double          m_llg;
    double          m_llgCAT1;
    double          m_llgCAT2;
    double          m_llgCAT3;
    double          m_llgCAT4;
    double          m_llgCAT5;
    double          m_llgCAT6;
    double          m_llgCAT7;
    double          m_llgCAT8;
    double          m_llgCAT9;
    double          unBinnedWeight;
    double          unBinnedLumiXS;

		// Declaration of leaf types
		TClonesArray  *recoJets;
		TClonesArray  *recoMET;
		TClonesArray  *genJets;
		TClonesArray  *genParticles;
		TClonesArray  *recoMuons;
		TClonesArray  *recoElectrons;
		TClonesArray  *recoPhotons;
		TClonesArray  *primaryVtx;
		TClonesArray  *triggerObjects;
    float         unskimmedEvents;
		int           runNumber;
		int           eventNumber;
		int           lumiSection;
		int           nPUVertices;
		float         nPUVerticesTrue;
		double        ptHat;
		float         rhoFactor;
		ULong64_t     triggerStatus;
		unsigned int           hltPrescale[64];
		bool          isRealData;

		// List of branches
		TBranch        *b_recoJets;   //!
		TBranch        *b_recoMET;   //!
		TBranch        *b_genJets;   //!
		TBranch        *b_genParticles;   //!
		TBranch        *b_recoMuons;     //!
		TBranch        *b_recoElectrons;  //!
		TBranch        *b_recoPhotons;    //!
		TBranch        *b_primaryVtx;   //!
		TBranch        *b_triggerObjects;   //!
		TBranch        *b_runNumber;   //!
		TBranch        *b_eventNumber;   //!
		TBranch        *b_lumiSection;   //!
		TBranch        *b_rhoFactor;   //!
		TBranch        *b_ptHat;   //!
		TBranch        *b_triggerStatus;   //!
		TBranch        *b_hltPrescale;   //!
		TBranch        *b_nPUVertices;   //!
		TBranch        *b_nPUVerticesTrue;   //!
		TBranch        *b_isRealData;   //!


		//For counting events
    int          nEvents[100];
    int          genAccept[2];

		higgsAnalyzer(TTree * /*tree*/ =0): catNum(-1),fChain(0){ }
		virtual ~higgsAnalyzer() {}
		virtual int     Version() const { return 2; }
		virtual void    Begin(TTree *tree);
		//virtual void    SlaveBegin(TTree *tree) { TString option = GetOption();};
		virtual void    Init(TTree *tree);
		virtual bool    Notify();
		virtual bool    Process(Long64_t entry);
		virtual int     GetEntry(Long64_t entry, int getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
		virtual void    SetOption(const char *option) { fOption = option; }
		virtual void    SetObject(TObject *obj) { fObject = obj; }
		virtual void    SetInputList(TList *input) { fInput = input; }
		virtual TList  *GetOutputList() const { return fOutput; }
		virtual void    SlaveTerminate() {};
		virtual void    Terminate();

    static bool P4SortCondition(const TLorentzVector& p1, const TLorentzVector& p2) {return (p1.Pt() > p2.Pt());} 
    static bool VertexSortCondition(const TCPrimaryVtx& pv1, const TCPrimaryVtx& pv2) {return (pv1.SumPt2Trks() > pv2.SumPt2Trks());}

    virtual float   Dz(TVector3 objVtx, TLorentzVector objP4, TVector3 vtx);
    virtual float   Dxy(TVector3 objVtx, TLorentzVector objP4, TVector3 vtx);
		virtual void    MetPlusZPlots(TLorentzVector metP4, TLorentzVector ZP4, float evtWeight);
		virtual void    MetPlusLeptonPlots(TLorentzVector metP4, TLorentzVector p1, TLorentzVector p2, float evtWeight);
		virtual void    LeptonBasicPlots(TLorentzVector p1, TLorentzVector p2, float evtWeight);
		virtual void    GenPlots(vector<TCGenParticle> Zs, vector<TCGenParticle> leps, vector<TCGenParticle> phots, vector<TCGenParticle> Hs, TLorentzVector ZP4,TLorentzVector GP4, float evtWeight); 
    virtual void    StandardPlots(TLorentzVector p1, TLorentzVector p2, float evtWeight,string tag, string folder);
    virtual void    StandardPlots(TLorentzVector p1, TLorentzVector p2, TLorentzVector gamma, float evtWeight,string tag, string folder);
    virtual void    AnglePlots(ZGAngles &zga, float eventWeight, string folder, string tag);
		virtual void    DileptonBasicPlots(TLorentzVector ZP4, float evtWeight);
		virtual bool    CosmicMuonFilter(TCMuon muon1, TCMuon muon2);
		virtual float   CalculateTransMass(TLorentzVector p1, TLorentzVector p2);
		virtual float   CalculateTransMassAlt(TLorentzVector p1, TLorentzVector p2);
    virtual float   GetPhotonMass();
    virtual float   CalculateM12sqrd(TLorentzVector p1, TLorentzVector p2);
    virtual float   CalculateX1(TLorentzVector p1,TLorentzVector p2);
    virtual float   CalculateX2(TLorentzVector p1, TLorentzVector p2);
    virtual float   MEDiscriminator(TCPhysObject lepton1, TCPhysObject lepton2, TLorentzVector gamma);
    virtual void    LumiXSWeight(double *_LumiXSWeight);
    virtual void    PhotonR9Corrector(TCPhoton& ph);
    virtual bool    GoodLeptonsCat(const TCMuon& m1, const TCMuon& m2);
    virtual bool    GoodLeptonsCat(const TCElectron& e1, const TCElectron& e2);
    virtual bool    GoodLeptonsCat(const TCPhysObject& m1, const TCPhysObject& m2);
    virtual bool    GoodLeptonsCat(const float SCEta1, const float SCEta2);
    bool            SpikeVeto(const TCPhoton& ph);
    
    void            UniversalEnergyCorrector(TCPhoton& ph, vector<TCGenParticle>& _genPhotons);
    void            UniversalEnergyCorrector(TCMuon& mu); 
    void            UniversalEnergyCorrector(TCElectron& el); 


    ///////////////////////
    // ZGAngles MVA defs //
    ///////////////////////

    // TMVA weights directory


    // here we will use only BDTG... but will keep the structure 
    enum DISCR_METHOD {
      MLPBNN, BDTG, BDT
    };

    enum DISCR_TYPE {
      D_ZJets
    };


    TMVA::Reader* tmvaReader;

    struct mvaInitStruct{
      TString weightsDir; 
      TString discrMethodName[3];
      TString discrSampleName; 
      TString discrSuffixName; 
      TString discrSelection; 
      Int_t mvaHiggsMassPoint[1];
      Float_t bdtCut[1];
    } mvaInits;

    struct mvaVarStruct{
      float _medisc;
      float _smallTheta;
      float _bigTheta;
      float _comPhi;
      float _GPtOM;
      float _diffZGvectorOM;
      float _threeBodyPtOM;
      float _ZPtOM;
      float _GEta;
      float _ZEta;
      float _l1Eta;
      float _l2Eta;
      float _threeBodyEta;
      float _GPtOHPt;
      float _threeBodyMass;
      float _R9;
      float _sieip;
      float _sipip;
      float _SCRawE;
      float _SCPSE;
      float _e5x5;
      float _e2x2;
      float _e2x2O5x5;
      float _SCRawEOPt;
      float _SCPSEOPt;
    } mvaVars; 

    virtual void    MVAPlots(mvaVarStruct _mvaVars, float _mvaVal, float eventWeight, string tag, string folder);

    TMVA::Reader*   MVAInitializer();
    float MVACalculator (mvaInitStruct inits, TMVA::Reader* _tmvaReader);




    ClassDef(higgsAnalyzer,0);
};

#endif

#ifdef higgsAnalyzer_cxx
void higgsAnalyzer::Init(TTree *tree)
{
	// The Init() function is called when the selector needs to initialize
	// a new tree or chain. Typically here the branch addresses and branch
	// pointers of the tree will be set.
	// It is normally not necessary to make changes to the generated
	// code, but the routine can be extended by the user if needed.
	// Init() will be called many times when running on PROOF
	// (once per file to be processed).

	// Set object pointer
  fileCount = 0;
  unskimmedEvents = 0;
  unskimmedEventsTotal = 0;
	recoJets = 0;
	recoMET = 0;
	genJets = 0;
	genParticles = 0;
	primaryVtx = 0;
	recoMuons = 0;
	recoElectrons = 0;
	recoPhotons = 0;
  triggerObjects = 0;
	// Set branch addresses and branch pointers
	if (!tree) return;
  thisTree = tree;
  //TTree *cloneTree = new TTree(&tree->CloneTree());
	//fChain = tree->CloneTree();
	fChain = tree;
	fChain->SetMakeClass(1);
  cout<<"init"<<endl;

	fChain->SetBranchAddress("recoJets", &recoJets, &b_recoJets);
	fChain->SetBranchAddress("recoElectrons", &recoElectrons, &b_recoElectrons);
	fChain->SetBranchAddress("recoMuons", &recoMuons, &b_recoMuons);
	fChain->SetBranchAddress("recoPhotons", &recoPhotons, &b_recoPhotons);
	fChain->SetBranchAddress("recoMET", &recoMET, &b_recoMET);
	fChain->SetBranchAddress("genJets", &genJets, &b_genJets);
	fChain->SetBranchAddress("genParticles", &genParticles, &b_genParticles);
	fChain->SetBranchAddress("primaryVtx", &primaryVtx, &b_primaryVtx);
	fChain->SetBranchAddress("triggerObjects", &triggerObjects, &b_triggerObjects);

	fChain->SetBranchAddress("isRealData", &isRealData, &b_isRealData);
  fChain->SetBranchAddress("runNumber", &runNumber, &b_runNumber);
	fChain->SetBranchAddress("eventNumber", &eventNumber, &b_eventNumber);
	fChain->SetBranchAddress("lumiSection", &lumiSection, &b_lumiSection);
	fChain->SetBranchAddress("rhoFactor", &rhoFactor, &b_rhoFactor);
	fChain->SetBranchAddress("ptHat", &ptHat, &b_ptHat);
	fChain->SetBranchAddress("nPUVertices", &nPUVertices, &b_nPUVertices);
	fChain->SetBranchAddress("nPUVerticesTrue", &nPUVerticesTrue, &b_nPUVerticesTrue);
	fChain->SetBranchAddress("triggerStatus", &triggerStatus, &b_triggerStatus);
	fChain->SetBranchAddress("hltPrescale",&hltPrescale, &b_hltPrescale);



}

bool higgsAnalyzer::Notify()
{
  fileCount+= 1;
  file0 = thisTree->GetCurrentFile();
  h1_numOfEvents = (TH1F*) file0->Get("ntupleProducer/numOfEvents");
  unskimmedEvents = h1_numOfEvents->GetBinContent(1);
  cout<<"THIS IS FILE NUMBER: "<<fileCount<<" and it has this many events: "<<unskimmedEvents<<endl;
  unskimmedEventsTotal += unskimmedEvents;
	return kTRUE;
}

#endif 

