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

  // Change muon pT cuts
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

  amumuTree->Branch("muonOne",&muonOne);
  amumuTree->Branch("muonTwo",&muonTwo);
  amumuTree->Branch("dimuon",&dimuon);
  amumuTree->Branch("bjet",&bjet);
  amumuTree->Branch("fjet",&fjet);
  amumuTree->Branch("met",&met);
  amumuTree->Branch("ncjets",&ncjets);
  amumuTree->Branch("nbjets",&nbjets);
  amumuTree->Branch("nfjets",&nfjets);
  amumuTree->Branch("nmuons",&nmuons);
  amumuTree->Branch("nelectrons",&nelectrons);
  amumuTree->Branch("nphotons",&nphotons);
  amumuTree->Branch("passSasha",&passSasha);
  amumuTree->Branch("passMass",&passMass);
  amumuTree->Branch("passFjet",&passFjet);
  amumuTree->Branch("muonOneCharge",&muonOneCharge);
  amumuTree->Branch("muonOneIsPF",&muonOneIsPF);
  amumuTree->Branch("muonOneIsGLB",&muonOneIsGLB);
  amumuTree->Branch("muonOneNormalizedChi2",&muonOneNormalizedChi2);
  amumuTree->Branch("muonOneNumberOfMatchedStations",&muonOneNumberOfMatchedStations);
  amumuTree->Branch("muonOneNumberOfValidMuonHits",&muonOneNumberOfValidMuonHits);
  amumuTree->Branch("muonOneNumberOfValidPixelHits",&muonOneNumberOfValidPixelHits);
  amumuTree->Branch("muonOneTrackLayersWithMeasurement",&muonOneTrackLayersWithMeasurement);
  amumuTree->Branch("muonOneDxy",&muonOneDxy);
  amumuTree->Branch("muonOneDz",&muonOneDz);
  amumuTree->Branch("muonOneTrkIso",&muonOneTrkIso);
  amumuTree->Branch("muonOneRelIso",&muonOneRelIso);
  amumuTree->Branch("muonTwoCharge",&muonTwoCharge);
  amumuTree->Branch("muonTwoIsPF",&muonTwoIsPF);
  amumuTree->Branch("muonTwoIsGLB",&muonTwoIsGLB);
  amumuTree->Branch("muonTwoNormalizedChi2",&muonTwoNormalizedChi2);
  amumuTree->Branch("muonTwoNumberOfMatchedStations",&muonTwoNumberOfMatchedStations);
  amumuTree->Branch("muonTwoNumberOfValidMuonHits",&muonTwoNumberOfValidMuonHits);
  amumuTree->Branch("muonTwoNumberOfValidPixelHits",&muonTwoNumberOfValidPixelHits);
  amumuTree->Branch("muonTwoTrackLayersWithMeasurement",&muonTwoTrackLayersWithMeasurement);
  amumuTree->Branch("muonTwoDxy",&muonTwoDxy);
  amumuTree->Branch("muonTwoDz",&muonTwoDz);
  amumuTree->Branch("muonTwoTrkIso",&muonTwoTrkIso);
  amumuTree->Branch("muonTwoRelIso",&muonTwoRelIso);
  amumuTree->Branch("bjetCSV",&bjetCSV);
  amumuTree->Branch("bjetCSVv1",&bjetCSVv1);
  amumuTree->Branch("bjetCSVMVA",&bjetCSVMVA);
  amumuTree->Branch("bjetPUID",&bjetPUID);
  amumuTree->Branch("fjetCSV",&fjetCSV);
  amumuTree->Branch("fjetCSVv1",&fjetCSVv1);
  amumuTree->Branch("fjetCSVMVA",&fjetCSVMVA);
  amumuTree->Branch("fjetPUID",&fjetPUID);
  amumuTree->Branch("x",&x);

  genTree.reset(new TTree(("genTree_"+params->suffix).c_str(),"three body mass values"));
  //genTree->Branch("muonOneGen",&muonOneGen);
  //genTree->Branch("muonTwoGen",&muonTwoGen);

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


  ///////////
  // Muons //
  ///////////

  vector<TCMuon> muonsID;
  vector<TCMuon> muonsIDIso;
  //vector<TCMuon> muonsIDIsoUnCor;
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


  ///////////////
  // Electrons //
  ///////////////

  vector<TCElectron> electronsID;
  vector<TCElectron> electronsIDIso;

  for (int i = 0; i <  recoElectrons->GetSize(); ++i) {
    TCElectron* thisElec = (TCElectron*) recoElectrons->At(i);

    bool passID = false;
    bool passIso = false;

    if (particleSelector->PassElectronID(*thisElec, cuts->mediumElID, *recoMuons, false)) passID = true;
    if (particleSelector->PassElectronIso(*thisElec, cuts->mediumElIso, cuts->EAEle)) passIso = true;

    // eng cor

    //if(params->engCor && !params->doSync && params->PU == "S10") UniversalEnergyCorrector(*thisElec);

    if(passID) electronsID.push_back(*thisElec);
    if(passID&&passIso) electronsIDIso.push_back(*thisElec);
  }


  /////////////
  // Photons //
  /////////////

  vector<TCPhoton> photonsID;
  vector<TCPhoton> photonsIDIso;
  for (Int_t i = 0; i < recoPhotons->GetSize(); ++i) {
    TCPhoton* thisPhoton = (TCPhoton*) recoPhotons->At(i);

    bool passID = false;
    bool passIso = false;

    // non MVA selection
    if(!params->doPhoMVA){
      if (particleSelector->PassPhotonID(*thisPhoton, cuts->mediumPhID)) passID = true;
      if (particleSelector->PassPhotonIso(*thisPhoton, cuts->mediumPhIso, cuts->EAPho)) passIso = true;

    // MVA Selection
    }else{
      bool goodLepPre = false;
      if (particleSelector->PassPhotonID(*thisPhoton, cuts->preSelPhID)) passID = true;
      //if (params->selection == "mumuGamma"){
      //  if (muonsIDIso.size() > 1){
      //    goodLepPre = GoodLeptonsCat( muonsIDIso[0], muonsIDIso[1]);
      //  }
      //}else{
      //  if (electronsIDIso.size() > 1){
      //    goodLepPre = GoodLeptonsCat( electronsIDIso[0], electronsIDIso[1]);
      //  }
      //}
      if (particleSelector->PassPhotonMVA(*thisPhoton, cuts->catPhMVAID, goodLepPre)) passIso = true;
    }

    // energy correction after ID and ISO

    //if (params->engCor && !params->doSync) UniversalEnergyCorrector(*thisPhoton, genPhotons);

    if (passID) photonsID.push_back(*thisPhoton);
    if (passID && passIso) photonsIDIso.push_back(*thisPhoton);
  }


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
  int passCJVcnt = 0;
  if (cjetsVetoID.size() > 0){
    for (Int_t i = 0; i < cjetsVetoID.size(); ++i){
      if (goodBJet.DeltaR(cjetsVetoID[i])>0.1) {
        passCJVcnt += 1;
        //goodFJet = cjetsVetoID[i];
      }
    }
  }
  if (passCJVcnt > 0) passCJV = false;
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
  //passFjet = (fjetsID.size() > 0) && (fabs(ZP4.DeltaPhi(goodBJet+goodFJet))>2.5);
  //passFjet = (fjetsID.size() == 0) && (passCJVcnt == 1);
  //passFjet = (fjetsID.size() == 0) && (passCJVcnt == 1) && (fabs(ZP4.DeltaPhi(goodBJet+goodFJet))>2.5) && (recoMET->Mod() <= 40);
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


  //////////
  // FILL //
  //////////
  if (!passFjet)  return kTRUE;

  int muonOneInt = -1;
  int muonTwoInt = -1;
  if (lepton1.Pt() >= lepton2.Pt()){
    muonOneInt = lepton1int;
    muonTwoInt = lepton2int;
  }else{
    muonOneInt = lepton2int;
    muonTwoInt = lepton1int;
  }
  muonOne = muonsIDIso[muonOneInt];
  muonTwo = muonsIDIso[muonTwoInt];
  dimuon = ZP4;
  bjet = goodBJet;
  fjet = goodFJet;
  met.SetPxPyPzE(recoMET->Px(), recoMET->Py(), 0, 0);

  ncjets = cjetsVetoID.size();
  nbjets = bjetsID.size();
  nfjets = fjetsID.size();
  nmuons = muonsIDIso.size();
  nelectrons = electronsIDIso.size();
  nphotons = photonsIDIso.size();

  muonOneCharge = muonsIDIso[muonOneInt].Charge();
  muonOneIsPF = muonsIDIso[muonOneInt].IsPF();
  muonOneIsGLB = muonsIDIso[muonOneInt].IsGLB();
  muonOneNormalizedChi2 = muonsIDIso[muonOneInt].NormalizedChi2();
  muonOneNumberOfMatchedStations = muonsIDIso[muonOneInt].NumberOfMatchedStations();
  muonOneNumberOfValidMuonHits = muonsIDIso[muonOneInt].NumberOfValidMuonHits();
  muonOneNumberOfValidPixelHits = muonsIDIso[muonOneInt].NumberOfValidPixelHits();
  muonOneTrackLayersWithMeasurement = muonsIDIso[muonOneInt].TrackLayersWithMeasurement();
  muonOneDxy = muonsIDIso[muonOneInt].Dxy(&(*pvPosition));
  muonOneDz = muonsIDIso[muonOneInt].Dz(&(*pvPosition));
  muonOneTrkIso = muonsIDIso[muonOneInt].IdMap("SumPt_R03")/muonsIDIso[muonOneInt].Pt();
  muonOneRelIso = (muonsIDIso[muonOneInt].PfIsoChargedHad() 
                + max(0.,(double)muonsIDIso[muonOneInt].PfIsoNeutral() + muonsIDIso[muonOneInt].PfIsoPhoton() - 0.5*muonsIDIso[muonOneInt].PfIsoPU()))/muonsIDIso[muonOneInt].Pt();
  muonTwoCharge = muonsIDIso[muonTwoInt].Charge();
  muonTwoIsPF = muonsIDIso[muonTwoInt].IsPF();
  muonTwoIsGLB = muonsIDIso[muonTwoInt].IsGLB();
  muonTwoNormalizedChi2 = muonsIDIso[muonTwoInt].NormalizedChi2();
  muonTwoNumberOfMatchedStations = muonsIDIso[muonTwoInt].NumberOfMatchedStations();
  muonTwoNumberOfValidMuonHits = muonsIDIso[muonTwoInt].NumberOfValidMuonHits();
  muonTwoNumberOfValidPixelHits = muonsIDIso[muonTwoInt].NumberOfValidPixelHits();
  muonTwoTrackLayersWithMeasurement = muonsIDIso[muonTwoInt].TrackLayersWithMeasurement();
  muonTwoDxy = muonsIDIso[muonTwoInt].Dxy(&(*pvPosition));
  muonTwoDz = muonsIDIso[muonTwoInt].Dz(&(*pvPosition));
  muonTwoTrkIso = muonsIDIso[muonTwoInt].IdMap("SumPt_R03")/muonsIDIso[muonTwoInt].Pt();
  muonTwoRelIso = (muonsIDIso[muonTwoInt].PfIsoChargedHad() 
                + max(0.,(double)muonsIDIso[muonTwoInt].PfIsoNeutral() + muonsIDIso[muonTwoInt].PfIsoPhoton() - 0.5*muonsIDIso[muonTwoInt].PfIsoPU()))/muonsIDIso[muonTwoInt].Pt();

  bjetCSV = goodBJet.BDiscriminatorMap("CSV");
  bjetCSVv1 = goodBJet.BDiscriminatorMap("CSVv1");
  bjetCSVMVA = goodBJet.BDiscriminatorMap("CSVMVA");
  bjetPUID = goodBJet.IdMap("PUID_MVA");
  fjetCSV = goodFJet.BDiscriminatorMap("CSV");
  fjetCSVv1 = goodFJet.BDiscriminatorMap("CSVv1");
  fjetCSVMVA = goodFJet.BDiscriminatorMap("CSVMVA");
  fjetPUID = goodFJet.IdMap("PUID_MVA");
  x = passFjet ? ZP4.M() : 0.;  // for unbinned fit


  
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
