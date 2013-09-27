#!/bin/csh
source /uscmst1/prod/sw/cms/cshrc prod
scram pro CMSSW CMSSW_5_3_8_patch1
cd CMSSW_5_3_8_patch1/src
cmsenv 
cd ${_CONDOR_SCRATCH_DIR}
#### Leave this blank #######

#############################
set count     = $1
set dataName  = $2

set suffix    = $3
set abcd      = $4
set selection = $5
set period    = $6

#mkdir otherHistos
#mkdir plugins
#mkdir printouts


tar -vzxf stageball.tar.gz
mkdir -v higgsDir
mv -v higgsAnalyzer_Template.C higgsDir/.
mv -v higgsAnalyzer.h higgsDir/.
mv -v input.txt higgsDir/.
cd -v higgsDir
mkdir -v otherHistos
mv -v ../*.root otherHistos/.

cp higgsAnalyzer_Template.C higgsAnalyzer.C

sed -i "s/SUFFIX/$suffix/g" higgsAnalyzer.C
sed -i "s/ABCD/$abcd/g" higgsAnalyzer.C
sed -i "s/SELECTION/$selection/g" higgsAnalyzer.C
sed -i "s/PERIOD/$period/g" higgsAnalyzer.C
sed -i "s/COUNT/$count/g" higgsAnalyzer.C
sed -i "s/DATANAME/$dataName/g" higgsAnalyzer.C
#sed -i "s/\.\.\/src/src/g" higgsAnalyzer.h

ls
cat > run.C << +EOF
    
  #include <iostream>
  #include <fstream>
  #include <string>
  #include <vector>
  #include <cstdlib>

  using namespace std;

  void run() {

    gROOT->SetMacroPath(".:../src/:../interface/:../plugins/");
    gROOT->LoadMacro("TCPhysObject.cc++");
    gROOT->LoadMacro("TCJet.cc++");
    gROOT->LoadMacro("TCMET.cc++");
    gROOT->LoadMacro("TCElectron.cc++");
    gROOT->LoadMacro("TCMuon.cc++");
    gROOT->LoadMacro("TCTau.cc++");
    gROOT->LoadMacro("TCPhoton.cc++");
    gROOT->LoadMacro("TCGenJet.cc++");
    gROOT->LoadMacro("TCGenParticle.cc++");
    gROOT->LoadMacro("TCPrimaryVtx.cc++");
    gROOT->LoadMacro("TCTriggerObject.cc++");
    gROOT->LoadMacro("HistManager.cc++");
    gROOT->LoadMacro("WeightUtils.cc++");
    gROOT->LoadMacro("TriggerSelector.cc++");
    gROOT->LoadMacro("ElectronFunctions.cc++");
    gROOT->LoadMacro("rochcor_2011.cc++");
    gROOT->LoadMacro("rochcor2012v2.C++");
    gROOT->LoadMacro("PhosphorCorrectorFunctor.cc++");
    gROOT->LoadMacro("LeptonScaleCorrections.h++");
    gROOT->LoadMacro("EGammaMvaEleEstimator.cc++");
    gROOT->LoadMacro("ZGAngles.cc++");
    gROOT->LoadMacro("AnalysisParameters.cc++");
    gROOT->LoadMacro("ParticleSelectors.cc++");
    gROOT->LoadMacro("Dumper.cc++");
    cout<<"loading fortran"<<endl;
    gSystem->Load("libgfortran.so");
    gSystem->Load("../hzgammaME/MCFM-6.6/obj/libmcfm_6p6.so");
    gSystem->Load("../hzgammaME/libME.so");
    cout<<"fortran and ME loaded"<<endl;

    TChain* fChain = new TChain("ntupleProducer/eventTree");

    ifstream sourceFiles("input.txt");
    string line;
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
    }
    sourceFiles.close();

    TStopwatch timer;
    timer.Start();

    fChain->Process("higgsAnalyzer.C++");
  }
                                          
+EOF

root -l -b -q run.C

mv *.root ../.
rm higgsAnalyzer*
rm ../input.txt 
rm run.C
rm process.DAT
rm stageball.tar.gz
rm garbage.txt
rm br.sm1
rm br.sm2
