#!/bin/csh

set dir=`echo $1 | cut -d _ -f 1 `

cp $6 higgsAnalyzer.C

sed -i "s/SUFFIX/$1/g" higgsAnalyzer.C
sed -i "s/ABCD/$2/g" higgsAnalyzer.C
sed -i "s/SELECTION/$4/g" higgsAnalyzer.C
sed -i "s/PERIOD/$5/g" higgsAnalyzer.C
sed -i "s/DATANAME/$3/g" higgsAnalyzer.C
sed -i "s/COUNT/local/g" higgsAnalyzer.C

cat > run.C << +EOF

  #include <iostream>
  #include <fstream>
  #include <string>
  #include <vector>
  #include <cstdlib>

  using namespace std;

  void run() {

    gROOT->SetMacroPath(".:../src/:../interface/:../plugins/");
    gROOT->LoadMacro("TCPhysObject.cc++g");
    gROOT->LoadMacro("TCJet.cc++g");
    gROOT->LoadMacro("TCMET.cc++g");
    gROOT->LoadMacro("TCElectron.cc++g");
    gROOT->LoadMacro("TCMuon.cc++g");
    gROOT->LoadMacro("TCTau.cc++g");
    gROOT->LoadMacro("TCPhoton.cc++g");
    gROOT->LoadMacro("TCGenJet.cc++g");
    gROOT->LoadMacro("TCGenParticle.cc++g");
    gROOT->LoadMacro("TCPrimaryVtx.cc++g");
    gROOT->LoadMacro("TCTriggerObject.cc++g");
    gROOT->LoadMacro("HistManager.cc++g");
    gROOT->LoadMacro("WeightUtils.cc++g");
    gROOT->LoadMacro("TriggerSelector.cc++g");
    gROOT->LoadMacro("ElectronFunctions.cc++g");
    gROOT->LoadMacro("rochcor_2011.cc++");
    gROOT->LoadMacro("rochcor2012v2.C++");
    gROOT->LoadMacro("PhosphorCorrectorFunctor.cc++g");
    gROOT->LoadMacro("LeptonScaleCorrections.h++g");
    gROOT->LoadMacro("EGammaMvaEleEstimator.cc++g");
    gROOT->LoadMacro("ZGAngles.cc++g");
    gROOT->LoadMacro("AnalysisParameters.cc++g");
    gROOT->LoadMacro("ParticleSelectors.cc++g");
    gSystem->Load("libgfortran.so");
    gSystem->Load("../hzgammaME/MCFM-6.6/obj/libmcfm_6p6.so");
    gSystem->Load("../hzgammaME/libME.so");

    TChain* fChain = new TChain("ntupleProducer/eventTree");

    ifstream sourceFiles("sourceFiles/$3.txt");
    string line;
    int  count = 0;
    cout<<"Adding files from $3 to chain..."<<endl;

    while (sourceFiles >> line) {
      if (count == 0 && line.find("dcache")==string::npos){
      float rhoFactor;
      TBranch        *b_rhoFactor;   //!
      TFile fixFile(line.c_str(),"open");
      TTree *fixTree = (TTree*)fixFile.Get("ntupleProducer/eventTree");
      fixTree->SetBranchAddress("rhoFactor",&rhoFactor,&b_rhoFactor);
      for(int i =0; i<fixTree->GetEntries();i++){
      fixTree->GetEntry(i);
    }
    delete fixTree;

    }
    fChain->Add(line.c_str());      
    ++count;
    }
    cout<<count<<" files added!"<<endl;
    sourceFiles.close();

    TStopwatch timer;
    timer.Start();

    fChain->Process("higgsAnalyzer.C++g");

    cout << "\n\nDone!" << endl;
    cout << "CPU Time : " << timer.CpuTime() << endl;
    cout << "RealTime : " << timer.RealTime() << endl;
    cout << "\n";
  }

+EOF

root -l -b -q run.C

#rm run.C
#mv *local.root localHistos/.
