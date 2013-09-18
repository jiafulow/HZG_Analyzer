#include "../interface/ParticleSelectors.h"

ParticleSelector::ParticleSelector(Cuts* cuts, bool isRealData, int runNumber, TRandom3* rEl){
  _cuts = cuts;
  _isRealData = isRealData;
  _runNumber = runNumber;
  _rEl = rEl;
}

bool ParticleSelector::FindGoodZElectron(vector<TCElectron*> electronList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* ZP4,float* eta1, float* eta2, int* int1, int* int2){
  TLorentzVector tmpZ;
  bool goodZ = false;
  float ZmassDiff=99999;
  for(unsigned int i =0; i<electronList.size(); i++){
    if (electronList[i]->Pt() > _cuts->leadElePt){
      for(unsigned int j =1; j<electronList.size(); j++){
        if (electronList[j]->Pt() > _cuts->trailElePt && electronList[j]->Charge() != electronList[i]->Charge()){
          goodZ = true;
          tmpZ = (*electronList[i]+*electronList[j]);
          if(fabs(91.1876-tmpZ.M()) < ZmassDiff){
            *ZP4 = (*electronList[i]+*electronList[j]);
            *lepton1 = *electronList[i];
            *int1 = i;
            *lepton2 = *electronList[j];
            *int2 = j;
            *eta1 = electronList[i]->SCEta();
            *eta2 = electronList[j]->SCEta();
            ZmassDiff = fabs(91.1876-tmpZ.M());
          }
        }
      }
    }
  }
  return goodZ;
}
          
bool ParticleSelector::FindGoodZElectron(vector<TCElectron*> electronList, vector<TCElectron*> uncorElectronList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* uncorLepton1, TLorentzVector* uncorLepton2, TLorentzVector* ZP4, float* eta1, float* eta2, int* int1, int* int2){
  TLorentzVector tmpZ;
  bool goodZ = false;
  float ZmassDiff=99999;
  for(unsigned int i =0; i<electronList.size(); i++){
    if (electronList[i]->Pt() > _cuts->leadElePt){
      for(unsigned int j =1; j<electronList.size(); j++){
        if (electronList[j]->Pt() > _cuts->trailElePt && electronList[j]->Charge() != electronList[i]->Charge()){
          goodZ = true;
          /// do regression
          if (parameters::doEleReg && parameters::period.find("2012") != string::npos){
            if (electronList[i]->RegressionMomCombP4().E() != 0){
              electronList[i]->SetPtEtaPhiE(electronList[i]->RegressionMomCombP4().E()/cosh(electronList[i]->RegressionMomCombP4().Eta()),electronList[i]->RegressionMomCombP4().Eta(),electronList[i]->RegressionMomCombP4().Phi(),electronList[i]->RegressionMomCombP4().E());
              //electronList[i]->SetPtEtaPhiE(electronList[i]->RegressionMomCombP4().Pt(),electronList[i]->RegressionMomCombP4().Eta(),electronList[i]->RegressionMomCombP4().Phi(),electronList[i]->RegressionMomCombP4().E());
            }
            if (electronList[j]->RegressionMomCombP4().E() != 0){
              electronList[j]->SetPtEtaPhiE(electronList[j]->RegressionMomCombP4().E()/cosh(electronList[j]->RegressionMomCombP4().Eta()),electronList[j]->RegressionMomCombP4().Eta(),electronList[j]->RegressionMomCombP4().Phi(),electronList[j]->RegressionMomCombP4().E());
              //electronList[j]->SetPtEtaPhiE(electronList[j]->RegressionMomCombP4().Pt(),electronList[j]->RegressionMomCombP4().Eta(),electronList[j]->RegressionMomCombP4().Phi(),electronList[j]->RegressionMomCombP4().E());
            }
          }
          /// do eng cor
          if(parameters::engCor){
            float energyElCorI;
            float energyElCorJ;
            if ( parameters::period.find("2011") != string::npos ){
              energyElCorI = correctedElectronEnergy( electronList[i]->E(), electronList[i]->SCEta(), electronList[i]->R9(), _runNumber, 0, "2011", !_isRealData, _rEl );
              energyElCorJ = correctedElectronEnergy( electronList[j]->E(), electronList[j]->SCEta(), electronList[j]->R9(), _runNumber, 0, "2011", !_isRealData, _rEl );
            }else{
              if(parameters::doEleReg && electronList[i]->RegressionMomCombP4().E() != 0){
                energyElCorI = correctedElectronEnergy( electronList[i]->E(), electronList[i]->SCEta(), electronList[i]->R9(), _runNumber, 1, "Moriond2013", !_isRealData, _rEl );
              }else{
                energyElCorI = correctedElectronEnergy( electronList[i]->E(), electronList[i]->SCEta(), electronList[i]->R9(), _runNumber, 0, "HCP2012", !_isRealData, _rEl );
              }
              if(parameters::doEleReg && electronList[j]->RegressionMomCombP4().E() != 0){
                energyElCorJ = correctedElectronEnergy( electronList[j]->E(), electronList[j]->SCEta(), electronList[j]->R9(), _runNumber, 1, "Moriond2013", !_isRealData, _rEl );
              }else{
                energyElCorJ = correctedElectronEnergy( electronList[j]->E(), electronList[j]->SCEta(), electronList[j]->R9(), _runNumber, 0, "HCP2012", !_isRealData, _rEl );
              }
            }
            //float newPtI = sqrt((pow(energyElCorI,2)-pow(0.000511,2))/pow(cosh(electronList[i]->Eta()),2));
            //float newPtJ = sqrt((pow(energyElCorJ,2)-pow(0.000511,2))/pow(cosh(electronList[j]->Eta()),2));
            float newPtI = energyElCorI/cosh(electronList[i]->Eta());
            float newPtJ = energyElCorJ/cosh(electronList[j]->Eta());
            electronList[i]->SetPtEtaPhiM(newPtI,electronList[i]->Eta(),electronList[i]->Phi(),0.000511);
            electronList[j]->SetPtEtaPhiM(newPtJ,electronList[j]->Eta(),electronList[j]->Phi(),0.000511);
          }
          tmpZ = (*electronList[i]+*electronList[j]);
          if(fabs(91.1876-tmpZ.M()) < ZmassDiff){
            *ZP4 = (*electronList[i]+*electronList[j]);
            *lepton1 = *electronList[i];
            *lepton2 = *electronList[j];
            *int1 = i;
            *int2 = j;
            *eta1 = electronList[i]->SCEta();
            *eta2 = electronList[j]->SCEta();
            *uncorLepton1 = *uncorElectronList[i];
            *uncorLepton2 = *uncorElectronList[j];
            ZmassDiff = fabs(91.1876-tmpZ.M());
          }
        }
      }
    }
  }
  return goodZ;
}
          
bool ParticleSelector::FindGoodZMuon(vector<TCMuon*> muonList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* ZP4, int* int1, int* int2){
  TLorentzVector tmpZ;
  bool goodZ = false;
  float ZmassDiff=99999;
  for(unsigned int i =0; i<muonList.size(); i++){
    if (muonList[i]->Pt() > _cuts->leadMuPt){
      for(unsigned int j =1; j<muonList.size(); j++){
        if (muonList[j]->Pt() > _cuts->trailMuPt && muonList[j]->Charge() != muonList[i]->Charge()){
          goodZ = true;
          tmpZ = (*muonList[i]+*muonList[j]);
          if(fabs(91.1876-tmpZ.M()) < ZmassDiff){
            *ZP4 = (*muonList[i]+*muonList[j]);
            *lepton1 = *muonList[i];
            *lepton2 = *muonList[j];
            *int1 = i;
            *int2 = j;
            ZmassDiff = fabs(91.1876-tmpZ.M());
          }
        }
      }
    }
  }
  return goodZ;
}

bool ParticleSelector::FindGoodZMuon(vector<TCMuon*> muonList, vector<TCMuon*> uncorMuonList, TCPhysObject* lepton1, TCPhysObject* lepton2, TLorentzVector* uncorLepton1, TLorentzVector* uncorLepton2, TLorentzVector* ZP4, int* int1, int* int2){
  TLorentzVector tmpZ;
  bool goodZ = false;
  float ZmassDiff=99999;
  for(unsigned int i =0; i<muonList.size(); i++){
    if (muonList[i]->Pt() > _cuts->leadMuPt){
      for(unsigned int j =1; j<muonList.size(); j++){
        if (muonList[j]->Pt() > _cuts->trailMuPt && muonList[j]->Charge() != muonList[i]->Charge()){
          goodZ = true;
          tmpZ = (*muonList[i]+*muonList[j]);
          if(fabs(91.1876-tmpZ.M()) < ZmassDiff){
            *ZP4 = (*muonList[i]+*muonList[j]);
            *lepton1 = *muonList[i];
            *lepton2 = *muonList[j];
            *uncorLepton1 = *uncorMuonList[i];
            *uncorLepton2 = *uncorMuonList[j];
            *int1 = i;
            *int2 = j;
            ZmassDiff = fabs(91.1876-tmpZ.M());
          }
        }
      }
    }
  }
  return goodZ;
}
