#define amumuAnalyzer4_cxx
// The class definition in amumuAnalyzer4.h has been generated automatically
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
// Root > T->Process("amumuAnalyzer4.C")
// Root > T->Process("amumuAnalyzer4.C","some options")
// Root > T->Process("amumuAnalyzer4.C+")
//

#include "amumuAnalyzer4.h"
#include <TH2.h>
#include <TStyle.h>


void amumuAnalyzer4::Begin(TTree * tree)
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
  //cuts->leadMuPt = 25;
  //cuts->trailMuPt = 25;
  cuts->leadMuPt = 15;
  cuts->trailMuPt = 15;

  // Change jet pT cuts
  //cuts->leadJetPt = 30;
  //cuts->trailJetPt = 30;
  cuts->leadJetPt = 20;
  cuts->trailJetPt = 20;

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

  //amumuTree->Branch("muonOne",&muonOne);
  //amumuTree->Branch("muonTwo",&muonTwo);
  //amumuTree->Branch("dimuon",&dimuon);
  //amumuTree->Branch("jetOne",&jetOne);
  //amumuTree->Branch("jetTwo",&jetTwo);
  //amumuTree->Branch("jetThree",&jetThree);
  //amumuTree->Branch("met",&met);
  amumuTree->Branch("njets",&njets);
  amumuTree->Branch("ncjets",&ncjets);
  amumuTree->Branch("nbjets",&nbjets);
  amumuTree->Branch("nfjets",&nfjets);
  amumuTree->Branch("nmuons",&nmuons);
  amumuTree->Branch("njets_pt20",&njets_pt20);
  amumuTree->Branch("njets_pt25",&njets_pt25);
  amumuTree->Branch("njets_pt30",&njets_pt30);
  amumuTree->Branch("njets_pt40",&njets_pt40);
  amumuTree->Branch("njets_pt50",&njets_pt50);
  amumuTree->Branch("ncjets_pt20",&ncjets_pt20);
  amumuTree->Branch("ncjets_pt25",&ncjets_pt25);
  amumuTree->Branch("ncjets_pt30",&ncjets_pt30);
  amumuTree->Branch("ncjets_pt40",&ncjets_pt40);
  amumuTree->Branch("ncjets_pt50",&ncjets_pt50);
  amumuTree->Branch("nbjets_pt20",&nbjets_pt20);
  amumuTree->Branch("nbjets_pt25",&nbjets_pt25);
  amumuTree->Branch("nbjets_pt30",&nbjets_pt30);
  amumuTree->Branch("nbjets_pt40",&nbjets_pt40);
  amumuTree->Branch("nbjets_pt50",&nbjets_pt50);
  amumuTree->Branch("nfjets_pt20",&nfjets_pt20);
  amumuTree->Branch("nfjets_pt25",&nfjets_pt25);
  amumuTree->Branch("nfjets_pt30",&nfjets_pt30);
  amumuTree->Branch("nfjets_pt40",&nfjets_pt40);
  amumuTree->Branch("nfjets_pt50",&nfjets_pt50);
  amumuTree->Branch("nelectrons",&nelectrons);
  amumuTree->Branch("nphotons",&nphotons);
  amumuTree->Branch("muonOneCharge",&muonOneCharge);
  amumuTree->Branch("muonOnePt",&muonOnePt);
  amumuTree->Branch("muonOneEta",&muonOneEta);
  amumuTree->Branch("muonOnePhi",&muonOnePhi);
  amumuTree->Branch("muonOneEnergy",&muonOneEnergy);
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
  amumuTree->Branch("muonTwoPt",&muonTwoPt);
  amumuTree->Branch("muonTwoEta",&muonTwoEta);
  amumuTree->Branch("muonTwoPhi",&muonTwoPhi);
  amumuTree->Branch("muonTwoEnergy",&muonTwoEnergy);
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
  amumuTree->Branch("dimuonPt",&dimuonPt);
  amumuTree->Branch("dimuonEta",&dimuonEta);
  amumuTree->Branch("dimuonPhi",&dimuonPhi);
  amumuTree->Branch("dimuonEnergy",&dimuonEnergy);
  amumuTree->Branch("dimuonMass",&dimuonMass);
  amumuTree->Branch("dimuonDeltaR",&dimuonDeltaR);
  amumuTree->Branch("jetOnePt",&jetOnePt);
  amumuTree->Branch("jetOneEta",&jetOneEta);
  amumuTree->Branch("jetOnePhi",&jetOnePhi);
  amumuTree->Branch("jetOneEnergy",&jetOneEnergy);
  amumuTree->Branch("jetOneCSV",&jetOneCSV);
  amumuTree->Branch("jetOneCSVv1",&jetOneCSVv1);
  amumuTree->Branch("jetOneCSVMVA",&jetOneCSVMVA);
  amumuTree->Branch("jetOnePUID",&jetOnePUID);
  amumuTree->Branch("jetTwoPt",&jetTwoPt);
  amumuTree->Branch("jetTwoEta",&jetTwoEta);
  amumuTree->Branch("jetTwoPhi",&jetTwoPhi);
  amumuTree->Branch("jetTwoEnergy",&jetTwoEnergy);
  amumuTree->Branch("jetTwoCSV",&jetTwoCSV);
  amumuTree->Branch("jetTwoCSVv1",&jetTwoCSVv1);
  amumuTree->Branch("jetTwoCSVMVA",&jetTwoCSVMVA);
  amumuTree->Branch("jetTwoPUID",&jetTwoPUID);
  amumuTree->Branch("jetThreePt",&jetThreePt);
  amumuTree->Branch("jetThreeEta",&jetThreeEta);
  amumuTree->Branch("jetThreePhi",&jetThreePhi);
  amumuTree->Branch("jetThreeEnergy",&jetThreeEnergy);
  amumuTree->Branch("jetThreeCSV",&jetThreeCSV);
  amumuTree->Branch("jetThreeCSVv1",&jetThreeCSVv1);
  amumuTree->Branch("jetThreeCSVMVA",&jetThreeCSVMVA);
  amumuTree->Branch("jetThreePUID",&jetThreePUID);
  amumuTree->Branch("metPt",&metPt);
  amumuTree->Branch("metPhi",&metPhi);

  amumuTree->Branch("runNumber",  &runNumber);
  amumuTree->Branch("eventNumber",&eventNumber);
  amumuTree->Branch("lumiSection",&lumiSection);
  amumuTree->Branch("bunchCross", &bunchCross);
  //amumuTree->Branch("lhe_nup", &lhe_nup); lhe_nup = 0;

  amumuTree->Branch("x",&x);
  amumuTree->Branch("w", &w);

  genTree.reset(new TTree(("genTree_"+params->suffix).c_str(),"three body mass values"));
  //genTree->Branch("muonOneGen",&muonOneGen);
  //genTree->Branch("muonTwoGen",&muonTwoGen);
  //genTree->Branch("bjetGen",&bjetGen);
  //genTree->Branch("fjetGen",&fjetGen);

}


Bool_t amumuAnalyzer4::Process(Long64_t entry)
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


  // Check trigger

  bool passTrigger = triggerSelector->SelectTrigger(triggerStatus, hltPrescale);
  //if (!passTrigger) return kTRUE;


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
  if (!passVertex) return kTRUE;

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
  if (!bothMus) return kTRUE;

  TCPhysObject   lepton1;
  TCPhysObject   lepton2;
  int            lepton1int =-1;
  int            lepton2int =-1;
  TLorentzVector ZP4;


  bool goodZ = particleSelector->FindGoodZMuon(muonsIDIso,lepton1,lepton2,ZP4,lepton1int,lepton2int,0.0);
  if (!goodZ) return kTRUE;


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


  //////////
  // Jets //
  //////////

  vector<TCJet> fjetsID;
  vector<TCJet> cjetsVetoID;
  vector<TCJet> bjetsID;
  vector<TCJet> ajetsVetoID;  // any jet
  TCJet goodFJet;
  TCJet goodBJet;

  double jetCleaningPt = 20;

  njets_pt20 = 0;
  njets_pt25 = 0;
  njets_pt30 = 0;
  njets_pt40 = 0;
  njets_pt50 = 0;
  ncjets_pt20 = 0;
  ncjets_pt25 = 0;
  ncjets_pt30 = 0;
  ncjets_pt40 = 0;
  ncjets_pt50 = 0;
  nbjets_pt20 = 0;
  nbjets_pt25 = 0;
  nbjets_pt30 = 0;
  nbjets_pt40 = 0;
  nbjets_pt50 = 0;
  nfjets_pt20 = 0;
  nfjets_pt25 = 0;
  nfjets_pt30 = 0;
  nfjets_pt40 = 0;
  nfjets_pt50 = 0;

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

    //if (skip)
    //    continue;

    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_cJetVetoID)) {
        if (!skip) {
            if (thisJet->Pt() > cuts->leadJetPt) {
                ajetsVetoID.push_back(*thisJet);
                cjetsVetoID.push_back(*thisJet);
            }

            if (thisJet->Pt() > 20) {
                ++njets_pt20;
                ++ncjets_pt20;
            }
            if (thisJet->Pt() > 25) {
                ++njets_pt25;
                ++ncjets_pt25;
            }
            if (thisJet->Pt() > 30) {
                ++njets_pt30;
                ++ncjets_pt30;
            }
            if (thisJet->Pt() > 40) {
                ++njets_pt40;
                ++ncjets_pt40;
            }
            if (thisJet->Pt() > 50) {
                ++njets_pt50;
                ++ncjets_pt50;
            }
        }
    }

    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_bJetID_v2)) {
        if (!skip) {
            if (thisJet->Pt() > cuts->leadJetPt) {
                bjetsID.push_back(*thisJet);
            }

            if (thisJet->Pt() > 20) {
                ++nbjets_pt20;
            }
            if (thisJet->Pt() > 25) {
                ++nbjets_pt25;
            }
            if (thisJet->Pt() > 30) {
                ++nbjets_pt30;
            }
            if (thisJet->Pt() > 40) {
                ++nbjets_pt40;
            }
            if (thisJet->Pt() > 50) {
                ++nbjets_pt50;
            }
        }
    }

    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->amumu_fJetID_v2)) {
        if (!skip) {
            if (thisJet->Pt() > cuts->leadJetPt) {
                ajetsVetoID.push_back(*thisJet);
                fjetsID.push_back(*thisJet);
            }

            if (thisJet->Pt() > 20) {
                ++njets_pt20;
                ++nfjets_pt20;
            }
            if (thisJet->Pt() > 25) {
                ++njets_pt25;
                ++nfjets_pt25;
            }
            if (thisJet->Pt() > 30) {
                ++njets_pt30;
                ++nfjets_pt30;
            }
            if (thisJet->Pt() > 40) {
                ++njets_pt40;
                ++nfjets_pt40;
            }
            if (thisJet->Pt() > 50) {
                ++njets_pt50;
                ++nfjets_pt50;
            }
        }
    }
  }

  sort(ajetsVetoID.begin(), ajetsVetoID.end(), P4SortCondition);
  sort(cjetsVetoID.begin(), cjetsVetoID.end(), P4SortCondition);
  sort(bjetsID.begin(), bjetsID.end(), P4SortCondition);
  sort(fjetsID.begin(), fjetsID.end(), P4SortCondition);




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
  dimuon = muonOne+muonTwo;

  int jetOneInt   = -1;
  int jetTwoInt   = -1;
  int jetThreeInt = -1;
  if (ajetsVetoID.size() > 0) {
    jetOneInt = 0;
    jetOne = ajetsVetoID[jetOneInt];
  }
  if (ajetsVetoID.size() > 1) {
    jetTwoInt = 1;
    jetTwo = ajetsVetoID[jetTwoInt];
  }
  if (ajetsVetoID.size() > 2) {
    jetThreeInt = 2;
    jetThree = ajetsVetoID[jetThreeInt];
  }

  met.SetPxPyPzE(recoMET->Px(), recoMET->Py(), 0, 0);


  // Initialize all the variables to -999999
  njets = -999999;
  ncjets = -999999;
  nbjets = -999999;
  nfjets = -999999;
  //njets_pt20 = -999999;
  //njets_pt25 = -999999;
  //njets_pt30 = -999999;
  //njets_pt40 = -999999;
  //njets_pt50 = -999999;
  //ncjets_pt20 = -999999;
  //ncjets_pt25 = -999999;
  //ncjets_pt30 = -999999;
  //ncjets_pt40 = -999999;
  //ncjets_pt50 = -999999;
  //nbjets_pt20 = -999999;
  //nbjets_pt25 = -999999;
  //nbjets_pt30 = -999999;
  //nbjets_pt40 = -999999;
  //nbjets_pt50 = -999999;
  //nfjets_pt20 = -999999;
  //nfjets_pt25 = -999999;
  //nfjets_pt30 = -999999;
  //nfjets_pt40 = -999999;
  //nfjets_pt50 = -999999;
  nmuons = -999999;
  nelectrons = -999999;
  nphotons = -999999;
  muonOneCharge = -999999;
  muonOnePt = -999999;
  muonOneEta = -999999;
  muonOnePhi = -999999;
  muonOneEnergy = -999999;
  muonOneIsPF = -999999;
  muonOneIsGLB = -999999;
  muonOneNormalizedChi2 = -999999;
  muonOneNumberOfMatchedStations = -999999;
  muonOneNumberOfValidMuonHits = -999999;
  muonOneNumberOfValidPixelHits = -999999;
  muonOneTrackLayersWithMeasurement = -999999;
  muonOneDxy = -999999;
  muonOneDz = -999999;
  muonOneTrkIso = -999999;
  muonOneRelIso = -999999;
  muonTwoCharge = -999999;
  muonTwoPt = -999999;
  muonTwoEta = -999999;
  muonTwoPhi = -999999;
  muonTwoEnergy = -999999;
  muonTwoIsPF = -999999;
  muonTwoIsGLB = -999999;
  muonTwoNormalizedChi2 = -999999;
  muonTwoNumberOfMatchedStations = -999999;
  muonTwoNumberOfValidMuonHits = -999999;
  muonTwoNumberOfValidPixelHits = -999999;
  muonTwoTrackLayersWithMeasurement = -999999;
  muonTwoDxy = -999999;
  muonTwoDz = -999999;
  muonTwoTrkIso = -999999;
  muonTwoRelIso = -999999;
  dimuonPt = -999999;
  dimuonEta = -999999;
  dimuonPhi = -999999;
  dimuonEnergy = -999999;
  dimuonMass = -999999;
  dimuonDeltaR = -999999;
  jetOnePt = -999999;
  jetOneEta = -999999;
  jetOnePhi = -999999;
  jetOneEnergy = -999999;
  jetOneCSV = -999999;
  jetOneCSVv1 = -999999;
  jetOneCSVMVA = -999999;
  jetOnePUID = -999999;
  jetTwoPt = -999999;
  jetTwoEta = -999999;
  jetTwoPhi = -999999;
  jetTwoEnergy = -999999;
  jetTwoCSV = -999999;
  jetTwoCSVv1 = -999999;
  jetTwoCSVMVA = -999999;
  jetTwoPUID = -999999;
  metPt =-999999;
  metPhi =-999999;
  x = -999999; // for unbinned fit
  w = -999999; // event weight


  // Set values
  njets = ajetsVetoID.size();
  ncjets = cjetsVetoID.size();
  nbjets = bjetsID.size();
  nfjets = fjetsID.size();
  assert(njets == ncjets + nfjets);
  nmuons = muonsIDIso.size();
  nelectrons = electronsIDIso.size();
  nphotons = photonsIDIso.size();

  if (muonOneInt != -1) {
    muonOneCharge = muonsIDIso[muonOneInt].Charge();
    muonOnePt = muonOne.Pt();
    muonOneEta = muonOne.Eta();
    muonOnePhi = muonOne.Phi();
    muonOneEnergy = muonOne.Energy();
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
  }

  if (muonTwoInt != -1) {
    muonTwoCharge = muonsIDIso[muonTwoInt].Charge();
    muonTwoPt = muonTwo.Pt();
    muonTwoEta = muonTwo.Eta();
    muonTwoPhi = muonTwo.Phi();
    muonTwoEnergy = muonTwo.Energy();
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
  }

  if (muonOneInt != -1 && muonTwoInt != -1) {
    dimuonPt = dimuon.Pt();
    dimuonEta = dimuon.Eta();
    dimuonPhi = dimuon.Phi();
    dimuonEnergy = dimuon.Energy();
    dimuonMass = dimuon.M();
    dimuonDeltaR = muonOne.DeltaR(muonTwo);
  }

  if (jetOneInt != -1) {
    jetOnePt = jetOne.Pt();
    jetOneEta = jetOne.Eta();
    jetOnePhi = jetOne.Phi();
    jetOneEnergy = jetOne.Energy();
    jetOneCSV = ajetsVetoID[jetOneInt].BDiscriminatorMap("CSV");
    jetOneCSVv1 = ajetsVetoID[jetOneInt].BDiscriminatorMap("CSVv1");
    jetOneCSVMVA = ajetsVetoID[jetOneInt].BDiscriminatorMap("CSVMVA");
    jetOnePUID = ajetsVetoID[jetOneInt].IdMap("PUID_MVA");
  }

  if (jetTwoInt != -1) {
    jetTwoPt = jetTwo.Pt();
    jetTwoEta = jetTwo.Eta();
    jetTwoPhi = jetTwo.Phi();
    jetTwoEnergy = jetTwo.Energy();
    jetTwoCSV = ajetsVetoID[jetTwoInt].BDiscriminatorMap("CSV");
    jetTwoCSVv1 = ajetsVetoID[jetTwoInt].BDiscriminatorMap("CSVv1");
    jetTwoCSVMVA = ajetsVetoID[jetTwoInt].BDiscriminatorMap("CSVMVA");
    jetTwoPUID = ajetsVetoID[jetTwoInt].IdMap("PUID_MVA");
  }

  if (jetThreeInt != -1) {
    jetThreePt = jetThree.Pt();
    jetThreeEta = jetThree.Eta();
    jetThreePhi = jetThree.Phi();
    jetThreeEnergy = jetThree.Energy();
    jetThreeCSV = ajetsVetoID[jetThreeInt].BDiscriminatorMap("CSV");
    jetThreeCSVv1 = ajetsVetoID[jetThreeInt].BDiscriminatorMap("CSVv1");
    jetThreeCSVMVA = ajetsVetoID[jetThreeInt].BDiscriminatorMap("CSVMVA");
    jetThreePUID = ajetsVetoID[jetThreeInt].IdMap("PUID_MVA");
  }

  metPt = met.Pt();
  metPhi = met.Phi();

  x = dimuon.M();  // for unbinned fit
  w = eventWeight;  // event weight


  amumuTree->Fill();


  return kTRUE;
}

void amumuAnalyzer4::Terminate()
{
  cout<<"TOTAL NUMBER OF FILES: "<<fileCount<<" and they have this many events: "<<unskimmedEventsTotal<<endl;

  TH1I* totalEvents = new TH1I(("unskimmedEventsTotal_"+params->suffix).c_str(),("unskimmedEventsTotal_"+params->suffix).c_str(),1,0,1);
  totalEvents->SetBinContent(1,unskimmedEventsTotal);

  amumuFile->Write();
  amumuFile->Close();

  dumper->CloseDumps();
}
