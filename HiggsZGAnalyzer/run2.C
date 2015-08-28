#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "TString.h"
#include "TObjString.h"

using namespace std;

void run2(string args="") {
    string libMake = gSystem->GetMakeSharedLib();
    const string delWarn("-Wshadow");
    int pos1 = libMake.find(delWarn);
    libMake= libMake.substr(0, pos1) + libMake.substr(pos1+delWarn.size()+1);
    gSystem->SetMakeSharedLib(libMake.c_str());

    //cout<<gSystem->GetMakeSharedLib()<<endl;
    
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
    gROOT->LoadMacro("MuScleFitCorrector.h+");
    gROOT->LoadMacro("LeptonScaleCorrections.h+");
    gROOT->LoadMacro("EGammaMvaEleEstimator.cc+");
    gROOT->LoadMacro("ZGAngles.cc+");
    gROOT->LoadMacro("AnalysisParameters.cc+");
    gROOT->LoadMacro("ParticleSelectors.cc+");
    gROOT->LoadMacro("Dumper.cc+");
    gSystem->Load("libgfortran.so");
    gSystem->Load("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/hzgammaME/MCFM-6.6/obj/libmcfm_6p6.so");
    gSystem->Load("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/hzgammaME/libME.so");
    
    TChain* fChain = new TChain("ntupleProducer/eventTree");

    TString option = args;
    string suffix = ((TObjString *)(option.Tokenize(" ")->At(0)))->GetString();
    //string suffix = "DYJetsToLL_M-10To50_910";
    //string suffix = "DYJetsToLL_M-50_910";
    //string suffix = "TTJets_FullLep_910";
    //string suffix = "TTJets_Hadronic_910";
    //string suffix = "TTJets_SemiLep_910";

    //ifstream sourceFiles(Form("sourceFiles/%s.txt", suffix.c_str()));
    ifstream sourceFiles(Form("sourceFiles2/%s.txt", suffix.c_str()));
    string myLine;
    int count = 0;
    cout << "Adding files from " << suffix << " to chain..." << endl;
    
    while (sourceFiles >> myLine) {
        fChain->Add(myLine.c_str());
        ++count;
    }
    
    cout << count << " files added!" << endl;
    sourceFiles.close();
    
    TStopwatch timer;
    timer.Start();

    fChain->Process("amumuAnalyzer2.C+", args.c_str());

    cout << "\n\nDone!" << endl;
    cout << "CPU Time : " << timer.CpuTime() << endl;
    cout << "RealTime : " << timer.RealTime() << endl;
    cout << "\n";
}