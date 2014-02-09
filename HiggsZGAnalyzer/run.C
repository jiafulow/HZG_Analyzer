
  #include <iostream>
  #include <fstream>
  #include <string>
  #include <vector>
  #include <cstdlib>

  using namespace std;

  void run(string args="") {

    gROOT->SetMacroPath(".:../src/:../interface/:../plugins/");
    gROOT->LoadMacro("TCPhysObject.cc+");
    gROOT->LoadMacro("TCEGamma.cc+");
    gROOT->LoadMacro("TCTrack.cc+");
    gROOT->LoadMacro("TCJet.cc+");
    gROOT->LoadMacro("TCMET.cc+");
    gROOT->LoadMacro("TCElectron.cc+");
    gROOT->LoadMacro("TCMuon.cc+");
    gROOT->LoadMacro("TCTau.cc+");
    gROOT->LoadMacro("TCPhoton.cc+");
    gROOT->LoadMacro("TCGenJet.cc+");
    gROOT->LoadMacro("TCGenParticle.cc+");
    gROOT->LoadMacro("TCPrimaryVtx.cc+");
    gROOT->LoadMacro("TCTriggerObject.cc+");
    gROOT->LoadMacro("HistManager.cc+");
    gROOT->LoadMacro("WeightUtils.cc+");
    gROOT->LoadMacro("TriggerSelector.cc+");
    gROOT->LoadMacro("ElectronFunctions.cc+");
    gROOT->LoadMacro("rochcor_2011.cc+");
    gROOT->LoadMacro("rochcor2012v2.C+");
    gROOT->LoadMacro("PhosphorCorrectorFunctor.cc+");
    gROOT->LoadMacro("LeptonScaleCorrections.h+");
    gROOT->LoadMacro("EGammaMvaEleEstimator.cc+");
    gROOT->LoadMacro("ZGAngles.cc+");
    gROOT->LoadMacro("AnalysisParameters.cc+");
    gROOT->LoadMacro("ParticleSelectors.cc+");
    gROOT->LoadMacro("Dumper.cc+");
    gSystem->Load("libgfortran.so");
    gSystem->Load("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/hzgammaME/MCFM-6.6/obj/libmcfm_6p6.so");
    gSystem->Load("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/hzgammaME/libME.so");

    TChain* fChain = new TChain("ntupleProducer/eventTree");

    ifstream sourceFiles("sourceFiles/ggM125_NWU_v0905.txt");
    string myLine;
    int  count = 0;
    cout<<"Adding files from ggM125_NWU_v0905 to chain..."<<endl;

    while (sourceFiles >> myLine) {
      if (count == 0 && myLine.find("dcache")==string::npos){
      float rhoFactor;
      TBranch        *b_rhoFactor;   //!
      TFile fixFile(myLine.c_str(),"open");
      TTree *fixTree = (TTree*)fixFile.Get("ntupleProducer/eventTree");
      fixTree->SetBranchAddress("rhoFactor",&rhoFactor,&b_rhoFactor);
      for(int i =0; i<fixTree->GetEntries();i++){
      fixTree->GetEntry(i);
    }
    delete fixTree;

    }
    fChain->Add(myLine.c_str());      
    ++count;
    }
    cout<<count<<" files added!"<<endl;
    sourceFiles.close();

    TStopwatch timer;
    timer.Start();

    fChain->Process("higgsAnalyzer.C+", args.c_str());

    cout << "\n\nDone!" << endl;
    cout << "CPU Time : " << timer.CpuTime() << endl;
    cout << "RealTime : " << timer.RealTime() << endl;
    cout << "\n";
  }

