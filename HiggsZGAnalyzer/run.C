
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

    gROOT->LoadMacro("AnalysisParameters.cc++g");
    gROOT->LoadMacro("ParticleSelectors.cc++g");
    gROOT->LoadMacro("HistManager.cc++g");
    gROOT->LoadMacro("ZGAngles.cc++g");

    TChain* fChain = new TChain("ntupleProducer/eventTree");

    ifstream sourceFiles("sourceFiles/DYJets.txt");
    char line[128];
    int  count = 0;
    cout<<"Adding files from DYJets to chain..."<<endl;

    while (sourceFiles >> line) {
      fChain->Add(line);      
      ++count;
    }
    cout<<count<<" files added!"<<endl;
    sourceFiles.close();

    TStopwatch timer;
    timer.Start();

    fChain->Process("simple.C++g");

    cout << "\n\nDone!" << endl;
    cout << "CPU Time : " << timer.CpuTime() << endl;
    cout << "RealTime : " << timer.RealTime() << endl;
    cout << "\n";
  }

