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
  cuts->InitEA(params->period);

  params->DYGammaVeto      = false;

  doSyst = true;

  if(!doSyst){
    cuts->leadMuPt = 30;
    cuts->leadElePt = 30;
  }else{
    cuts->leadMuPt = 20;
    cuts->leadElePt = 20;
  }
  cuts->trailMuPt = 10;
  cuts->trailElePt = 10;
  cuts->zMassHigh = 200.0;
  cuts->zMassLow = 40.0;


  //tnpType = "ID";
  //tnpType = "Iso";
  //tnpType = "leadingTrigger";
  tnpType = "trailingTrigger";

  if(tnpType.find("Trigger") == string::npos){
    tagHLT = "HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v";
  }else{
    if (!doSyst){
      tagHLT = "HLT_Ele27_WP80_v";
    }else{
      tagHLT = "HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v";
    }
  }

  weighter.reset(new WeightUtils(*params, isRealData, runNumber));
  triggerSelector.reset(new TriggerSelector(params->selection, params->period, *triggerNames));
  if(tnpType.find("Trigger")!=string::npos){
    triggerSelector->AddTriggers(vector<string>(1, tagHLT));
  }else{
    triggerSelector->TriggerDefaults();
  }
  triggerSelector->SetSelectedBits();
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

  assert(tnpType == "leadingTrigger" || tnpType == "trailingTrigger" || tnpType == "ID" || tnpType == "Iso");

}

float totalPass, totalFail;
Bool_t tnpProducer::Process(Long64_t entry)
{
  GetEntry(entry,1);
  particleSelector->SetRho(rhoFactor);

  //////////////////////////////////////
  // get gen particles, might need em //
  //////////////////////////////////////

  vector<TCGenParticle> genPhotons;
  vector<TCGenParticle> genMuons;
  vector<TCGenParticle> genElectrons;
  vector<TCGenParticle> genZs;
  bool vetoDY = false;
  particleSelector->CleanUpGen(genHZG);
  if(!isRealData) particleSelector->FindGenParticles(*genParticles, *recoPhotons, genPhotons, genMuons, genElectrons, genZs, genHZG, vetoDY);

  /////////////////////
  // require trigger //
  /////////////////////

  for(int i = 0; i < 64; ++i) {
    unsigned long iHLT = 0x0; 
    iHLT = 0x01 << i;  
  } 

  bool triggerPass   = triggerSelector->SelectTrigger(triggerStatus, hltPrescale);
  if (params->period.find("2012") != string::npos) if (!triggerPass) return kTRUE;

  /* for quick gen trigger test
  bool good1, good2;
  good1 = good2 = false;
  for(vector<TCGenParticle>::iterator it = genElectrons.begin(); it != genElectrons.end(); it++){
    if(!good1 && it->MotherId() == 23 && it->Pt()>30 && fabs(it->Eta())<2.5){
      good1 = true;
      continue;
    }
    if(good1 && !good2&& it->MotherId() == 23 && it->Pt()>30 && fabs(it->Eta())<2.5){
      good2 = true;
      break;
    }
  }
  if ((good2 && good1) && triggerPass) totalPass+=1;
  else if ((good2 && good1) && !triggerPass) totalFail +=1;
  */


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

  TCElectron vTag, vProbe;
  //cout<<"new event"<<endl;
  for (int i = 0; i <  recoElectrons->GetSize(); ++i) {
    TCElectron* thisElecTag = (TCElectron*) recoElectrons->At(i);

    bool passIDTag = false;
    bool passIsoTag = false;
    bool passTrigTag = false;

    thisElecTag->SetPtEtaPhiM(thisElecTag->Pt(),thisElecTag->Eta(),thisElecTag->Phi(),0.000511);

    // tag finder
    double IDCut = -0.5;
    if(doSyst) IDCut = -0.4;
    if (thisElecTag->Pt() >= cuts->leadElePt && particleSelector->PassElectronID(*thisElecTag, cuts->mvaPreElID, *recoMuons,true) && thisElecTag->MvaID_Old() > IDCut){
      passIDTag = true; 
    }else{
      continue;
    }
    if(!doSyst){
      if (particleSelector->PassElectronIso(*thisElecTag, cuts->looseElIso, cuts->EAEle)){
        passIsoTag = true;
      }else{
        continue;
      }
    }else{
      if (particleSelector->PassElectronIso(*thisElecTag, cuts->mediumElIso, cuts->EAEle)){
        passIsoTag = true;
      }else{
        continue;
      }
    }

    map<string, vector<string> > trigMapTag = thisElecTag->GetTriggers();
    //for (map<string, vector<string> >::iterator it = trigMapTag.begin(); it != trigMapTag.end(); it++) cout<<it->first<<endl;
    if(tnpType.find("Trigger") != string::npos){
      if (trigMapTag.find(tagHLT) != trigMapTag.end()){
        vector<string> trigLegsTag = trigMapTag[tagHLT];
        //for (vector<string>::iterator it = trigLegsTag.begin(); it != trigLegsTag.end(); it++) cout<<*it<<endl;
        if(!doSyst){
          if(find(trigLegsTag.begin(), trigLegsTag.end(), "hltEle27WP80TrackIsoFilter")!=trigLegsTag.end()){
            passTrigTag = true;
          }else{
            continue;
          }
        }else{
          if(find(trigLegsTag.begin(), trigLegsTag.end(), "hltEle17CaloIdTCaloIsoVLTrkIdVLTrkIsoVLTrackIsoFilter")!=trigLegsTag.end()){
            passTrigTag = true;
          }else{
            continue;
          }
        }
      }
    }else{
      if (trigMapTag.find("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v") != trigMapTag.end()){
        vector<string> trigLegsTag = trigMapTag["HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v"];
        if(find(trigLegsTag.begin(), trigLegsTag.end(), "hltEle17TightIdLooseIsoEle8TightIdLooseIsoTrackIsoDoubleFilter")!=trigLegsTag.end()){
          passTrigTag = true;
        }else{
          continue;
        }
      }
    }

    if (!isRealData){
      bool match = false;
      for(vector<TCGenParticle>::iterator it = genElectrons.begin(); it != genElectrons.end(); it++){
        if (it->DeltaR(*thisElecTag) < 0.5){
          match = true;
          break;
        }
      }
      if (!match) continue;
    }
    //cout<<"  new tag found"<<endl;

    if(passIDTag && passIsoTag && passTrigTag){
      vTag = *thisElecTag;

      // probe finder

      for (int j = 0; j <  recoElectrons->GetSize(); ++j) {
        if (j==i) continue;
        TCElectron* thisElecProbe = (TCElectron*) recoElectrons->At(j);

        bool passProbe = false;
        bool passIDProbe = false;
        bool passIsoProbe = false;
        bool passTrigLeadProbe = false;
        bool passTrigTrailProbe = false;

        thisElecProbe->SetPtEtaPhiM(thisElecProbe->Pt(),thisElecProbe->Eta(),thisElecProbe->Phi(),0.000511);

        /// low pt
        if (thisElecProbe->Pt() < 20 && thisElecProbe->Pt() > cuts->trailElePt && particleSelector->PassElectronID(*thisElecProbe, cuts->mvaPreElID, *recoMuons, true) && thisElecProbe->MvaID_Old() > -0.9){
          passIDProbe = true; 
        /// high pt
        }else if (thisElecProbe->Pt() >= 20 && particleSelector->PassElectronID(*thisElecProbe, cuts->mvaPreElID, *recoMuons,true) && thisElecProbe->MvaID_Old() > -0.5){
          passIDProbe = true; 
        }
        if (particleSelector->PassElectronIso(*thisElecProbe, cuts->looseElIso, cuts->EAEle)) passIsoProbe = true;

        map<string, vector<string> > trigMapProbe = thisElecProbe->GetTriggers();
        /*
        cout<<"new probe electron"<<endl;
        for (map<string, vector<string> >::iterator it = trigMapTag.begin(); it != trigMapTag.end(); it++){
          cout<<" "<<it->first<<endl;
          if (it->second.size() == 0){                            
            cout<<"hlt has no legs, how is this possible?"<<endl; 
            cin.ignore();                                         
          }                                                       
          for (vector<string>::iterator itt = it->second.begin(); itt != it->second.end(); itt++){
            cout<<"   "<<*itt<<endl;
          }
        }
        */

        if (trigMapProbe.find("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v") != trigMapProbe.end()){
          vector<string> trigLegsProbe = trigMapProbe["HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v"];
          if(find(trigLegsProbe.begin(), trigLegsProbe.end(), "hltEle17TightIdLooseIsoEle8TightIdLooseIsoTrackIsoFilter")!=trigLegsProbe.end() ){
            passTrigLeadProbe = true;
          }
          if(find(trigLegsProbe.begin(), trigLegsProbe.end(), "hltEle17TightIdLooseIsoEle8TightIdLooseIsoTrackIsoDoubleFilter")!=trigLegsProbe.end()){
            passTrigTrailProbe = true;
          }
        }

        if (!isRealData){
          bool match = false;
          for(vector<TCGenParticle>::iterator it = genElectrons.begin(); it != genElectrons.end(); it++){
            if (it->DeltaR(*thisElecProbe) < 0.5){
              match = true;
              break;
            }
          }
          if (!match) continue;
        }

        vProbe = *thisElecProbe;

        TLorentzVector ZP4 = vProbe+vTag;
        if ((ZP4.M() < cuts->zMassLow || ZP4.M() > cuts->zMassHigh)) continue; 

        
        if (tnpType == "ID"){
          passProbe = passIDProbe;
          if (passProbe){
            //cout<<"passed: "<<endl;
            totalPass+=1;
          }else{
            //cout<<"failed: "<<endl;
            totalFail+=1;
          }
        }else if(tnpType == "Iso"){
          if(!passIDProbe) continue;
          passProbe = passIsoProbe;
          if (passProbe){
            //cout<<"passed: "<<endl;
            totalPass+=1;
          }else{
            //cout<<"failed: "<<endl;
            totalFail+=1;
          }
        }else if(tnpType == "trailingTrigger"){ 
          if(!(passIDProbe && passIsoProbe)) continue;
          //cout<<"    new probe found"<<endl;
          passProbe = passTrigTrailProbe;
          if (passProbe){
            //cout<<"passed: "<<endl;
            totalPass+=1;
          }else{
            //cout<<"failed: "<<endl;
            totalFail+=1;
          }
        }else if(tnpType == "leadingTrigger"){ 
          if(!(passIDProbe && passIsoProbe)) continue;
          //cout<<"    new probe found"<<endl;
          passProbe = passTrigLeadProbe;
          if (passProbe){
            //cout<<"passed: "<<endl;
            totalPass+=1;
          }else{
            //cout<<"failed: "<<endl;
            totalFail+=1;
          }
          
          /*
          cout<<"tag:"<<endl;
          for (map<string, vector<string> >::iterator it = trigMapTag.begin(); it != trigMapTag.end(); it++){
            cout<<" "<<it->first<<endl;
            for (vector<string>::iterator itt = it->second.begin(); itt != it->second.end(); itt++){
              cout<<"  "<<*itt<<endl;
            }
          }
          cout<<"probe:"<<endl;
          for (map<string, vector<string> >::iterator it = trigMapProbe.begin(); it != trigMapProbe.end(); it++){
            cout<<" "<<it->first<<endl;
            for (vector<string>::iterator itt = it->second.begin(); itt != it->second.end(); itt++){
              cout<<"  "<<*itt<<endl;
            }
          }
        */
        }

        //////////////////////
        // Fill output tree //
        //////////////////////

        runNum   = runNumber;
        lumiSec  = lumiSection;
        evtNum   = eventNumber;
        npv      = goodVertices.size();
        npu      = nPUVerticesTrue;
        pass     = passProbe ? 1 : 0;
        //scale1fb = eventWeight;
        if(!isRealData){
          scale1fb = 1966.7*1000/42705454;
          scale1fb *= weighter->PUWeight(nPUVertices);
        }else{
          scale1fb = 1.0;
        }
        mass     = ZP4.M();	     
        qtag     = vTag.Charge();
        tag      = &vTag;
        qprobe   = vTag.Charge();
        probe    = &vProbe;

        tnpTree->Fill();
      }

    }
  }

  return kTRUE;
}

void tnpProducer::Terminate()
{
  tnpFile->Write();
  tnpFile->Close();
  cout<<"eff: "<<totalPass/(totalPass+totalFail)<<endl;
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
