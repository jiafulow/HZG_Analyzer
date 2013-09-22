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
    gROOT->LoadMacro("TCPhysObject.cc+");
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

    fChain->Process("higgsAnalyzer.C+");

    cout << "\n\nDone!" << endl;
    cout << "CPU Time : " << timer.CpuTime() << endl;
    cout << "RealTime : " << timer.RealTime() << endl;
    cout << "\n";
  }

+EOF

root -l -b -q run.C

rm run.C
mv *local.root localHistos/.
