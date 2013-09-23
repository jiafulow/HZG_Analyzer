#include "../interface/Dumper.h"

Dumper::Dumper(){
  InitDumps();
}

void Dumper::SetCuts(Cuts* cuts){
  _cuts = cuts;
}

void Dumper::SetRho(float rhoFactor){
  _rhoFactor = rhoFactor;
}

void Dumper::SetPSelect(ParticleSelector* psel){
  _psel = psel;
}

void Dumper::SetRun(int runNumber){
  _runNumber = runNumber;
}

void Dumper::SetEvent(int eventNumber){
  _eventNumber = eventNumber;
}

void Dumper::SetPv(TVector3* pv){
  _pv = pv;
}

void Dumper::SetLumi(int lumiSection){
  _lumiSection = lumiSection;
}

void Dumper::InitDumps(){
  muonDump = false;
  electronDump = false;
  if (parameters::dumps){
    if (parameters::selection == "mumuGamma") muonDump = true;
    if (parameters::selection == "eeGamma") electronDump= true;
  }

  char buffer[512];
  if (electronDump){
    sprintf(buffer, "dumps/electronDump2_%s.txt",parameters::suffix.c_str());
    elDump2.open(buffer);
    if (!elDump2.good()) cout << "ERROR: can't open file for writing." << endl;
    elDump2<<"run"             <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"             <<" "<<"dEtaIn"      <<" "<<"dPhiIn"
      <<" "<<"sigmaIetaIeta"   <<" "<<"HoE"         <<" "<<"ooemoop"
      <<" "<<"conPass"         <<" "<<"mHits"       <<" "<<"d0"
      <<" "<<"dz"              <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"           <<" "<<"combIso"     <<" "<<"rho"
      <<" "<<"EA"              <<" "<<"passID"      <<" "<<"passIso"
      <<endl<<endl;
    sprintf(buffer, "dumps/photonDump1El_%s.txt",parameters::suffix.c_str());
    phDump1.open(buffer);
    if (!phDump1.good()) cout << "ERROR: can't open file for writing." << endl;
    sprintf(buffer, "dumps/photonDump2El_%s.txt",parameters::suffix.c_str());
    phDump2.open(buffer);
    if (!phDump2.good()) cout << "ERROR: can't open file for writing." << endl;

    sprintf(buffer, "dumps/electronDumpFinal_%s.txt",parameters::suffix.c_str());
    elDumpFinal.open(buffer);
    if (!elDumpFinal.good()) cout << "ERROR: can't open file for writing." << endl;
    elDumpFinal<<"run"             <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"             <<" "<<"dEtaIn"      <<" "<<"dPhiIn"
      <<" "<<"sigmaIetaIeta"   <<" "<<"HoE"         <<" "<<"ooemoop"
      <<" "<<"conPass"         <<" "<<"mHits"       <<" "<<"d0"
      <<" "<<"dz"              <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"           <<" "<<"combIso"     <<" "<<"rho"
      <<" "<<"EA"              <<" "<<"passID"      <<" "<<"passIso"
      <<endl<<endl;

    if(parameters::doEleMVA){
      sprintf(buffer, "dumps/electronDumpMVA_%s.txt",parameters::suffix.c_str());
      elDumpMVA.open(buffer);
      if (!elDumpMVA.good()) cout << "ERROR: can't open file for writing." << endl;
    }
  }
  if (muonDump){
    sprintf(buffer, "dumps/muonDump1_%s.txt",parameters::suffix.c_str());
    muDump1.open(buffer);
    if (!muDump1.good()) cout << "ERROR: can't open file for writing." << endl;
    muDump1<<"run"       <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"       <<" "<<"isGlobal"    <<" "<<"isPf"
      <<" "<<"rChi2"     <<" "<<"nHits"       <<" "<<"nMatch"
      <<" "<<"d0"        <<" "<<"dz"          <<" "<<"nPixel"
      <<" "<<"nLayer"    <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"     <<" "<<"combIso"     <<" "<<"dB"
      <<" "<<"passID"    <<" "<<"passIso"
      <<endl<<endl;
    sprintf(buffer, "dumps/photonDump1Mu_%s.txt",parameters::suffix.c_str());
    phDump1.open(buffer);
    if (!phDump1.good()) cout << "ERROR: can't open file for writing." << endl;
    sprintf(buffer, "dumps/photonDump2Mu_%s.txt",parameters::suffix.c_str());
    phDump2.open(buffer);
    if (!phDump2.good()) cout << "ERROR: can't open file for writing." << endl;

    sprintf(buffer, "dumps/muonDumpFinal_%s.txt",parameters::suffix.c_str());
    muDumpFinal.open(buffer);
    if (!muDumpFinal.good()) cout << "ERROR: can't open file for writing." << endl;
    muDumpFinal<<"run"       <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"       <<" "<<"isGlobal"    <<" "<<"isPf"
      <<" "<<"rChi2"     <<" "<<"nHits"       <<" "<<"nMatch"
      <<" "<<"d0"        <<" "<<"dz"          <<" "<<"nPixel"
      <<" "<<"nLayer"    <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"     <<" "<<"combIso"     <<" "<<"dB"
      <<" "<<"passID"    <<" "<<"passIso"
      <<endl<<endl;
  }

  if (parameters::dumps){
    phDump1<<"run"             <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"              <<" "<<"CSElVeto"    <<" "<<"HoE"
      <<" "<<"sigmaIetaIeta"    <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"            <<" "<<"rho"         <<" "<<"chEA"
      <<" "<<"nhEA"             <<" "<<"phEA"        <<" "<<"passID"
      <<" "<<"passIso"
      <<endl<<endl;

    phDump2<<"run"             <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"              <<" "<<"CSElVeto"    <<" "<<"HoE"
      <<" "<<"sigmaIetaIeta"    <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"            <<" "<<"rho"         <<" "<<"chEA"
      <<" "<<"nhEA"             <<" "<<"phEA"        <<" "<<"passID"
      <<" "<<"passIso"          <<" "<<"dr1"         <<" "<<"dr2"
      <<" "<<"pt/Mllg"
      <<endl<<endl;

    sprintf(buffer, "dumps/finalDump_%s_%s_%s.txt",parameters::selection.c_str(),parameters::period.c_str(),parameters::suffix.c_str());
    finalDump.open(buffer);
  }

  if (parameters::dataDumps && parameters::suffix == "DATA"){
    sprintf(buffer, "dumps/dataDump_%s_%s.txt",parameters::dataname.c_str(),parameters::jobCount.c_str());
    dataDump.open(buffer);
    if (!dataDump.good()) cout << "ERROR: can't open file for writing." << endl;
  }
}

void Dumper::ElectronDump(TCElectron *el,TClonesArray* recoMuons, bool final)
{
  if(!electronDump) return;
  float thisEA = 0;
  if (fabs(el->Eta())     <  1.0) thisEA = _cuts->EAEle[0];
  else if (fabs(el->Eta())     <  1.5) thisEA = _cuts->EAEle[1];
  else if (fabs(el->Eta())     <  2.0) thisEA = _cuts->EAEle[2];
  else if (fabs(el->Eta())     <  2.2) thisEA = _cuts->EAEle[3];
  else if (fabs(el->Eta())     <  2.3) thisEA = _cuts->EAEle[4];
  else if (fabs(el->Eta())     <  2.4) thisEA = _cuts->EAEle[5];
  else if (fabs(el->Eta())     >  2.4) thisEA = _cuts->EAEle[6];

  bool idPass = _psel->PassElectronID(el, _cuts->looseElID, recoMuons);
  bool isoPass = _psel->PassElectronIso(el, _cuts->looseElIso, _cuts->EAEle);

  float combIso = (el->IsoMap("pfChIso_R04")
    + max(0.,(double)el->IsoMap("pfNeuIso_R04") + el->IsoMap("pfPhoIso_R04") - _rhoFactor*thisEA));
  ofstream* dump;
  if (final) dump = &elDumpFinal;
  else dump = &elDump2;

  *dump << _runNumber << " "                          << _eventNumber << " "                                 << el->Pt()
       << " "       << el->Eta()                    << " "         << el->DetaSuperCluster()              << " "      << el->DphiSuperCluster()
       << " "       << el->SigmaIEtaIEta()          << " "         << el->HadOverEm()                     << " "      << el->IdMap("fabsEPDiff")
       << " "       << el->ConversionVeto()         << " "         << el->ConversionMissHits()            << " "      << el->Dxy(_pv)
       << " "       << el->Dz(_pv)           << " "         << el->IsoMap("pfChIso_R04")           << " "      << el->IsoMap("pfNeuIso_R04")
       << " "       << el->IsoMap("pfPhoIso_R04")   << " "         << combIso                             << " "      << _rhoFactor
       << " "       << thisEA                       << " "         << idPass                              << " "      << isoPass
       << endl;
}

void Dumper::MuonDump(TCMuon *mu, bool final) 
{
  if(!muonDump) return;

  bool idPass = _psel->PassMuonID(mu, _cuts->tightMuID);
  bool isoPass;
  if (parameters::doLooseMuIso){
    isoPass = _psel->PassMuonIso(mu, _cuts->looseMuIso);
  }else{
    isoPass = _psel->PassMuonIso(mu, _cuts->tightMuIso);
  }

  float combIso; 
  combIso = (mu->IsoMap("pfChargedHadronPt_R04")
    + max(0.,(double)mu->IsoMap("pfNeutralHadronEt_R04") + mu->IsoMap("pfPhotonEt_R04") - 0.5*mu->IsoMap("pfPUPt_R04")));
  ofstream* dump;
  if (final) dump = &muDumpFinal;
  else dump = &muDump1;

  *dump << _runNumber << " "                              << _eventNumber << " "                                 << mu->Pt()
       << " "       << mu->Eta()                        << " "         << mu->IsGLB()                         << " "      << mu->IsPF()
       << " "       << mu->NormalizedChi2()             << " "         << mu->NumberOfValidMuonHits()         << " "      << mu->NumberOfMatchedStations()
       << " "       << mu->Dxy(_pv)              << " "         << mu->Dz(_pv)                  << " "      << mu->NumberOfValidPixelHits()
       << " "       << mu->TrackLayersWithMeasurement() << " "         << mu->IsoMap("pfChargedHadronPt_R04") << " "      << mu->IsoMap("pfNeutralHadronEt_R04")
       << " "       << mu->IsoMap("pfPhotonEt_R04")     << " "         << combIso                             << " "      << mu->IsoMap("pfPUPt_R04")
       << " "         << idPass                              << " "      << isoPass
       << endl;
}

void Dumper::PhotonDump(TCPhoton *ph) 
{
  if(!parameters::dumps) return;
  float chEA,nhEA,phEA,tmpEta;
  tmpEta = ph->SCEta();

  if (fabs(tmpEta) < 1.0){
    chEA = _cuts->EAPho[0][0];
    nhEA = _cuts->EAPho[0][1];
    phEA = _cuts->EAPho[0][2];
  }else if (fabs(tmpEta) < 1.479){
    chEA = _cuts->EAPho[1][0];
    nhEA = _cuts->EAPho[1][1];
    phEA = _cuts->EAPho[1][2];
  }else if (fabs(tmpEta) < 2.0){
    chEA = _cuts->EAPho[2][0];
    nhEA = _cuts->EAPho[2][1];
    phEA = _cuts->EAPho[2][2];
  }else if (fabs(tmpEta) < 2.2){
    chEA = _cuts->EAPho[3][0];
    nhEA = _cuts->EAPho[3][1];
    phEA = _cuts->EAPho[3][2];
  }else if (fabs(tmpEta) < 2.3){ 
    chEA = _cuts->EAPho[4][0];
    nhEA = _cuts->EAPho[4][1];
    phEA = _cuts->EAPho[4][2];
  }else if (fabs(tmpEta) < 2.4){
    chEA = _cuts->EAPho[5][0];
    nhEA = _cuts->EAPho[5][1];
    phEA = _cuts->EAPho[5][2];
  }else{                                  
    chEA = _cuts->EAPho[6][0];
    nhEA = _cuts->EAPho[6][1];
    phEA = _cuts->EAPho[6][2];
  }

  bool idPass = _psel->PassPhotonID(ph, _cuts->mediumPhID);
  bool isoPass = _psel->PassPhotonIso(ph, _cuts->mediumPhIso, _cuts->EAPho);

  phDump1 << _runNumber << " "                   << _eventNumber << " "                   << ph->Pt()
       << " "       << tmpEta                << " "         << ph->ConversionVeto()  << " "      << ph->HadOverEm()
       << " "       << ph->SigmaIEtaIEta()   << " "         << ph->IsoMap("chIso03") << " "      << ph->IsoMap("nhIso03")
       << " "       << ph->IsoMap("phIso03") << " "         << _rhoFactor             << " "      << chEA
       << " "       << nhEA                  << " "         << phEA                  << " "      << idPass
       << " "       << isoPass
       << endl;
}

void Dumper::PhotonDump2(TCPhoton *ph, TLorentzVector lepton1, TLorentzVector lepton2)
{
  if(!parameters::dumps) return;
  float chEA,nhEA,phEA,tmpEta;
  tmpEta = ph->SCEta();

  if (fabs(tmpEta) < 1.0){
    chEA = _cuts->EAPho[0][0];
    nhEA = _cuts->EAPho[0][1];
    phEA = _cuts->EAPho[0][2];
  }else if (fabs(tmpEta) < 1.479){
    chEA = _cuts->EAPho[1][0];
    nhEA = _cuts->EAPho[1][1];
    phEA = _cuts->EAPho[1][2];
  }else if (fabs(tmpEta) < 2.0){
    chEA = _cuts->EAPho[2][0];
    nhEA = _cuts->EAPho[2][1];
    phEA = _cuts->EAPho[2][2];
  }else if (fabs(tmpEta) < 2.2){
    chEA = _cuts->EAPho[3][0];
    nhEA = _cuts->EAPho[3][1];
    phEA = _cuts->EAPho[3][2];
  }else if (fabs(tmpEta) < 2.3){ 
    chEA = _cuts->EAPho[4][0];
    nhEA = _cuts->EAPho[4][1];
    phEA = _cuts->EAPho[4][2];
  }else if (fabs(tmpEta) < 2.4){
    chEA = _cuts->EAPho[5][0];
    nhEA = _cuts->EAPho[5][1];
    phEA = _cuts->EAPho[5][2];
  }else{                                  
    chEA = _cuts->EAPho[6][0];
    nhEA = _cuts->EAPho[6][1];
    phEA = _cuts->EAPho[6][2];
  }

  bool idPass = _psel->PassPhotonID(ph, _cuts->mediumPhID);
  bool isoPass = _psel->PassPhotonIso(ph, _cuts->mediumPhIso, _cuts->EAPho);

  float dr1 = ph->DeltaR(lepton1);
  float dr2 = ph->DeltaR(lepton2);

  float scaledPt = ph->Pt()/(*ph+lepton1+lepton2).M();

  phDump2 << _runNumber << " "                   << _eventNumber << " "                   << ph->Pt()
       << " "       << tmpEta                << " "         << ph->ConversionVeto()  << " "      << ph->HadOverEm()
       << " "       << ph->SigmaIEtaIEta()   << " "         << ph->IsoMap("chIso03") << " "      << ph->IsoMap("nhIso03")
       << " "       << ph->IsoMap("phIso03") << " "         << _rhoFactor             << " "      << chEA
       << " "       << nhEA                  << " "         << phEA                  << " "      << idPass
       << " "       << isoPass               << " "         << dr1                   << " "      << dr2
       << " "       << scaledPt              << endl;
}

void Dumper::DataDumper(TLorentzVector* lepton1, TLorentzVector* lepton2, TLorentzVector* gamma, float R9input, float SCEta, float eta1, float eta2){
  if(!parameters::dataDumps) return;
  int cat = -1;
  bool goodLep = false;
  if (parameters::selection == "mumuGamma"){
    goodLep = (fabs(lepton1->Eta()) < 0.9 || fabs(lepton2->Eta()) < 0.9) && (fabs(lepton1->Eta()) < 2.1 && fabs(lepton2->Eta()) < 2.1);
  }else if (parameters::selection == "eeGamma"){
    goodLep = (fabs(eta1) < 1.4442 && fabs(eta2) < 1.4442);
  }

  if (goodLep && (fabs(SCEta) < 1.4442) ){
    if (R9input>=0.94) cat = 0;
    else cat = 3;
  }else if (!goodLep && (fabs(SCEta) < 1.4442) ){
    cat = 1;
  } else {
    cat = 2;
  }
  float mll = (*lepton1 + *lepton2).M();
  float mllg = (*lepton1 + *lepton2 + *gamma).M();

  dataDump << "cat: "     << setw(2)  << cat         << " run: "   << setw(7)  << _runNumber      << " evt: "   << setw(10) << _eventNumber
       << " lumi: "   << setw(5)  << _lumiSection << " mll: "   << setw(10) << mll            << " phopt: " << setw(10) << gamma->Pt()
       << " mllg: "   << setw(10) << mllg        << " l1eta: " << setw(10) << lepton1->Eta() << " l2eta: " << setw(10) << lepton2->Eta()
       << " pSCeta: " << setw(10) << SCEta       << " r9: "    << setw(10) << R9input        << endl;

}

void Dumper::DataDumper(TLorentzVector* lepton1, TLorentzVector* lepton2, TLorentzVector* gamma, TLorentzVector* uncorLepton1, TLorentzVector* uncorLepton2, TLorentzVector* uncorGamma, float R9input, float SCEta, float eta1, float eta2){
  if(!parameters::dataDumps) return;
  int cat = -1;
  bool goodLep = false;
  if (parameters::selection == "mumuGamma"){
    goodLep = (fabs(lepton1->Eta()) < 0.9 || fabs(lepton2->Eta()) < 0.9) && (fabs(lepton1->Eta()) < 2.1 && fabs(lepton2->Eta()) < 2.1);
  }else if (parameters::selection == "eeGamma"){
    goodLep = (fabs(eta1) < 1.4442 && fabs(eta2) < 1.4442);
  }

  if (goodLep && (fabs(SCEta) < 1.4442) ){
    if (R9input>=0.94) cat = 0;
    else cat = 3;
  }else if (!goodLep && (fabs(SCEta) < 1.4442) ){
    cat = 1;
  } else {
    cat = 2;
  }
  float mll = (*lepton1 + *lepton2).M();
  float mllg = (*lepton1 + *lepton2 + *gamma).M();
  float mllUncor = (*uncorLepton1 + *uncorLepton2).M();
  float mllgUncor = (*uncorLepton1 + *uncorLepton2 + *uncorGamma).M();

  dataDump << "cat: "      << setw(2)  << cat           << " run: "      << setw(7)  << _runNumber           << " evt: "     << setw(10) << _eventNumber
       << " lumi: "    << setw(5)  << _lumiSection   << " mll: "      << setw(10) << mllUncor            << " phopt: "   << setw(10) << uncorGamma->Pt()
       << " mllg: "    << setw(10) << mllgUncor     << " l1eta: "    << setw(10) << uncorLepton1->Eta() << " l2eta: "   << setw(10) << uncorLepton2->Eta()
       << " pSCeta: "  << setw(10) << SCEta

       << " mllCor: "  << setw(10) << mll           << " phoptCor: " << setw(10) << gamma->Pt()         << " l1PtCor: " << setw(10) << lepton1->Pt()
       << " l2PtCor: " << setw(10) << lepton2->Pt() << " mllgCor: "  << setw(10) << mllg
       << " r9: "      << setw(10) << R9input       << endl;
}

void Dumper::FinalDumper(TLorentzVector* lepton1, TLorentzVector* lepton2, TLorentzVector* gamma, int catNum){
  if(!parameters::dumps) return;
  float mll = (*lepton1 + *lepton2).M();
  float mllg = (*lepton1 + *lepton2 + *gamma).M();

  if (parameters::doEleReg){
     finalDump << "cat: "   << setw(2)  << catNum        << " evt: "   << setw(10) << _eventNumber << " l1RegE-P: " << setw(10) << lepton1->E() << " l1pt: " << setw(10) << lepton1->Pt()
         << " l2RegE-P: " << setw(10) << lepton2->E() << " l2pt: " << setw(10) << lepton2->Pt() << " l1eta: " << setw(10) << lepton1->Eta() << " l2eta: " << setw(10) << lepton2->Eta() << " phopt: " << setw(10) << gamma->Pt() << " mll: "  << setw(10) << mll
         << " mllg: " << setw(10) << mllg          << endl;
  }else{
    finalDump << "cat: "   << setw(2)  << catNum        << " evt: "   << setw(10) << _eventNumber << " l1pt: " << setw(10) << lepton1->Pt()
         << " l2pt: " << setw(10) << lepton2->Pt() << " l1eta: " << setw(10) << lepton1->Eta() << " l2eta: " << setw(10) << lepton2->Eta() << " phopt: " << setw(10) << gamma->Pt() << " mll: "  << setw(10) << mll
         << " mllg: " << setw(10) << mllg          << endl;
  }
}

void Dumper::MVADumper(TCElectron* ele, EGammaMvaEleEstimator* mvaMaker)
{
  if(!electronDump || !parameters::doEleMVA) return;
  bool passPreSel = false;
  bool passMVA = false;
  if (ele->IdMap("preSelPassV1")) passPreSel = true;			
  double tmpMVAValue = mvaMaker->mvaValue(
      ele->IdMap("fbrem"),    
      ele->IdMap("kfChi2"),                            
      ele->IdMap("kfNLayers"),                            
      ele->IdMap("gsfChi2"),                           
      ele->IdMap("dEta"),
      ele->IdMap("dPhi"),            
      ele->IdMap("dEtaAtCalo"),
      ele->SigmaIEtaIEta(), 
      ele->IdMap("SigmaIPhiIPhi"),
      ele->IdMap("SCEtaWidth"),
      ele->IdMap("SCPhiWidth"),
      ele->IdMap("ome1x5oe5x5"),
      ele->IdMap("R9"),
      ele->HadOverEm(),
      ele->IdMap("EoP"),
      ele->IdMap("ooemoopV1"), 
      ele->IdMap("eopOut"), 
      ele->IdMap("preShowerORaw"), 
      ele->IdMap("d0"),
      ele->IdMap("ip3d"), 
      ele->SCEta(),
      ele->Pt(),
      false);                

  if (fabs(ele->SCEta()) < 0.8 && tmpMVAValue > 0.5){
    passMVA = true;
  }else if (fabs(ele->SCEta()) > 0.8 && fabs(ele->SCEta()) < 1.479 && tmpMVAValue > 0.12){
    passMVA = true;
  }else if (fabs(ele->SCEta()) > 1.479 && tmpMVAValue > 0.6){
    passMVA = true;
  }

  bool passIso = false;
  passIso = _psel->PassElectronIso(ele, _cuts->looseElIso, _cuts->EAEle);

elDumpMVA << " run: "                   << setw(7)  << _runNumber                            << " evt: "                   << setw(10) << _eventNumber                          << " lumi: "                  << setw(5)  << _lumiSection
     << " pt: "                    << setw(10) << ele->Pt()                            << " SCeta: "                 << setw(10) << ele->SCEta()                         << " fbrem: "                 << setw(10) << ele->IdMap("fbrem")
     << " kfChi2: "                << setw(10) << ele->IdMap("kfChi2")                 << " kfHits: "                << setw(10) << ele->IdMap("kfNLayers")              << " gsfChi2: "               << setw(10) << ele->IdMap("gsfChi2")
     << " dEta: "                  << setw(10) << ele->IdMap("dEta")                   << " dPhi: "                  << setw(10) << ele->IdMap("dPhi")                   << " dEtaCalo: "              << setw(10) << ele->IdMap("dEtaAtCalo")
     << " sieie: "                 << setw(10) << ele->SigmaIEtaIEta()                 << " sipip: "                 << setw(10) << ele->IdMap("SigmaIPhiIPhi")          << " etaWidth: "              << setw(10) << ele->IdMap("SCEtaWidth")
     << " phiWidth: "              << setw(10) << ele->IdMap("SCPhiWidth")             << " ome1x5oe5x5: "           << setw(10) << ele->IdMap("ome1x5oe5x5")            << " R9: "                    << setw(10) << ele->IdMap("R9")
     << " HoE: "                   << setw(10) << ele->HadOverEm()                     << " EoP: "                   << setw(10) << ele->IdMap("EoP")                    << " ooemoop: "               << setw(10) << ele->IdMap("ooemoopV1")
     << " EoPout: "                << setw(10) << ele->IdMap("eopOut")                 << " preShowerOverRaw: "      << setw(10) << ele->IdMap("preShowerORaw")          << " d0: "                    << setw(10) << ele->IdMap("d0")
     << " ip3d: "                  << setw(10) << ele->IdMap("ip3d")                   << " ChargedIso_DR0p0To0p1: " << setw(10) << ele->IsoMap("ChargedIso_DR0p0To0p1") << " ChargedIso_DR0p1To0p2: " << setw(10) << ele->IsoMap("ChargedIso_DR0p1To0p2")
     << " ChargedIso_DR0p2To0p3: " << setw(10) << ele->IsoMap("ChargedIso_DR0p2To0p3") << " ChargedIso_DR0p3To0p4: " << setw(10) << ele->IsoMap("ChargedIso_DR0p3To0p4") << " ChargedIso_DR0p4To0p5: " << setw(10) << ele->IsoMap("ChargedIso_DR0p4To0p5")
     << " GammaIso_DR0p0To0p1: "   << setw(10) << ele->IsoMap("GammaIso_DR0p0To0p1")   << " GammaIso_DR0p1To0p2: "   << setw(10) << ele->IsoMap("GammaIso_DR0p1To0p2")   << " GammaIso_DR0p2To0p3: "   << setw(10) << ele->IsoMap("GammaIso_DR0p2To0p3")
     << " GammaIso_DR0p3To0p4: "   << setw(10) << ele->IsoMap("GammaIso_DR0p3To0p4")   << " GammaIso_DR0p4To0p5: "   << setw(10) << ele->IsoMap("GammaIso_DR0p4To0p5")
     << " NeutralHadronIso_DR0p0To0p1: " << setw(10) << ele->IsoMap("NeutralHadronIso_DR0p0To0p1") << " NeutralHadronIso_DR0p1To0p2: " << setw(10) << ele->IsoMap("NeutralHadronIso_DR0p1To0p2")
     << " NeutralHadronIso_DR0p2To0p3: " << setw(10) << ele->IsoMap("NeutralHadronIso_DR0p2To0p3") << " NeutralHadronIso_DR0p3To0p4: " << setw(10) << ele->IsoMap("NeutralHadronIso_DR0p3To0p4")
     << " NeutralHadronIso_DR0p4To0p5: " << setw(10) << ele->IsoMap("NeutralHadronIso_DR0p4To0p5") << " _rhoFactor: "                   << setw(10) << _rhoFactor
     << " mva Value: " << setw(10) << tmpMVAValue << " pass PreSel: " << setw(5) << passPreSel << " pass MVA: " << setw(5) << passMVA << " pass ISO: " << setw(5) << passIso
     << endl;
}


void Dumper::CloseDumps(){
  if(electronDump){
    elDump2.close();
    elDumpFinal.close();
  }
  if(electronDump && parameters::doEleMVA){
    elDumpMVA.close();
  }
  if(muonDump){
    muDump1.close();
    muDumpFinal.close();
  }
  if(parameters::dumps){
    phDump1.close();
    phDump2.close();
    finalDump.close();
  }
  if(parameters::dataDumps && parameters::suffix == "DATA"){
    dataDump.close();
  }
}
