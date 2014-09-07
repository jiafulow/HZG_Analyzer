#!/bin/sh
. /etc/bashrc
export OSG_APP=/software/tier3/osg
export SCRAM_ARCH=slc5_amd64_gcc462
source /software/tier3/osg/cmsset_default.sh
scramv1 project CMSSW_6_1_1
cd CMSSW_6_1_1/src
cmsenv
cd ${_CONDOR_SCRATCH_DIR}
#cd /scratch/condor
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
  analyzer='higgsAnalyzer'
else
  analyzer="$8"
fi


#cp -v /tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/stageball.tar.gz .
tar -zxf stageball.tar.gz
mkdir -v higgsDir
mv -v $analyzer* higgsDir/.
mv -v input.txt higgsDir/.
mv -v otherHistos higgsDir/.
cd higgsDir

cat > run.C << +EOF
    
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
    cout<<"loading fortran"<<endl;
    gSystem->Load("libgfortran.so");
    gSystem->Load("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/hzgammaME/MCFM-6.6/obj/libmcfm_6p6.so");
    gSystem->Load("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/hzgammaME/libME.so");
    cout<<"fortran and ME loaded"<<endl;

    TChain* fChain = new TChain("ntupleProducer/eventTree");

    ifstream sourceFiles("input.txt");
    string myLine;
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
    }
    sourceFiles.close();

    TStopwatch timer;
    timer.Start();

    fChain->Process("$analyzer.C+",args.c_str());
  }
                                          
+EOF

root -l -b -q 'run.C("'$suffix' '$abcd' '$selection' '$period' '$dataName' '$count' '$PU'")'

mv *.root ../.
rm $analyzer*
rm ../input.txt 
rm run.C
rm ../process.DAT
rm ../input.DAT
rm ../stageball.tar.gz
rm ../garbage.txt
rm ../br.sm1
rm ../br.sm2
