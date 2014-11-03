#define tnpProducer_cxx
// The class definition in tnpProducer.h has been generated automatically
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
// Root > T->Process("tnpProducer.C")
// Root > T->Process("tnpProducer.C","some options")
// Root > T->Process("tnpProducer.C+")
//

#include "tnpProducer.h"
#include <TH2.h>
#include <TStyle.h>


void tnpProducer::Begin(TTree * tree)
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

  if (params->period.find("2012") != string::npos){
    cuts->gPt = 25;
    cuts->leadMuPt = 23;
    cuts->trailMuPt = 4;
  }else{
    cuts->gPt = 25;
    cuts->leadMuPt = 4;
    cuts->trailMuPt = 4;
  }


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

  tnpFile.reset(new TFile(("tnpFile_"+params->dataname+"_"+params->selection+"_"+params->jobCount+".root").c_str(), "RECREATE"));
  tnpFile->cd();

  unsigned int runNum, lumiSec, evtNum;   // event ID
  unsigned int npv, npu;                  // number of PV / PU
  unsigned int pass;                      // whether probe passes requirements
  float        scale1fb;                  // event weight per 1/fb
  float        mass;                      // tag-probe mass
  int          qtag, qprobe;              // tag, probe charge
  TLorentzVector *tag=0, *probe=0;        // tag, probe 4-vector


  tnpTree.reset(new TTree(("tnpTree_"+params->suffix).c_str(),"three body mass values"));

  tnpTree->Branch("runNum",   &runNum,   "runNum/i");
  tnpTree->Branch("lumiSec",  &lumiSec,  "lumiSec/i");
  tnpTree->Branch("evtNum",   &evtNum,   "evtNum/i");
  tnpTree->Branch("npv",      &npv,      "npv/i");
  tnpTree->Branch("npu",      &npu,      "npu/i");
  tnpTree->Branch("pass",     &pass,     "pass/i");
  tnpTree->Branch("scale1fb", &scale1fb, "scale1fb/F");
  tnpTree->Branch("mass",     &mass,     "mass/F");

  // tag variables
  tnpTree->Branch("qtag", &qtag, "qtag/I");
  tnpTree->Branch("tag", "TLorentzVector", &tag);

  // probe variables
  tnpTree->Branch("qprobe", &qprobe, "qprobe/I");
  tnpTree->Branch("probe", "TLorentzVector", &probe);


}


Bool_t tnpProducer::Process(Long64_t entry)
{
  GetEntry(entry,1);
  particleSelector->SetRho(rhoFactor);

  //////////////////////////////////////
  // get gen particles, might need em //
  //////////////////////////////////////

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
  


  /////////////////////////
  // require good vertex //
  /////////////////////////

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

  /////////////////////
  // ready dileptons //
  /////////////////////
  
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

    if (params->EVENTNUMBER == eventNumber) cout<<"regE: "<<thisElec->IdMap("EnergyRegression")<<" regE-p: "<<thisElec->RegressionMomCombP4().E()<<endl;
    

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

    if (eventNumber == params->EVENTNUMBER){
      cout<< "electron before cor: "<<TCPhysObject(*thisElec)<<endl;
    }
    if(params->engCor && !params->doSync && params->PU == "S10") UniversalEnergyCorrector(*thisElec);
    //else if(params->engCor && !params->doSync && !isRealData){
    //  float ptCor = ElectronMCScale(thisElec->SCEta(), thisElec->Pt());
    //  thisElec->SetPtEtaPhiM(thisElec->Pt()*ptCor, thisElec->Eta(), thisElec->Phi(), thisElec->M());
    //}

    if (eventNumber == params->EVENTNUMBER){
      cout<< "electron after cor: "<<TCPhysObject(*thisElec)<<endl;
    }


    dumper->ElectronDump(*thisElec, *recoMuons, 1);

    if(passID) electronsID.push_back(*thisElec);
    if(passID&&passIso) electronsIDIso.push_back(*thisElec);

  }

  sort(electronsID.begin(), electronsID.end(), P4SortCondition);
  sort(electronsIDIso.begin(), electronsIDIso.end(), P4SortCondition);
  
  ///////////
  // muons //
  ///////////

  vector<TCMuon> muonsID;
  vector<TCMuon> muonsIDIso;

  for (int i = 0; i < recoMuons->GetSize(); ++ i)
  {
    TCMuon* thisMuon = (TCMuon*) recoMuons->At(i);    

    if (eventNumber == params->EVENTNUMBER){
      cout<< "muon uncor: " << TCPhysObject(*thisMuon) << endl;
    }

    bool passID = false;
    bool passIso = false;

    if (particleSelector->PassMuonID(*thisMuon, cuts->tightMuID)) passID = true;

    if (params->doLooseMuIso){
      if (particleSelector->PassMuonIso(*thisMuon, cuts->looseMuIso)) passIso = true;
    }else{
      if (particleSelector->PassMuonIso(*thisMuon, cuts->tightMuIso)) passIso = true;
    }
    
    if (params->engCor && !params->doSync) UniversalEnergyCorrector(*thisMuon);

    if (passID) muonsID.push_back(*thisMuon);
    if (passID && passIso) muonsIDIso.push_back(*thisMuon);

    if (eventNumber == params->EVENTNUMBER){
      cout<< "muon cor: " << TCPhysObject(*thisMuon) << endl;
    }

    dumper->MuonDump(*thisMuon, 1);

  }

  sort(muonsID.begin(), muonsID.end(), P4SortCondition);
  sort(muonsIDIso.begin(), muonsIDIso.end(), P4SortCondition);
  sort(extraLeptons.begin(), extraLeptons.begin(), P4SortCondition);

  /////////////
  // photons //
  /////////////

  vector<TCPhoton> photonsID; 
  vector<TCPhoton> photonsIDIso; 
  vector<TCPhoton> photonsLIDMIso; 
  vector<TCPhoton> photonsMIDLIso; 
  vector<TCPhoton> photonsLIDLIso; 
  vector<TCPhoton> photonsNoIDIso; 


  if (params->selection == "mumuGamma" || params->selection == "eeGamma") {
    for (Int_t i = 0; i < recoPhotons->GetSize(); ++i) {
      //cout<<endl;
      //cout<<"new photon!!!!!!!"<<endl;
      vector<float> TrkIsoVec;
      TCPhoton* thisPhoton = (TCPhoton*) recoPhotons->At(i);

      if (params->spikeVeto && (params->period == "2012A_Jul13" || params->period == "2012A_Aug06rec" || params->period == "2012B_Jul13")){
        bool veto = false;
        veto = SpikeVeto(*thisPhoton);
        if(veto) continue;
      }

      ////// R9 Correction ///////

      if (params->doR9Cor) PhotonR9Corrector(*thisPhoton);

      ////// Currently Using Cut-Based Photon ID, 2012

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
        if (params->selection == "mumuGamma"){
          if (muonsIDIso.size() > 1){
            goodLepPre = GoodLeptonsCat( muonsIDIso[0], muonsIDIso[1]);
          }
        }else{
          if (electronsIDIso.size() > 1){
            goodLepPre = GoodLeptonsCat( electronsIDIso[0], electronsIDIso[1]);
          }
        }
        if (particleSelector->PassPhotonMVA(*thisPhoton, cuts->catPhMVAID, goodLepPre)) passIso = true;
      }

      // energy correction after ID and ISO
  
      if (params->engCor && !params->doSync) UniversalEnergyCorrector(*thisPhoton, genPhotons);

      if (passID) photonsID.push_back(*thisPhoton);
      if (passID && passIso) photonsIDIso.push_back(*thisPhoton);

      dumper->PhotonDump(*thisPhoton, 1); 

    }
    //cout<<"debug0"<<endl;
    //return kTRUE;
    sort(photonsID.begin(), photonsID.end(), P4SortCondition);
    sort(photonsIDIso.begin(), photonsIDIso.end(), P4SortCondition);
  }
  
  // 2 good muons
  

  if (muonsID.size() < 2) return kTRUE;
  if (muonsIDIso.size() < 1) return kTRUE;
  bool firstMu = false;
  bool bothMus = false;
  for (UInt_t i = 0; i<muonsID.size(); i++){
    if (!firstMu && (muonsID[i].Pt() >cuts->trailMuPt)){
      firstMu = true;
    }else if (firstMu && (muonsID[i].Pt() >cuts->trailMuPt)){
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
  TLorentzVector HP4; 

  bool goodZ = particleSelector->FindGoodZMuon(muonsIDIso,muonsID,lepton1,lepton2,ZP4,lepton1int,lepton2int,0.0);
  if (!goodZ) return kTRUE;


  // good photon
  
  TCPhoton GP4;
  float GP4scEta = -9999;
  if (photonsIDIso.size() < 1) return kTRUE;
  bool goodPhoton = particleSelector->FindGoodPhoton(photonsIDIso, GP4, lepton1, lepton2, GP4scEta);
  if(!goodPhoton) return kTRUE;

  HP4 = ZP4+GP4;
  if (HP4.M() < 50) return kTRUE;


  // put them in the branch

  if (lepton1.Charge() == 1){
    muonPos = lepton1;
    muonNeg = lepton2;
  }else{
    muonPos = lepton2;
    muonNeg = lepton1;
  }
  photon = GP4;


  
  tnpTree->Fill();

  // trigger

  for(int i = 0; i < 64; ++i) {
    unsigned long iHLT = 0x0; 
    iHLT = 0x01 << i;  
  } 

  bool triggerPass   = triggerSelector->SelectTrigger(triggerStatus, hltPrescale);
  if (params->period.find("2012") != string::npos) if (!triggerPass) return kTRUE;




  return kTRUE;
}

void tnpProducer::Terminate()
{
  tnpFile->Write();
  tnpFile->Close();

}

void tnpProducer::PhotonR9Corrector(TCPhoton& ph){
  //old R9 correction
  float R9Cor;
  if (params->suffix.find("S10") != string::npos){
    R9Cor = ph.R9();
    if (params->suffix != "DATA"){
      if (params->period == "2011"){
        if (fabs(ph.SCEta()) < 1.479){
          R9Cor = ph.R9()*1.0048;
        }else{
          R9Cor = ph.R9()*1.00492;
        }
      } else if (params->period == "2012"){
        if (fabs(ph.SCEta()) < 1.479){
          R9Cor = ph.R9()*1.0045 + 0.0010;
        }else{
          R9Cor = ph.R9()*1.0086 - 0.0007;
        }
      }
    }
    //new R9 correction
  }else{
    R9Cor = ph.R9();
    if (params->suffix != "DATA"){
      if (params->period == "2011"){
        if (fabs(ph.SCEta()) < 1.479){
          R9Cor = ph.R9()*1.00153 + 0.000854;
        }else{
          R9Cor = ph.R9()*1.00050+ 0.001231;
        }
      } else if (params->period == "2012"){
        if (fabs(ph.SCEta()) < 1.479){
          R9Cor = ph.R9()*1.00139 + 0.000740;
        }else{
          R9Cor = ph.R9()*1.00016- 0.000399;
        }
      }
    }
  }
  ph.SetR9(R9Cor);
}

void tnpProducer::UniversalEnergyCorrector(TCPhoton& ph, vector<TCGenParticle>& _genPhotons){
  // Section for photon energy/momentum corrections.  NOTE: this will change the pt and thus ID/ISO of photon


  float corrPhoPt = -1;
  float corrPhoE = -1;
  int periodNum   = -1;
  if (params->period.find("2011") != string::npos) periodNum = 2011;
  if (params->period.find("2012") != string::npos) periodNum = 2012;

  //shervin photon cors
  if (params->PU == "S10" && periodNum == 2012){ 
    if (ph.Pt() >20.){
      corrPhoE = correctedPhotonEnergy(ph.SCEnergy(), ph.SCEta(), ph.R9(), runNumber, 0, "Moriond2014", !isRealData, rnGenerator.get());
      //cout<<"before:" <<ph.E()<<" after: "<<corrPhoE<<endl;
      float escale = corrPhoE/ph.E();
      ph.SetXYZM(escale*ph.Px(), escale*ph.Py(), escale*ph.Pz(), 0.0);
    }
  }else{

  //phosphor
    if(!isRealData && ph.Pt()>10.){
      TCGenParticle goodGenPhoton;
      float testDr = 9999;
      //cout<<"veto photon size: "<<_genPhotons.size()<<endl;
      for (UInt_t j = 0; j<_genPhotons.size(); j++){
        //if (_genPhotons[j].Mother() && fabs(_genPhotons[j].Mother()->GetPDGId()) == 22) cout<<"mother: "<<_genPhotons[j].Mother()->GetPDGId()<<endl;
        if(_genPhotons[j].Mother() && (fabs(_genPhotons[j].Mother()->GetPDGId()) == 25 || fabs(_genPhotons[j].Mother()->GetPDGId()) == 22) && _genPhotons[j].GetStatus()==1){
          if(ph.DeltaR(_genPhotons[j])<testDr){
            goodGenPhoton = _genPhotons[j];
            testDr = ph.DeltaR(_genPhotons[j]);
            //cout<<"testdr: "<<testDr<<endl;
          }
        }
      }
      if (testDr < 0.2){
        corrPhoPt = phoCorrector->GetCorrEtMC(ph.R9(), periodNum, ph.Pt(), ph.Eta(), goodGenPhoton.E());
        ph.SetPtEtaPhiM(corrPhoPt,ph.Eta(),ph.Phi(),0.0);
      }

    }else if (isRealData && ph.Pt()>10.){
      corrPhoPt = phoCorrector->GetCorrEtData(ph.R9(), periodNum, ph.Pt(), ph.Eta());
      ph.SetPtEtaPhiM(corrPhoPt,ph.Eta(),ph.Phi(),0.0);
    }
  }
}

void tnpProducer::UniversalEnergyCorrector(TCMuon& mu){

  // Section for muon energy/momentum corrections.  NOTE: this will change the pt and thus ID/ISO of muon

  if (eventNumber == params->EVENTNUMBER){
    rmcor2012.reset(new rochcor2012(666));
    cout<< "resetting rochcor seed to 666"<<endl;
  }
  TLorentzVector tmpMuCor(mu);
  if ( params->period.find("2011") != string::npos ){
    if (isRealData){
      if ( params->period.find("A") != string::npos ){
        rmcor2011->momcor_data(tmpMuCor,mu.Charge(),0,0);
      }else{
        rmcor2011->momcor_data(tmpMuCor,mu.Charge(),0,1);
      }
    }
    if (!isRealData){
      if (rMuRun->Rndm() < 0.458){
        rmcor2011->momcor_mc(tmpMuCor,mu.Charge(),0,0);
      }else{
        rmcor2011->momcor_mc(tmpMuCor,mu.Charge(),0,1);
      }
    }
  }else{
    float ptErrMu = 1.0;
    if (isRealData) rmcor2012->momcor_data(tmpMuCor,mu.Charge(),0,ptErrMu);
    if (!isRealData) rmcor2012->momcor_mc(tmpMuCor,mu.Charge(),0,ptErrMu);
  }
  mu.SetPtEtaPhiM(tmpMuCor.Pt(), tmpMuCor.Eta(), tmpMuCor.Phi(), tmpMuCor.M());
}

void tnpProducer::UniversalEnergyCorrector(TCElectron& el){
  if (params->period.find("2012") != string::npos){
    if (el.RegressionMomCombP4().E() != 0){
      el.SetPtEtaPhiM(el.RegressionMomCombP4().Pt(),el.RegressionMomCombP4().Eta(),el.RegressionMomCombP4().Phi(),0.000511);
    }
    if (eventNumber == params->EVENTNUMBER){
      cout<< "electron after cor: "<<TCPhysObject(el)<<endl;
    }
  }
}
