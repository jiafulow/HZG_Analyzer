#define smzgAnalyzer_cxx
// The class definition in smzgAnalyzer.h has been generated automatically
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
// Root > T->Process("smzgAnalyzer.C")
// Root > T->Process("smzgAnalyzer.C","some options")
// Root > T->Process("smzgAnalyzer.C+")
//

#include "smzgAnalyzer.h"
#include <TH2.h>
#include <TStyle.h>


void smzgAnalyzer::Begin(TTree * tree)
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

  cuts->gPt = 25;
  cuts->leadMuPt = 23;
  cuts->trailMuPt = 4;

  cuts->InitEA(params->period);
  weighter.reset(new WeightUtils(*params, isRealData, runNumber));
  triggerSelector.reset(new TriggerSelector(params->selection, params->period, *triggerNames));
  rmcor2011.reset(new rochcor_2011(229+100*jobNum));
  rmcor2012.reset(new rochcor2012(229+100*jobNum));
  rEl.reset(new TRandom3(230+100*jobNum));
  rMuRun.reset(new TRandom3(231+100*jobNum));
  phoCorrector.reset(new zgamma::PhosphorCorrectionFunctor("../plugins/PHOSPHOR_NUMBERS_EXPFIT_ERRORS.txt", true));
  particleSelector.reset(new ParticleSelector(*params, *cuts, isRealData, runNumber, *rEl));

  // Random numbers! //
  rnGenerator.reset(new TRandom3);

  liteFile.reset(new TFile(("liteFile_"+params->dataname+"_"+params->selection+"_"+params->jobCount+".root").c_str(), "RECREATE"));
  liteFile->cd();
  zgTree.reset(new TTree(("zgTree_"+params->suffix).c_str(),"three body mass values"));

  zgTree->Branch("muonPos",&muonPos);
  zgTree->Branch("muonNeg",&muonNeg);
  zgTree->Branch("photon",&photon);
}


Bool_t smzgAnalyzer::Process(Long64_t entry)
{
  GetEntry(entry,1);
  particleSelector->SetRho(rhoFactor);

  // trigger

  for(int i = 0; i < 64; ++i) {
    unsigned long iHLT = 0x0; 
    iHLT = 0x01 << i;  
  } 

  bool triggerPass   = triggerSelector->SelectTrigger(triggerStatus, hltPrescale);
  if (!triggerPass) return kTRUE;

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

  vector<TCGenParticle> vetoPhotons;
  particleSelector->CleanUpGen(genHZG);
  //genHZG = {0,0,0,0,0,0};
  if(!isRealData) particleSelector->FindGenParticles(*genParticles, vetoPhotons, genHZG);

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
      if ( params->period.find("2011") != string::npos ){
        if (isRealData){
          if ( params->period.find("A") != string::npos ){
            rmcor2011->momcor_data(tmpMuCor,thisMuon->Charge(),0,0);
          }else{
            rmcor2011->momcor_data(tmpMuCor,thisMuon->Charge(),0,1);
          }
        }
        if (!isRealData){
          if (rMuRun->Rndm() < 0.458){
            rmcor2011->momcor_mc(tmpMuCor,thisMuon->Charge(),0,0);
          }else{
            rmcor2011->momcor_mc(tmpMuCor,thisMuon->Charge(),0,1);
          }
        }
      }else{
        float ptErrMu = 1.0;
        if (isRealData){
          if (params->abcd.find("D") != string::npos ){
            rmcor2012->momcor_data(tmpMuCor,thisMuon->Charge(),1,ptErrMu);
          }else{
            rmcor2012->momcor_data(tmpMuCor,thisMuon->Charge(),0,ptErrMu);
          }
        }

        if (!isRealData) rmcor2012->momcor_mc(tmpMuCor,thisMuon->Charge(),0,ptErrMu);
      }
      thisMuon->SetPtEtaPhiM(tmpMuCor.Pt(), tmpMuCor.Eta(), tmpMuCor.Phi(), tmpMuCor.M());
    }


    // tight muon id

    if (particleSelector->PassMuonID(*thisMuon, cuts->tightMuID)){
      muonsID.push_back(*thisMuon);
    }

    //tight ID and Iso

    if (params->doLooseMuIso){
      if (particleSelector->PassMuonID(*thisMuon, cuts->tightMuID) && particleSelector->PassMuonIso(*thisMuon, cuts->looseMuIso)){
        muonsIDIso.push_back(*thisMuon);
      }
    }else{
      if (particleSelector->PassMuonID(*thisMuon, cuts->tightMuID) && particleSelector->PassMuonIso(*thisMuon, cuts->tightMuIso)){
        muonsIDIso.push_back(*thisMuon);
      }
    }

  }

  sort(muonsID.begin(), muonsID.end(), P4SortCondition);
  sort(muonsIDIso.begin(), muonsIDIso.end(), P4SortCondition);


  //photons


  vector<TCPhoton> photonsID; 
  vector<TCPhoton> photonsIDIso; 


  for (Int_t i = 0; i < recoPhotons->GetSize(); ++i) {
    TCPhoton* thisPhoton = (TCPhoton*) recoPhotons->At(i);

    // Section for photon energy/momentum corrections.  NOTE: this will change the pt and thus ID/ISO of photon

    if(params->engCor){
      //old R9 correction
      if (params->doR9Cor) PhotonR9Corrector(*thisPhoton);

      float corrPhoPt = -1;
      int periodNum   = -1;
      if (params->period.find("2011") != string::npos) periodNum = 2011;
      if (params->period.find("2012") != string::npos) periodNum = 2012;
      if(!isRealData && thisPhoton->Pt()>10.){
        TCGenParticle goodGenPhoton;
        float testDr = 9999;
        int vetoPos = -1;
        for (UInt_t j = 0; j<vetoPhotons.size(); j++){
          if(vetoPhotons[j].Mother() && (fabs(vetoPhotons[j].Mother()->GetPDGId()) == 25 || fabs(vetoPhotons[j].Mother()->GetPDGId()) == 22) && vetoPhotons[j].GetStatus()==1){
            if(thisPhoton->DeltaR(vetoPhotons[j])<testDr){
              goodGenPhoton = vetoPhotons[j];
              testDr = thisPhoton->DeltaR(vetoPhotons[j]);
              vetoPos = j;
            }
          }
        }
        if (testDr < 0.2){
          corrPhoPt = phoCorrector->GetCorrEtMC(thisPhoton->R9(), periodNum, thisPhoton->Pt(), thisPhoton->Eta(), goodGenPhoton.E());
          thisPhoton->SetPtEtaPhiM(corrPhoPt,thisPhoton->Eta(),thisPhoton->Phi(),0.0);
        }
      }else if (isRealData && thisPhoton->Pt()>10.){
        corrPhoPt = phoCorrector->GetCorrEtData(thisPhoton->R9(), periodNum, thisPhoton->Pt(), thisPhoton->Eta());
        thisPhoton->SetPtEtaPhiM(corrPhoPt,thisPhoton->Eta(),thisPhoton->Phi(),0.0);
      }

    }


    ////// Currently Using Cut-Based Photon ID, 2012

    if (particleSelector->PassPhotonID(*thisPhoton, cuts->mediumPhID)) photonsID.push_back(*thisPhoton);
    if (particleSelector->PassPhotonID(*thisPhoton, cuts->mediumPhID) && particleSelector->PassPhotonIso(*thisPhoton, cuts->mediumPhIso, cuts->EAPho)){
      //standard selection photons
      photonsIDIso.push_back(*thisPhoton);
    }





  }
  sort(photonsID.begin(), photonsID.end(), P4SortCondition);
  sort(photonsIDIso.begin(), photonsIDIso.end(), P4SortCondition);


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

  bool goodZ = particleSelector->FindGoodZMuon(muonsIDIso,lepton1,lepton2,ZP4,lepton1int,lepton2int,0.0);
  if (!goodZ) return kTRUE;


  // good photon
  
  TCPhoton GP4;
  float GP4scEta = -9999;
  if (photonsIDIso.size() < 1) return kTRUE;
  bool goodPhoton = particleSelector->FindGoodPhoton(photonsIDIso, GP4, lepton1, lepton2, GP4scEta, vetoPhotons);
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

  
  zgTree->Fill();




  return kTRUE;
}

void smzgAnalyzer::Terminate()
{
  liteFile->Write();
  liteFile->Close();

}

void smzgAnalyzer::PhotonR9Corrector(TCPhoton& ph){
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
