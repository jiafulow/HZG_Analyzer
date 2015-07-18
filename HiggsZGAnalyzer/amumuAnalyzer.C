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
  params->dumps          = true;

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
  //vector<string> triggers{"HLT_Mu17_Mu8_v"};
  //vector<string> triggers{"HLT_IsoMu24_v"};
  vector<string> triggers{"HLT_IsoMu24_eta2p1_v"};
  triggerSelector->AddTriggers(triggers);
  triggerSelector->SetSelectedBits();
  rmcor2011.reset(new rochcor_2011(229+100*jobNum));
  rmcor2012.reset(new rochcor2012(229+100*jobNum));
  rEl.reset(new TRandom3(230+100*jobNum));
  rMuRun.reset(new TRandom3(231+100*jobNum));
  phoCorrector.reset(new zgamma::PhosphorCorrectionFunctor("../plugins/PHOSPHOR_NUMBERS_EXPFIT_ERRORS.txt", true));
  particleSelector.reset(new ParticleSelector(*params, *cuts, isRealData, runNumber));
  dumper.reset(new Dumper(*params,*cuts,*particleSelector));
  if (params->dataname.find("Run2012D") != string::npos){
    muscleFitCor.reset(new MuScleFitCorrector("../interface/MuScleFit_2012D_DATA_ReReco_53X.txt"));
    //cout<<"musclefit for run D"<<endl;
  }else{
    muscleFitCor.reset(new MuScleFitCorrector("../interface/MuScleFit_2012ABC_DATA_ReReco_53X.txt"));
    //cout<<"musclefit for run ABC"<<endl;
  }


  // Random numbers! //
  rnGenerator.reset(new TRandom3);

  amumuFile.reset(new TFile(("amumuFile_"+params->dataname+"_"+params->selection+"_"+params->jobCount+".root").c_str(), "RECREATE"));
  amumuFile->cd();
  amumuTree.reset(new TTree(("amumuTree_"+params->suffix).c_str(),"three body mass values"));
  hm.reset(new HistManager(amumuFile.get()));

  amumuTree->Branch("muonPos",&muonPos);
  amumuTree->Branch("muonNeg",&muonNeg);
  amumuTree->Branch("dimuon",&dimuon);
  amumuTree->Branch("ncjets",&ncjets);
  amumuTree->Branch("nbjets",&nbjets);
  amumuTree->Branch("nfjets",&nfjets);
  amumuTree->Branch("bjet",&bjet);
  amumuTree->Branch("fjet",&fjet);
  amumuTree->Branch("met",&met);
  amumuTree->Branch("passSasha",&passSasha);
  amumuTree->Branch("passMass",&passMass);
  amumuTree->Branch("passFjet",&passFjet);
  amumuTree->Branch("bjetCSV",&bjetCSV);
  amumuTree->Branch("bjetCSVv1",&bjetCSVv1);
  amumuTree->Branch("bjetCSVMVA",&bjetCSVMVA);
  amumuTree->Branch("bjetPUID",&bjetPUID);
  amumuTree->Branch("fjetPUID",&fjetPUID);

  genTree.reset(new TTree(("genTree_"+params->suffix).c_str(),"three body mass values"));
  genTree->Branch("muonPosGen",&muonPosGen);
  genTree->Branch("muonNegGen",&muonNegGen);

  eidTree.reset(new TTree(("eidTree_"+params->suffix).c_str(), "event ID values"));
  eidTree->Branch("runNumber",  &runNumber,   "runNumber/i");
  eidTree->Branch("eventNumber",&eventNumber, "eventNumber/l");
  eidTree->Branch("lumiSection",&lumiSection, "lumiSection/i");
  eidTree->Branch("bunchCross", &bunchCross,  "bunchCross/i");

}


Bool_t amumuAnalyzer::Process(Long64_t entry)
{
  GetEntry(entry,1);
  particleSelector->SetRho(rhoFactor);
  particleSelector->SetEventNumber(eventNumber);
  dumper->SetRho(rhoFactor);
  dumper->SetRun(runNumber);
  dumper->SetEvent(eventNumber);
  dumper->SetLumi(lumiSection);

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

  // Sasha
  passSasha = false;
  std::vector<TString> sasha_eids = {
    "194210 330188889 353",
    "195013 155914555 137",
    "195013 432365639 294",
    "195016 500333554 461",
    "195530 191933309 121",
    "195930 473942607 534",
    "194076 44538663 39",
    "198208 6083441 8",
    "199608 1041294296 910",
    "200244 546645828 392",
    "201611 25727947 92",
    "201625 710936075 506",
    "202272 1324877158 1244",
    "203002 661609587 495",
    "205344 1290153995 1293",
    "205344 760686468 688",
    "206187 300828672 230",
    "206207 351305609 243",
    "207487 488341242 295",
    "207492 28138469 33",
    "206389 360651834 266",
    "206512 1191941546 972",
    "206542 890723046 596",
    "208429 18763549 16",
    "207454 1307316295 969",
    "207492 49930022 59",
    "207515 1541194263 1128",
    "207905 931671297 663",
    "208427 237292625 153",
    "208541 146684805 95",
    "206940 332466391 247",
    "205217 315513605 316",
    "207279 1265277673 976"
  };
  assert(sasha_eids.size() == 33);
  for (int i = 0; i < sasha_eids.size(); ++i) {
    TString eid = Form("%d %ld %d", runNumber, eventNumber, lumiSection);
    if (eid == sasha_eids.at(i)) {
        passSasha = true;
        std::cout << "SASHA: " << eid << std::endl;  //SASHA
        break;
    }
  }
  //if (!passSasha) return kTRUE;  //SASHA //FIXME


  // trigger

  for(int i = 0; i < 64; ++i) {
    unsigned long iHLT = 0x0; 
    iHLT = 0x01 << i;  
  } 

  bool triggerPass   = triggerSelector->SelectTrigger(triggerStatus, hltPrescale);
  if (passSasha)  std::cout << "SASHA: passTrigger: " << triggerPass << std::endl;  //SASHA
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
  if (passSasha)  std::cout << "SASHA: passVertex: " << (goodVertices.size()>=1) << std::endl;  //SASHA
  if (goodVertices.size() < 1) return kTRUE;

  pvPosition.reset(new TVector3());
  *pvPosition = goodVertices[0];
  particleSelector->SetPv(*pvPosition);
  dumper->SetPv(*pvPosition);
  
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
      /*
      float ptErrMu = 1.0;
      if (isRealData){
        if (params->abcd.find("D") != string::npos ){
          rmcor2012->momcor_data(tmpMuCor,thisMuon->Charge(),1,ptErrMu);
        }else{
          rmcor2012->momcor_data(tmpMuCor,thisMuon->Charge(),0,ptErrMu);
        }
      }

      if (!isRealData) rmcor2012->momcor_mc(tmpMuCor,thisMuon->Charge(),0,ptErrMu);
      */
      muscleFitCor->applyPtCorrection(tmpMuCor,thisMuon->Charge());
      //muscleFitCor->applyPtSmearing(tmpMuCor,thisMuon->Charge(),false);
      thisMuon->SetPtEtaPhiM(tmpMuCor.Pt(), tmpMuCor.Eta(), tmpMuCor.Phi(), tmpMuCor.M());
    }


    //tight ID and Iso

    if (particleSelector->PassMuonID(*thisMuon, cuts->amumu_MuID) && particleSelector->PassMuonIso(*thisMuon, cuts->amumu_MuIso)){
      muonsIDIso.push_back(*thisMuon);
    }

    if (params->dumps && passSasha) {  //SASHA
        dumper->MuonDump(*thisMuon, 1);
        std::cout << "runNumber: " <<  runNumber << " eventNumber: " << eventNumber << " lumiSection: " << lumiSection << " MUON: " << *thisMuon
            << " ID: " << particleSelector->PassMuonID(*thisMuon, cuts->amumu_MuID)
            << " looseMuIso: " << particleSelector->PassMuonIso(*thisMuon, cuts->looseMuIso)
            << " tightMuIso: " << particleSelector->PassMuonIso(*thisMuon, cuts->tightMuIso)
            << " trkIso: " << particleSelector->PassMuonIso(*thisMuon, cuts->amumu_MuIso) << endl;
    }

  }
  sort(muonsIDIso.begin(), muonsIDIso.end(), P4SortCondition);


  for (Int_t i = 0; i < recoJets->GetSize(); ++i) {
    TCJet* thisJet = (TCJet*) recoJets->At(i);

    if (params->dumps && passSasha) {  //SASHA
        dumper->JetDump(*thisJet, 1);
        std::cout << "runNumber: " <<  runNumber << " eventNumber: " << eventNumber << " lumiSection: " << lumiSection << " JET: " << *thisJet
            << " cJetVetoID: " << particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), TCPhysObject(), TCPhysObject(), cuts->amumu_cJetVetoID)
            << " bJetID: " << particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), TCPhysObject(), TCPhysObject(), cuts->amumu_bJetID)
            << " bJetIDv2: " << particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), TCPhysObject(), TCPhysObject(), cuts->amumu_bJetID_v2)
            << " fJetIDv2: " << particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_fJetID_v2)
            << endl;
    }
  }


  // 2 good muons
  
  if (passSasha)  std::cout << "SASHA: passMuMu: " << (muonsIDIso.size()>=2) << std::endl;  //SASHA
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
  if (passSasha)  std::cout << "SASHA: passBothMus: " << bothMus << std::endl;  //SASHA
  if (!bothMus) return kTRUE;

  TCPhysObject   lepton1;
  TCPhysObject   lepton2;
  int            lepton1int =-1;
  int            lepton2int =-1;
  TLorentzVector ZP4; 


  bool goodZ = particleSelector->FindGoodZMuon(muonsIDIso,lepton1,lepton2,ZP4,lepton1int,lepton2int,0.0);
  if (passSasha)  std::cout << "SASHA: passGoodZ: " << goodZ << std::endl;  //SASHA
  if (!goodZ) return kTRUE;

  // Apply dimuon mass window cut
  if (passSasha)  std::cout << "SASHA: passStep1: " << (12. <= ZP4.M() && ZP4.M() <= 70.) << std::endl;  //SASHA
  if (!(12. <= ZP4.M() && ZP4.M() <= 70.)) return kTRUE;
  hm->fill1DHist(1, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
  hm->fill1DHist(ZP4.M(), "h1_dimuonMass_1_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);


  //////////
  // Jets //
  //////////

  vector<TCJet> fjetsID;
  vector<TCJet> cjetsVetoID;
  vector<TCJet> bjetsID;
  TCJet goodFJet;
  TCJet goodBJet;

  //central jet finder
  for (Int_t i = 0; i < recoJets->GetSize(); ++i) {
    TCJet* thisJet = (TCJet*) recoJets->At(i);
    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), lepton1, lepton2, cuts->amumu_cJetVetoID)) cjetsVetoID.push_back(*thisJet);
  }
  //sort(cjetsVetoID.begin(), cjetsVetoID.end(), P4SortCondition);

  if (passSasha)  std::cout << "SASHA: passStep2: " << (cjetsVetoID.size() > 0) << std::endl;  //SASHA
  if (cjetsVetoID.size() > 0){
    // do nothing
  }else{
    return kTRUE;
  }
  hm->fill1DHist(2, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
  hm->fill1DHist(ZP4.M(), "h1_dimuonMass_2_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);

  //bjet finder
  for (Int_t i = 0; i < recoJets->GetSize(); ++i) {
    TCJet* thisJet = (TCJet*) recoJets->At(i);
    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), lepton1, lepton2, cuts->amumu_bJetID_v2)) bjetsID.push_back(*thisJet);
  }
  sort(bjetsID.begin(), bjetsID.end(), P4SortCondition);

  if (passSasha)  std::cout << "SASHA: passStep3: " << (bjetsID.size() > 0) << std::endl;  //SASHA
  if (bjetsID.size() > 0){
    goodBJet= bjetsID[0];
  }else{
    return kTRUE;
  }
  hm->fill1DHist(3, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
  hm->fill1DHist(ZP4.M(), "h1_dimuonMass_3_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);

  //central jet veto
  bool passCJV = true;
  if (cjetsVetoID.size() > 0){
    for (Int_t i = 0; i < cjetsVetoID.size(); ++i){
      if (goodBJet.DeltaR(cjetsVetoID[i])>0.1) {
        passCJV = false;
        break;
      }
    }
  }
  if (passSasha)  std::cout << "SASHA: passStep4: " << passCJV << std::endl;  //SASHA
  if (!passCJV) return kTRUE;
  hm->fill1DHist(4, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
  hm->fill1DHist(ZP4.M(), "h1_dimuonMass_4_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);

  //forward jet finder
  for (Int_t i = 0; i < recoJets->GetSize(); ++i) {
    TCJet* thisJet = (TCJet*) recoJets->At(i);
    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_fJetID_v2)) fjetsID.push_back(*thisJet);
  }
  sort(fjetsID.begin(), fjetsID.end(), P4SortCondition);

  if (fjetsID.size() > 0){

    goodFJet= fjetsID[0];
  }else{
    //return kTRUE;
  }

  passFjet = (fjetsID.size() > 0);
  if (passSasha)  std::cout << "SASHA: passStep5: " << passFjet << std::endl;  //SASHA
  if (passFjet) {
    hm->fill1DHist(5, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
    hm->fill1DHist(ZP4.M(), "h1_dimuonMass_5a_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);
  } else {
    hm->fill1DHist(ZP4.M(), "h1_dimuonMass_5b_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);
  }

  passMass = (26. <= ZP4.M() && ZP4.M() <= 32.);
  if (passSasha)  std::cout << "SASHA: passStep6: " << passMass << std::endl;  //SASHA
  if (passFjet && passMass)
    hm->fill1DHist(6, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);


  /////////
  // MET // 
  /////////

  //if(recoMET->Mod() > 40) return kTRUE;


  // put them in the branch

  if (lepton1.Charge() == 1){
    muonPos = lepton1;
    muonNeg = lepton2;
  }else{
    muonPos = lepton2;
    muonNeg = lepton1;
  }
  dimuon = ZP4;
  bjet = goodBJet;
  fjet = goodFJet;
  met.SetPxPyPzE(recoMET->Px(), recoMET->Py(), 0, 0);

  ncjets = cjetsVetoID.size();
  nbjets = bjetsID.size();
  nfjets = fjetsID.size();

  bjetCSV = goodBJet.BDiscriminatorMap("CSV");
  bjetCSVv1 = goodBJet.BDiscriminatorMap("CSVv1");
  bjetCSVMVA = goodBJet.BDiscriminatorMap("CSVMVA");
  bjetPUID = goodBJet.IdMap("PUID_MVA");
  fjetPUID = goodFJet.IdMap("PUID_MVA");


  
  amumuTree->Fill();

  eidTree->Fill();


  return kTRUE;
}

void amumuAnalyzer::Terminate()
{
  amumuFile->Write();
  amumuFile->Close();

  dumper->CloseDumps();
}
