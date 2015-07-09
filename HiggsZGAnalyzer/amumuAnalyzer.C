#define amumuAnalyzer_cxx
// The class definition in amumuAnalyzer.h has been generated automatically
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
// Root > T->Process("amumuAnalyzer.C")
// Root > T->Process("amumuAnalyzer.C","some options")
// Root > T->Process("amumuAnalyzer.C+")
//

#include "amumuAnalyzer.h"
#include <TH2.h>
#include <TStyle.h>


void amumuAnalyzer::Begin(TTree * tree)
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

  cuts->leadMuPt = 25;
  cuts->trailMuPt = 25;

  //params->engCor = false;

  cuts->InitEA(params->period);
  weighter.reset(new WeightUtils(*params, isRealData, runNumber));
  triggerSelector.reset(new TriggerSelector(params->selection, params->period, *triggerNames));
  vector<string> triggers{"HLT_Mu17_Mu8_v"};
  //vector<string> triggers{"HLT_IsoMu24_v"};
  triggerSelector->AddTriggers(triggers);
  triggerSelector->SetSelectedBits();
  rmcor2011.reset(new rochcor_2011(229+100*jobNum));
  rmcor2012.reset(new rochcor2012(229+100*jobNum));
  rEl.reset(new TRandom3(230+100*jobNum));
  rMuRun.reset(new TRandom3(231+100*jobNum));
  phoCorrector.reset(new zgamma::PhosphorCorrectionFunctor("../plugins/PHOSPHOR_NUMBERS_EXPFIT_ERRORS.txt", true));
  particleSelector.reset(new ParticleSelector(*params, *cuts, isRealData, runNumber));

  // Random numbers! //
  rnGenerator.reset(new TRandom3);

  amumuFile.reset(new TFile(("amumuFile_"+params->dataname+"_"+params->selection+"_"+params->jobCount+".root").c_str(), "RECREATE"));
  amumuFile->cd();
  amumuTree.reset(new TTree(("amumuTree_"+params->suffix).c_str(),"three body mass values"));
  hm.reset(new HistManager(amumuFile.get()));

  amumuTree->Branch("muonPos",&muonPos);
  amumuTree->Branch("muonNeg",&muonNeg);
  amumuTree->Branch("dimuon",&dimuon);
  amumuTree->Branch("bjet",&bjet);
  amumuTree->Branch("fjet",&fjet);

  genTree.reset(new TTree(("genTree_"+params->suffix).c_str(),"three body mass values"));
  genTree->Branch("muonPosGen",&muonPosGen);
  genTree->Branch("muonNegGen",&muonNegGen);
}


Bool_t amumuAnalyzer::Process(Long64_t entry)
{
  GetEntry(entry,1);
  particleSelector->SetRho(rhoFactor);

  /*
  // gen

  vector<TCGenParticle> genPhotons;
  vector<TCGenParticle> genMuons;
  vector<TCGenParticle> genZs;
  bool vetoDY = false;
  particleSelector->CleanUpGen(genHZG);
  if(!isRealData) particleSelector->FindGenParticles(*genParticles, *recoPhotons, genPhotons, genMuons, genZs, genHZG, vetoDY);

  bool muon1 = false;
  bool muon2 = false;
  bool pho = false;
  if (genMuons.size() > 1){
    for(vector<TCGenParticle>::iterator it = genMuons.begin(); it != genMuons.end(); ++it){
      if (it->GetStatus() == 1){
        if (it->Charge() == 1 && !muon1){
          muonPosGen = *it;
          muon1 = true;
        }else if(it->Charge() == -1 && !muon2){
          muonNegGen = *it;
          muon2 = true;
        }
      }
      if(muon1 && muon2) break;
    }
  }
    
  if (genPhotons.size() > 1){
    photonGen = genPhotons[0];
    pho = true;
  }
  if (muon1 && muon2 && pho){
    genTree->Fill();
  }

  

*/
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
  
  // muons

  vector<TCMuon> muonsID;
  vector<TCMuon> muonsIDIso;
  vector<TCMuon> muonsIDIsoUnCor;
  TLorentzVector tmpMuCor;

  for (int i = 0; i < recoMuons->GetSize(); ++ i)
    //for (int i = 0; i < pfMuons->GetSize(); ++ i)
  {
    TCMuon* thisMuon = (TCMuon*) recoMuons->At(i);    

    // Section for muon energy/momentum corrections.  NOTE: this will change the pt and thus ID/ISO of muon

    if(params->engCor){
      tmpMuCor = *thisMuon;
      float ptErrMu = 1.0;
      if (isRealData){
        if (params->abcd.find("D") != string::npos ){
          rmcor2012->momcor_data(tmpMuCor,thisMuon->Charge(),1,ptErrMu);
        }else{
          rmcor2012->momcor_data(tmpMuCor,thisMuon->Charge(),0,ptErrMu);
        }
      }

      if (!isRealData) rmcor2012->momcor_mc(tmpMuCor,thisMuon->Charge(),0,ptErrMu);
      thisMuon->SetPtEtaPhiM(tmpMuCor.Pt(), tmpMuCor.Eta(), tmpMuCor.Phi(), tmpMuCor.M());
    }


    //tight ID and Iso

    if (particleSelector->PassMuonID(*thisMuon, cuts->amumu_MuID) && particleSelector->PassMuonIso(*thisMuon, cuts->amumu_MuIso)){
      muonsIDIso.push_back(*thisMuon);
    }

  }

  sort(muonsIDIso.begin(), muonsIDIso.end(), P4SortCondition);

  /////////
  // MET //
  /////////

  //if(recoMET->Mod() > 40) return kTRUE;



  //////////
  // Jets //
  //////////

  vector<TCJet> fjetsID;
  vector<TCJet> cjetsVetoID;
  vector<TCJet> bjetsID;
  TCPhysObject goodFJet;
  TCPhysObject goodBJet;
  
  //forward jet finder
  for (Int_t i = 0; i < recoJets->GetSize(); ++i) {
    TCJet* thisJet = (TCJet*) recoJets->At(i);
    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_fJetID)) fjetsID.push_back(*thisJet);
  }
  sort(fjetsID.begin(), fjetsID.end(), P4SortCondition);

  if (fjetsID.size() >= 1){

    goodFJet= fjetsID[0];
  }else{
    return kTRUE;
  }



  // 2 good muons
  

  if (muonsIDIso.size() < 2) return kTRUE;
  bool firstMu = false;
  bool bothMus = false;
  for (UInt_t i = 0; i<muonsIDIso.size(); i++){
    if (!firstMu && (muonsIDIso[i].Pt() >cuts->leadMuPt)){
      firstMu = true;
    }else if (firstMu && (muonsIDIso[i].Pt() >cuts->trailMuPt)){
      bothMus = true;
      break;
    }
  }
  if (!bothMus) return kTRUE;

  TCPhysObject   lepton1;
  TCPhysObject   lepton2;
  int            lepton1int =-1;
  int            lepton2int =-1;
  TLorentzVector ZP4; 


  bool goodZ = particleSelector->FindGoodZMuon(muonsIDIso,lepton1,lepton2,ZP4,lepton1int,lepton2int,0.0);
  if (!goodZ) return kTRUE;

  //bjet finder
  for (Int_t i = 0; i < recoJets->GetSize(); ++i) {
    TCJet* thisJet = (TCJet*) recoJets->At(i);
    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), lepton1, lepton2, cuts->amumu_bJetID)) bjetsID.push_back(*thisJet);
  }
  sort(bjetsID.begin(), bjetsID.end(), P4SortCondition);

  if (bjetsID.size() == 1){

    goodBJet= bjetsID[0];
  }else{
    return kTRUE;
  }

  //central jet veto
  for (Int_t i = 0; i < recoJets->GetSize(); ++i) {
    TCJet* thisJet = (TCJet*) recoJets->At(i);
    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_cJetVetoID)) cjetsVetoID.push_back(*thisJet);
  }

  if (cjetsVetoID.size() > 0){
    for (Int_t i = 0; i < cjetsVetoID.size(); ++i){
      if (goodBJet.DeltaR(cjetsVetoID[0])>0.1) return kTRUE;
    }
  }

  // put them in the branch

  if (lepton1.Charge() == 1){
    muonPos = lepton1;
    muonNeg = lepton2;
  }else{
    muonPos = lepton2;
    muonNeg = lepton1;
  }
  fjet = goodFJet;
  bjet = goodBJet;
  dimuon = ZP4;
  hm->fill1DHist(ZP4.M(),"h1_dimuonMass_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 60, 10., 70., 1);     


  
  amumuTree->Fill();




  return kTRUE;
}

void amumuAnalyzer::Terminate()
{
  amumuFile->Write();
  amumuFile->Close();

}
