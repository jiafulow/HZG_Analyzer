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
  params->doScaleFactors = true;

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
  //vector<string> triggers{"HLT_Mu17_Mu8_v", "HLT_Mu17_TkMu8_v"};
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
  amumuTree->Branch("passStep5",&passStep5);
  amumuTree->Branch("passStep6",&passStep6);
  amumuTree->Branch("passStep7",&passStep7);
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
  amumuTree->Branch("w", &w);

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

  // Check data integrity
  int run = 0;
  if (isRealData) {
    if (190456 <= runNumber && runNumber <= 193621)
      run = 0;
    else if (193833 <= runNumber && runNumber <= 196531)
      run = 1;
    else if (198022 <= runNumber && runNumber <= 203742)
      run = 2;
    else if (203777 <= runNumber && runNumber <= 208686)
      run = 3;
  }
  hm->fill1DHist(run, "h1_runPeriod_"+params->suffix, "; run period;N_{evts}", 10, 0., 10., 1);

  hm->fill1DHist(0, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);


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

  // Check Sasha events
  passSasha = false;

  const std::vector<TString> sasha_eids = {
    "194210 353 330188889",
    "195013 137 155914555",
    "195013 294 432365639",
    "195016 461 500333554",
    "195530 121 191933309",
    "195930 534 473942607",
    "194076 39 44538663",
    "198208 8 6083441",
    "199608 910 1041294296",
    "200244 392 546645828",
    "201611 92 25727947",
    "201625 506 710936075",
    "202272 1244 1324877158",
    "203002 495 661609587",
    "205344 1293 1290153995",
    "205344 688 760686468",
    "206187 230 300828672",
    "206207 243 351305609",
    "207487 295 488341242",
    "207492 33 28138469",
    "206389 266 360651834",
    "206512 972 1191941546",
    "206542 596 890723046",
    "208429 16 18763549",
    "207454 969 1307316295",
    "207492 59 49930022",
    "207515 1128 1541194263",
    "207905 663 931671297",
    "208427 153 237292625",
    "208541 95 146684805",
    "206940 247 332466391",
    "205217 316 315513605",
    "207279 976 1265277673"
  };
/*
  const std::vector<TString> sasha_eids = {
    "191830 125 152854442",
    "190705 53 50860738",
    "193621 1209 975815942",
    "190895 123 111107771",
    "195016 136 161083986",
    "194224 206 323759619",
    "194455 161 236424459",
    "194480 1265 1087237460",
    "194789 215 289604351",
    "195304 992 1094662329",
    "195552 1419 1595419498",
    "194912 163 248510143",
    "195930 442 392233716",
    "199573 4 2911156",
    "200473 335 347337178",
    "199428 261 330250825",
    "199876 127 154045284",
    "199754 754 735945030",
    "200042 313 260799519",
    "201097 348 466797852",
    "201679 76 65045593",
    "201164 312 455736849",
    "202469 89 36484059",
    "198269 84 139280575",
    "202972 532 664432225",
    "202478 52 58305710",
    "202299 233 309616793",
    "198487 354 417831070",
    "205111 399 560426709",
    "205193 861 1101003681",
    "205238 193 205662660",
    "203894 385 432423733",
    "205617 182 194734479",
    "205694 227 246522965",
    "205826 421 601271515",
    "206484 265 365712461",
    "205193 262 368420410",
    "206512 690 886257376",
    "203912 591 679065943",
    "207099 457 664635525",
    "207898 77 130106298",
    "207515 198 280295923",
    "204113 93 79839868"
  };
*/

  if (isRealData) {
    for (UInt_t i = 0; i < sasha_eids.size(); ++i) {
      TString eid = Form("%u %u %llu", runNumber, lumiSection, eventNumber);
      if (eid == sasha_eids.at(i)) {
          passSasha = true;
          std::cout << "SASHA: " << eid << std::endl;  //SASHA
          break;
      }
    }
  }


  // Check trigger

  bool passTrigger = triggerSelector->SelectTrigger(triggerStatus, hltPrescale);
  if (passSasha)  std::cout << "SASHA: passTrigger: " << passTrigger << std::endl;  //SASHA
  if (!passTrigger) return kTRUE;


  // Check primary vertex

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

  bool passVertex = (goodVertices.size() > 0);
  if (passSasha)  std::cout << "SASHA: passVertex: " << passVertex << std::endl;  //SASHA
  if (!passVertex) return kTRUE;

  pvPosition.reset(new TVector3());
  *pvPosition = goodVertices[0];
  particleSelector->SetPv(*pvPosition);
  dumper->SetPv(*pvPosition);

  // Dump info
  for (Int_t i = 0; i < recoMuons->GetSize(); ++ i)
  {
    TCMuon* thisMuon = (TCMuon*) recoMuons->At(i);

    if (params->dumps && passSasha) {  //SASHA
      dumper->MuonDump(*thisMuon, 1);
      std::cout << "runNumber: " <<  runNumber << " eventNumber: " << eventNumber << " lumiSection: " << lumiSection << " MUON: " << *thisMuon
          << " ID: " << particleSelector->PassMuonID(*thisMuon, cuts->amumu_MuID)
          << " looseMuIso: " << particleSelector->PassMuonIso(*thisMuon, cuts->looseMuIso)
          << " tightMuIso: " << particleSelector->PassMuonIso(*thisMuon, cuts->tightMuIso)
          << " trkIso: " << particleSelector->PassMuonIso(*thisMuon, cuts->amumu_MuIso)
          << endl;

      //const map<string, vector<string> >& myMap = thisMuon->GetTriggers();
      //std::cout << "IsTriggered: " << thisMuon->IsTriggered() << " Triggers: " << myMap.size();
      //map<string, vector<string> >::const_iterator it;
      //for(it = myMap.begin(); it!=myMap.end(); it++){
      //  cout << it->first << " ";
      //  for(vector<string>::const_iterator vit=it->second.begin(); vit!=it->second.end(); vit++){
      //    cout << (*vit) << " ";
      //  }
      //}
      //cout << endl;
    }
  }

  for (Int_t i = 0; i < recoJets->GetSize(); ++i)
  {
    TCJet* thisJet = (TCJet*) recoJets->At(i);

    if (params->dumps && passSasha) {  //SASHA
      dumper->JetDump(*thisJet, 1);
      std::cout << "runNumber: " <<  runNumber << " eventNumber: " << eventNumber << " lumiSection: " << lumiSection << " JET: " << *thisJet
          << " cJetVetoID: " << particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_cJetVetoID)
          << " bJetID: " << particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_bJetID)
          << " bJetIDv2: " << particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_bJetID_v2)
          << " fJetIDv2: " << particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_fJetID_v2)
          << endl;
    }
  }


  ///////////
  // Muons //
  ///////////

  vector<TCMuon> muonsID;
  vector<TCMuon> muonsIDIso;
  //vector<TCMuon> muonsIDIsoUnCor;
  TLorentzVector tmpMuCor;

  for (Int_t i = 0; i < recoMuons->GetSize(); ++ i)
  {
    TCMuon* thisMuon = (TCMuon*) recoMuons->At(i);

    // Section for muon energy/momentum corrections.  NOTE: this will change the pt and thus ID/ISO of muon

    if(params->engCor){
      tmpMuCor = *thisMuon;

      //float ptErrMu = 1.0;
      //if (isRealData){
      //  if (params->abcd.find("D") != string::npos ){
      //    rmcor2012->momcor_data(tmpMuCor,thisMuon->Charge(),1,ptErrMu);
      //  }else{
      //    rmcor2012->momcor_data(tmpMuCor,thisMuon->Charge(),0,ptErrMu);
      //  }
      //}
      //
      //if (!isRealData) rmcor2012->momcor_mc(tmpMuCor,thisMuon->Charge(),0,ptErrMu);

      muscleFitCor->applyPtCorrection(tmpMuCor,thisMuon->Charge());
      //muscleFitCor->applyPtSmearing(tmpMuCor,thisMuon->Charge(),false);
      thisMuon->SetPtEtaPhiM(tmpMuCor.Pt(), tmpMuCor.Eta(), tmpMuCor.Phi(), tmpMuCor.M());
    }


    // ID and Iso
    if (particleSelector->PassMuonID(*thisMuon, cuts->amumu_MuID) && particleSelector->PassMuonIso(*thisMuon, cuts->amumu_MuIso)){
      muonsIDIso.push_back(*thisMuon);
    }

  }
  sort(muonsIDIso.begin(), muonsIDIso.end(), P4SortCondition);


  // 2 good muons
  bool firstMu = false;
  bool bothMus = false;
  for (UInt_t i = 0; i<muonsIDIso.size(); i++){
    if (!firstMu && (muonsIDIso[i].Pt() > cuts->leadMuPt)){
      firstMu = true;
    }else if (firstMu && (muonsIDIso[i].Pt() > cuts->trailMuPt)){
      bothMus = true;
      break;
    }
  }
  if (passSasha)  std::cout << "SASHA: passMuMu: " << bothMus << std::endl;  //SASHA
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
  bool passStep1 = (12. <= ZP4.M() && ZP4.M() <= 70.);
  if (passSasha)  std::cout << "SASHA: passStep1: " << passStep1 << std::endl;  //SASHA
  if (passStep1) {
    hm->fill1DHist(1, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
    hm->fill1DHist(ZP4.M(), "h1_dimuonMass_1_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);
  } else {
    return kTRUE;
  }


  ///////////////
  // Electrons //
  ///////////////

  vector<TCElectron> electronsID;
  vector<TCElectron> electronsIDIso;

  for (Int_t i = 0; i < recoElectrons->GetSize(); ++i) {
    TCElectron* thisElec = (TCElectron*) recoElectrons->At(i);

    bool passID = false;
    bool passIso = false;

    if (particleSelector->PassElectronID(*thisElec, cuts->looseElID, *recoMuons, false)) passID = true;
    if (particleSelector->PassElectronIso(*thisElec, cuts->looseElIso, cuts->EAEle)) passIso = true;

    //if (particleSelector->PassElectronID(*thisElec, cuts->mediumElID, *recoMuons, false)) passID = true;
    //if (particleSelector->PassElectronIso(*thisElec, cuts->mediumElIso, cuts->EAEle)) passIso = true;

    // energy correction after ID and ISO

    //if(params->engCor && !params->doSync && params->PU == "S10") UniversalEnergyCorrector(*thisElec);

    if(passID) electronsID.push_back(*thisElec);
    if(passID&&passIso) electronsIDIso.push_back(*thisElec);
  }
  sort(electronsIDIso.begin(), electronsIDIso.end(), P4SortCondition);


  /////////////
  // Photons //
  /////////////

  vector<TCPhoton> photonsID;
  vector<TCPhoton> photonsIDIso;
  for (Int_t i = 0; i < recoPhotons->GetSize(); ++i) {
    TCPhoton* thisPhoton = (TCPhoton*) recoPhotons->At(i);

    bool passID = false;
    bool passIso = false;

    if (particleSelector->PassPhotonID(*thisPhoton, cuts->loosePhID)) passID = true;
    if (particleSelector->PassPhotonIso(*thisPhoton, cuts->loosePhIso, cuts->EAPho)) passIso = true;

    // energy correction after ID and ISO

    //if (params->engCor && !params->doSync) UniversalEnergyCorrector(*thisPhoton, genPhotons);

    if (passID) photonsID.push_back(*thisPhoton);
    if (passID && passIso) photonsIDIso.push_back(*thisPhoton);
  }
  sort(photonsIDIso.begin(), photonsIDIso.end(), P4SortCondition);


  /////////////////
  // Lepton Veto //
  /////////////////

  double leptonVetoPt = 20;
  int muonVetoCount = 0;
  for (UInt_t j=0; j < muonsIDIso.size(); ++j) {
    if (muonsIDIso.at(j).Pt() > leptonVetoPt)
      muonVetoCount += 1;
  }

  int electronVetoCount = 0;
  for (UInt_t j=0; j < electronsIDIso.size(); ++j) {
    if (electronsIDIso.at(j).Pt() > leptonVetoPt)
      electronVetoCount += 1;
  }

  //bool passLeptonVeto = (muonVetoCount == 2) && (electronVetoCount == 0);
  bool passLeptonVeto = true;  // always pass
  if (passSasha)  std::cout << "SASHA: passLepVeto: " << passLeptonVeto << std::endl;  //SASHA
  if (!passLeptonVeto) return kTRUE;


  //////////
  // Jets //
  //////////

  vector<TCJet> fjetsID;
  vector<TCJet> cjetsVetoID;
  vector<TCJet> bjetsID;
  TCJet goodFJet;
  TCJet goodBJet;

  double jetCleaningPt = 20;

  // jet finder
  for (Int_t i = 0; i < recoJets->GetSize(); ++i) {
    TCJet* thisJet = (TCJet*) recoJets->At(i);

    // Skip jets that overlap with leptons or photons
    bool skip = false;
    for (UInt_t j=0; j < muonsIDIso.size(); ++j) {
        const TCMuon& thisMuon = muonsIDIso.at(j);
        if (thisMuon.Pt() > jetCleaningPt && thisJet->DeltaR(thisMuon)<0.5)
            skip = true;
    }

    for (UInt_t j=0; j < electronsIDIso.size(); ++j) {
        const TCElectron& thisElec = electronsIDIso.at(j);
        if (thisElec.Pt() > jetCleaningPt && thisJet->DeltaR(thisElec)<0.5)
            skip = true;
    }

    for (UInt_t j=0; j < photonsIDIso.size(); ++j) {
        const TCPhoton& thisPhoton = photonsIDIso.at(j);
        if (thisPhoton.Pt() > jetCleaningPt && thisJet->DeltaR(thisPhoton)<0.5)
            skip = true;
    }

    if (skip)
        continue;

    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_cJetVetoID)) cjetsVetoID.push_back(*thisJet);

    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_bJetID_v2)) bjetsID.push_back(*thisJet);

    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_fJetID_v2)) fjetsID.push_back(*thisJet);
  }

  sort(cjetsVetoID.begin(), cjetsVetoID.end(), P4SortCondition);
  sort(bjetsID.begin(), bjetsID.end(), P4SortCondition);
  sort(fjetsID.begin(), fjetsID.end(), P4SortCondition);

  bool passStep2 = (cjetsVetoID.size() > 0);
  if (passSasha)  std::cout << "SASHA: passStep2: " << passStep2 << std::endl;  //SASHA
  if (passStep2){
    hm->fill1DHist(2, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
    hm->fill1DHist(ZP4.M(), "h1_dimuonMass_2_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);
  }else{
    return kTRUE;
  }

  bool passStep3 = (bjetsID.size() > 0);
  if (passSasha)  std::cout << "SASHA: passStep3: " << passStep3 << std::endl;  //SASHA
  if (passStep3){
    goodBJet= bjetsID[0];

    hm->fill1DHist(3, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
    hm->fill1DHist(ZP4.M(), "h1_dimuonMass_3_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);
  }else{
    return kTRUE;
  }

  // Central jet veto
  int goodBJetInt = -1;
  if (cjetsVetoID.size() > 0){
    for (UInt_t i = 0; i < cjetsVetoID.size(); ++i){
      if (goodBJet.DeltaR(cjetsVetoID[i])<0.1)
        goodBJetInt = i;
    }

    if (goodBJetInt != -1)
      cjetsVetoID.erase(cjetsVetoID.begin() + goodBJetInt);
  }

  bool passStep4 = (cjetsVetoID.size() == 0);
  //bool passStep4 = (cjetsVetoID.size() == 1);
  //bool passStep4 = true;
  if (passSasha)  std::cout << "SASHA: passStep4: " << passStep4 << std::endl;  //SASHA
  if (passStep4) {
    //goodFJet = cjetsVetoID[0];

    hm->fill1DHist(4, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
    hm->fill1DHist(ZP4.M(), "h1_dimuonMass_4_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);
  }else{
    return kTRUE;
  }

  passStep5 = (fjetsID.size() > 0);
  //passStep5 = (fjetsID.size() == 0) && (cjetsVetoID.size() == 1);
  //passStep5 = true;
  if (passSasha)  std::cout << "SASHA: passStep5: " << passStep5 << std::endl;  //SASHA
  if (passStep5){
    goodFJet = fjetsID[0];

    hm->fill1DHist(5, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
    hm->fill1DHist(ZP4.M(), "h1_dimuonMass_5a_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);
  }else{
    hm->fill1DHist(ZP4.M(), "h1_dimuonMass_5b_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);
    return kTRUE;
  }


  /////////
  // MET //
  /////////

  //if(recoMET->Mod() > 40) return kTRUE;

  bool passMETFilters = !(NoiseFilters_isScraping
                       || NoiseFilters_isNoiseHcalHBHE
                       || NoiseFilters_isNoiseHcalLaser
                       || NoiseFilters_isNoiseEcalTP
                       //|| NoiseFilters_isNoiseEcalBE
                       || NoiseFilters_isCSCTightHalo
                       //|| NoiseFilters_isCSCLooseHalo
                       || NoiseFilters_isNoiseTracking
                       || NoiseFilters_isNoiseEEBadSc
                       // Where is my ecal laser filter?
                       || NoiseFilters_isNoisetrkPOG1
                       || NoiseFilters_isNoisetrkPOG2
                       || NoiseFilters_isNoisetrkPOG3
                       );

  passStep6 = true;
  //passStep6 = passMETFilters && (recoMET->Mod() <= 40) && (fabs(ZP4.DeltaPhi(goodBJet+goodFJet))>2.5);
  //passStep6 = (cjetsVetoID.size() == 0) && (fjetsID.size() == 0);  // 1j-only
  //passStep6 = (cjetsVetoID.size() == 0) && (fjetsID.size() == 0) && passMETFilters && (recoMET->Mod() <= 40);  // 1j-only
  //passStep6 = (cjetsVetoID.size() == 1) && (fjetsID.size() == 0);  // 2j-only
  //passStep6 = (cjetsVetoID.size() == 1) && (fjetsID.size() == 0) && passMETFilters && (recoMET->Mod() <= 40);  // 2j-only
  if (passSasha)  std::cout << "SASHA: passStep6: " << passStep6 << std::endl;  //SASHA
  if (passStep6){
    //goodFJet = goodBJet;  //FIXME
    //goodFJet = cjetsVetoID[0]; //FIXME

    hm->fill1DHist(6, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
    hm->fill1DHist(ZP4.M(), "h1_dimuonMass_6a_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);
  }else{
    hm->fill1DHist(ZP4.M(), "h1_dimuonMass_6b_"+params->suffix, "M_{#mu#mu}; M_{#mu#mu};N_{evts}", 58, 12., 70., 1);
    return kTRUE;
  }

  passStep7 = (26. <= ZP4.M() && ZP4.M() <= 32.);
  if (passSasha)  std::cout << "SASHA: passStep7: " << passStep7 << std::endl;  //SASHA
  if (passStep7){
    hm->fill1DHist(7, "h1_cutFlow_"+params->suffix, "; cut flow step;N_{evts}", 10, 0., 10., 1);
  }else{
    // do nothing
  }


  ////////
  // MC //
  ////////
  
  float eventWeight = 1;
  float eventWeightPU = 1;
  float eventWeightPho = 1;
  float eventWeightLep = 1;
  float eventWeightTrig = 1;
  
  weighter->SetIsRealData(isRealData);
  weighter->SetRunNumber(runNumber);
  
  if (!isRealData){
  
    if (params->period.find("2011") != string::npos){
      if (params->doScaleFactors) eventWeight   *= weighter->PUWeight(nPUVertices);
      eventWeightPU   *= weighter->PUWeight(nPUVertices);
    }else if (params->period.find("2012") != string::npos){
      if (params->doScaleFactors) eventWeight   *= weighter->PUWeight(nPUVerticesTrue);
      eventWeightPU   *= weighter->PUWeight(nPUVerticesTrue);
    }

    if (params->doScaleFactors){
      //eventWeight   *= weighter->MuonTriggerWeight(lepton1, lepton2);
      eventWeight   *= weighter->MuonTriggerWeightV2(lepton1, lepton2);
      eventWeight   *= weighter->MuonSelectionWeight(lepton1);
      eventWeight   *= weighter->MuonSelectionWeight(lepton2);
    }
    //eventWeightTrig   *= weighter->MuonTriggerWeight(lepton1, lepton2);
    eventWeightTrig   *= weighter->MuonTriggerWeightV2(lepton1, lepton2);
    eventWeightLep   *= weighter->MuonSelectionWeight(lepton1);
    eventWeightLep   *= weighter->MuonSelectionWeight(lepton2);
  }
  //cout<<eventWeight<<endl;



  //////////
  // FILL //
  //////////
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
  x = ZP4.M();  // for unbinned fit
  w = eventWeight;  // event weight

  
  amumuTree->Fill();

  eidTree->Fill();


  return kTRUE;
}

void amumuAnalyzer::Terminate()
{
  cout<<"TOTAL NUMBER OF FILES: "<<fileCount<<" and they have this many events: "<<unskimmedEventsTotal<<endl;

  TH1I* totalEvents = new TH1I(("unskimmedEventsTotal_"+params->suffix).c_str(),("unskimmedEventsTotal_"+params->suffix).c_str(),1,0,1);
  totalEvents->SetBinContent(1,unskimmedEventsTotal);

  amumuFile->Write();
  amumuFile->Close();

  dumper->CloseDumps();
}
