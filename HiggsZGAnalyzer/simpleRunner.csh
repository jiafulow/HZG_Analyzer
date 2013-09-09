#!/bin/csh

cp simple_Template.C simple.C

sed -i "s/SUFFIX/$1/g" simple.C

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
    gROOT->LoadMacro("ZGAngles.cc+");

    TChain* fChain = new TChain("ntupleProducer/eventTree");

    ifstream sourceFiles("sourceFiles/$1.txt");
    char line[128];
    int  count = 0;
    cout<<"Adding files from $1 to chain..."<<endl;

    while (sourceFiles >> line) {
      fChain->Add(line);      
      ++count;
    }
    cout<<count<<" files added!"<<endl;
    sourceFiles.close();

    TStopwatch timer;
    timer.Start();

    fChain->Process("simple.C+");

    cout << "\n\nDone!" << endl;
    cout << "CPU Time : " << timer.CpuTime() << endl;
    cout << "RealTime : " << timer.RealTime() << endl;
    cout << "\n";
  }

+EOF

root -l -b -q run.C

rm run.C
rm simple.C
