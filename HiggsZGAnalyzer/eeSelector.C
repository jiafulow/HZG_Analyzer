#define eeSelector_cxx
// The class definition in eeSelector.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("eeSelector.C")
// Root > T->Process("eeSelector.C","some options")
// Root > T->Process("eeSelector.C+")
//

#include "eeSelector.h"
#include <TH2.h>
#include <TStyle.h>


void eeSelector::Begin(TTree * tree)
{
  /////////////////////////////
  //Specify parameters here. //
  /////////////////////////////

  TString option = GetOption();
  string suffix = static_cast<string>(static_cast<TObjString*>(option.Tokenize(" ")->At(0))->GetString());
  string abcd = static_cast<string>(static_cast<TObjString*>(option.Tokenize(" ")->At(1))->GetString());
  string selection = static_cast<string>(static_cast<TObjString*>(option.Tokenize(" ")->At(2))->GetString());
  string period = static_cast<string>(static_cast<TObjString*>(option.Tokenize(" ")->At(3))->GetString());
  string dataname = static_cast<string>(static_cast<TObjString*>(option.Tokenize(" ")->At(4))->GetString());
  string count = static_cast<string>(static_cast<TObjString*>(option.Tokenize(" ")->At(5))->GetString()); 

  params.reset(new Parameters());

  params->selection      = selection;
  params->period         = period;
  params->JC_LVL         = 0;
  params->abcd           = abcd;
  params->suffix         = suffix;
  params->dataname       = dataname;
  params->jobCount       = count;

  // Get trigger names from jobTree
  vector<string>* triggerNames = 0;
  TFile   *inFile         = tree->GetCurrentFile();
  TTree   *jobTree        = (TTree*)inFile->Get("ntupleProducer/jobTree");

  jobTree->SetBranchAddress("triggerNames", &triggerNames);
  jobTree->GetEntry();

  // Initialize utilities and selectors here //
  int jobNum = atoi(params->jobCount.c_str());
  cuts.reset(new Cuts());

  

  cuts->InitEA(params->period);
  weighter.reset(new WeightUtils(*params, isRealData, runNumber));
  triggerSelector.reset(new TriggerSelector(params->selection, params->period, *triggerNames));
  rmcor2011.reset(new rochcor_2011(229+100*jobNum));
  rmcor2012.reset(new rochcor2012(229+100*jobNum));
  rEl.reset(new TRandom3(230+100*jobNum));
  rMuRun.reset(new TRandom3(231+100*jobNum));
  phoCorrector.reset(new zgamma::PhosphorCorrectionFunctor("../plugins/PHOSPHOR_NUMBERS_EXPFIT_ERRORS.txt", true));
  particleSelector.reset(new ParticleSelector(*params, *cuts, isRealData, runNumber));

  // Random numbers! //
  rnGenerator.reset(new TRandom3);

  eleFile.reset(new TFile(("eleFile_"+params->dataname+"_"+params->selection+"_"+params->jobCount+".root").c_str(), "RECREATE"));
  eleFile->cd();
  eleTree.reset(new TTree(("eleTree_"+params->suffix).c_str(),"three body mass values"));

  eleTree->Branch("ele1",&ele1);
  eleTree->Branch("ele2",&ele2);
  eleTree->Branch("runNumber",&runNumber);
  eleTree->Branch("eventNumber",&eventNumber);
  eleTree->Branch("lumiSection",&lumiSection);
  eleTree->Branch("eventWeight",&eventWeight);
  eleTree->Branch("SCetaEl1",&SCetaEl1);
  eleTree->Branch("SCetaEl2",&SCetaEl2);
  
}


Bool_t eeSelector::Process(Long64_t entry)
{
  GetEntry(entry,1);
  particleSelector->SetRho(rhoFactor);
  weighter->SetIsRealData(isRealData);
  weighter->SetRunNumber(runNumber);

  // trigger

  for(int i = 0; i < 64; ++i) {
    unsigned long iHLT = 0x0; 
    iHLT = 0x01 << i;  
  } 

  bool triggerPass   = triggerSelector->SelectTrigger(triggerStatus, hltPrescale);
  if (params->period.find("2012") != string::npos) if (!triggerPass) return kTRUE;


  // vertex

  vector<TVector3> goodVertices;
  int nTracks = 0;
  for (int i = 0; i < primaryVtx->GetSize(); ++i) {
    TCPrimaryVtx* pVtx = (TCPrimaryVtx*) primaryVtx->At(i);
    nTracks+=pVtx->Ntracks();
    if (
        !pVtx->IsFake() 
        && pVtx->NDof() > 4.
        && fabs(pVtx->z()) <= 24.
        && fabs(pVtx->Perp()) <= 2.
       )

      goodVertices.push_back(*pVtx);

  }
  if (goodVertices.size() < 1) return kTRUE;

  pvPosition.reset(new TVector3());
  *pvPosition = goodVertices[0];
  particleSelector->SetPv(*pvPosition);

  // gen

  vector<TCGenParticle> genPhotons;
  vector<TCGenParticle> genMuons;
  bool vetoDY = false;
  particleSelector->CleanUpGen(genHZG);
  //genHZG = {0,0,0,0,0,0};
  if(!isRealData) particleSelector->FindGenParticles(*genParticles, *recoPhotons, genPhotons, genMuons, genHZG, vetoDY);

  ///////////////
  // electrons //
  ///////////////

  vector<TLorentzVector> extraLeptons;
  vector<TCElectron> electronsID;
  vector<TCElectron> electronsIDIso;

  for (int i = 0; i <  recoElectrons->GetSize(); ++i) {
    TCElectron* thisElec = (TCElectron*) recoElectrons->At(i);    

    bool passID = false;
    bool passIso = false;

    thisElec->SetPtEtaPhiM(thisElec->Pt(),thisElec->Eta(),thisElec->Phi(),0.000511);


    if(params->doEleMVA){

      /// low pt
      if (thisElec->Pt() < 20 && particleSelector->PassElectronID(*thisElec, cuts->mvaPreElID, *recoMuons, true) && thisElec->MvaID_Old() > -0.9){
        passID = true; 
      /// high pt
      }else if (thisElec->Pt() > 20 && particleSelector->PassElectronID(*thisElec, cuts->mvaPreElID, *recoMuons,true) && thisElec->MvaID_Old() > -0.5){
        passID = true; 
      }
      if (particleSelector->PassElectronIso(*thisElec, cuts->looseElIso, cuts->EAEle)) passIso = true;
                                                                  

    }else{
      if (particleSelector->PassElectronID(*thisElec, cuts->mediumElID, *recoMuons, false)) passID = true;
      if (particleSelector->PassElectronIso(*thisElec, cuts->mediumElIso, cuts->EAEle)) passIso = true; 
    } 
    
    // eng cor

    if(params->engCor && !params->doSync && isRealData) UniversalEnergyCorrector(*thisElec);


    if(passID) electronsID.push_back(*thisElec);
    if(passID&&passIso) electronsIDIso.push_back(*thisElec);

  }

  sort(electronsID.begin(), electronsID.end(), P4SortCondition);
  sort(electronsIDIso.begin(), electronsIDIso.end(), P4SortCondition);



  //////////////////////
  // Good Z selection //
  //////////////////////
  
  TCPhysObject   lepton1;
  TCPhysObject   lepton2;
  int            lepton1int =-1;
  int            lepton2int =-1;
  TLorentzVector uncorLepton1;
  TLorentzVector uncorLepton2;
  TLorentzVector ZP4;
  eventWeight = 1.0;

  if (electronsID.size() < 2) return kTRUE;

  bool firstEl = false;
  bool bothEls = false;
  for (UInt_t i = 0; i<electronsID.size(); i++){
    if (!firstEl && (electronsID[i].Pt() >cuts->trailElePt)){
      firstEl = true;
    }else if (firstEl && (electronsID[i].Pt() >cuts->trailElePt)){
      bothEls = true;
      break;
    }
  }
  if (!bothEls) return kTRUE;


  if (electronsIDIso.size() < 2) return kTRUE;
  firstEl = false;
  bothEls = false;
  for (UInt_t i = 0; i<electronsIDIso.size(); i++){
    if (!firstEl && (electronsIDIso[i].Pt() >cuts->trailElePt)){
      firstEl = true;
    }else if (firstEl && (electronsIDIso[i].Pt() >cuts->trailElePt)){
      bothEls = true;
      break;
    }
  }
  if (!bothEls) return kTRUE;

  bool goodZ = false;
  goodZ = particleSelector->FindGoodZElectron(electronsIDIso,lepton1,lepton2,ZP4,SCetaEl1,SCetaEl2,lepton1int,lepton2int);
  if (eventNumber == params->EVENTNUMBER) cout<<"el num: "<<electronsIDIso.size()<<" goodZ: "<<goodZ<<endl;
  if (!goodZ) return kTRUE;
  if (!isRealData){ 
    if (params->doScaleFactors){
      eventWeight   *= weighter->PUWeight(nPUVerticesTrue);
      //eventWeight   *= weighter->ElectronTriggerWeight(lepton1, lepton2, true);
      if (params->period.find("2011") != string::npos){
        //eventWeight   *= getEfficiencyWeight( &lepton1, CorrectionType::CENTRAL, atoi(params->period.c_str()));
        //eventWeight   *= getEfficiencyWeight( &lepton2, CorrectionType::CENTRAL, atoi(params->period.c_str()));
      }else if (params->period.find("2012") != string::npos){
        //eventWeight   *= weighter->ElectronSelectionWeight(lepton1);
        //eventWeight   *= weighter->ElectronSelectionWeight(lepton2);
      }
    }
  }


  // put them in the branch

  ele1 = lepton1;
  ele2 = lepton2;


  
  eleTree->Fill();




  return kTRUE;
}

void eeSelector::Terminate()
{
  eleFile->Write();
  eleFile->Close();

}


void eeSelector::UniversalEnergyCorrector(TCElectron& el){
if (params->period.find("2012") != string::npos){
  if (el.RegressionMomCombP4().E() != 0){
    el.SetPtEtaPhiM(el.RegressionMomCombP4().Pt(),el.RegressionMomCombP4().Eta(),el.RegressionMomCombP4().Phi(),0.000511);
  }
  if (eventNumber == params->EVENTNUMBER){
    cout<< "electron after cor: "<<TCPhysObject(el)<<endl;
  }
}
}
