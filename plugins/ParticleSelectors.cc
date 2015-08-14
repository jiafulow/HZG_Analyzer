#include "../interface/ParticleSelectors.h"

ParticleSelector::ParticleSelector(const Parameters& parameters, const Cuts& cuts, bool isRealData, int runNumber):
  _parameters(parameters),
  _cuts(cuts),
  _isRealData(isRealData),
  _runNumber(runNumber),
  _pv(),
  _rhoFactor(-1),
  _evtnum(-1)
{
}

void ParticleSelector::SetPv(const TVector3& pv){
  _pv = pv;
}

void ParticleSelector::SetAllPvs(const vector<TVector3>& pvs){
  _allPvs = pvs;
}

void ParticleSelector::SetRho(float rhoFactor){
  _rhoFactor = rhoFactor;
}

void ParticleSelector::SetEventNumber(int evtnum){
  _evtnum = evtnum;
}

bool ParticleSelector::FindGoodZElectron(const vector<TCElectron>& electronList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& ZP4,float& eta1, float& eta2, int& int1, int& int2){
  TLorentzVector tmpZ;
  bool goodZ = false;
  float ZmassDiff=99999;
  for(unsigned int i =0; i<electronList.size(); i++){
    if (electronList[i].Pt() > _cuts.leadElePt){
      for(unsigned int j =1; j<electronList.size(); j++){
        if (electronList[j].Pt() > _cuts.trailElePt && electronList[j].Charge() != electronList[i].Charge()){
          goodZ = true;
          tmpZ = (electronList[i]+electronList[j]);
          if(fabs(91.1876-tmpZ.M()) < ZmassDiff){
            ZP4 = (electronList[i]+electronList[j]);
            lepton1 = electronList[i];
            int1 = i;
            lepton2 = electronList[j];
            int2 = j;
            eta1 = electronList[i].SCEta();
            eta2 = electronList[j].SCEta();
            ZmassDiff = fabs(91.1876-tmpZ.M());
          }
        }
      }
    }
  }
  return goodZ;
}
          
          
bool ParticleSelector::FindGoodZMuon(const vector<TCMuon>& muonList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& ZP4, int& int1, int& int2){
  TLorentzVector tmpZ;
  bool goodZ = false;
  float ZmassDiff=99999;
  for(unsigned int i =0; i<muonList.size(); i++){
    if (muonList[i].Pt() > _cuts.leadMuPt){
      for(unsigned int j =1; j<muonList.size(); j++){
        if ((muonList[j].Pt() > _cuts.trailMuPt) && (muonList[j].Charge() != muonList[i].Charge())){
          goodZ = true;
          tmpZ = (muonList[i]+muonList[j]);
          if(fabs(91.1876-tmpZ.M()) < ZmassDiff){
            ZP4 = (muonList[i]+muonList[j]);
            lepton1 = muonList[i];
            lepton2 = muonList[j];
            int1 = i;
            int2 = j;
            ZmassDiff = fabs(91.1876-tmpZ.M());
          }
        }
      }
    }
  }
  return goodZ;
}

bool ParticleSelector::FindGoodZMuon(const vector<TCMuon>& muonList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& ZP4, int& int1, int& int2, float diLepMass){
  TLorentzVector tmpZ;
  bool goodZ = false;
  float ZmassDiff=99999;
  for(unsigned int i =0; i<muonList.size(); i++){
    if (muonList[i].Pt() > _cuts.leadMuPt){
      for(unsigned int j =1; j<muonList.size(); j++){
        if ((muonList[j].Pt() > _cuts.trailMuPt) && (muonList[j].Charge() != muonList[i].Charge())){
          tmpZ = (muonList[i]+muonList[j]);
          if(fabs(diLepMass-tmpZ.M()) < ZmassDiff){
            goodZ = true;
            ZP4 = (muonList[i]+muonList[j]);
            lepton1 = muonList[i];
            lepton2 = muonList[j];
            int1 = i;
            int2 = j;
            //ZmassDiff = fabs(diLepMass-tmpZ.M());
          }
          if (goodZ)  break;
        }
      }
      if (goodZ)  break;
    }
  }
  return goodZ;
}

bool ParticleSelector::FindGoodZMuon(const vector<TCMuon>& muonList1, const vector<TCMuon>& muonList2, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& ZP4, int& int1, int& int2, float diLepMass){
  // for finding 1 iso and 1 non iso muon
  TLorentzVector tmpZ;
  bool goodZ = false;
  float ZmassDiff=99999;
  for(unsigned int i =0; i<muonList1.size(); i++){
    if (muonList1[i].Pt() > _cuts.leadMuPt){
      for(unsigned int j =0; j<muonList2.size(); j++){
        if (muonList2[j].Pt() > _cuts.trailMuPt && muonList2[j].Charge() != muonList1[i].Charge()){
          tmpZ = (muonList1[i]+muonList2[j]);
          if(fabs(diLepMass-tmpZ.M()) < ZmassDiff){
            goodZ = true;
            ZP4 = (muonList1[i]+muonList2[j]);
            lepton1 = muonList1[i];
            lepton2 = muonList2[j];
            int1 = i;
            int2 = j;
            //ZmassDiff = fabs(diLepMass-tmpZ.M());
          }
          if (goodZ)  break;
        }
      }
      if (goodZ)  break;
    }
  }
  return goodZ;
}

bool ParticleSelector::FindGoodPhoton(const vector<TCPhoton>& photonList, TCPhoton& gamma, const TCPhysObject& lepton1, const TCPhysObject& lepton2, float& scEta, bool doScaledPt){
  bool goodPhoton = false;
  for (UInt_t i = 0; i<photonList.size(); i++){

    gamma = photonList[i];    // define GP4
    scEta = photonList[i].SCEta();

    if (_parameters.EVENTNUMBER == _evtnum){
      cout<<"FindGoodPhoton exe"<<endl;
      cout<<"PHOTON: "<<TCPhysObject(gamma)<<endl;
      cout<<"pass dr1: :"<<(gamma.DeltaR(lepton1)<_cuts.dR)<<endl;
      cout<<"pass dr2: :"<<(gamma.DeltaR(lepton2)<_cuts.dR)<<endl;
      cout<<"pass scaled pt: :"<<(gamma.Pt()/(gamma+lepton1+lepton2).M() > _cuts.gPtOverMass)<<endl;
      cout<<"pass straight pt: :"<<(gamma.Pt() > _cuts.gPt)<<endl;
    }

    if ((gamma.DeltaR(lepton1)<_cuts.dR || gamma.DeltaR(lepton2)<_cuts.dR)) continue;
    if (doScaledPt){
      if (gamma.Pt()/(gamma+lepton1+lepton2).M() < _cuts.gPtOverMass) continue;
    }
    if (gamma.Pt() > _cuts.gPt){
      goodPhoton = true;
      break;
    }
  }
  return goodPhoton;
}

bool ParticleSelector::FindGoodDiJets(const vector<TCJet>& jetList, const TCPhysObject& lepton1, const TCPhysObject& lepton2, const TLorentzVector& gamma, TCJet& jet1, TCJet& jet2){
  bool goodDiJets = false;
  if (jetList[0].Pt() < _cuts.leadJetPt) return goodDiJets;
  for (unsigned int i = 0; i < jetList.size(); i++){
    if (jetList[i].Pt() < _cuts.leadJetPt) return goodDiJets;
    if (fabs(jetList[i].Eta()) > 4.7) continue;
    jet1 = jetList[i];
    for (unsigned int j = i; j < jetList.size(); j++){
      if (jetList[j].Pt() < _cuts.leadJetPt) break; 
      if (fabs(jetList[j].Eta()) > 4.7) continue;
      jet2 = jetList[j];
      if (fabs(jet1.Eta() - jet2.Eta()) < _cuts.dEtaJet) continue;
      if (Zeppenfeld((lepton1+lepton2+gamma),jet1,jet2) > _cuts.zepp) continue;
      if ((jet1+jet2).M() < _cuts.mjj) continue;
      if ((jet1+jet2).DeltaPhi(lepton1+lepton2+gamma) < _cuts.dPhiJet) continue;
      goodDiJets = true;
      return goodDiJets;
    }
  }
  return goodDiJets;
}

void  ParticleSelector::FindGenParticles(const TClonesArray& genParticles, const TClonesArray& _recoPhotons, vector<TCGenParticle>& _genPhotons, vector<TCGenParticle>& _genMuons, vector<TCGenParticle>& _genElectrons, vector<TCGenParticle>& _genZs, genHZGParticles& _genHZG, bool& vetoDY){
  vector<TCGenParticle> genElectrons;
  vector<TCGenParticle> genMuons;
  vector<TCGenParticle> genZs;
  vector<TCGenParticle> genWs;
  vector<TCGenParticle> genHs;
  vector<TCGenParticle> genPhotons;
  vector<TCGenParticle> genLeptons;
  bool isMuMuGamma = false;
  bool isEEGamma = false;
  bool goodPhot = false;

  for (int i = 0; i < genParticles.GetSize(); ++i) {
    TCGenParticle* thisGen = (TCGenParticle*) genParticles.At(i);    
  //  cout<<thisGen->GetPDGId()<<endl;
    if (abs(thisGen->GetPDGId()) == 11){
      genElectrons.push_back(*thisGen);
      if (thisGen->Mother() && abs(thisGen->Mother()->GetPDGId())==23) isEEGamma = true;
    }else if (abs(thisGen->GetPDGId()) == 13){
      genMuons.push_back(*thisGen);
      if (thisGen->Mother() && abs(thisGen->Mother()->GetPDGId())==23) isMuMuGamma = true;
    }else if (abs(thisGen->GetPDGId()) == 23) genZs.push_back(*thisGen);
    else if (abs(thisGen->GetPDGId()) == 24) genWs.push_back(*thisGen);
    else if (abs(thisGen->GetPDGId()) == 22) genPhotons.push_back(*thisGen);
    else if (abs(thisGen->GetPDGId()) == 25 || abs(thisGen->GetPDGId()) == 36) genHs.push_back(*thisGen);
  }
  ///////// sort gen particles by PT ///////////

  sort(genElectrons.begin(), genElectrons.end(), P4SortCondition);
  sort(genMuons.begin(), genMuons.end(), P4SortCondition);
  sort(genZs.begin(), genZs.end(), P4SortCondition);
  sort(genWs.begin(), genWs.end(), P4SortCondition);
  sort(genPhotons.begin(), genPhotons.end(), P4SortCondition);
  sort(genHs.begin(), genHs.end(), P4SortCondition);
  _genPhotons = genPhotons;
  _genMuons = genMuons;
  _genElectrons = genElectrons;
  _genZs = genZs;




  if (isMuMuGamma && (_parameters.selection == "mumuGamma")) genLeptons = genMuons;
  else if (isEEGamma && (_parameters.selection == "eeGamma")) genLeptons = genElectrons;

  //////////// DYJets Gamma Veto ////////////
  if (_parameters.DYGammaVeto && (_parameters.suffix.find("DY") != string::npos && genPhotons.size() > 0)){
    vector<TCGenParticle>::iterator phoIt;
    vector<TCGenParticle>::iterator lepIt;
    for (phoIt = genPhotons.begin(); phoIt<genPhotons.end(); phoIt++){
      if (abs((*phoIt).MotherId()) < 22 && (*phoIt).Pt() > 5){
        if((*phoIt).Mother()){
            if((*phoIt).DeltaR(*(*phoIt).Mother()) > 0.3){
              vetoDY = true;
              return;
            }
        }
      }
    }
  }
    /*
    if ((abs((*thisGen).MotherId()) < 22) && _recoPhotons.GetSize()>0 ){
      for (int j = 0; j<_recoPhotons.GetSize(); j++){
        TCPhoton* recoPho = (TCPhoton*) _recoPhotons.At(j);    
        if (recoPho->DeltaR(*thisGen) < 0.2 && fabs(recoPho->Pt() - thisGen->Pt()) < 10){ 
          vetoDY = true;
          return;
        }
      }
    }
    */
  
  if (_genHZG.lp){
    cout<<"well here's your fucking problem"<<endl;
    _genHZG.lp->Print();
    cout<<endl;
  }

  if (genZs.size() > 0) _genHZG.z = new TCGenParticle(genZs.front());
  if (genWs.size() > 0) _genHZG.w = new TCGenParticle(genWs.front());
  if (genHs.size() > 0){
    _genHZG.h = new TCGenParticle(genHs.front());
  }
  
  bool posLep = false;
  bool negLep = false;
  vector<TCGenParticle>::iterator testIt;

  if (genLeptons.size() > 1){
    for (testIt=genLeptons.begin(); testIt<genLeptons.end(); testIt++){
      if((*testIt).Mother() && (*testIt).Mother()->Mother() && (*testIt).Mother()->GetPDGId() == 23
          && ((*testIt).Mother()->Mother()->GetPDGId() == 25 ||(*testIt).Mother()->Mother()->GetPDGId() == 36  )&& (*testIt).Charge() == 1 ){

        _genHZG.lp = new TCGenParticle(*testIt);
        posLep = true;
      }else if((*testIt).Mother() && (*testIt).Mother()->Mother() && (*testIt).Mother()->GetPDGId()== 23
          && ((*testIt).Mother()->Mother()->GetPDGId() == 25 || (*testIt).Mother()->Mother()->GetPDGId() == 36)&& (*testIt).Charge() == -1){
        _genHZG.lm = new TCGenParticle(*testIt);
        negLep = true;
      }
      if (posLep && negLep) break;
    }
  }else { return;}

  if (genPhotons.size() > 0 && posLep && negLep && _genHZG.h){
      for (testIt=genPhotons.begin(); testIt<genPhotons.end(); testIt++){
        //cout<<"mother: "<<testIt->Mother()<<"\tstatus: "<<testIt->GetStatus()<<endl;
        if (fabs((*testIt+*_genHZG.lm+*_genHZG.lp).M()- _genHZG.h->M()) < 0.1) _genHZG.g = new TCGenParticle(*testIt); goodPhot = true; break;
        if ( ((*testIt).Mother() && (*testIt).Mother()->GetPDGId() == 25 || (*testIt).Mother() && (*testIt).Mother()->GetPDGId() == 36)
            && fabs((*testIt+*_genHZG.lm+*_genHZG.lp).M()-_genHZG.h->M()) < 0.1) _genHZG.g = new TCGenParticle(*testIt); goodPhot = true; break;
      }
      if (!goodPhot) return;
    //_genHZG.g = new TCGenParticle(genPhotons.front());
  }else{ return;}



  return;
}

void ParticleSelector::CleanUpGen(genHZGParticles& _genHZG){
  if (_genHZG.lp) delete _genHZG.lp;
  if (_genHZG.lm) delete _genHZG.lm;
  if (_genHZG.g) delete _genHZG.g;
  if (_genHZG.w) delete _genHZG.w;
  if (_genHZG.z) delete _genHZG.z;
  if (_genHZG.h) delete _genHZG.h;
  _genHZG.lp = _genHZG.lm = _genHZG.g = _genHZG.w = _genHZG.z = _genHZG.h = 0;
}

bool ParticleSelector::PassMuonID(const TCMuon& mu, const Cuts::muIDCuts& cutLevel){

  bool muPass = false;
  bool testPvBullshit = false;
  if (cutLevel.cutName == "tightMuID"){
    if (_parameters.suffix.find("2011") != string::npos){
      if (
          fabs(mu.Eta()) < 2.4
          && mu.IsGLB()                         == cutLevel.IsGLB
          && mu.NormalizedChi2()                < cutLevel.NormalizedChi2
          && mu.NumberOfValidMuonHits()         > cutLevel.NumberOfValidMuonHits
          && mu.NumberOfMatchedStations()       > cutLevel.NumberOfMatchedStations
          && mu.NumberOfValidPixelHits()        > cutLevel.NumberOfValidPixelHits
          && mu.TrackLayersWithMeasurement()    > cutLevel.TrackLayersWithMeasurement
          && fabs(mu.Dxy(&_pv))           < cutLevel.dxy
          && fabs(mu.Dz(&_pv))            < cutLevel.dz
         ) muPass = true;
    }else if(testPvBullshit){
      if (
          fabs(mu.Eta()) < 2.4
          && mu.IsPF()                          == cutLevel.IsPF
          && mu.IsGLB()                         == cutLevel.IsGLB
          && mu.NormalizedChi2()                < cutLevel.NormalizedChi2
          && mu.NumberOfValidMuonHits()         > cutLevel.NumberOfValidMuonHits
          && mu.NumberOfMatchedStations()       > cutLevel.NumberOfMatchedStations
          && mu.NumberOfValidPixelHits()        > cutLevel.NumberOfValidPixelHits
          && mu.TrackLayersWithMeasurement()    > cutLevel.TrackLayersWithMeasurement
         ){
        for (vector<TVector3>::iterator it = _allPvs.begin(); it != _allPvs.end(); ++it){
          if((fabs(mu.Dxy(&(*it))) < cutLevel.dxy) && (fabs(mu.Dz(&(*it)))  < cutLevel.dz)){
            muPass = true;
            break;
          }
        }
      }
    }else{
      if (
          fabs(mu.Eta()) < 2.4
          && mu.IsPF()                          == cutLevel.IsPF
          && mu.IsGLB()                         == cutLevel.IsGLB
          && mu.NormalizedChi2()                < cutLevel.NormalizedChi2
          && mu.NumberOfValidMuonHits()         > cutLevel.NumberOfValidMuonHits
          && mu.NumberOfMatchedStations()       > cutLevel.NumberOfMatchedStations
          && mu.NumberOfValidPixelHits()        > cutLevel.NumberOfValidPixelHits
          && mu.TrackLayersWithMeasurement()    > cutLevel.TrackLayersWithMeasurement
          && fabs(mu.Dxy(&_pv))           < cutLevel.dxy
          && fabs(mu.Dz(&_pv))            < cutLevel.dz
         ) muPass = true;
    }
  }else if (cutLevel.cutName == "amumu_MuID"){
    if (
        fabs(mu.Eta()) < 2.1
        && mu.IsGLB()                         == cutLevel.IsGLB
        && mu.NormalizedChi2()                < cutLevel.NormalizedChi2
        && mu.NumberOfMatchedStations()       > cutLevel.NumberOfMatchedStations
        && mu.NumberOfValidMuonHits()         > cutLevel.NumberOfValidMuonHits
        && mu.NumberOfValidPixelHits()        > cutLevel.NumberOfValidPixelHits
        && mu.TrackLayersWithMeasurement()    > cutLevel.TrackLayersWithMeasurement
        && fabs(mu.Dxy(&_pv))           < cutLevel.dxy
        && fabs(mu.Dz(&_pv))            < cutLevel.dz
       ) muPass = true;

  }else if (cutLevel.cutName == "dalitzMuID"){
    if (
        fabs(mu.Eta()) < 2.4
        && mu.IsPF()                          == cutLevel.IsPF
        && ((mu.IsTRK()                        == cutLevel.IsTRK
            && mu.NumberOfMatchedStations()       > cutLevel.NumberOfMatchedStations)
          || mu.IsGLB()                         == cutLevel.IsGLB)
        && fabs(mu.Dxy(&_pv))           < cutLevel.dxy
        && fabs(mu.Dz(&_pv))            < cutLevel.dz
       ) muPass = true;

  }
  return muPass;
}

bool ParticleSelector::PassMuonIso(const TCMuon& mu, const Cuts::muIsoCuts& cutLevel){

  bool isoPass = false;
  if(cutLevel.cutName == "amumu_MuIso"){
    if(mu.IdMap("SumPt_R03")/mu.Pt() < 0.1) isoPass = true;  // loose
    //if(mu.IdMap("SumPt_R03")/mu.Pt() < 0.05) isoPass = true;  // tight
  }else if (cutLevel.cutName == "tightMuIso" || cutLevel.cutName == "looseMuIso"){
    float combIso;

    combIso = (mu.PfIsoChargedHad()
      + max(0.,(double)mu.PfIsoNeutral()+ mu.PfIsoPhoton() - 0.5*mu.PfIsoPU()));

    if (combIso/mu.Pt() < cutLevel.relCombIso04) isoPass = true;
  }
  return isoPass;
}


bool ParticleSelector::PassElectronID(const TCElectron& el, const Cuts::elIDCuts& cutLevel, const TClonesArray& recoMuons, const bool cleanVsMuons)
{
  bool elPass = false;
  if (fabs(el.SCEta()) > 2.5) return elPass;
  if (cutLevel.cutName == "mvaPreElID"){
    if (fabs(el.SCEta()) < 1.479){
      if(el.SigmaIEtaIEta()                             < cutLevel.sigmaIetaIeta[0]
          && el.IdMap("hadronicOverEm")                 < cutLevel.HadOverEm[0]
          && el.IdMap("dr03TkSumPt")/el.Pt()            < cutLevel.dr03TkSumPt[0]
          && el.IdMap("dr03EcalRecHitSumEt")/el.Pt()    < cutLevel.dr03EcalRecHitSumEt[0]
          && el.IdMap("dr03HcalTowerSumEt")/el.Pt()     < cutLevel.dr03HcalTowerSumEt[0]
          && el.IdMap("gsf_numberOfLostHits")          == cutLevel.numberOfLostHits[0])
        elPass = true;
    }else {
      if(el.SigmaIEtaIEta()                             < cutLevel.sigmaIetaIeta[1]
          && el.IdMap("hadronicOverEm")                 < cutLevel.HadOverEm[1]
          && el.IdMap("dr03TkSumPt")/el.Pt()            < cutLevel.dr03TkSumPt[1]
          && el.IdMap("dr03EcalRecHitSumEt")/el.Pt()    < cutLevel.dr03EcalRecHitSumEt[1]
          && el.IdMap("dr03HcalTowerSumEt")/el.Pt()     < cutLevel.dr03HcalTowerSumEt[1]
          && el.IdMap("gsf_numberOfLostHits")          == cutLevel.numberOfLostHits[1])
        elPass = true;
    }
  }else{
    if (fabs(el.SCEta()) > 1.4446 && fabs(el.SCEta()) < 1.566) return elPass;
    if (
        (fabs(el.SCEta()) < 1.479
         && fabs(el.DeltaEtaSeedCluster())    < cutLevel.dEtaIn[0]
         && fabs(el.DeltaPhiSeedCluster())    < cutLevel.dPhiIn[0]
         && el.SigmaIEtaIEta()             < cutLevel.sigmaIetaIeta[0]
         && el.HadOverEm()                 < cutLevel.HadOverEm[0]
         && fabs(el.Dxy(&_pv))       < cutLevel.dxy[0]
         && fabs(el.Dz(&_pv))        < cutLevel.dz[0]
         && el.InverseEnergyMomentumDiff()         < cutLevel.fabsEPDiff[0]
         && el.ConversionMissHits()        <= cutLevel.ConversionMissHits[0]
         && el.PassConversionVeto()            == cutLevel.PassedConversionProb[0]
        ) ||
        (fabs(el.Eta()) > 1.479 
         && fabs(el.DeltaEtaSeedCluster())    < cutLevel.dEtaIn[1]
         && fabs(el.DeltaPhiSeedCluster())    < cutLevel.dPhiIn[1]
         && el.SigmaIEtaIEta()             < cutLevel.sigmaIetaIeta[1]
         && el.HadOverEm()                 < cutLevel.HadOverEm[1]
         && fabs(el.Dxy(&_pv))       < cutLevel.dxy[1]
         && fabs(el.Dz(&_pv))        < cutLevel.dz[1]
         && el.InverseEnergyMomentumDiff()         < cutLevel.fabsEPDiff[1]
         && el.ConversionMissHits()        <= cutLevel.ConversionMissHits[1]
         && el.PassConversionVeto()            == cutLevel.PassedConversionProb[1]
        )
        ) elPass = true; 
    //   cout<<"evt: "<<eventNumber<<" muon num: "<<recoMuons->GetSize()<<endl;
    if(cleanVsMuons){
      for (int j = 0; j < recoMuons.GetSize(); ++ j)
      {
        TCMuon* thisMuon = (TCMuon*) recoMuons.At(j);    
        //     if (eventNumber==11944 || eventNumber==1780) cout<<thisMuon->DeltaR(*el)<<endl;
        if (thisMuon->DeltaR(el) < 0.05){
          //cout<<"event: "<<eventNumber<<endl;
          //cout<<"unclean"<<endl;
          //el.Print();
          elPass = false;
          break;
        }
      }
    }
  }
  return elPass;
}

bool ParticleSelector::PassElectronMVA(const TCElectron& el, const Cuts::elMVACuts& cutLevel){
  bool passMVA = false;
  if (cutLevel.cutName == "hzgMVAID"){
    if (el.Pt() > cutLevel.pt[0] && el.Pt() < cutLevel.pt[1]){
      if (el.MvaID_Old() > cutLevel.mvaVal[0]) passMVA = true;
    }else if(el.Pt() > cutLevel.pt[1]){
      if (el.MvaID_Old() > cutLevel.mvaVal[1]) passMVA = true;
    }

  }else if(cutLevel.cutName == "hzzMVAID"){
    if (el.Pt() > cutLevel.pt[0] && el.Pt() < cutLevel.pt[1]){
      if(abs(el.Eta()) < cutLevel.eta[0]){
        if ((el.MvaID_HZZ()   >  cutLevel.mvaVal[0])
           && (el.ConversionMissHits()  <  cutLevel.missHits[0])
           && (el.IP3dSig()       <  cutLevel.sip[0])) passMVA = true;
      }else if(abs(el.Eta()) < cutLevel.eta[1]){
        if ((el.MvaID_HZZ()   >  cutLevel.mvaVal[1])
           && (el.ConversionMissHits()  <  cutLevel.missHits[1])
           && (el.IP3dSig()       <  cutLevel.sip[1])) passMVA = true;
      }else if(abs(el.Eta()) < cutLevel.eta[2]){
        if ((el.MvaID_HZZ()   >  cutLevel.mvaVal[2])
           && (el.ConversionMissHits()  <  cutLevel.missHits[2])
           && (el.IP3dSig()       <  cutLevel.sip[2])) passMVA = true;
      }
    }else if(el.Pt() > cutLevel.pt[1]){
      if(abs(el.Eta()) < cutLevel.eta[0]){
        if ((el.MvaID_HZZ()   >  cutLevel.mvaVal[3])
           && (el.ConversionMissHits()  <  cutLevel.missHits[3])
           && (el.IP3dSig()       <  cutLevel.sip[3])) passMVA = true;
      }else if(abs(el.Eta()) < cutLevel.eta[1]){
        if ((el.MvaID_HZZ()   >  cutLevel.mvaVal[4])
           && (el.ConversionMissHits()  <  cutLevel.missHits[4])
           && (el.IP3dSig()       <  cutLevel.sip[4])) passMVA = true;
      }else if(abs(el.Eta()) < cutLevel.eta[2]){
        if ((el.MvaID_HZZ()   >  cutLevel.mvaVal[5])
           && (el.ConversionMissHits()  <  cutLevel.missHits[5])
           && (el.IP3dSig()       <  cutLevel.sip[5])) passMVA = true;
      }
    }
  }else if(cutLevel.cutName == "hwwMVAID"){
    if (el.Pt() > cutLevel.pt[0] && el.Pt() < cutLevel.pt[1]){
      if(abs(el.Eta()) < cutLevel.eta[0]){
        if ((el.MvaID_Old()   >  cutLevel.mvaVal[0])
           && (el.ConversionMissHits()  ==  cutLevel.missHits[0])
           && (el.PassConversionVeto()       ==  cutLevel.conversionVeto[0])) passMVA = true;
      }else if(abs(el.Eta()) < cutLevel.eta[1]){
        if ((el.MvaID_HZZ()   >  cutLevel.mvaVal[1])
           && (el.ConversionMissHits()  ==  cutLevel.missHits[1])
           && (el.PassConversionVeto()       ==  cutLevel.conversionVeto[1])) passMVA = true;
      }else if(abs(el.Eta()) < cutLevel.eta[2]){
        if ((el.MvaID_HZZ()   >  cutLevel.mvaVal[2])
           && (el.ConversionMissHits()  ==  cutLevel.missHits[2])
           && (el.PassConversionVeto()       ==  cutLevel.conversionVeto[2])) passMVA = true;
      }
    }else if(el.Pt() > cutLevel.pt[1]){
      if(abs(el.Eta()) < cutLevel.eta[0]){
        if ((el.MvaID_HZZ()   >  cutLevel.mvaVal[3])
           && (el.ConversionMissHits()  ==  cutLevel.missHits[3])
           && (el.PassConversionVeto()       ==  cutLevel.conversionVeto[3])) passMVA = true;
      }else if(abs(el.Eta()) < cutLevel.eta[1]){
        if ((el.MvaID_HZZ()   >  cutLevel.mvaVal[4])
           && (el.ConversionMissHits()  ==  cutLevel.missHits[4])
           && (el.PassConversionVeto()       ==  cutLevel.conversionVeto[4])) passMVA = true;
      }else if(abs(el.Eta()) < cutLevel.eta[2]){
        if ((el.MvaID_HZZ()   >  cutLevel.mvaVal[5])
           && (el.ConversionMissHits()  ==  cutLevel.missHits[5])
           && (el.PassConversionVeto()       ==  cutLevel.conversionVeto[5])) passMVA = true;
      }
    }
  }


  return passMVA;
}


bool ParticleSelector::PassElectronIso(const TCElectron& el, const Cuts::elIsoCuts& cutLevel, float EAEle[7]){
  float thisEA = 0;

  thisEA = el.EffArea();

  float combIso = (el.PfIsoCharged()
    + max(0.,(double)el.PfIsoNeutral() + el.PfIsoPhoton() - _rhoFactor*thisEA));
  bool isoPass = false;

  if (cutLevel.cutName == "mediumElIso"){
    if (el.Pt() < 20){
      if (combIso/el.Pt() < 0.10) isoPass = true;
    }else{
      if (combIso/el.Pt() < 0.15) isoPass = true;
    }

  }else{
    if (combIso/el.Pt() < cutLevel.relCombIso04) isoPass = true;
  }
  return isoPass;
}

bool ParticleSelector::PassPhotonID(const TCPhoton& ph, const Cuts::phIDCuts& cutLevel){
  bool phoPass = false;
  bool phoPass1 = false;
  bool phoPass2 = false;
  if (fabs(ph.SCEta()) > 2.5) return phoPass;
  if (cutLevel.cutName == "preSelPhID"){
    if(fabs(ph.SCEta()) > 1.4442 && fabs(ph.SCEta()) < 1.566) return phoPass;
    if(fabs(ph.SCEta()) < 1.479){
      if(
          ph.ConversionVeto()          == cutLevel.PassedEleSafeVeto[0]
          && ph.SigmaIEtaIEta()        < cutLevel.sigmaIetaIeta[0]
        ){
        if(ph.R9() > 0.9){
          if(ph.HadOverEm()            < cutLevel.HadOverEm[0]) phoPass1 = true;
        }else{
          if(ph.HadOverEm()            < cutLevel.HadOverEm[1]) phoPass1 = true;
        }
      }
    }else{
      if(
          ph.ConversionVeto()          == cutLevel.PassedEleSafeVeto[1]
          && ph.SigmaIEtaIEta()        < cutLevel.sigmaIetaIeta[1]
          && ph.HadOverEm()            < cutLevel.HadOverEm[1]
        ) phoPass1 = true;
    }
    if(ph.R9() > 0.9){
      if(
          ph.IdMap("HadIso_R03") - 0.005*ph.Pt()     < cutLevel.HcalIso[0]
          && ph.IdMap("TrkIso_R03") - 0.002*ph.Pt()  < cutLevel.TrkIso[0]
          && ph.CiCPF4chgpfIso02()[0]                  < cutLevel.ChPfIso[0]
        ) phoPass2 = true;
    }else{
      if(
          ph.IdMap("HadIso_R03") - 0.005*ph.Pt()     < cutLevel.HcalIso[1]
          && ph.IdMap("TrkIso_R03") - 0.002*ph.Pt()  < cutLevel.TrkIso[1]
          && ph.CiCPF4chgpfIso02()[0]                  < cutLevel.ChPfIso[1]
        ) phoPass2 = true;
    }

    if(phoPass1 && phoPass2) phoPass = true;
  }else{
    if (fabs(ph.SCEta()) > 1.4442 && fabs(ph.SCEta()) < 1.566) return phoPass;
    if(
        (fabs(ph.SCEta())  < 1.4442
         && ph.ConversionVeto()       == cutLevel.PassedEleSafeVeto[0]
         && ph.HadOverEm()               < cutLevel.HadOverEm[0]
         && ph.SigmaIEtaIEta()           < cutLevel.sigmaIetaIeta[0]
        ) ||
        (fabs(ph.SCEta())  > 1.566
         && ph.ConversionVeto()       == cutLevel.PassedEleSafeVeto[1]
         && ph.HadOverEm()               < cutLevel.HadOverEm[1]
         && ph.SigmaIEtaIEta()           < cutLevel.sigmaIetaIeta[1]
        )
      ) phoPass = true;
  }
  return phoPass;
}

bool ParticleSelector::PassPhotonIso(const TCPhoton& ph, const Cuts::phIsoCuts& cutLevel, float EAPho[7][3]){
  float chEA,nhEA,phEA,chIsoCor,nhIsoCor,phIsoCor;
  bool isoPass = false;

  if(fabs(ph.SCEta()) > 2.5) return isoPass;

  if (fabs(ph.SCEta()) < 1.0){
    chEA = EAPho[0][0];
    nhEA = EAPho[0][1];
    phEA = EAPho[0][2];
  }else if (fabs(ph.SCEta()) < 1.479){
    chEA = EAPho[1][0];
    nhEA = EAPho[1][1];
    phEA = EAPho[1][2];
  }else if (fabs(ph.SCEta()) < 2.0){
    chEA = EAPho[2][0];
    nhEA = EAPho[2][1];
    phEA = EAPho[2][2];
  }else if (fabs(ph.SCEta()) < 2.2){
    chEA = EAPho[3][0];
    nhEA = EAPho[3][1];
    phEA = EAPho[3][2];
  }else if (fabs(ph.SCEta()) < 2.3){ 
    chEA = EAPho[4][0];
    nhEA = EAPho[4][1];
    phEA = EAPho[4][2];
  }else if (fabs(ph.SCEta()) < 2.4){
    chEA = EAPho[5][0];
    nhEA = EAPho[5][1];
    phEA = EAPho[5][2];
  }else{                                  
    chEA = EAPho[6][0];
    nhEA = EAPho[6][1];
    phEA = EAPho[6][2];
  }

  chIsoCor = ph.PfIsoCharged()-_rhoFactor*chEA;
  nhIsoCor = ph.PfIsoNeutral()-_rhoFactor*nhEA;
  phIsoCor = ph.PfIsoPhoton() -_rhoFactor*phEA;

  if (cutLevel.cutName == "loosePhIso"){
    if (
        (fabs(ph.SCEta()) < 1.4442
        //(fabs(ph.Eta())  < 1.566
         && max((double)chIsoCor,0.)          < cutLevel.chIso03[0]
         && max((double)nhIsoCor,0.)          < cutLevel.nhIso03[0] + 0.04*ph.Pt()
         && max((double)phIsoCor,0.)          < cutLevel.phIso03[0] + 0.005*ph.Pt() 
        ) ||
        (fabs(ph.SCEta()) > 1.566
        //(fabs(ph.Eta())  > 1.566
         && max((double)chIsoCor,0.)          < cutLevel.chIso03[1]
         && max((double)nhIsoCor,0.)          < cutLevel.nhIso03[1] + 0.04*ph.Pt() 
         //&& phoCut["phIso03"]/ph.Pt() < nuthin
        )
       ) isoPass = true;
  } else {
    if (
        //(fabs(ph.Eta())  < 1.566
        (fabs(ph.SCEta()) < 1.4442
         && max((double)chIsoCor,0.)          < cutLevel.chIso03[0]
         && max((double)nhIsoCor,0.)          < cutLevel.nhIso03[0] + 0.04*ph.Pt()
         && max((double)phIsoCor,0.)          < cutLevel.phIso03[0] + 0.005*ph.Pt() 
        ) ||
        //(fabs(ph.Eta())  > 1.566
        (fabs(ph.SCEta()) > 1.566
         && max((double)chIsoCor,0.)          < cutLevel.chIso03[1]
         && max((double)nhIsoCor,0.)          < cutLevel.nhIso03[1] + 0.04*ph.Pt() 
         && max((double)phIsoCor,0.)          < cutLevel.phIso03[1] + 0.005*ph.Pt() 
        )
       ) isoPass = true;
  }
  return isoPass;
}

bool ParticleSelector::PassPhotonMVA(const TCPhoton& ph, const Cuts::phMVACuts& cutLevel, const bool isGoodLeptons){
  bool mvaPass = false;
  /* Photon identification with the MVA. Returns the MVA evaluated value.
   *
   * i = index of a photon candidate to consider.
   */

  // classification variables
  static Float_t phoEt_, phoEta_, phoPhi_, phoR9_;
  static Float_t phoSigmaIEtaIEta_, phoSigmaIEtaIPhi_;
  static Float_t phoS13_, phoS4_, phoS25_, phoSCEta_, phoSCRawE_;
  static Float_t phoSCEtaWidth_, phoSCPhiWidth_, rho2012_;
  static Float_t phoPFPhoIso_, phoPFChIso_, phoPFChIsoWorst_;
  static Float_t phoESEnToRawE_, phoESEffSigmaRR_x_;

  // MVA classifiers for 0=ECAL barrel and 1=ECAL endcaps
  static TMVA::Reader* tmvaReader[2] = {NULL, NULL};

  // 0=ECAL barrel or 1=ECAL endcaps
  int iBE = (fabs(ph.SCEta()) < 1.479) ? 0 : 1;

  // one-time MVA initialization
  if (!tmvaReader[iBE]) {
    tmvaReader[iBE] = new TMVA::Reader("!Color:Silent");

    // add classification variables
    tmvaReader[iBE]->AddVariable("phoPhi", &phoPhi_);
    tmvaReader[iBE]->AddVariable("phoR9", &phoR9_);
    tmvaReader[iBE]->AddVariable("phoSigmaIEtaIEta", &phoSigmaIEtaIEta_);
    tmvaReader[iBE]->AddVariable("phoSigmaIEtaIPhi", &phoSigmaIEtaIPhi_);
    tmvaReader[iBE]->AddVariable("s13", &phoS13_);
    tmvaReader[iBE]->AddVariable("s4ratio", &phoS4_);
    tmvaReader[iBE]->AddVariable("s25", &phoS25_);
    tmvaReader[iBE]->AddVariable("phoSCEta", &phoSCEta_);
    tmvaReader[iBE]->AddVariable("phoSCRawE", &phoSCRawE_);
    tmvaReader[iBE]->AddVariable("phoSCEtaWidth", &phoSCEtaWidth_);
    tmvaReader[iBE]->AddVariable("phoSCPhiWidth", &phoSCPhiWidth_);

    if (iBE == 1) {
      tmvaReader[iBE]->AddVariable("phoESEn/phoSCRawE", &phoESEnToRawE_);
      tmvaReader[iBE]->AddVariable("phoESEffSigmaRR", &phoESEffSigmaRR_x_);
    }

    tmvaReader[iBE]->AddVariable("rho2012", &rho2012_);
    tmvaReader[iBE]->AddVariable("phoPFPhoIso", &phoPFPhoIso_);
    tmvaReader[iBE]->AddVariable("phoPFChIso", &phoPFChIso_);

    tmvaReader[iBE]->AddVariable("phoPFChIsoWorst", &phoPFChIsoWorst_);

    tmvaReader[iBE]->AddSpectator("phoEt", &phoEt_);
    tmvaReader[iBE]->AddSpectator("phoEta", &phoEta_);

    // weight files
    if (iBE == 0)
      tmvaReader[0]->BookMVA("BDT", "otherHistos/EB_BDT.weights.xml");
    else
      tmvaReader[1]->BookMVA("BDT", "otherHistos/EE_BDT.weights.xml");

  } // one-time initialization

  // set MVA variables
  phoPhi_ = ph.Phi();
  phoR9_ = ph.R9();
  phoSigmaIEtaIEta_ = ph.SigmaIEtaIEta();
  phoSigmaIEtaIPhi_ = ph.SigmaIEtaIPhi();
  phoS4_ = ph.E2x2()/ph.E5x5();
  phoS13_ = ph.E1x3()/ph.E5x5();
  phoS25_ = ph.E2x5Max()/ph.E5x5();
  phoSCEta_ = ph.SCEta();
  phoSCRawE_ = ph.SCRawEnergy();
  phoSCEtaWidth_ = ph.SCEtaWidth();
  phoSCPhiWidth_ = ph.SCPhiWidth();
  rho2012_ = _rhoFactor;
  phoPFPhoIso_ = ph.PfIsoPhoton();
  phoPFChIso_ = ph.PfIsoCharged();
  phoESEnToRawE_ = ph.PreShowerOverRaw();
  phoESEffSigmaRR_x_= ph.ESEffSigmaRR()[0];
  phoEt_ = ph.Pt();
  phoEta_ = ph.Eta();

  // evaluate largest isolation value
  phoPFChIsoWorst_ = 0;
  for (size_t k = 0; k < ph.CiCPF4chgpfIso03().size(); ++k){
    if (phoPFChIsoWorst_ < ph.CiCPF4chgpfIso03()[k]) phoPFChIsoWorst_ = ph.CiCPF4chgpfIso03()[k];
  }

  //find the most likely cat (cats are not assigned till further particle correction and selection,
  //but this will work as a temporary stand-in
  int catNum = 0;

  if (isGoodLeptons && (fabs(ph.SCEta()) < 1.4442) ){
    if (ph.R9()>= 0.94){
      catNum = 1;
    }else{
      catNum = 2;
    }
  } else if ( fabs(ph.SCEta()) < 1.4442){
    catNum = 3;
  } else {
    catNum = 4;
  }

  float mvaVal = tmvaReader[iBE]->EvaluateMVA("BDT");
  if (catNum == 1){
    if (mvaVal > cutLevel.mvaValCat1) mvaPass = true;
  }else if (catNum == 2){
    if (mvaVal > cutLevel.mvaValCat2) mvaPass = true;
  }else if (catNum == 3){
    if (mvaVal > cutLevel.mvaValCat3) mvaPass = true;
  }else{
    if (mvaVal > cutLevel.mvaValCat4) mvaPass = true;
  }
  return mvaPass;
}

bool ParticleSelector::PassJetID(const TCJet& jet, int nVtx, const Cuts::jetIDCuts& cutLevel){
  bool idPass = false;

  if (cutLevel.cutName == "vbfJetID"){
    if (fabs(jet.Eta()) < 2.5){
      if(
            jet.BetaStarClassic()/log(nVtx-0.64)  < cutLevel.betaStarC[0]
         && jet.DR2Mean()                         < cutLevel.dR2Mean[0]
        ) idPass = true;
    }else if (fabs(jet.Eta()) < 2.75){
      if(
            jet.BetaStarClassic()/log(nVtx-0.64)  < cutLevel.betaStarC[1]
         && jet.DR2Mean()                         < cutLevel.dR2Mean[1]
        ) idPass = true;
    }else if (fabs(jet.Eta()) < 3.0){
      if(
            jet.DR2Mean()                         < cutLevel.dR2Mean[2]
        ) idPass = true;
    }else{ 
      if(
            jet.DR2Mean()                         < cutLevel.dR2Mean[3]
        ) idPass = true;
    }

  }else if (cutLevel.cutName=="amumu_fJetID"){
    if (fabs(jet.Eta()) < 2.4) return false;
    if (fabs(jet.Eta()) > 4.7) return false;
    if (jet.Pt() < 30) return false;

    if( !(jet.NumConstit()  > 1
      && jet.NeuHadFrac()  < 0.99
      && jet.NeuEmFrac()   < 0.99)) return false;
    //if( !(jet.NumConstit()  > 1
    //  && jet.NeuHadFrac()  < 0.9
    //  && jet.NeuEmFrac()   < 0.9)) return false;

    if (fabs(jet.Eta()) < 2.5){
      if(
            jet.BetaStarClassic()/log(nVtx-0.64)  < cutLevel.betaStarC[0]
         && jet.DR2Mean()                         < cutLevel.dR2Mean[0]
        ) idPass = true;
    }else if (fabs(jet.Eta()) < 2.75){
      if(
            jet.BetaStarClassic()/log(nVtx-0.64)  < cutLevel.betaStarC[1]
         && jet.DR2Mean()                         < cutLevel.dR2Mean[1]
        ) idPass = true;
    }else if (fabs(jet.Eta()) < 3.0){
      if(
            jet.DR2Mean()                         < cutLevel.dR2Mean[2]
        ) idPass = true;
    }else{ 
      if(
            jet.DR2Mean()                         < cutLevel.dR2Mean[3]
        ) idPass = true;
    }

  }else if (cutLevel.cutName=="amumu_fJetID_v2"){
    if (fabs(jet.Eta()) < 2.4) return false;
    if (fabs(jet.Eta()) > 4.7) return false;
    if (jet.Pt() < 30) return false;
    //if (jet.Pt() < 25) return false;

    if( !(jet.NumConstit()  > 1
      && jet.NeuHadFrac()  < 0.99
      && jet.NeuEmFrac()   < 0.99)) return false;
    //if( !(jet.NumConstit()  > 1
    //  && jet.NeuHadFrac()  < 0.9
    //  && jet.NeuEmFrac()   < 0.9)) return false;
    
    if (jet.IdMap("PUID_MVA_Loose")) idPass = true;


  }else if (cutLevel.cutName=="amumu_cJetVetoID"){
    if (fabs(jet.Eta()) > 2.4) return false;
    if (jet.Pt() < 30) return false;

    if( !(jet.NumConstit()  > 1
      && jet.NeuHadFrac()  < 0.99
      && jet.NeuEmFrac()   < 0.99
      && jet.ChHadFrac()   > 0.
      && jet.NumChPart()   > 0.
      && jet.ChEmFrac()    < 0.99)) return false;

    if (jet.IdMap("PUID_MVA_Loose")) idPass = true;

  } else if (cutLevel.cutName=="amumu_bJetID"){
    if (fabs(jet.Eta()) > 2.4) return false;
    if (jet.Pt() < 30) return false;

    if( !(jet.NumConstit()  > 1
      && jet.NeuHadFrac()  < 0.99
      && jet.NeuEmFrac()   < 0.99
      && jet.ChHadFrac()   > 0.
      && jet.NumChPart()   > 0
      && jet.ChEmFrac()    < 0.99)) return false;

    if (jet.BDiscriminatorMap("CSV") > 0.679) idPass = true;  // CSV Medium

  } else if (cutLevel.cutName=="amumu_bJetID_v2"){
    if (fabs(jet.Eta()) > 2.4) return false;
    if (jet.Pt() < 30) return false;

    if( !(jet.NumConstit()  > 1
      && jet.NeuHadFrac()  < 0.99
      && jet.NeuEmFrac()   < 0.99
      && jet.ChHadFrac()   > 0.
      && jet.NumChPart()   > 0
      && jet.ChEmFrac()    < 0.99)) return false;

    //if (jet.BDiscriminatorMap("CSVv1") > 0.783) idPass = true;  // CSVv1 medium
    //if (jet.BDiscriminatorMap("CSVv1") > 0.920) idPass = true;  // CSVv1 tight
    if (jet.BDiscriminatorMap("CSVMVA") > 0.783) idPass = true;  // CSVMVA olga
    //if (jet.BDiscriminatorMap("CSV") > 0.679) idPass = true;  // CSV medium
    //if (jet.BDiscriminatorMap("CSV") > 0.898) idPass = true;  // CSV tight
    //if (jet.BDiscriminatorMap("CSV") > 0.783) idPass = true;  // CSV olga
  }

  return idPass;
}

//bool ParticleSelector::PassJetID(const TCJet& jet, int nVtx, const TCPhysObject& muon1, const TCPhysObject& muon2, const Cuts::jetIDCuts& cutLevel){
//  bool idPass = false;
//  return idPass;
//}

    
float ParticleSelector::Zeppenfeld(const TLorentzVector& p, const TLorentzVector& pj1, const TLorentzVector& pj2)
{
  float zep = p.Eta()-(pj1.Eta()+pj2.Eta())/2.;
  return zep;
}

