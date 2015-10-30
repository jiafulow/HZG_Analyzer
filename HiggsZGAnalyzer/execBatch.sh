#!/bin/sh

if [ ! -z "${_CONDOR_SCRATCH_DIR}" ]
then
  is_batch=1

  . /etc/bashrc
  echo `hostname`
  export SCRAM_ARCH=slc5_amd64_gcc472
  export OSG_APP=/software/tier3/osg
  source ${OSG_APP}/cmsset_default.sh
  scram project CMSSW CMSSW_6_1_1
  cd CMSSW_6_1_1/src
  eval `scramv1 runtime -sh`
  cd ${_CONDOR_SCRATCH_DIR}
  #cd /scratch/condor
else
  is_batch=0
fi

#### Leave this blank #######

#############################

count=$1
dataName=$2

suffix=$3
abcd=$4
selection=$5
period=$6
PU=$7
if [ -z $8 ]
then
  analyzer="higgsAnalyzer"
else
  analyzer=$8
fi

if [ $is_batch -eq 1 ]
then
  #cp -v /tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/stageball.tar.gz .
  tar -zxf stageball.tar.gz
  mkdir -v higgsDir
  mv -v $analyzer* higgsDir/.
  mv -v input.txt higgsDir/.
  mv -v otherHistos higgsDir/.
  cd higgsDir

  input="input.txt"
else
  count="local"

  input="sourceFiles/$2.txt"
fi

cat > run.C << +EOF

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

void run(string args="") {
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
    cout<<"loading fortran"<<endl;
    gSystem->Load("libgfortran.so");
    gSystem->Load("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/hzgammaME/MCFM-6.6/obj/libmcfm_6p6.so");
    gSystem->Load("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/hzgammaME/libME.so");
    cout<<"fortran and ME loaded"<<endl;

    TChain* fChain = new TChain("ntupleProducer/eventTree");

    ifstream sourceFiles("$input");
    string myLine;
    int count = 0;
    cout<<"Adding files to chain..."<<endl;

    while (sourceFiles >> myLine) {
        //if (count == 0 && myLine.find("dcache")==string::npos){
        //    float rhoFactor;
        //    TBranch        *b_rhoFactor;   //!
        //    TFile fixFile(myLine.c_str(),"open");
        //    TTree *fixTree = (TTree*)fixFile.Get("ntupleProducer/eventTree");
        //    fixTree->SetBranchAddress("rhoFactor",&rhoFactor,&b_rhoFactor);
        //    for(int i =0; i<fixTree->GetEntries();i++){
        //      fixTree->GetEntry(i);
        //    }
        //    delete fixTree;
        //}

        fChain->Add(myLine.c_str());
        ++count;
    }
    cout<<count<<" files added!"<<endl;
    sourceFiles.close();

    TStopwatch timer;
    timer.Start();

    fChain->Process("$analyzer.C+", args.c_str());

    cout << "\n\nDone!" << endl;
    cout << "CPU Time : " << timer.CpuTime() << endl;
    cout << "RealTime : " << timer.RealTime() << endl;
    cout << "\n";
}

+EOF

root -l -b -q 'run.C("'$suffix' '$abcd' '$selection' '$period' '$dataName' '$count' '$PU'")'
if [ $is_batch -eq 1 ]
then
  mv *.root ..
else
  #mv *local.root localHistos/
  rm run.C
fi

