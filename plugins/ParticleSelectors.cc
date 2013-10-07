#include "../interface/ParticleSelectors.h"

ParticleSelector::ParticleSelector(Cuts* cuts, bool isRealData, int runNumber, TRandom3* rEl):
  _cuts(cuts),
  _isRealData(isRealData),
  _runNumber(runNumber),
  _rEl(rEl),
  _pv(),
  _rhoFactor(-1),
  _evtnum(-1)
{
}

void ParticleSelector::SetPv(TVector3* pv){
  _pv = *pv;
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
    if (electronList[i].Pt() > _cuts->leadElePt){
      for(unsigned int j =1; j<electronList.size(); j++){
        if (electronList[j].Pt() > _cuts->trailElePt && electronList[j].Charge() != electronList[i].Charge()){
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
          
bool ParticleSelector::FindGoodZElectron(vector<TCElectron>& electronList, const vector<TCElectron>& uncorElectronList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& uncorLepton1, TLorentzVector& uncorLepton2, TLorentzVector& ZP4, float& eta1, float& eta2, int& int1, int& int2){
  TLorentzVector tmpZ;
  bool goodZ = false;
  float ZmassDiff=99999;
  for(unsigned int i =0; i<electronList.size(); i++){
    if (electronList[i].Pt() > _cuts->leadElePt){
      for(unsigned int j =1; j<electronList.size(); j++){
        if (electronList[j].Pt() > _cuts->trailElePt && electronList[j].Charge() != electronList[i].Charge()){
          goodZ = true;
          /// do regression
          if (parameters::doEleReg && parameters::period.find("2012") != string::npos){
            if (electronList[i].RegressionMomCombP4().E() != 0){
              electronList[i].SetPtEtaPhiE(electronList[i].RegressionMomCombP4().E()/cosh(electronList[i].RegressionMomCombP4().Eta()),electronList[i].RegressionMomCombP4().Eta(),electronList[i].RegressionMomCombP4().Phi(),electronList[i].RegressionMomCombP4().E());
              //electronList[i].SetPtEtaPhiE(electronList[i].RegressionMomCombP4().Pt(),electronList[i].RegressionMomCombP4().Eta(),electronList[i].RegressionMomCombP4().Phi(),electronList[i].RegressionMomCombP4().E());
            }
            if (electronList[j].RegressionMomCombP4().E() != 0){
              electronList[j].SetPtEtaPhiE(electronList[j].RegressionMomCombP4().E()/cosh(electronList[j].RegressionMomCombP4().Eta()),electronList[j].RegressionMomCombP4().Eta(),electronList[j].RegressionMomCombP4().Phi(),electronList[j].RegressionMomCombP4().E());
              //electronList[j].SetPtEtaPhiE(electronList[j].RegressionMomCombP4().Pt(),electronList[j].RegressionMomCombP4().Eta(),electronList[j].RegressionMomCombP4().Phi(),electronList[j].RegressionMomCombP4().E());
            }
          }
          /// do eng cor
          if(parameters::engCor){
            float energyElCorI;
            float energyElCorJ;
            if ( parameters::period.find("2011") != string::npos ){
              energyElCorI = correctedElectronEnergy( electronList[i].E(), electronList[i].SCEta(), electronList[i].R9(), _runNumber, 0, "2011", !_isRealData, _rEl );
              energyElCorJ = correctedElectronEnergy( electronList[j].E(), electronList[j].SCEta(), electronList[j].R9(), _runNumber, 0, "2011", !_isRealData, _rEl );
            }else{
              if(parameters::doEleReg && electronList[i].RegressionMomCombP4().E() != 0){
                energyElCorI = correctedElectronEnergy( electronList[i].E(), electronList[i].SCEta(), electronList[i].R9(), _runNumber, 1, "Moriond2013", !_isRealData, _rEl );
              }else{
                energyElCorI = correctedElectronEnergy( electronList[i].E(), electronList[i].SCEta(), electronList[i].R9(), _runNumber, 0, "HCP2012", !_isRealData, _rEl );
              }
              if(parameters::doEleReg && electronList[j].RegressionMomCombP4().E() != 0){
                energyElCorJ = correctedElectronEnergy( electronList[j].E(), electronList[j].SCEta(), electronList[j].R9(), _runNumber, 1, "Moriond2013", !_isRealData, _rEl );
              }else{
                energyElCorJ = correctedElectronEnergy( electronList[j].E(), electronList[j].SCEta(), electronList[j].R9(), _runNumber, 0, "HCP2012", !_isRealData, _rEl );
              }
            }
            //float newPtI = sqrt((pow(energyElCorI,2)-pow(0.000511,2))/pow(cosh(electronList[i].Eta()),2));
            //float newPtJ = sqrt((pow(energyElCorJ,2)-pow(0.000511,2))/pow(cosh(electronList[j].Eta()),2));
            float newPtI = energyElCorI/cosh(electronList[i].Eta());
            float newPtJ = energyElCorJ/cosh(electronList[j].Eta());
            electronList[i].SetPtEtaPhiM(newPtI,electronList[i].Eta(),electronList[i].Phi(),0.000511);
            electronList[j].SetPtEtaPhiM(newPtJ,electronList[j].Eta(),electronList[j].Phi(),0.000511);
          }
          tmpZ = (electronList[i]+electronList[j]);
          if(fabs(91.1876-tmpZ.M()) < ZmassDiff){
            ZP4 = (electronList[i]+electronList[j]);
            lepton1 = electronList[i];
            lepton2 = electronList[j];
            int1 = i;
            int2 = j;
            eta1 = electronList[i].SCEta();
            eta2 = electronList[j].SCEta();
            uncorLepton1 = uncorElectronList[i];
            uncorLepton2 = uncorElectronList[j];
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
    if (muonList[i].Pt() > _cuts->leadMuPt){
      for(unsigned int j =1; j<muonList.size(); j++){
        if (muonList[j].Pt() > _cuts->trailMuPt && muonList[j].Charge() != muonList[i].Charge()){
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

bool ParticleSelector::FindGoodZMuon(const vector<TCMuon>& muonList, const vector<TCMuon>& uncorMuonList, TCPhysObject& lepton1, TCPhysObject& lepton2, TLorentzVector& uncorLepton1, TLorentzVector& uncorLepton2, TLorentzVector& ZP4, int& int1, int& int2){
  TLorentzVector tmpZ;
  bool goodZ = false;
  float ZmassDiff=99999;
  for(unsigned int i =0; i<muonList.size(); i++){
    if (muonList[i].Pt() > _cuts->leadMuPt){
      for(unsigned int j =1; j<muonList.size(); j++){
        if (muonList[j].Pt() > _cuts->trailMuPt && muonList[j].Charge() != muonList[i].Charge()){
          goodZ = true;
          tmpZ = (muonList[i]+muonList[j]);
          if(fabs(91.1876-tmpZ.M()) < ZmassDiff){
            ZP4 = (muonList[i]+muonList[j]);
            lepton1 = muonList[i];
            lepton2 = muonList[j];
            uncorLepton1 = uncorMuonList[i];
            uncorLepton2 = uncorMuonList[j];
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

bool ParticleSelector::FindGoodDiJets(const vector<TCJet>& jetList, const TCPhysObject& lepton1, const TCPhysObject& lepton2, const TLorentzVector& gamma, TCJet& jet1, TCJet& jet2){
  bool goodDiJets = false;
  if (jetList[0].Pt() < _cuts->leadJetPt) return goodDiJets;
  for (unsigned int i = 0; i < jetList.size(); i++){
    if (jetList[i].Pt() < _cuts->leadJetPt) return goodDiJets;
    if (fabs(jetList[i].Eta()) > 4.7) continue;
    jet1 = jetList[i];
    for (unsigned int j = i; j < jetList.size(); j++){
      if (jetList[j].Pt() < _cuts->leadJetPt) break; 
      if (fabs(jetList[j].Eta()) > 4.7) continue;
      jet2 = jetList[j];
      if (fabs(jet1.Eta() - jet2.Eta()) < _cuts->dEtaJet) continue;
      if (Zeppenfeld((lepton1+lepton2+gamma),jet1,jet2) > _cuts->zepp) continue;
      if ((jet1+jet2).M() < _cuts->mjj) continue;
      if ((jet1+jet2).DeltaPhi(lepton1+lepton2+gamma) < _cuts->dPhiJet) continue;
      goodDiJets = true;
      return goodDiJets;
    }
  }
  return goodDiJets;
}

void  ParticleSelector::FindGenParticles(const TClonesArray& genParticles, string selection, vector<TCGenParticle>& vetoPhotons, genHZGParticles& _genHZG){
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
    else if (abs(thisGen->GetPDGId()) == 25) genHs.push_back(*thisGen);
  }
  ///////// sort gen particles by PT ///////////

  sort(genElectrons.begin(), genElectrons.end(), P4SortCondition);
  sort(genMuons.begin(), genMuons.end(), P4SortCondition);
  sort(genZs.begin(), genZs.end(), P4SortCondition);
  sort(genWs.begin(), genWs.end(), P4SortCondition);
  sort(genPhotons.begin(), genPhotons.end(), P4SortCondition);
  sort(genHs.begin(), genHs.end(), P4SortCondition);

  vetoPhotons = genPhotons;

  if (isMuMuGamma && (selection == "mumuGamma")) genLeptons = genMuons;
  else if (isEEGamma && (selection == "eeGamma")) genLeptons = genElectrons;
  
  if (_genHZG.lp){
    cout<<"well here's your fucking problem"<<endl;
    _genHZG.lp->Print();
    cout<<endl;
  }
  
  bool posLep = false;
  bool negLep = false;
  vector<TCGenParticle>::iterator testIt;

  if (genLeptons.size() > 1){
    for (testIt=genLeptons.begin(); testIt<genLeptons.end(); testIt++){
      if((*testIt).Mother() && (*testIt).Mother()->GetPDGId() == 23 && (*testIt).Charge() == 1 ){
        _genHZG.lp = new TCGenParticle(*testIt);
        posLep = true;
      }else if((*testIt).Mother() && (*testIt).Mother()->GetPDGId()== 23 && (*testIt).Charge() == -1){
        _genHZG.lm = new TCGenParticle(*testIt);
        negLep = true;
      }
      if (posLep && negLep) break;
    }
  }else { return;}

  if (genPhotons.size() > 0 && posLep && negLep){
      for (testIt=genPhotons.begin(); testIt<genPhotons.end(); testIt++){
        //cout<<"mother: "<<testIt->Mother()<<"\tstatus: "<<testIt->GetStatus()<<endl;
        if (fabs((*testIt+*_genHZG.lm+*_genHZG.lp).M()-125.0) < 0.1) _genHZG.g = new TCGenParticle(*testIt); goodPhot = true; break;
        if ((*testIt).Mother() && (*testIt).Mother()->GetPDGId() == 25 && fabs((*testIt+*_genHZG.lm+*_genHZG.lp).M()-125.0) < 0.1) _genHZG.g = new TCGenParticle(*testIt); goodPhot = true; break;
      }
      if (!goodPhot) return;
    //_genHZG.g = new TCGenParticle(genPhotons.front());
  }else{ return;}


  if (genZs.size() > 0) _genHZG.z = new TCGenParticle(genZs.front());
  if (genWs.size() > 0) _genHZG.w = new TCGenParticle(genWs.front());
  if (genHs.size() > 0) _genHZG.h = new TCGenParticle(genHs.front());

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

  if (parameters::suffix.find("2011") != string::npos){
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
  return muPass;
}

bool ParticleSelector::PassMuonIso(const TCMuon& mu, const Cuts::muIsoCuts& cutLevel){

  float combIso;

  combIso = (mu.IsoMap("pfChargedHadronPt_R04")
    + max(0.,(double)mu.IsoMap("pfNeutralHadronEt_R04") + mu.IsoMap("pfPhotonEt_R04") - 0.5*mu.IsoMap("pfPUPt_R04")));

  bool isoPass = false;
  if (combIso/mu.Pt() < cutLevel.relCombIso04) isoPass = true;
  return isoPass;
}


bool ParticleSelector::PassElectronID(const TCElectron& el, const Cuts::elIDCuts& cutLevel, const TClonesArray& recoMuons)
{
  bool elPass = false;
  if (fabs(el.SCEta()) > 2.5) return elPass;
  if (fabs(el.SCEta()) > 1.4442 && fabs(el.SCEta()) < 1.566) return elPass;
  if (
    (fabs(el.Eta()) < 1.566
      && fabs(el.DetaSuperCluster())    < cutLevel.dEtaIn[0]
      && fabs(el.DphiSuperCluster())    < cutLevel.dPhiIn[0]
      && el.SigmaIEtaIEta()             < cutLevel.sigmaIetaIeta[0]
      && el.HadOverEm()                 < cutLevel.HadOverEm[0]
      && fabs(el.Dxy(&_pv))       < cutLevel.dxy[0]
      && fabs(el.Dz(&_pv))        < cutLevel.dz[0]
      && el.IdMap("fabsEPDiff")         < cutLevel.fabsEPDiff[0]
      && el.ConversionMissHits()        <= cutLevel.ConversionMissHits[0]
      && el.ConversionVeto()            == cutLevel.PassedConversionProb[0]
      ) ||
    (fabs(el.Eta()) > 1.566  
      && fabs(el.DetaSuperCluster())    < cutLevel.dEtaIn[1]
      && fabs(el.DphiSuperCluster())    < cutLevel.dPhiIn[1]
      && el.SigmaIEtaIEta()             < cutLevel.sigmaIetaIeta[1]
      && el.HadOverEm()                 < cutLevel.HadOverEm[1]
      && fabs(el.Dxy(&_pv))       < cutLevel.dxy[1]
      && fabs(el.Dz(&_pv))        < cutLevel.dz[1]
      && el.IdMap("fabsEPDiff")         < cutLevel.fabsEPDiff[1]
      && el.ConversionMissHits()        <= cutLevel.ConversionMissHits[1]
      && el.ConversionVeto()            == cutLevel.PassedConversionProb[1]
      )
      ) elPass = true; 
    //   cout<<"evt: "<<eventNumber<<" muon num: "<<recoMuons->GetSize()<<endl;
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
       return elPass;
}

bool ParticleSelector::PassElectronIso(const TCElectron& el, const Cuts::elIsoCuts& cutLevel, float EAEle[7]){
  float thisEA = 0;
  if (fabs(el.Eta())     <  1.0) thisEA = EAEle[0];
  else if (fabs(el.Eta())     <  1.5) thisEA = EAEle[1];
  else if (fabs(el.Eta())     <  2.0) thisEA = EAEle[2];
  else if (fabs(el.Eta())     <  2.2) thisEA = EAEle[3];
  else if (fabs(el.Eta())     <  2.3) thisEA = EAEle[4];
  else if (fabs(el.Eta())     <  2.4) thisEA = EAEle[5];
  else if (fabs(el.Eta())     >  2.4) thisEA = EAEle[6];

  float combIso = (el.IsoMap("pfChIso_R04")
    + max(0.,(double)el.IsoMap("pfNeuIso_R04") + el.IsoMap("pfPhoIso_R04") - _rhoFactor*thisEA));
  bool isoPass = false;
  if (combIso/el.Pt() < cutLevel.relCombIso04) isoPass = true;
  return isoPass;
}

bool ParticleSelector::PassPhotonID(const TCPhoton& ph, const Cuts::phIDCuts& cutLevel){
  float tmpEta;
  bool phoPass = false;
  tmpEta = ph.SCEta();
  if (fabs(tmpEta) > 2.5) return phoPass;
  if (fabs(tmpEta) > 1.4442 && fabs(tmpEta) < 1.566) return phoPass;
  if(
      (fabs(tmpEta)  < 1.4442
       && ph.ConversionVeto()       == cutLevel.PassedEleSafeVeto[0]
       && ph.HadOverEm()               < cutLevel.HadOverEm[0]
       && ph.SigmaIEtaIEta()           < cutLevel.sigmaIetaIeta[0]
      ) ||
      (fabs(tmpEta)  > 1.566
       && ph.ConversionVeto()       == cutLevel.PassedEleSafeVeto[1]
       && ph.HadOverEm()               < cutLevel.HadOverEm[1]
       && ph.SigmaIEtaIEta()           < cutLevel.sigmaIetaIeta[1]
      )
    ) phoPass = true;
  return phoPass;
}

bool ParticleSelector::PassPhotonIso(const TCPhoton& ph, const Cuts::phIsoCuts& cutLevel, float EAPho[7][3]){
  float chEA,nhEA,phEA,chIsoCor,nhIsoCor,phIsoCor,tmpEta;
  bool isoPass = false;
  tmpEta = ph.SCEta();

  if(fabs(tmpEta) > 2.5) return isoPass;

  if (fabs(tmpEta) < 1.0){
    chEA = EAPho[0][0];
    nhEA = EAPho[0][1];
    phEA = EAPho[0][2];
  }else if (fabs(tmpEta) < 1.479){
    chEA = EAPho[1][0];
    nhEA = EAPho[1][1];
    phEA = EAPho[1][2];
  }else if (fabs(tmpEta) < 2.0){
    chEA = EAPho[2][0];
    nhEA = EAPho[2][1];
    phEA = EAPho[2][2];
  }else if (fabs(tmpEta) < 2.2){
    chEA = EAPho[3][0];
    nhEA = EAPho[3][1];
    phEA = EAPho[3][2];
  }else if (fabs(tmpEta) < 2.3){ 
    chEA = EAPho[4][0];
    nhEA = EAPho[4][1];
    phEA = EAPho[4][2];
  }else if (fabs(tmpEta) < 2.4){
    chEA = EAPho[5][0];
    nhEA = EAPho[5][1];
    phEA = EAPho[5][2];
  }else{                                  
    chEA = EAPho[6][0];
    nhEA = EAPho[6][1];
    phEA = EAPho[6][2];
  }

  chIsoCor = ph.IsoMap("chIso03")-_rhoFactor*chEA;
  nhIsoCor = ph.IsoMap("nhIso03")-_rhoFactor*nhEA;
  phIsoCor = ph.IsoMap("phIso03")-_rhoFactor*phEA;

  if (cutLevel.cutName == "loosePhIso"){
    if (
        (fabs(tmpEta) < 1.4442
        //(fabs(ph.Eta())  < 1.566
         && max((double)chIsoCor,0.)          < cutLevel.chIso03[0]
         && max((double)nhIsoCor,0.)          < cutLevel.nhIso03[0] + 0.04*ph.Pt()
         && max((double)phIsoCor,0.)          < cutLevel.phIso03[0] + 0.005*ph.Pt() 
        ) ||
        (fabs(tmpEta) > 1.566
        //(fabs(ph.Eta())  > 1.566
         && max((double)chIsoCor,0.)          < cutLevel.chIso03[1]
         && max((double)nhIsoCor,0.)          < cutLevel.nhIso03[1] + 0.04*ph.Pt() 
         //&& phoCut["phIso03"]/ph.Pt() < nuthin
        )
       ) isoPass = true;
  } else {
    if (
        //(fabs(ph.Eta())  < 1.566
        (fabs(tmpEta) < 1.4442
         && max((double)chIsoCor,0.)          < cutLevel.chIso03[0]
         && max((double)nhIsoCor,0.)          < cutLevel.nhIso03[0] + 0.04*ph.Pt()
         && max((double)phIsoCor,0.)          < cutLevel.phIso03[0] + 0.005*ph.Pt() 
        ) ||
        //(fabs(ph.Eta())  > 1.566
        (fabs(tmpEta) > 1.566
         && max((double)chIsoCor,0.)          < cutLevel.chIso03[1]
         && max((double)nhIsoCor,0.)          < cutLevel.nhIso03[1] + 0.04*ph.Pt() 
         && max((double)phIsoCor,0.)          < cutLevel.phIso03[1] + 0.005*ph.Pt() 
        )
       ) isoPass = true;
  }
  return isoPass;
}

bool ParticleSelector::PassJetID(const TCJet& jet, int nVtx, const Cuts::jetIDCuts& cutLevel){
  bool idPass = false;

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
  return idPass;
}
        
    
float ParticleSelector::Zeppenfeld(const TLorentzVector& p, const TLorentzVector& pj1, const TLorentzVector& pj2)
{
  float zep = p.Eta()-(pj1.Eta()+pj2.Eta())/2.;
  return zep;
}


