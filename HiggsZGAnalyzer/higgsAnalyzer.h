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
#include "../interface/rochcor2012v2.h"
#include "../interface/PhosphorCorrectorFunctor.hh"
#include "../interface/LeptonScaleCorrections.h"
#include "../interface/EGammaMvaEleEstimator.h"
#include "../interface/ZGAngles.h"
#include "../interface/AnalysisParameters.h"
#include "../interface/ParticleSelectors.h"
#include "../interface/Dumper.h"
#include "../hzgammaME/TVar.hh"
#include "../hzgammaME/TEvtProb.cc"

#ifdef __MAKECINT__
#pragma link C++ class vector<string>+;
#endif

class higgsAnalyzer : public TSelector {

	private:

    //Params and Cuts:
    Cuts* cuts;
    Dumper* dumper;

		TFile* histoFile;
		TFile* trainingFile;
		TFile* sampleFile;
		TFile* higgsFile;
    TFile* eleSmearFile;
    TFile* eleIDISOFile;
    TFile* m_llgFile;
    HistManager* hm;
    HistManager* hmHiggs;
    HistManager* hmEleIDISO;
    HistManager* hmEleSmear;

    float unskimmedEventsTotal;
    int fileCount;

    TVector3 *pvPosition;

    // Random number generator
    TRandom3* rnGenerator;
    TRandom3* rEl;
    TRandom3* rMuRun;

    // Selectors
    //GenParticleSelector genParticleSelector;
    TriggerSelector *triggerSelector;
    ParticleSelector *particleSelector;
    ParticleSelector::genHZGParticles genHZG;

    // Utilities
    WeightUtils *weighter;
    rochcor_2011 * rmcor2011;
    rochcor2012 * rmcor2012;
    zgamma::PhosphorCorrectionFunctor* phoCorrector;

    //Unskimmed events
    TTree *thisTree;
    TFile *file0;
    TH1F * h1_numOfEvents;

    //R9
    float R9;
    float R9Cor;

    //ElectronMVA selection
    EGammaMvaEleEstimator* myMVATrig;

    TMVA::Reader             *myTMVAReader;

    //ZGAngles ME shit

    TEvtProb* Xcal2;

  public :
    TTree          *fChain;   //!pointer to the analyzed TTree or TChain
    TTree          *sampleChain; 
    TTree          *trainingChain; 
    TTree          *m_llgChain;

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
    float          scaleFactor;

    //M_llg branches
    float          m_llg;
    float          m_llgCAT1;
    float          m_llgCAT2;
    float          m_llgCAT3;
    float          m_llgCAT4;
    float          unBinnedWeight;
    float          unBinnedLumiXS;

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

		higgsAnalyzer(TTree * /*tree*/ =0): fChain(0){ }
		virtual ~higgsAnalyzer() {delete cuts;cout<<"destruct HA"<<endl; }
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

    static bool P4SortCondition(const TLorentzVector* p1, const TLorentzVector* p2) {return (p1->Pt() > p2->Pt());} 
    static bool VertexSortCondition(const TCPrimaryVtx& pv1, const TCPrimaryVtx& pv2) {return (pv1.SumPt2Trks() > pv2.SumPt2Trks());}

    virtual float   Dz(TVector3 objVtx, TLorentzVector objP4, TVector3 vtx);
    virtual float   Dxy(TVector3 objVtx, TLorentzVector objP4, TVector3 vtx);
		virtual void    MetPlusZPlots(TLorentzVector metP4, TLorentzVector ZP4, float evtWeight);
		virtual void    MetPlusLeptonPlots(TLorentzVector metP4, TLorentzVector p1, TLorentzVector p2, float evtWeight);
		virtual void    LeptonBasicPlots(TLorentzVector p1, TLorentzVector p2, float evtWeight);
		virtual void    GenPlots(vector<TCGenParticle*> Zs, vector<TCGenParticle*> leps, vector<TCGenParticle*> phots, vector<TCGenParticle*> Hs, TLorentzVector ZP4,TLorentzVector GP4, float evtWeight); 
    virtual void    StandardPlots(TLorentzVector p1, TLorentzVector p2, TLorentzVector gamma, float evtWeight,string tag, string folder);
    virtual void    AnglePlots(ZGAngles &zga, float eventWeight);
		virtual void    DileptonBasicPlots(TLorentzVector ZP4, float evtWeight);
		virtual bool    CosmicMuonFilter(TCMuon muon1, TCMuon muon2);
		virtual float   CalculateTransMass(TLorentzVector p1, TLorentzVector p2);
		virtual float   CalculateTransMassAlt(TLorentzVector p1, TLorentzVector p2);
    virtual float   GetPhotonMass();
    virtual float   CalculateM12sqrd(TLorentzVector p1, TLorentzVector p2);
    virtual float   CalculateX1(TLorentzVector p1,TLorentzVector p2);
    virtual float   CalculateX2(TLorentzVector p1, TLorentzVector p2);
    virtual float   Zeppenfeld(TLorentzVector p, TLorentzVector pj1, TLorentzVector pj2);
    virtual float   MEDiscriminator(TCPhysObject lepton1, TCPhysObject lepton2, TLorentzVector gamma);
    virtual void    LumiXSWeight(float *lumiXS);


    ///////////////////////
    // ZGAngles MVA defs //
    ///////////////////////

    // TMVA weights directory

#define N_DISCR_METHODS 3
#define N_HIGGS_MASSES 1

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
      Int_t mvaHiggsMassPoint[N_HIGGS_MASSES];
      Float_t bdtCut[N_HIGGS_MASSES];
    } mvaInits;

    struct mvaVarStruct{
      Float_t _diffZGvector;
      Float_t _threeBodyPt;
      Float_t _GPt;
      Float_t _cosZ;
      Float_t _diffPlaneMVA;
      Float_t _vtxVariable;
      Float_t _threeBodyMass;
      Float_t _dr1;
      Float_t _dr2;
      Float_t _M12;
    } mvaVars; 

    virtual TMVA::Reader*   MVAInitializer(mvaVarStruct vars, mvaInitStruct inits);
    virtual void MVACalulator (mvaVarStruct vars, mvaInitStruct inits, TMVA::Reader* tmvaReader);




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


  ///////////////////////
  // ZGAngles MVA init //
  ///////////////////////

  // TMVA weights directory
  mvaInits.weightsDir = "/uscms_data/d2/bpollack/CMSSW_4_4_2/src/MVACodes/mvaExamples_brian/weights/";

  // here we will use only BDTG... but will keep the structure 
  mvaInits.discrMethodName[0] = "MLPBNN";
  mvaInits.discrMethodName[1] = "BDTG";
  mvaInits.discrMethodName[2] = "BDT";

  mvaInits.discrSampleName = "MVA_ZJets";


  mvaInits.mvaHiggsMassPoint[0] = 125;

  mvaInits.bdtCut[0] = 0.75;

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

