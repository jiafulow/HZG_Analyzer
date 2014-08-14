#define higgsAnalyzer_cxx

#include "higgsAnalyzer.h"

using namespace std;
string  str(int i) {return static_cast<ostringstream*>( &(ostringstream() << i) )->str();}


void higgsAnalyzer::Begin(TTree * tree)
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
  string PU = static_cast<string>(static_cast<TObjString*>(option.Tokenize(" ")->At(6))->GetString()); 

  params.reset(new Parameters());

  params->selection      = selection;
  params->period         = period;
  params->JC_LVL         = 0;
  params->abcd           = abcd;
  params->suffix         = suffix;
  params->dataname       = dataname;
  params->jobCount       = count;
  params->PU             = PU;

  // change any params from default

  //params->doPhoMVA       = false; //FOR PROPER
  params->doAnglesMVA    = false; //FOR PROPER or No KinMVA

  //params->doSync         = true;
  //params->dumps          = true;
  //params->EVENTNUMBER    = 54768;

  //params->doAltMVA         = true; //FOR MVA OPT TEST
  //params->DYGammaVeto      = false;
  //params->doLooseMuIso     = false;
  //params->doEleMVA         = false;
  
  //params->doLeptonPrune    = false;
  params->doVBF              = false;

  // Initialize utilities and selectors here //
  int jobNum = atoi(params->jobCount.c_str());
  cout<<jobNum<<endl;
  cuts.reset(new Cuts());
  cuts->InitEA(params->period);
  
  // Change any cuts from non-default values

  cuts->zgMassHigh = 999.0;
  //cuts->gPt = 40.0;
  //cuts->trailElePt = 20.0;
  




  for (int i =0; i<100; i++){
    nEvents[i] = 0;
  }
  for (int i =0; i<2; i++){
    genAccept[i] = 0;
  }

  // Get trigger names from jobTree
  vector<string>* triggerNames = 0;
  TFile   *inFile         = tree->GetCurrentFile();
  TTree   *jobTree        = (TTree*)inFile->Get("ntupleProducer/jobTree");

  jobTree->SetBranchAddress("triggerNames", &triggerNames);
  jobTree->GetEntry();


  // More plugin init

  weighter.reset(new WeightUtils(*params, isRealData, runNumber));
  triggerSelector.reset(new TriggerSelector(params->selection, params->period, *triggerNames));
  rmcor2011.reset(new rochcor_2011(229+100*jobNum));
  rmcor2012.reset(new rochcor2012(229+100*jobNum));
  rMuRun.reset(new TRandom3(231+100*jobNum));
  phoCorrector.reset(new zgamma::PhosphorCorrectionFunctor("../plugins/PHOSPHOR_NUMBERS_EXPFIT_ERRORS.txt", true));
  Xcal2.reset(new TEvtProb);
  particleSelector.reset(new ParticleSelector(*params, *cuts, isRealData, runNumber));
  dumper.reset(new Dumper(*params,*cuts,*particleSelector));


  // Random numbers! //
  rnGenerator.reset(new TRandom3);

  TH1::SetDefaultSumw2(kTRUE);
  TH2::SetDefaultSumw2(kTRUE);

  histoFile.reset(new TFile(("higgsHistograms_"+params->dataname+"_"+params->selection+"_"+params->jobCount+".root").c_str(), "RECREATE"));
  trainingFile.reset(new TFile(("higgsTraining_"+params->dataname+"_"+params->selection+"_"+params->jobCount+".root").c_str(), "RECREATE"));
  sampleFile.reset(new TFile(("higgsSample_"+params->dataname+"_"+params->selection+"_"+params->jobCount+".root").c_str(), "RECREATE"));
  m_llgFile.reset(new TFile(("m_llgFile_"+params->dataname+"_"+params->selection+"_"+params->jobCount+".root").c_str(),"RECREATE"));

  trainingFile->cd();
  trainingChain = new TTree("varMVA","for training the MVA");
  sampleFile->cd();
  sampleChain.reset(new TTree("varMVA","for testing the MVA"));

  m_llgFile->cd();
  m_llgChain.reset(new TTree(("m_llg_"+params->suffix).c_str(),"three body mass values"));

  initializeEfficiencyWeights( "otherHistos/elsf2011.root", "otherHistos/elsf2012.root");

  histoFile->cd();
  hm.reset(new HistManager(histoFile.get()));
  histoFile->mkdir("Misc", "Misc");
  histoFile->mkdir("Lepton", "Lepton");
  histoFile->mkdir("Jet", "Jet");
  histoFile->mkdir("TESTS", "TESTS");
  histoFile->mkdir("ZGamma", "ZGamma");
  histoFile->mkdir("pT-Eta-Phi", "pT-Eta-Phi");
  histoFile->mkdir("Vtx", "Vtx");
  histoFile->mkdir("PreSelDiLep", "PreSelDiLep");
  histoFile->mkdir("PreSelThreeBody", "PreSelThreeBody");
  histoFile->mkdir("PreSelDiLepNoW", "PreSelDiLepNoW");
  histoFile->mkdir("PreSelThreeBodyNoW", "PreSelThreeBodyNoW");
  histoFile->mkdir("PhotonIsoDR04", "PhotonIsoDR04");
  histoFile->mkdir("MuonIso", "MuonIso");
  histoFile->mkdir("GenLvl", "GenLvl");
  histoFile->mkdir("GenPlots", "GenPlots");
  histoFile->mkdir("Jet-Photons", "Jet-Photons");
  histoFile->mkdir("CAT1", "CAT1");
  histoFile->mkdir("CAT2", "CAT2");
  histoFile->mkdir("CAT3", "CAT3");
  histoFile->mkdir("CAT4", "CAT4");
  histoFile->mkdir("CAT5", "CAT5");
  histoFile->mkdir("CAT6", "CAT6");
  histoFile->mkdir("CAT7", "CAT7");
  histoFile->mkdir("CAT8", "CAT8");
  histoFile->mkdir("CAT9", "CAT9");
  histoFile->mkdir("PreGen", "PreGen");
  histoFile->mkdir("PostGen", "PostGen");
  histoFile->mkdir("ZGAngles", "ZGAngles");
  histoFile->mkdir("ZGAngles_RECO", "ZGAngles_RECO");
  histoFile->mkdir("PhotonPurity", "PhotonPurity");
  histoFile->mkdir("MEPlots", "MEPlots");
  histoFile->mkdir("MVAPlots", "MVAPlots");
  histoFile->mkdir("HighMass", "HighMass");
  //histoFile->mkdir("FakeRateWeight", "FakeRateWeight");

  diffZGscalar = diffZGvector = threeBodyMass = threeBodyPt = divPt = cosZ = cosG = METdivQt = GPt = ZPt = DPhi = diffPlaneMVA = vtxVariable = dr1 = dr2 = M12 = medisc = smallTheta = bigTheta = comPhi = GPtOM = diffZGvectorOM = threeBodyPtOM = ZPtOM = GEta = ZEta = threeBodyEta = GPtOHPt = l1Eta = l2Eta = R9var = sieip = sipip = SCRawE = SCPSE = e5x5 = e2x2 = SCPSEOPt = SCRawEOPt = e2x2O5x5 = scaleFactor = -99999;

  sampleChain->Branch("diffZGscalar",&diffZGscalar,"diffZGscalar/F");
  trainingChain->Branch("diffZGscalar",&diffZGscalar,"diffZGscalar/F");
  sampleChain->Branch("diffZGvector",&diffZGvector,"diffZGvector/F");
  trainingChain->Branch("diffZGvector",&diffZGvector,"diffZGvector/F");
  sampleChain->Branch("threeBodyMass",&threeBodyMass,"threeBodyMass/F");
  trainingChain->Branch("threeBodyMass",&threeBodyMass,"threeBodyMass/F");
  sampleChain->Branch("threeBodyPt",&threeBodyPt,"threeBodyPt/F");
  trainingChain->Branch("threeBodyPt",&threeBodyPt,"threeBodyPt/F");
  sampleChain->Branch("divPt",&divPt,"divPt/F");
  trainingChain->Branch("divPt",&divPt,"divPt/F");
  sampleChain->Branch("cosZ",&cosZ,"cosZ/F");
  trainingChain->Branch("cosZ",&cosZ,"cosZ/F");
  sampleChain->Branch("cosG",&cosG,"cosG/F");
  trainingChain->Branch("cosG",&cosG,"cosG/F");
  sampleChain->Branch("METdivQt",&METdivQt,"METdivQt/F");
  trainingChain->Branch("METdivQt",&METdivQt,"METdivQt/F");
  sampleChain->Branch("GPt",&GPt,"GPt/F");
  trainingChain->Branch("GPt",&GPt,"GPt/F");
  sampleChain->Branch("ZPt",&ZPt,"ZPt/F");
  trainingChain->Branch("ZPt",&ZPt,"ZPt/F");
  sampleChain->Branch("DPhi",&DPhi,"DPhi/F");
  trainingChain->Branch("DPhi",&DPhi,"DPhi/F");
  sampleChain->Branch("diffPlaneMVA",&diffPlaneMVA,"diffPlaneMVA/F");
  trainingChain->Branch("diffPlaneMVA",&diffPlaneMVA,"diffPlaneMVA/F");
  sampleChain->Branch("vtxVariable",&vtxVariable,"vtxVariable/F");
  trainingChain->Branch("vtxVariable",&vtxVariable,"vtxVariable/F");
  sampleChain->Branch("dr1",&dr1,"dr1/F");
  trainingChain->Branch("dr1",&dr1,"dr1/F");
  sampleChain->Branch("dr2",&dr2,"dr2/F");
  trainingChain->Branch("dr2",&dr2,"dr2/F");
  sampleChain->Branch("M12",&M12,"M12/F");
  trainingChain->Branch("M12",&M12,"M12/F");
  sampleChain->Branch("medisc",&medisc,"medisc/F");
  trainingChain->Branch("medisc",&medisc,"medisc/F");
  sampleChain->Branch("smallTheta",&smallTheta,"smallTheta/F");
  trainingChain->Branch("smallTheta",&smallTheta,"smallTheta/F");
  sampleChain->Branch("bigTheta",&bigTheta,"bigTheta/F");
  trainingChain->Branch("bigTheta",&bigTheta,"bigTheta/F");
  sampleChain->Branch("comPhi",&comPhi,"comPhi/F");
  trainingChain->Branch("comPhi",&comPhi,"comPhi/F");
  sampleChain->Branch("GPtOM",&GPtOM,"GPtOM/F");
  trainingChain->Branch("GPtOM",&GPtOM,"GPtOM/F");
  sampleChain->Branch("diffZGvectorOM",&diffZGvectorOM,"diffZGvectorOM/F");
  trainingChain->Branch("diffZGvectorOM",&diffZGvectorOM,"diffZGvectorOM/F");
  sampleChain->Branch("threeBodyPtOM",&threeBodyPtOM,"threeBodyPtOM/F");
  trainingChain->Branch("threeBodyPtOM",&threeBodyPtOM,"threeBodyPtOM/F");
  sampleChain->Branch("ZPtOM",&ZPtOM,"ZPtOM/F");
  trainingChain->Branch("ZPtOM",&ZPtOM,"ZPtOM/F");
  sampleChain->Branch("GEta",&GEta,"GEta/F");
  trainingChain->Branch("GEta",&GEta,"GEta/F");
  sampleChain->Branch("ZEta",&ZEta,"ZEta/F");
  trainingChain->Branch("ZEta",&ZEta,"ZEta/F");
  sampleChain->Branch("l1Eta",&l1Eta,"l1Eta/F");
  trainingChain->Branch("l1Eta",&l1Eta,"l1Eta/F");
  sampleChain->Branch("l2Eta",&l2Eta,"l2Eta/F");
  trainingChain->Branch("l2Eta",&l2Eta,"l2Eta/F");
  sampleChain->Branch("threeBodyEta",&threeBodyEta,"threeBodyEta/F");
  trainingChain->Branch("threeBodyEta",&threeBodyEta,"threeBodyEta/F");
  sampleChain->Branch("GPtOHPt",&GPtOHPt,"GPtOHPt/F");
  trainingChain->Branch("GPtOHPt",&GPtOHPt,"GPtOHPt/F");
  sampleChain->Branch("R9var",&R9var,"R9var/F");
  trainingChain->Branch("R9var",&R9var,"R9var/F");
  sampleChain->Branch("sieip",&sieip,"sieip/F");
  trainingChain->Branch("sieip",&sieip,"sieip/F");
  sampleChain->Branch("sipip",&sipip,"sipip/F");
  trainingChain->Branch("sipip",&sipip,"sipip/F");
  sampleChain->Branch("SCRawE",&SCRawE,"SCRawE/F");
  trainingChain->Branch("SCRawE",&SCRawE,"SCRawE/F");
  sampleChain->Branch("SCRawEOPt",&SCRawEOPt,"SCRawEOPt/F");
  trainingChain->Branch("SCRawEOPt",&SCRawEOPt,"SCRawEOPt/F");
  sampleChain->Branch("SCPSE",&SCPSE,"SCPSE/F");
  trainingChain->Branch("SCPSE",&SCPSE,"SCPSE/F");
  sampleChain->Branch("SCPSEOPt",&SCPSEOPt,"SCPSEOPt/F");
  trainingChain->Branch("SCPSEOPt",&SCPSEOPt,"SCPSEOPt/F");
  sampleChain->Branch("e5x5",&e5x5,"e5x5/F");
  trainingChain->Branch("e5x5",&e5x5,"e5x5/F");
  sampleChain->Branch("e2x2",&e2x2,"e2x2/F");
  trainingChain->Branch("e2x2",&e2x2,"e2x2/F");
  sampleChain->Branch("e2x2O5x5",&e2x2O5x5,"e2x2O5x5/F");
  trainingChain->Branch("e2x2O5x5",&e2x2O5x5,"e2x2O5x5/F");

  sampleChain->Branch("scaleFactor",&scaleFactor,"scaleFactor/F");
  trainingChain->Branch("scaleFactor",&scaleFactor,"scaleFactor/F");

  m_llgChain->Branch(("m_llg_"+params->suffix).c_str(), &m_llg, "m_llg/D");
  m_llgChain->Branch(("m_llgCAT1_"+params->suffix).c_str(), &m_llgCAT1, "m_llgCAT1/D");
  m_llgChain->Branch(("m_llgCAT2_"+params->suffix).c_str(), &m_llgCAT2, "m_llgCAT2/D");
  m_llgChain->Branch(("m_llgCAT3_"+params->suffix).c_str(), &m_llgCAT3, "m_llgCAT3/D");
  m_llgChain->Branch(("m_llgCAT4_"+params->suffix).c_str(), &m_llgCAT4, "m_llgCAT4/D");
  m_llgChain->Branch(("m_llgCAT6_"+params->suffix).c_str(), &m_llgCAT6, "m_llgCAT6/D");
  m_llgChain->Branch(("m_llgCAT7_"+params->suffix).c_str(), &m_llgCAT7, "m_llgCAT7/D");
  m_llgChain->Branch(("m_llgCAT8_"+params->suffix).c_str(), &m_llgCAT8, "m_llgCAT8/D");
  m_llgChain->Branch(("m_llgCAT9_"+params->suffix).c_str(), &m_llgCAT9, "m_llgCAT9/D");
  m_llgChain->Branch(("m_llgCAT5_"+params->suffix).c_str(), &m_llgCAT5, "m_llgCAT5/D");
  m_llgChain->Branch(("unBinnedWeight_"+params->suffix).c_str(), &unBinnedWeight, "unBinnedWeight/D");
  m_llgChain->Branch(("unBinnedLumiXS_"+params->suffix).c_str(), &unBinnedLumiXS, "unBinnedLumiXS/D");
  //m_llgChain->Branch(("unskimmedEventsTotal_"+params->suffix).c_str(), &unskimmedEventsTotal, "unskimmedEventsTotal/I");

  ///////////////////////
  // ZGAngles MVA init //
  ///////////////////////

  // TMVA weights directory
  if(params->doAltMVA){
    mvaInits.weightsDir = "../mva/testWeights2/";
  }else{
    mvaInits.weightsDir = "../mva/testWeights/";
  }

  // here we will use only BDTG... but will keep the structure 
  mvaInits.discrMethodName[0] = "MLPBNN";
  mvaInits.discrMethodName[1] = "BDTG";
  mvaInits.discrMethodName[2] = "BDT";

  mvaInits.discrSelection = params->selection;
  mvaInits.discrSampleName = "allBG";
  //mvaInits.discrSuffixName = "anglesOnly";
  //mvaInits.discrSuffixName = "newAnglesR9";
  //mvaInits.discrSuffixName = "01-29-14_v0905";
  if (params->selection == "mumuGamma"){ 
    mvaInits.discrSuffixName = "07-6-14_PhoMVA";
  }else{
    mvaInits.discrSuffixName = "07-24-14_PhoMVA";
  }


  mvaInits.mvaHiggsMassPoint[0] = 123;

  mvaInits.bdtCut[0] = -0.1;
  tmvaReader = MVAInitializer();

  if (params->doSync) cout<<"WARNING: Sync Mode"<<endl;

}



Bool_t higgsAnalyzer::Process(Long64_t entry)
{

  GetEntry(entry,1);
  // skip event if event number investigator is being used 
  if(params->EVENTNUMBER != -999 && params->EVENTNUMBER != eventNumber) return kTRUE;
  if(eventNumber == params->EVENTNUMBER) cout<<params->EVENTNUMBER<<endl;

  // 2011 bad electron run veto
  if(params->selection == "eeGamma" && isRealData && params->period.find("2011") != string::npos){
    if( (runNumber == 171050 && (lumiSection >= 47 && lumiSection <= 92 )) ||
        (runNumber == 171156 && (lumiSection >= 42 && lumiSection <= 211)) ||
        (runNumber == 171219 && (lumiSection >= 48 && lumiSection <= 163)) ||
        (runNumber == 171274 && (lumiSection >= 88 && lumiSection <= 148)) ||
        (runNumber == 171282 && (lumiSection >= 1  && lumiSection <= 172)) ||
        (runNumber == 171315 && (lumiSection >= 53 && lumiSection <= 226)) ||
        (runNumber == 171369 && (lumiSection >= 42 && lumiSection <= 162)) ||
        (runNumber == 171446 && (lumiSection >= 58 && lumiSection <= 374)) ||
        (runNumber == 171484 && (lumiSection >= 61 && lumiSection <= 358)) ||
        (runNumber == 171578 && (lumiSection >= 47 && lumiSection <= 347))
      ) return kTRUE;
  }
    

  hm->fill1DHist(1,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  hm->fill1DHist(1,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[0];
  //if (nEvents[0] > 10) Abort("quick terminate");

  if (nEvents[0] == 1) weighter->SetIsRealData(isRealData);
  weighter->SetRunNumber(runNumber);

  if (nEvents[0] % (int)1e5 == 0) cout<<nEvents[17]<<" events passed of "<<nEvents[0]<<" checked!"<<endl;

  m_llg = m_llgCAT1 = m_llgCAT2 = m_llgCAT3 = m_llgCAT4 = m_llgCAT5 = m_llgCAT6 = m_llgCAT7 = m_llgCAT8 = m_llgCAT9 =  -1;
  unBinnedWeight = unBinnedLumiXS = 1;

  particleSelector->SetRho(rhoFactor);
  particleSelector->SetEventNumber(eventNumber);
  dumper->SetRho(rhoFactor);
  dumper->SetRun(runNumber);
  dumper->SetEvent(eventNumber);
  dumper->SetLumi(lumiSection);

  ///////////////////
  // Gen Particles //
  ///////////////////
  

  vector<TCGenParticle> genPhotons;
  vector<TCGenParticle> genMuons;
  vector<TCGenParticle> genZs;
  particleSelector->CleanUpGen(genHZG);
  bool vetoDY = false;
  //genHZG = {0,0,0,0,0,0};
  if(!isRealData){
    ///////// load all the relevent particles into a struct /////////
    particleSelector->FindGenParticles(*genParticles, *recoPhotons, genPhotons, genMuons, genZs, genHZG, vetoDY);
    if (params->DYGammaVeto && (params->suffix.find("DY") != string::npos)){
      if (vetoDY) return kTRUE;
    }

    if (genPhotons.size() > 0 && genMuons.size() > 1 && abs(genPhotons[0].MotherId())<30 && genPhotons[0].GetStatus() == 1){
      hm->fill1DHist(genPhotons[0].Pt(),"h1_genPhotonPt_"+params->suffix, "pT_{#gamma}; pT_{#gamma};N_{evts}", 100, 0., 100., 1,"GenPlots");
      hm->fill1DHist(genMuons[0].Pt(),"h1_genLeptonPt_"+params->suffix, "pT_{l}; pT_{l};N_{evts}", 100, 0., 100., 1,"GenPlots");
      hm->fill1DHist(genMuons[1].Pt(),"h1_genLeptonPt_"+params->suffix, "pT_{l}; pT_{l};N_{evts}", 100, 0., 100., 1,"GenPlots");
      hm->fill1DHist(genPhotons[0].DeltaR(genMuons[0]),"h1_genPhotonLeptonDr_"+params->suffix, "dR(#gamma,l); dR(#gamma,l);N_{evts}", 300, 0., 3., 1,"GenPlots");
      hm->fill1DHist(genPhotons[0].DeltaR(genMuons[1]),"h1_genPhotonLeptonDr_"+params->suffix, "dR(#gamma,l); dR(#gamma,l);N_{evts}", 300, 0., 3., 1,"GenPlots");
    }


    ///////// whzh decomposition /////////////////

    if (params->suffix.find("zh") != string::npos && genHZG.w) return kTRUE;
    if (params->suffix.find("wh") != string::npos && !genHZG.w) return kTRUE;
    unskimmedEventsTotalSpecial += 1; 


    ///////// gen angles, plots before any kinematic/fiducial cleaning //////////////

    ZGLabVectors genLevelInputs;
    ZGAngles     genLevelOutputs;


    if(genHZG.lp && genHZG.lm && genHZG.g){
      if(genHZG.lp->Pt() > 0 && genHZG.lm->Pt() > 0){

        if (genHZG.lp->Pt() > genHZG.lm->Pt()){
          StandardPlots(*genHZG.lp,*genHZG.lm,*genHZG.g,1,"PreGen", "PreGen");
        }else{
          StandardPlots(*genHZG.lm,*genHZG.lp,*genHZG.g,1,"PreGen", "PreGen");
        }
        //genHZG.lp->Print();
        //genHZG.lm->Print();
        //genHZG.g->Print();
        genLevelInputs.veczg = *genHZG.lp+*genHZG.lm+*genHZG.g;
        //cout<<"higgsMass: "<<genLevelInputs.veczg.M()<<endl;
        genLevelInputs.vecz = *genHZG.lp+*genHZG.lm; 
        genLevelInputs.vecg = *genHZG.g;

        genLevelInputs.veclp = *genHZG.lp;
        genLevelInputs.veclm = *genHZG.lm;

        getZGAngles(genLevelInputs,genLevelOutputs, false);
        AnglePlots(genLevelOutputs,1,"ZGAngles","GEN");
      }
    }

    //////////// gen yields with basic kinematic cuts ////////////

    if(
        (params->selection == "mumuGamma" && genHZG.lm && genHZG.lp)
        && ((genHZG.lp->Pt() > cuts->leadMuPt && genHZG.lm->Pt() > cuts->trailMuPt) || (genHZG.lp->Pt() > cuts->trailMuPt && genHZG.lm->Pt() > cuts->leadMuPt))
        && fabs(genHZG.lp->Eta())   < 2.4
        && fabs(genHZG.lm->Eta())   < 2.4

      ){
      ++genAccept[0]; //cout<<"event passes fuck yeah!"<<endl;
    }
    if(
        (params->selection == "eeGamma" && genHZG.lm && genHZG.lp)
        && ((genHZG.lp->Pt() > cuts->leadElePt && genHZG.lm->Pt() > cuts->trailElePt) || (genHZG.lp->Pt() > cuts->trailElePt && genHZG.lm->Pt() > cuts->leadElePt))
        && fabs(genHZG.lp->Eta())   < 2.4
        && fabs(genHZG.lm->Eta())   < 2.4
      ){
      ++genAccept[0]; //cout<<"event passes fuck yeah!"<<endl;
    }
    if(
        (params->selection == "mumuGamma" && genHZG.lm && genHZG.lp && genHZG.g)
        && ((genHZG.lp->Pt() > cuts->leadMuPt && genHZG.lm->Pt() > cuts->trailMuPt) || (genHZG.lp->Pt() > cuts->trailMuPt && genHZG.lm->Pt() > cuts->leadMuPt))

        && fabs(genHZG.lp->Eta())   < 2.4
        && fabs(genHZG.lm->Eta())   < 2.4

        && (*genHZG.lp+*genHZG.lm).M()                    > cuts->zMassLow
        && (*genHZG.lp+*genHZG.lm).M()                    < cuts->zMassHigh
        && genHZG.lp->DeltaR(*genHZG.g)                   > cuts->dR
        && genHZG.lm->DeltaR(*genHZG.g)                   > cuts->dR
        && (*genHZG.lp+*genHZG.lm+*genHZG.g).M()              > cuts->zgMassLow 
        && (*genHZG.lp+*genHZG.lm+*genHZG.g).M()              < cuts->zgMassHigh
        && genHZG.g->Pt()/((*genHZG.lp+*genHZG.lm+*genHZG.g).M()) > cuts->gPtOverMass
        && fabs(genHZG.g->Eta())                      < 2.5
        && (fabs(genHZG.g->Eta())                     < 1.4442 || fabs(genHZG.g->Eta()) > 1.566)

      ){
      ++genAccept[1]; //cout<<"event passes fuck yeah!"<<endl;
      if (genHZG.lp->Pt() > genHZG.lm->Pt()){
        StandardPlots(*genHZG.lp,*genHZG.lm,*genHZG.g,1,"PostGen", "PostGen");
      }else{
        StandardPlots(*genHZG.lm,*genHZG.lp,*genHZG.g,1,"PostGen", "PostGen");
      }

    }
    if(
        (params->selection == "eeGamma" && genHZG.lm && genHZG.lp && genHZG.g)
        && ((genHZG.lp->Pt() > cuts->leadElePt && genHZG.lm->Pt() > cuts->trailElePt) || (genHZG.lp->Pt() > cuts->trailElePt && genHZG.lm->Pt() > cuts->leadElePt))

        && fabs(genHZG.lp->Eta())   < 2.4
        && fabs(genHZG.lm->Eta())   < 2.4

        && (*genHZG.lp+*genHZG.lm).M()                    > cuts->zMassLow
        && (*genHZG.lp+*genHZG.lm).M()                    < cuts->zMassHigh
        && genHZG.lp->DeltaR(*genHZG.g)                   > cuts->dR
        && genHZG.lm->DeltaR(*genHZG.g)                   > cuts->dR
        && (*genHZG.lp+*genHZG.lm+*genHZG.g).M()              > cuts->zgMassLow
        && (*genHZG.lp+*genHZG.lm+*genHZG.g).M()              < cuts->zgMassHigh
        && genHZG.g->Pt()/((*genHZG.lp+*genHZG.lm+*genHZG.g).M()) > cuts->gPtOverMass
        && fabs(genHZG.g->Eta())                      < 2.5
        && (fabs(genHZG.g->Eta())                     < 1.4442 || fabs(genHZG.g->Eta()) > 1.566)

      ){
      ++genAccept[1]; //cout<<"event passes fuck yeah!"<<endl;
      if (genHZG.lp->Pt() > genHZG.lm->Pt()){
        StandardPlots(*genHZG.lp,*genHZG.lm,*genHZG.g,1,"PostGen", "PostGen");
      }else{
        StandardPlots(*genHZG.lm,*genHZG.lp,*genHZG.g,1,"PostGen", "PostGen");
      }
    }
  }

  hm->fill1DHist(2,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  hm->fill1DHist(2,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  if (!isRealData) hm->fill1DHist(nPUVerticesTrue,"h1_simVertexMultTrueUltraFine_"+params->suffix, "Multiplicity of simulated vertices true", 500, 0, 100,1,"Misc");
  if (!isRealData && params->period.find("2012") != string::npos ){
    if(runNumber > 190456 && runNumber < 196531) hm->fill1DHist(nPUVerticesTrue,"h1_simVertexMultTrueRunAB_"+params->suffix, "Multiplicity of simulated vertices true", 500, 0, 100,1,"Misc");
    if(runNumber > 198022 && runNumber < 203742) hm->fill1DHist(nPUVerticesTrue,"h1_simVertexMultTrueRunC_"+params->suffix, "Multiplicity of simulated vertices true", 500, 0, 100,1,"Misc");
    if(runNumber > 203777 && runNumber < 208686) hm->fill1DHist(nPUVerticesTrue,"h1_simVertexMultTrueRunD_"+params->suffix, "Multiplicity of simulated vertices true", 500, 0, 100,1,"Misc");
  }
  ++nEvents[1];



  //////////////////
  //Trigger status//
  //////////////////

  for(int i = 0; i < 64; ++i) {
    unsigned long iHLT = 0x0; 
    iHLT = 0x01 << i;  
    if ((triggerStatus & iHLT) == iHLT) hm->fill1DHist(i+1,"h1_triggerStatus_"+params->suffix, "Triggers", 64, 0.5, 64.5,1,"Misc");  
  } 

  bool triggerPass   = triggerSelector->SelectTrigger(triggerStatus, hltPrescale);
  //cout<<"triggerStatus: "<<triggerStatus<<" hltPrescale: "<<hltPrescale<<" triggerPass: "<<triggerPass<<endl;
  //int  eventPrescale = triggerSelector->GetEventPrescale();
  //cout<<eventPrescale<<endl;

  if (!triggerPass) return kTRUE;
  hm->fill1DHist(3,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  hm->fill1DHist(3,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[2];




  ////////////////////////////
  //Check the event vertices//
  ////////////////////////////


  if (!isRealData) hm->fill1DHist(nPUVertices,"h1_simVertexMultStoyan_"+params->suffix, "Multiplicity of simulated vertices", 60, 0, 60, 1, "Misc");
  if (!isRealData) hm->fill1DHist(nPUVertices,"h1_simVertexMultPoter_"+params->suffix, "Multiplicity of simulated vertices", 100, 0, 100, 1, "Misc");
  if (!isRealData) hm->fill1DHist(nPUVertices,"h1_simVertexMult_"+params->suffix, "Multiplicity of simulated vertices", 50, -0.5, 49.5,1,"Misc");
  if (!isRealData) hm->fill1DHist(nPUVerticesTrue,"h1_simVertexMultTrue_"+params->suffix, "Multiplicity of simulated vertices true", 50, -0.5, 49.5,1,"Misc");
  if (!isRealData) hm->fill1DHist(nPUVerticesTrue,"h1_simVertexMultTrueFine_"+params->suffix, "Multiplicity of simulated vertices true", 200, 0, 100,1,"Misc");


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
  hm->fill1DHist(4,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  hm->fill1DHist(4,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[3];

  pvPosition.reset(new TVector3());
  *pvPosition = goodVertices[0];
  particleSelector->SetPv(*pvPosition);
  dumper->SetPv(*pvPosition);



  //////////////////
  // Data quality //
  //////////////////

  //if (isRealData && (isDeadEcalCluster || isScraping || isCSCTightHalo)) return kTRUE;
  //if (isRealData && (isNoiseHcal || isDeadEcalCluster || isScraping || isCSCTightHalo)) return kTRUE;
  hm->fill1DHist(5,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  hm->fill1DHist(5,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[4];


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
  
      if (params->engCor && !params->doSync && cuts->zgMassHigh != 999.0) UniversalEnergyCorrector(*thisPhoton, genPhotons);

      if (passID) photonsID.push_back(*thisPhoton);
      if (passID && passIso) photonsIDIso.push_back(*thisPhoton);

      dumper->PhotonDump(*thisPhoton, 1); 

    }
    //cout<<"debug0"<<endl;
    //return kTRUE;
    sort(photonsID.begin(), photonsID.end(), P4SortCondition);
    sort(photonsIDIso.begin(), photonsIDIso.end(), P4SortCondition);
  }

  if (electronsID.size() >0 && photonsID.size() > 0){
    for (UInt_t i = 0; i < electronsID.size(); i++) {
      hm->fill1DHist(electronsID[i].PfIsoPhoton()/electronsID[i].Pt(),"h1_elePfPhoIsoOelePt_"+params->suffix, "ID'd electron pf photon iso/ electron pt; pt ratio; N_{evts}", 40, 0, 2, 1,"Misc");
      for (UInt_t j = 0; j < photonsID.size(); j++) {
        if (electronsID[i].DeltaR(photonsID[j])<0.4 ){
          hm->fill1DHist(electronsID[i].PfIsoPhoton()/photonsID[j].Pt(),"h1_elePfPhoIsoOphoPt_"+params->suffix, "ID'd electron pf photon iso/ photon pt; pt ratio; N_{evts}", 40, 0, 2, 1,"Misc");
        }
      }
    }
    for (UInt_t i = 0; i < photonsID.size(); i++) {
      hm->fill1DHist(photonsID[i].PfIsoPhoton()/photonsID[i].Pt(),"h1_phoPfPhoIsoOphoPt_"+params->suffix, "ID'd photon pf photon iso/ photon pt; pt ratio; N_{evts}", 40, 0, 2, 1,"Misc");
    }
  }




  //////////
  // Jets //
  //////////

  vector<TCJet> jetsID;
  for (Int_t i = 0; i < recoJets->GetSize(); ++i) {
    TCJet* thisJet = (TCJet*) recoJets->At(i);
    if (particleSelector->PassJetID(*thisJet, primaryVtx->GetSize(), cuts->vbfJetID)) jetsID.push_back(*thisJet);
  }
  sort(jetsID.begin(), jetsID.end(), P4SortCondition);

  ////////////////////////
  // Analysis selection //
  ////////////////////////

  TLorentzVector ZP4;
  TLorentzVector ZP4Gen;
  TCPhoton GP4;
  TLorentzVector GP4Gen;
  TLorentzVector JetOne, JetTwo;
  float GP4scEta = -9999;
  float eventWeight = 1;
  float eventWeightPU = 1;
  float eventWeightPho = 1;
  float eventWeightLep = 1;
  float eventWeightTrig = 1;

  //float deltaPhiJetMET = 2*TMath::Pi();
  //if (jetP4.size() > 0) deltaPhiJetMET= DeltaPhiJetMET(metP4, jetP4, eventWeight);


  TCPhysObject   lepton1;
  TCPhysObject   lepton2;
  int            lepton1int =-1;
  int            lepton2int =-1;
  TLorentzVector uncorLepton1;
  TLorentzVector uncorLepton2;

  if (params->selection == "electron" || params->selection == "eeGamma") {

    //////////////////////
    // 2 good electrons //
    //////////////////////
    if(params->doLeptonPrune){
      if (params->suffix.find("zh") != string::npos || params->suffix.find("wh") != string::npos || params->suffix.find("tth") != string::npos){
        if(electronsIDIso.size() > 0) BadLeptonPrune(electronsIDIso, genHZG);
      }
    }

    if (electronsID.size() < 2) return kTRUE;

    bool firstEl = false;
    bool bothEls = false;
    for (UInt_t i = 0; i<electronsID.size(); i++){
      if (!firstEl && (electronsID[i].Pt() >cuts->trailElePt)){
        firstEl = true;
      }else if (firstEl && (electronsID[i].Pt() >cuts->trailElePt)){
        bothEls = true;
        break;
      }
    }
    if (!bothEls) return kTRUE;

    hm->fill1DHist(6,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
    hm->fill1DHist(6,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
    ++nEvents[5];

    if (electronsIDIso.size() < 2) return kTRUE;
    firstEl = false;
    bothEls = false;
    for (UInt_t i = 0; i<electronsIDIso.size(); i++){
      if (!firstEl && (electronsIDIso[i].Pt() >cuts->trailElePt)){
        firstEl = true;
      }else if (firstEl && (electronsIDIso[i].Pt() >cuts->trailElePt)){
        bothEls = true;
        break;
      }
    }
    if (!bothEls) return kTRUE;






    //////////////////
    // 2 good muons //
    //////////////////

  } else if (params->selection == "muon" || params->selection =="mumuGamma") {
    if (eventNumber == params->EVENTNUMBER) cout<<"two ID muons?: "<<muonsID.size()<<endl;

    if(params->doLeptonPrune){
      if (params->suffix.find("zh") != string::npos || params->suffix.find("wh") != string::npos || params->suffix.find("tth") != string::npos){
        if(muonsIDIso.size() > 0) BadLeptonPrune(muonsIDIso, genHZG);
      }
    }

    if (muonsID.size() < 2) return kTRUE;

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

    hm->fill1DHist(6,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
    hm->fill1DHist(6,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
    ++nEvents[5];

    if (eventNumber == params->EVENTNUMBER) cout<<"two ISO muons?: "<<muonsIDIso.size()<<endl;
    if (muonsIDIso.size() < 2) return kTRUE;
    firstMu = false;
    bothMus = false;
    for (UInt_t i = 0; i<muonsIDIso.size(); i++){
      if (!firstMu && (muonsIDIso[i].Pt() >cuts->trailMuPt)){
        firstMu = true;
      }else if (firstMu && (muonsIDIso[i].Pt() >cuts->trailMuPt)){
        bothMus = true;
        break;
      }
    }
    if (!bothMus) return kTRUE;

  }

  if (params->period.find("2011") != string::npos){
    if (params->doScaleFactors) eventWeight   *= weighter->PUWeight(nPUVertices);
    eventWeightPU   *= weighter->PUWeight(nPUVertices);
  }else if (params->period.find("2012") != string::npos){
    if (params->doScaleFactors) eventWeight   *= weighter->PUWeight(nPUVerticesTrue);
    eventWeightPU   *= weighter->PUWeight(nPUVerticesTrue);
  }


  ///////////////////////
  // Cosmics rejection //
  ///////////////////////

  //if (selection == "mumuGamma"){
  //if (muons.size() > 1 && CosmicMuonFilter(muons[0], muons[1])) return kTRUE;
  //}
  hm->fill1DHist(7,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(7,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[6];


  //////////////////////
  // Good Z selection //
  //////////////////////

  bool goodZ = false;
  float SCetaEl1 = -99999;
  float SCetaEl2 = -99999;
  if(params->selection == "eeGamma"){
    if (eventNumber == params->EVENTNUMBER) cout<<goodZ<<endl;
    goodZ = particleSelector->FindGoodZElectron(electronsIDIso,lepton1,lepton2,ZP4,SCetaEl1,SCetaEl2,lepton1int,lepton2int);
    if (eventNumber == params->EVENTNUMBER) cout<<"el num: "<<electronsIDIso.size()<<" goodZ: "<<goodZ<<endl;
    if (!goodZ) return kTRUE;
    if (!isRealData){ 
      if (params->doScaleFactors){
        eventWeight   *= weighter->ElectronTriggerWeight(lepton1, lepton2, true);
        if (params->period.find("2011") != string::npos){
          eventWeight   *= getEfficiencyWeight( &lepton1, CorrectionType::CENTRAL, atoi(params->period.c_str()));
          eventWeight   *= getEfficiencyWeight( &lepton2, CorrectionType::CENTRAL, atoi(params->period.c_str()));
        }else if (params->period.find("2012") != string::npos){
          eventWeight   *= weighter->ElectronSelectionWeight(lepton1);
          eventWeight   *= weighter->ElectronSelectionWeight(lepton2);
        }
      }
      eventWeightTrig   *= weighter->ElectronTriggerWeight(lepton1, lepton2, true);
      if (params->period.find("2011") != string::npos){
        eventWeightLep   *= getEfficiencyWeight( &lepton1, CorrectionType::CENTRAL, atoi(params->period.c_str()));
        eventWeightLep   *= getEfficiencyWeight( &lepton2, CorrectionType::CENTRAL, atoi(params->period.c_str()));
      }else if (params->period.find("2012") != string::npos){
        eventWeightLep   *= weighter->ElectronSelectionWeight(lepton1);
        eventWeightLep   *= weighter->ElectronSelectionWeight(lepton2);

      }

    }
    //cout<<eventWeight<<endl;
  }else{
    goodZ = particleSelector->FindGoodZMuon(muonsIDIso,lepton1,lepton2,ZP4,lepton1int,lepton2int, 91.1876);
    //goodZ = particleSelector->FindGoodZMuon(muonsIDIso,lepton1,lepton2,ZP4,lepton1int,lepton2int, 10);
    if (eventNumber == params->EVENTNUMBER) cout<<"goodZ?: "<<goodZ<<endl;
    if (!goodZ) return kTRUE;
    if (eventNumber == params->EVENTNUMBER) cout<<"goodZ?: "<<goodZ<<endl;
    if (!isRealData){ 
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
  }

  hm->fill1DHist(8,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(8,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[7];


  // new place for energy corrections:

  /*
  if(params->engCor && !params->doSync){
    if (params->selection == "eeGamma"){
      UniversalEnergyCorrector(electronsIDIso[lepton1int]);
      UniversalEnergyCorrector(electronsIDIso[lepton2int]);
      lepton1 = electronsIDIso[lepton1int];
      lepton2 = electronsIDIso[lepton2int];
      ZP4 = lepton1+lepton2;
    }else{
      UniversalEnergyCorrector(muonsIDIso[lepton1int]);
      UniversalEnergyCorrector(muonsIDIso[lepton2int]);
      lepton1 = muonsIDIso[lepton1int];
      lepton2 = muonsIDIso[lepton2int];
      ZP4 = lepton1+lepton2;
    }
  }
  */


  ////////////
  // Z mass //
  ////////////

  if (eventNumber == params->EVENTNUMBER) cout<<ZP4.M()<<endl;
  if ((ZP4.M() < cuts->zMassLow || ZP4.M() > cuts->zMassHigh)) return kTRUE;  
  //if ((ZP4.M() < 30 || ZP4.M() > cuts->zMassHigh)) return kTRUE;  
  hm->fill1DHist(9,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(9,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[8];



  ///////////////////////////////////////////////////////
  // lepton, Z plots directly after diLepton selection //
  ///////////////////////////////////////////////////////

  StandardPlots(lepton1,lepton2,eventWeight,"PreSelDiLep", "PreSelDiLep");
  StandardPlots(lepton1,lepton2,1,"PreSelDiLepNoW", "PreSelDiLepNoW");

  ////////////////////////////
  //**ZGamma** Gamma Energy //
  ////////////////////////////


  bool goodPhoton = false; 

  if (photonsIDIso.size() < 1) return kTRUE;
  goodPhoton = particleSelector->FindGoodPhoton(photonsIDIso, GP4, lepton1, lepton2, GP4scEta);
  if(!goodPhoton) return kTRUE;

  if (params->doScaleFactors){
      eventWeight   *= weighter->GammaSelectionWeight(GP4, GP4scEta);
      //if (params->suffix == "DYJets") eventWeight   *= weighter->PhotonFakeWeight(GP4.Eta(), GP4.Pt()); 
  }
  eventWeightPho   *= weighter->GammaSelectionWeight(GP4, GP4scEta);

  // new place for energy corrections:
  //if (params->engCor && !params->doSync) UniversalEnergyCorrector(GP4, genPhotons);


  hm->fill1DHist(10,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(10,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[9];

  //////////////////////////////////////////////////////////////////////
  // lepton, photon, Z, 3body plots directly after diLepton selection //
  //////////////////////////////////////////////////////////////////////

  StandardPlots(lepton1,lepton2,GP4,eventWeight,"PreSelThreeBody", "PreSelThreeBody");
  StandardPlots(lepton1,lepton2,GP4,1,"PreSelThreeBodyNoW", "PreSelThreeBodyNoW");

  ///////////////////////////////
  //**ZGamma** Photon DR Stuff //
  ///////////////////////////////

  if(params->selection == "mumuGamma" || params->selection == "eeGamma")
  {
    // Make an FSR plot for 1337 normalization of the ZGToMuMuG set

    if (ZP4.M() > cuts->zMassLow && (GP4.DeltaR(lepton1)<cuts->dR || GP4.DeltaR(lepton2)<cuts->dR)){
      hm->fill1DHist(ZP4.M(),"h1_diLeptonMassPreSelFSR_"+params->suffix, "M_{ll}; M_{ll};N_{evts}", 30, 50., 80.,eventWeight,"PreSelThreeBody");
    }

  }
  hm->fill1DHist(11,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(11,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[10];

  if (params->dumps){
    if (params->selection == "mumuGamma"){
      dumper->MuonDump(muonsIDIso[lepton1int],2);
      dumper->MuonDump(muonsIDIso[lepton2int],2);

    } else if (params->selection == "eeGamma"){
      dumper->ElectronDump(electronsIDIso[lepton1int],*recoMuons,2);
      dumper->ElectronDump(electronsIDIso[lepton2int],*recoMuons,2);
    }
    dumper->PhotonDump(GP4, 2);
  }

  /////////////////////
  // 3rd lepton veto //
  /////////////////////

  hm->fill1DHist(12,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(12,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[11];

  ////////////////
  // b-jet veto //
  ////////////////

  hm->fill1DHist(13,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(13,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[12];

  /////////
  // MET //
  /////////

  hm->fill1DHist(14,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(14,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[13];

  ////////////
  // MET/QT //
  ////////////

  hm->fill1DHist(15,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(15,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[14];

  //////////////////////
  // Jet Multiplicity //
  //////////////////////

  hm->fill1DHist(16,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(16,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[15];

  //////////////////////////
  //**ZGamma**    MZG Low //
  //////////////////////////

  if (params->selection == "mumuGamma"|| params->selection == "eeGamma" || params->selection == "gammaJets")  if ((ZP4+GP4).M() < cuts->zgMassLow) return kTRUE;
  if (params->doScaleFactors) eventWeight *= weighter->HqtWeight((ZP4+GP4));
  hm->fill1DHist(17,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(17,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[16];

  ///////////////////////////
  //**ZGamma**    MZG High //
  ///////////////////////////

  if (params->selection == "mumuGamma"|| params->selection == "eeGamma" || params->selection == "gammaJets")  if ((ZP4+GP4).M() > cuts->zgMassHigh  ) return kTRUE;
  hm->fill1DHist(18,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(18,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[17];

  //////////////////////////
  //**ZGamma** MZ+MZG Cut //
  //////////////////////////

  if(params->selection == "mumuGamma" || params->selection == "eeGamma") if(ZP4.M()+(ZP4+GP4).M() < cuts->mzPmzg) return kTRUE;
  hm->fill1DHist(19,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(19,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[18];


  //////////////////
  // Angular Cuts //
  //////////////////

  //if (fabs(cospolarZBoost) > AngCut) return kTRUE;
  hm->fill1DHist(20,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(20,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[19];

  //////////////
  // M12 Cuts //
  //////////////

  //if (CalculateM12sqrd(ZP4,GP4) > M12Cut) return kTRUE;
  hm->fill1DHist(21,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(21,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[20];



  ////////////////
  // MVA Angles //
  ////////////////


  hm->fill1DHist(22,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(22,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[21];

  //////////////
  // VBF Cuts //
  //////////////
  // VBF determination
  bool isVBF = false;

  if (jetsID.size() > 1){
    TCJet jet1, jet2;
    isVBF = particleSelector->FindGoodDiJets(jetsID, lepton1, lepton2, GP4, jet1, jet2);
  }


  hm->fill1DHist(23,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(23,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[22];

  /////////////////////
  // ME and MVA Disc //
  /////////////////////

  bool goodLep = false;
  if (params->selection == "mumuGamma"){
    goodLep = GoodLeptonsCat(lepton1, lepton2);
  }else{
    goodLep = GoodLeptonsCat(SCetaEl1, SCetaEl2);
  }
  // If lep1 or lep2 is in 0.9 and both are in 2.1 //
  if (isVBF){
    catNum = 5;
  }else if (goodLep && (fabs(GP4scEta) < 1.4442) ){
    if (GP4.R9()>= 0.94){
      catNum = 1;
    }else{
      catNum = 2;
    }
  } else if ( fabs(GP4scEta) < 1.4442){
    catNum = 3;
  } else {
    catNum = 4;
  }
  
  float MEdisc = MEDiscriminator(lepton1,lepton2,GP4);
  //if (MEdisc < cuts->ME) return kTRUE;
  /*
  if (catNum ==1){
    if (MEdisc < 0.018) return kTRUE;
  }else if (catNum==2){
    if (MEdisc < 0.02) return kTRUE;
  }else if (catNum==3){
    if (MEdisc < 0.0) return kTRUE;
  }else if (catNum==4){
    if (MEdisc < 0.029) return kTRUE;
  }
  */

  ZGLabVectors recoLevelInputs;
  ZGAngles     recoLevelOutputs;


  recoLevelInputs.veczg = lepton1+lepton2+GP4;
  recoLevelInputs.vecz = lepton1+lepton2;
  recoLevelInputs.vecg = GP4;

  if (lepton1.Charge() == 1){
    recoLevelInputs.veclp = lepton1;
    recoLevelInputs.veclm = lepton2;
  }else{
    recoLevelInputs.veclp = lepton2;
    recoLevelInputs.veclm = lepton1;
  }

  getZGAngles(recoLevelInputs,recoLevelOutputs, false);


  // MVA Variables

  diffZGscalar    = ZP4.Pt()-GP4.Pt();
  diffZGvector    = (ZP4-GP4).Pt();
  threeBodyMass   = (ZP4+GP4).M();
  threeBodyPt     = (ZP4+GP4).Pt();
  divPt           = ZP4.Pt()/GP4.Pt();
  GPt             = GP4.Pt();
  ZPt             = ZP4.Pt();
  DPhi            = fabs(ZP4.DeltaPhi(GP4));
  dr1             = lepton1.DeltaR(GP4);
  dr2             = lepton2.DeltaR(GP4);
  M12             = CalculateM12sqrd(ZP4,GP4);
  medisc          = MEdisc;
  smallTheta      = recoLevelOutputs.costheta_lp;
  bigTheta        = recoLevelOutputs.cosTheta;
  comPhi          = recoLevelOutputs.phi;
  GPtOM           = GP4.Pt()/(ZP4+GP4).M();
  diffZGvectorOM  = (ZP4-GP4).Pt()/(ZP4+GP4).M();
  threeBodyPtOM   = (ZP4+GP4).Pt()/(ZP4+GP4).M();
  ZPtOM           = ZP4.Pt()/ZP4.M();
  GEta            = GP4.Eta();
  l1Eta           = lepton1.Eta();
  l2Eta           = lepton2.Eta();
  ZEta            = ZP4.Eta();
  threeBodyEta    = (GP4+ZP4).Eta();
  GPtOHPt         = GP4.Pt()/(GP4+ZP4).Pt();
  R9var           = GP4.R9();
  sieip           = GP4.SigmaIEtaIPhi();
  sipip           = GP4.SigmaIPhiIPhi();
  SCRawE          = GP4.SCRawEnergy();
  SCPSE           = GP4.SCPSEnergy();
  SCRawEOPt       = GP4.SCRawEnergy()/GP4.Pt();
  SCPSEOPt        = GP4.SCPSEnergy()/GP4.Pt();
  e5x5            = GP4.E5x5();
  e2x2            = GP4.E2x2();
  e2x2O5x5        = GP4.E2x2()/GP4.E5x5();
  scaleFactor     = eventWeight;
  if (params->suffix.find("ggM123") != string::npos) scaleFactor *= 19.672/(unskimmedEventsTotal/(19.52*0.00154*0.10098*1000));
  if (params->suffix == "DYJets") scaleFactor *= 19.672/(unskimmedEventsTotal/(3503.71*1000));
  if (params->suffix == "DYToMuMu") scaleFactor *= 19.672/(unskimmedEventsTotal/(1966.7*1000));
  if (params->suffix == "DYToEE") scaleFactor *= 19.672/(unskimmedEventsTotal/(1966.7*1000));
  if (params->suffix == "ZGToLLG") scaleFactor *= 19.672/(unskimmedEventsTotal/(156.2*1000)); 
  //if (params->suffix == "ZGEE") scaleFactor *= 4.98/7.11;
  //if (params->suffix == "ZZJets") scaleFactor *= 4.98/6175;
  //if (params->suffix == "WZJets") scaleFactor *= 4.98/1426.5;
  //if (params->suffix == "WWJets") scaleFactor *= 4.98/250.4;

  mvaVars._medisc = medisc;
  mvaVars._smallTheta = smallTheta;
  mvaVars._bigTheta = bigTheta;
  mvaVars._comPhi = comPhi;
  mvaVars._GPtOM = GPtOM;
  mvaVars._diffZGvectorOM = diffZGvectorOM;
  mvaVars._threeBodyPtOM = threeBodyPtOM;
  mvaVars._ZPtOM = ZPtOM;
  mvaVars._GEta = GEta;
  mvaVars._ZEta = ZEta;
  mvaVars._l1Eta = l1Eta;
  mvaVars._l2Eta = l2Eta;
  mvaVars._threeBodyEta = threeBodyEta;
  mvaVars._GPtOHPt = GPtOHPt;
  mvaVars._threeBodyMass = threeBodyMass;
  mvaVars._R9 = R9var;
  mvaVars._sieip = sieip;
  mvaVars._sipip = sipip;
  mvaVars._SCRawE = SCRawE;
  mvaVars._SCPSE = SCPSE;
  mvaVars._e5x5 = e5x5;
  mvaVars._e2x2 = e2x2;
  mvaVars._e2x2O5x5 = e2x2/e5x5;
  mvaVars._SCRawEOPt = SCRawE/GP4.Pt();
  mvaVars._SCPSEOPt = SCPSE/GP4.Pt();

  float mvaVal = -99;
  mvaVal = MVACalculator(mvaInits, tmvaReader);
  hm->fill2DHist((GP4+ZP4).M(),mvaVal,"h2_MassVsMVACAT"+str(catNum)+"_"+params->suffix,"Mass vs MVA output (BTDG); m_{ll#gamma}; MVA Disc", 90,100,190,90,-1,1,eventWeight,"MVAPlots");
  hm->fill2DHist((GP4+ZP4).M(),mvaVal,"h2_MassVsMVA_"+params->suffix,"Mass vs MVA output (BTDG); m_{ll#gamma}; MVA Disc", 90,100,190,90,-1,1,eventWeight,"MVAPlots");
    
  if (params->doAnglesMVA){
    //with photon mva (good with seperation)
    if (params->selection == "mumuGamma"){
      if (catNum ==1){
        if (mvaVal < -0.089) catNum = 6;
      }else if (catNum==2){
        if (mvaVal < -0.044) catNum = 7; 
      }else if (catNum==3){
        if (mvaVal < -0.24) catNum = 8;
      }else if (catNum==4){
        if (mvaVal < -0.33) catNum = 9;
      }
    }else if (params->selection == "eeGamma"){
      if (catNum ==1){
        if (mvaVal < 0.089) catNum = 6;
      }else if (catNum==2){
        if (mvaVal < 0.089) catNum = 7;
      }else if (catNum==3){
        if (mvaVal < -0.24) catNum = 8;
      }else if (catNum==4){
        if (mvaVal < -0.31) catNum = 9;
      }
    }
   

  }

  MVAPlots(mvaVars,mvaVal,eventWeight,"CAT"+str(catNum)+"", "CAT"+str(catNum)+"");
  MVAPlots(mvaVars,mvaVal,eventWeight, "", "MVAPlots");

  hm->fill2DHist((GP4+ZP4).M(),MEdisc,"h2_MassVsME_"+params->suffix,"Mass vs ME; m_{ll#gamma}; ME Disc", 90,100,190,90,0,0.2,eventWeight,"MEPlots");
  hm->fill1DHist(MEdisc,"h1_ME_"+params->suffix,"ME Disc;ME Disc;Entries", 45,0,0.2,eventWeight,"MEPlots");

  hm->fill1DHist(24,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(24,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[23];



  //////////////
  // Non-Cuts //
  //////////////



  // Cross Section Weighting //
  LumiXSWeight(&unBinnedLumiXS);
  if(params->doLumiXS) eventWeight *= unBinnedLumiXS;

  if ( (GP4+ZP4).M() > 100){
    hm->fill1DHist(50,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,eventWeight,"Misc");
    hm->fill1DHist(50,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
    hm->fill1DHist(51,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,eventWeightPU,"Misc");
    hm->fill1DHist(51,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
    hm->fill1DHist(52,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,eventWeightPho,"Misc");
    hm->fill1DHist(52,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
    hm->fill1DHist(53,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,eventWeightTrig,"Misc");
    hm->fill1DHist(53,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
    hm->fill1DHist(54,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,eventWeightLep,"Misc");
    hm->fill1DHist(54,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
    ++nEvents[51];
  }

  if ( (GP4+ZP4).M() > 120 && (GP4+ZP4).M() < 150){
    hm->fill1DHist(70,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,eventWeight,"Misc");
    hm->fill1DHist(70,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  }

  ////////////////////////////
  // Fill lepton histograms //
  ////////////////////////////

  if (params->selection == "muon" || params->selection == "mumuGamma") {

    LeptonBasicPlots(muonsIDIso[0], muonsIDIso[1], eventWeight);
    hm->fill1DHist(muonsIDIso[0].Charge(),"h1_leadLeptonCharge_"+params->suffix, "Charge leading lepton;Charge;N_{evts}", 5, -2.5, 2.5, eventWeight,"Lepton");     
    hm->fill1DHist(muonsIDIso[1].Charge(),"h1_trailingLeptonCharge_"+params->suffix, "Charge trailing lepton;Charge;N_{evts}", 5, -2.5, 2.5, eventWeight,"Lepton");     

  } else if (params->selection == "electron" || params->selection == "eeGamma") {

    LeptonBasicPlots(electronsIDIso[0], electronsIDIso[1], eventWeight);
    hm->fill1DHist(electronsIDIso[0].Charge(),"h1_leadLeptonCharge_"+params->suffix, "Charge leading lepton;Charge;N_{evts}", 5, -2.5, 2.5, eventWeight,"Lepton");     
    hm->fill1DHist(electronsIDIso[1].Charge(),"h1_trailingLeptonCharge_"+params->suffix, "Charge trailing lepton;Charge;N_{evts}", 5, -2.5, 2.5, eventWeight,"Lepton");     
  } 

  DileptonBasicPlots(ZP4, eventWeight);


  ////////////////////////////
  // Fill ZGamma histograms //
  ////////////////////////////



  if(params->selection == "mumuGamma" || params->selection == "eeGamma")
  {

    if (cuts->zgMassLow < 90){
      hm->fill1DHist((ZP4+GP4).M(),"h1_InvariantMass_"+params->suffix,"Invariant Mass (Z#gamma);Mass (GeV);Entries",100,0,100,eventWeight,"ZGamma");
    }else{
      hm->fill1DHist((ZP4+GP4).M(),"h1_InvariantMass_"+params->suffix,"Invariant Mass (Z#gamma);Mass (GeV);Entries",100,100,200,eventWeight,"ZGamma");
    }
    hm->fill1DHist((ZP4+GP4).M(),"h1_InvariantMassCut_"+params->suffix,"Invariant Mass (H->Z#gamma);Mass (GeV);Entries",65,115,190,eventWeight,"ZGamma");


    m_llg = (GP4+ZP4).M();



    if (isVBF){
      m_llgCAT5 = (GP4+ZP4).M();
    }else if (catNum == 1) {
      m_llgCAT1 = (GP4+ZP4).M();
    }else if (catNum == 4) {
      m_llgCAT4 = (GP4+ZP4).M();
    }else if (catNum == 2){
      m_llgCAT2 = (GP4+ZP4).M();
    }else if (catNum == 3){
      m_llgCAT3 = (GP4+ZP4).M();
    }else if (catNum == 6){
      m_llgCAT6 = (GP4+ZP4).M();
    }else if (catNum == 7){
      m_llgCAT7 = (GP4+ZP4).M();
    }else if (catNum == 8){
      m_llgCAT8 = (GP4+ZP4).M();
    }else{ 
      m_llgCAT9 = (GP4+ZP4).M();
    }


    StandardPlots(lepton1,lepton2,GP4,eventWeight,"CAT"+str(catNum)+"", "CAT"+str(catNum)+"");
    hm->fill1DHist(GP4.R9(), "h1_R9CAT"+str(catNum)+"_"+params->suffix,"R9;R9;Entries",100,0,1,eventWeight);
    hm->fill2DHist(lepton1.Eta(),lepton2.Eta(),"h2_dilepEtaCAT"+str(catNum)+"_"+params->suffix,"Dilepton Eta CAT"+str(catNum)+"; Eta (leading); Eta (trailing)", 50,-2.5,2.5,50,-2.5,2.5,eventWeight,"CAT"+str(catNum)+"");
    hm->fill2DHist((GP4+ZP4).M(),MEdisc,"h2_MassVsMECAT"+str(catNum)+"_"+params->suffix,"Mass vs ME; m_{ll#gamma}; ME Disc", 90,100,190,90,0,0.2,eventWeight,"MEPlots");
    hm->fill1DHist(MEdisc,"h1_MECAT"+str(catNum)+"_"+params->suffix,"ME Disc CAT:"+str(catNum)+";ME Disc;Entries", 45,0,0.2,eventWeight,"MEPlots");
    hm->fill1DHist(59+catNum,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,eventWeight,"Misc");
    hm->fill1DHist(59+catNum,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
    ++nEvents[60+catNum];

    if (params->dumps){
      if (params->selection == "mumuGamma"){
        dumper->MuonDump(muonsIDIso[lepton1int],3);
        dumper->MuonDump(muonsIDIso[lepton2int],3);

      } else if (params->selection == "eeGamma"){
        dumper->ElectronDump(electronsIDIso[lepton1int],*recoMuons,3);
        dumper->ElectronDump(electronsIDIso[lepton2int],*recoMuons,3);
      }
      dumper->PhotonDump(GP4,3);
    }

    unBinnedWeight = eventWeight;



    hm->fill1DHist(lepton1.DeltaR(GP4),"h1_DeltaRLeading_"+params->suffix,"DeltaR leading lepton vs photon;#Delta R;Entries",16,0,4,eventWeight,"ZGamma");
    hm->fill1DHist(lepton2.DeltaR(GP4),"h1_DeltaRTrailing_"+params->suffix,"DeltaR trailing lepton vs photon;#Delta R;Entries",16,0,4,eventWeight,"ZGamma");
    hm->fill1DHist(ZP4.DeltaR(GP4),"h1_DeltaRZG_"+params->suffix,"DeltaR diLepton vs photon;#Delta R;Entries",16,0,4,eventWeight,"ZGamma");
    hm->fill1DHist(CalculateM12sqrd(ZP4,GP4),"h1_M12sqrd_"+params->suffix,"M12^{2} CORRECTED (H->Z#gamma);Mass^{2} (GeV^{2});Entries",25,3600,34400,eventWeight,"ZGamma");
    hm->fill1DHist(CalculateX1(ZP4,GP4),"h1_X1_"+params->suffix,"x1;x1;Entries",50,0,0.4,eventWeight,"ZGamma");
    hm->fill1DHist(CalculateX2(ZP4,GP4),"h1_X2_"+params->suffix,"x2;x2;Entries",50,0,0.4,eventWeight,"ZGamma");
    hm->fill1DHist(ZP4.Pt()-GP4.Pt(),"h1_PtDiffZG_"+params->suffix,"Zp_{T}-Gp_{T} (scalar);#Delta p_{T} (GeV);Entries",50,-100,100,eventWeight,"ZGamma");
    hm->fill1DHist((ZP4-GP4).Pt(),"h1_PtVecDiffZG_"+params->suffix,"Zp_{T}-Gp_{T} (vector);#Delta p_{T} (GeV);Entries",50,0,200,eventWeight,"ZGamma");
    hm->fill1DHist(ZP4.Pt()/GP4.Pt(),"h1_PtRatZG_"+params->suffix,"Zp_{T}/Gp_{T};#frac{Zp_{T}}{Gp_{T}};Entries",40,0,10,eventWeight,"ZGamma");
    hm->fill1DHist((ZP4+GP4).Pt(),"h1_PtSumZG_"+params->suffix,"3-Body p_{T};p_{T};Entries",25,0,70,eventWeight,"ZGamma");
    hm->fill1DHist(GP4.E(),"h1_GammaEnergy_"+params->suffix,"Gamma Energy;E (GeV);Entries",30,0,150,eventWeight,"ZGamma");
    hm->fill1DHist(GP4.Pt(),"h1_GammaPt_"+params->suffix,"Gamma p_{T};p_{T} (GeV);Entries",45,10,100,eventWeight,"ZGamma");
    hm->fill2DHist(CalculateX1(ZP4,GP4),CalculateX2(ZP4,GP4),"h2_X1X2_"+params->suffix,"x1 vs x2;x1;x2",50,0,0.4,50,0,0.4,eventWeight,"ZGamma");
    hm->fill2DHist(ZP4.M(),(ZP4+GP4).M(),"h2_InvariantMasses_"+params->suffix,"2 Body vs 3 Body Invariant Mass; Z (GeV); Z#gamma (GeV)",80,50,130,110,90,200,eventWeight,"ZGamma");
    hm->fill1DHist((ZP4+GP4).M()-ZP4.M(),"h1_DeltaM_"+params->suffix,"Z#gamma_{m}-Z_{m};#Deltam (GeV);Entries",70,0,140,eventWeight,"ZGamma");
    hm->fill2DHist(ZP4.M(),GP4.Pt(),"h2_2BodyVsGPt_"+params->suffix,"Dilepton Mass vs #gamma p_{T};Z_{m} (GeV);#gamma p_{T}",100,50,150,45,10,100,eventWeight,"ZGamma");
    hm->fill2DHist((ZP4+GP4).M(),GP4.Pt(),"h2_3BodyVsGPt_"+params->suffix,"3-Body Mass vs #gamma p_{T};Z#gamma_{m};#gamma p_{T}",20,100,190,45,10,100,eventWeight,"ZGamma");
    hm->fill2DHist((ZP4+GP4).M(),(ZP4.Pt()-GP4.Pt()),"h2_3BodyVsDiffPt_"+params->suffix,"3-Body Mass vs diff p_{T};Z#gamma_{m};Z p_{T} - #gamma p_{T}",20,100,190,100,-150,150,eventWeight,"ZGamma");
    hm->fill2DHist((ZP4+GP4).M(),(ZP4.Pt()/GP4.Pt()),"h2_3BodyVsRatPt_"+params->suffix,"3-Body Mass vs ratio p_{T};Z#gamma_{m};Ratio p_{T}",20,100,190,50,0,10,eventWeight,"ZGamma");
    hm->fill2DHist((ZP4+GP4).M(),ZP4.Pt(),"h2_3BodyVsZPt_"+params->suffix,"3-Body Mass vs #Z p_{T};Z#gamma_{m};Z p_{T}",20,100,190,50,0,150,eventWeight,"ZGamma");
    hm->fill2DHist((ZP4+GP4).M()/(ZP4+GP4).Pt(),GP4.Pt(),"h2_3BodyOptVsGPt_"+params->suffix,"3-Body Mass/pt vs #gamma p_{T};Z#gamma_{m}/Z#gamma_{pT};#gamma p_{T}",20,0,20,45,10,100,eventWeight,"ZGamma");
    hm->fill2DHist((ZP4+GP4).M()/(ZP4+GP4).Pt(),(ZP4.Pt()-GP4.Pt()),"h2_3BodyOptVsDiffPt_"+params->suffix,"3-Body Mass/pt vs diff p_{T};Z#gamma_{m}/Z#gamma_{pT};Z p_{T} - #gamma p_{T}",20,0,20,100,-150,150,eventWeight,"ZGamma");
    hm->fill2DHist((ZP4+GP4).M()/(ZP4+GP4).Pt(),(ZP4.Pt()/GP4.Pt()),"h2_3BodyOptVsRatPt_"+params->suffix,"3-Body Mass/pt vs ratio p_{T};Z#gamma_{m}/Z#gamma_{pT};Ratio p_{T}",20,0,20,50,0,10,eventWeight,"ZGamma");
    hm->fill2DHist((ZP4+GP4).M()/(ZP4+GP4).Pt(),ZP4.Pt(),"h2_3BodyOptVsZPt_"+params->suffix,"3-Body Mass/pt vs #Z p_{T};Z#gamma_{m}/Z#gamma_{pT};Z p_{T}",20,0,20,50,0,150,eventWeight,"ZGamma");
    hm->fillProfile(CalculateX1(ZP4,GP4),CalculateX2(ZP4,GP4),"profile_X1X2Pro_"+params->suffix,"x1 vs x2;x1;x2",50,0,0.4,0,0.4,eventWeight,"ZGamma");
    hm->fill1DHist(lepton1.Pt()/lepton2.Pt(),"h1_MuonRatPt_"+params->suffix,"leading/trailing p_{T};p_{T} Ratio;Entries",15,0,5,eventWeight,"ZGamma");
    hm->fill1DHist(photonsIDIso.size(),"h1_photonMult_"+params->suffix,"Photon Multiplicity;nPhoton;Entries",10,0.5,10.5,eventWeight,"ZGamma");
    hm->fill1DHist(fabs(ZP4.DeltaPhi(GP4)),"h1_DeltaPhiZG_"+params->suffix,"#Delta#phi (Z,#gamma);#Delta#phi (rad);Entries",20,0,TMath::Pi()+0.5,eventWeight,"ZGamma");
    if (params->suffix.find("GammaStar") !=string::npos){
      hm->fill1DHist(ZP4.M(),"h1_gammaStarMll1_"+params->suffix,"gammaStar M_ll full range; M_ll (GeV);Entries",120,-5,115,eventWeight,"ZGamma");
      hm->fill1DHist(ZP4.M(),"h1_gammaStarMll2_"+params->suffix,"gammaStar M_ll low range; M_ll (GeV);Entries",50,-5,45,eventWeight,"ZGamma");
    }

  }


  /////////////////////////
  // Fill std histograms //
  /////////////////////////

  StandardPlots(lepton1,lepton2,GP4,eventWeight,"", "pT-Eta-Phi");
  HighMassPlots(lepton1,lepton2,GP4,eventWeight,"", "HighMass");

  //////////////////////////////
  // Fill Vtx variable histos //
  //////////////////////////////


  hm->fill1DHist(primaryVtx->GetSize(),"h1_pvMult_"+params->suffix, "Multiplicity of PVs; nVtx; Entries", 50, 0.5, 50.5, eventWeight,"Vtx");
  hm->fillProfile(primaryVtx->GetSize(),nTracks,"h1_nTracks_"+params->suffix, "Average number of tracks per nVtx; nVtx; nTracks", 50, 0.5, 50.5, 1,"Vtx");
  hm->fill1DHist(pvPosition->Z(),"h1_pvPosZ_"+params->suffix, "Z position of beamspot; Z(cm); Entries", 50, -25, 25, eventWeight,"Vtx");
  if (isRealData) {
    hm->fillProfile(runNumber,primaryVtx->GetSize(),"h1_nVtcs_"+params->suffix, "Average number of vertices per run; Run Number; nVertices", 8700.0, 135000.0, 144200.0, 0.0, 6.0, 1,"Vtx");
  }

  //////////////////////////////
  // Fill Gen Particle Histos //
  //////////////////////////////



  //////////
  // misc //
  //////////
  
  AnglePlots(recoLevelOutputs,eventWeight,"ZGAngles_RECO","RECO");

  hm->fill1DHist(eventWeight,"h1_eventWeight_"+params->suffix, "event weight", 100, 0., 2.,1,"Misc");
  if (!isRealData) hm->fill1DHist(ptHat,"h1_ptHat_"+params->suffix,"ptHat",37, 15.0, 200.0, eventWeight,"Misc");

  if (isRealData) {
    hm->fill1DHist(runNumber,"h1_goodRuns_"+params->suffix, "Number of events passing selection cuts by run;Run number; nEvents", 9200, 160000., 175000.,1,"Misc");
    hm->fillProfile(runNumber,primaryVtx->GetSize(),"p1_nVtcs", "Average number of vertices per run; Run Number; nVertices", 8700.0, 135000.0, 144200.0, 0.0, 6.0, 1,"Misc");
  }


  if (nEvents[0]%2 == 0){
    trainingChain->Fill();
  }else{
    sampleChain->Fill();
  }

  m_llgChain->Fill();

   return kTRUE;
}

void higgsAnalyzer::Terminate()
{

  TH1F* eventHisto = (TH1F*)histoFile->GetDirectory("Misc")->Get(("h1_acceptanceByCut_"+params->suffix).c_str());

  

  cout<<"\nRunning over "<<params->suffix<<" dataset with "<<params->selection<<" selection."<<"\n"<<endl;
  cout << "| CUT DESCRIPTION                    |\t" << "\t|"                         << endl;
  cout << "| Unskimmed events:                  |\t" << unskimmedEventsTotal          << "\t|" << endl;
  cout << "| Initial number of events:          |\t" << nEvents[0]                    << "\t|" << endl;
  //cout << "| Electrion Preselection:            |\t" << nEvents[1]                    << "\t|" << endl;
  cout << "| Pass HLT selection:                |\t" << nEvents[2]                    << "\t|" << endl;
  cout << "| Good PV:                           |\t" << nEvents[3]                    << "\t|" << endl;
  cout << "| Data quality bits:                 |\t" << nEvents[4]                    << "\t|" << endl;
  cout << "| Two opp charged leptons (ID):      |\t" << nEvents[5]                    << "\t|" << endl;
  cout << "| Two opp charged leptons (ID/Iso):  |\t" << nEvents[6]                    << "\t|" << endl;
  cout << "| Good Z:                            |\t" << nEvents[7]                    << "\t|" << endl;
  cout << "| Z mass window:                     |\t" << nEvents[8]                    << "\t|" << endl;
  cout << "| Gamma Acc and Energy:              |\t" << nEvents[9]                    << "\t|" << endl;
  cout << "| Photon DR Cuts:                    |\t" << nEvents[10]                   << "\t|" << endl;
  //cout << "| Third lepton veto:                 |\t" << nEvents[11]                   << "\t|" << endl;
  //cout << "| b-jet veto:                        |\t" << nEvents[12]                   << "\t|" << endl;
  //cout << "| MET:                               |\t" << nEvents[13]                   << "\t|" << endl;
  //cout << "| MET/QT:                            |\t" << nEvents[14]                   << "\t|" << endl;
  //cout << "| Jet Multiplicity:                  |\t" << nEvents[15]                   << "\t|" << endl;
  cout << "| Higgs Mass Low:                    |\t" << nEvents[16]                   << "\t|" << endl;
  cout << "| Higgs Mass High:                   |\t" << nEvents[17]                   << "\t|" << endl;
  cout << "| MZ + MZG Cut:                      |\t" << nEvents[18]                   << "\t|" << endl;
  //cout << "| Angular Cut:                       |\t" << nEvents[19]                   << "\t|" << endl;
  //cout << "| M12 Cut:                           |\t" << nEvents[20]                   << "\t|" << endl;
  //cout << "| MVA Cuts:                          |\t" << nEvents[21]                   << "\t|" << endl;
  //cout << "| VBF Cuts:                          |\t" << nEvents[22]                   << "\t|" << endl;
  cout << "| ME Cuts:                           |\t" << nEvents[23]                   << "\t|" << endl;
  cout << "| 100-190 range:                     |\t" << nEvents[51]                   << "\t|" << endl;
  cout << "| 100-190 weighted (all):            |\t" << eventHisto->Integral(50,50) << "\t|" << endl;
  cout << "| 100-190 weighted (PU):             |\t" << eventHisto->Integral(51,51) << "\t|" << endl;
  cout << "| 100-190 weighted (pho):            |\t" << eventHisto->Integral(52,52) << "\t|" << endl;
  cout << "| 100-190 weighted (trig):           |\t" << eventHisto->Integral(53,53) << "\t|" << endl;
  cout << "| 100-190 weighted (lep):            |\t" << eventHisto->Integral(54,54) << "\t|" << endl;
  cout << "| CAT 1:                    |\t" << nEvents[61] <<"\t"<< eventHisto->Integral(60,60) << "\t|" << endl;
  cout << "| CAT 2:                    |\t" << nEvents[62] <<"\t"<< eventHisto->Integral(61,61)                  << "\t|" << endl;
  cout << "| CAT 3:                    |\t" << nEvents[63] <<"\t"<< eventHisto->Integral(62,62)                  << "\t|" << endl;
  cout << "| CAT 4:                    |\t" << nEvents[64] <<"\t"<< eventHisto->Integral(63,63)                  << "\t|" << endl;
  cout << "| CAT 6:                    |\t" << nEvents[66] <<"\t"<< eventHisto->Integral(65,65) << "\t|" << endl;
  cout << "| CAT 7:                    |\t" << nEvents[67] <<"\t"<< eventHisto->Integral(66,66)                  << "\t|" << endl;
  cout << "| CAT 8:                    |\t" << nEvents[68] <<"\t"<< eventHisto->Integral(67,67)                  << "\t|" << endl;
  cout << "| CAT 9:                    |\t" << nEvents[69] <<"\t"<< eventHisto->Integral(68,68)                  << "\t|" << endl;
  cout << "| CAT 5:                    |\t" << nEvents[65] <<"\t"<< eventHisto->Integral(64,64)                  << "\t|" << endl;
  cout << "| GEN ACCEPTANCE Leptons:            |\t" << genAccept[0]                  << "\t|" << endl;
  cout << "| GEN ACCEPTANCE Total:              |\t" << genAccept[1]                  << "\t|" << endl;

  m_llgFile->cd();
  TH1I* totalEvents = new TH1I(("unskimmedEventsTotal_"+params->suffix).c_str(),("unskimmedEventsTotal_"+params->suffix).c_str(),1,0,1);
  
  if (params->suffix.find("zh") != string::npos || (params->suffix.find("wh") != string::npos)){ 
    totalEvents->SetBinContent(1,unskimmedEventsTotalSpecial);
  }else{
    totalEvents->SetBinContent(1,unskimmedEventsTotal);
  }

  //hm->writeHists();
  trainingFile->Write();
  sampleFile->Write();
  histoFile->Write();
  m_llgFile->Write();
  trainingFile->Close();
  sampleFile->Close();
  histoFile->Close();  
  m_llgFile->Close();

  dumper->CloseDumps();
}

////////////////////////////
// User Defined Functions //
////////////////////////////

void higgsAnalyzer::MetPlusZPlots(TLorentzVector metP4, TLorentzVector ZP4, float eventWeight)
{

  hm->fill1DHist(metP4.Pt()/ZP4.Pt(),"h1_MetOverQt_"+params->suffix, "MET/Q_{T,ll};MET/q_{T};N_{evts}", 45, 0., 9., eventWeight,"MET+Lepton");
  hm->fill1DHist((metP4 + ZP4).Pt(),"h1_MetPlusQtMagnitude_"+params->suffix, "|MET + q_{T}|;|MET + Q_{T,ll}|;N_{evts}", 60, 0., 300., eventWeight,"MET+Lepton");
  hm->fill1DHist(fabs(metP4.DeltaPhi(ZP4)),"h1_MetQtDeltaPhi_"+params->suffix, "#Delta#phi(q_{T}, MET);#Delta#phi;N_{evts}", 36, 0., TMath::Pi(), eventWeight,"MET+Lepton");
  hm->fill2DHist(metP4.Pt(), metP4.Pt()/ZP4.Pt(),"h2_MetByMetOverQt_"+params->suffix, "MET/q_{T} vs. MET; MET; MET/q_{T}", 40, 0., 200., 40, 0., 4., eventWeight,"2D");
  hm->fill2DHist(metP4.Pt(), (metP4 + ZP4).Pt(),"h2_MetByMetPlusQtMag_"+params->suffix, "|MET + q_{T}| vs. MET; MET; MET/q_{T}", 40, 0., 200., 60, 0., 300., eventWeight,"2D");

  //MT
  hm->fill1DHist(CalculateTransMass(metP4, ZP4),"h1_MetDileptonMT_"+params->suffix, "M_{T};M_{T};N_{evts}", 90, 50., 500., eventWeight,"MET+Lepton");

  //ProjectedMET
  hm->fill1DHist(metP4.Pt()*cos(metP4.DeltaPhi(ZP4)),"h1_ProjMetByQt_"+params->suffix, "Longitudinal MET by q_{T};MET_{longitudinal};N_{evts}", 52, -190., 80., eventWeight,"MET+Lepton");
  hm->fill1DHist(metP4.Pt()*sin(metP4.DeltaPhi(ZP4)),"h1_OrthoMetByQt_"+params->suffix, "Transverse MET by q_{T};MET_{Transverse};N_{evts}", 64, -100., 80., eventWeight,"MET+Lepton");
  if (fabs(metP4.DeltaPhi(ZP4)) < TMath::Pi()/2) {
    hm->fill1DHist(metP4.Pt() * sin(fabs(metP4.DeltaPhi(ZP4))),"h1_ProjectedMet_"+params->suffix, "ProjMET; ProjMET; N_{evts}", 50, 0., 250., eventWeight,"MET");
  } else {
    hm->fill1DHist(metP4.Pt(),"h1_ProjectedMet_"+params->suffix, "ProjMET; ProjMET; N_{evts}", 50, 0., 250., eventWeight,"MET");
  }
}

void higgsAnalyzer::MetPlusLeptonPlots(TLorentzVector metP4, TLorentzVector p1, TLorentzVector p2, float eventWeight)
{
  hm->fill1DHist(CalculateTransMassAlt(metP4, p1),"h1_MetLeadLeptonMT_"+params->suffix, "M_{T,l1};M_{T,l1};N_{evts}", 50, 0., 250., eventWeight,"MET+Lepton");
  hm->fill1DHist(CalculateTransMassAlt(metP4, p1),"h1_MetTrailingLeptonMT_"+params->suffix, "M_{T,l2};M_{T,l2};N_{evts}", 50, 0., 250., eventWeight,"MET+Lepton");
  hm->fill2DHist(CalculateTransMassAlt(metP4, p1), CalculateTransMassAlt(metP4, p2),"h2_MetLeptonMT_"+params->suffix, "MT(MET,lepton);MT(MET, l1);MT(MET, l2)", 80, 0., 400., 80, 0., 400., eventWeight,"2D");
  hm->fill1DHist(fabs(metP4.DeltaPhi(p1)),"h1_MetLeadLeptonDeltaPhi_"+params->suffix, "#Delta#phi(l1, MET);#Delta#phi;N_{evts}", 36, 0., TMath::Pi(), eventWeight,"MET+Lepton");
  hm->fill1DHist(fabs(metP4.DeltaPhi(p2)),"h1_MetTrailingLeptonDeltaPhi_"+params->suffix, "#Delta#phi(l2, MET);#Delta#phi;N_{evts}", 36, 0., TMath::Pi(), eventWeight,"MET+Lepton");
}

void higgsAnalyzer::LeptonBasicPlots(TLorentzVector p1, TLorentzVector p2, float eventWeight)
{
  hm->fill1DHist(p1.Pt(),"h1_leadLeptonPt_"+params->suffix, "p_{T} leading lepton;p_{T};N_{evts}", 48, 10., 250., eventWeight,"Lepton");     
  hm->fill1DHist(p1.Eta(),"h1_leadLeptonEta_"+params->suffix, "#eta leading lepton;#eta;N_{evts}", 25, -2.5, 2.5, eventWeight,"Lepton");    
  hm->fill1DHist(p1.Phi(),"h1_leadLeptonPhi_"+params->suffix, "#phi leading lepton;#phi;N_{evts}", 36, -TMath::Pi(), TMath::Pi(), eventWeight,"Lepton");    

  hm->fill1DHist(p2.Pt(),"h1_trailingLeptonPt_"+params->suffix, "p_{T} trailing lepton;p_{T};N_{evts}", 38, 10., 200., eventWeight,"Lepton"); 
  hm->fill1DHist(p2.Eta(),"h1_trailingLeptonEta_"+params->suffix, "#eta trailing lepton;#eta;N_{evts}", 25, -2.5, 2.5, eventWeight,"Lepton");
  hm->fill1DHist(p2.Phi(),"h1_trailingLeptonPhi_"+params->suffix, "#phi trailing lepton;#phi;N_{evts}", 36, -TMath::Pi(), TMath::Pi(), eventWeight,"Lepton");

  hm->fill2DHist(p2.Pt(),fabs(p2.Eta()),"h2_trailingLeptonPtEta_"+params->suffix,"Trailing Lepton p_{T} vs. #||{#eta}",5,0,100,5,0,2.5,eventWeight,"Lepton");

  hm->fill1DHist(p2.Pt()/p1.Pt(),"h1_diLeptonPtRatio_"+params->suffix, "dilepton p_{T} ratio;p_{T,2}/p_{T,1};N_{evts}", 25, 0., 1., eventWeight,"Lepton"); 
  hm->fill1DHist(fabs(p2.Eta() - p1.Eta()),"h1_diLeptonDeltaEta_"+params->suffix, "dilepton #Delta#eta;#Delta#eta;N_{evts}", 40, 0., 8., eventWeight,"Lepton");
  hm->fill1DHist(fabs(p2.DeltaPhi(p1)),"h1_diLeptonDeltaPhi_"+params->suffix, "dilepton #Delta#phi;#Delta#phi;N_{evts}", 18, 0., TMath::Pi(), eventWeight,"Lepton");
  hm->fill1DHist(p2.DeltaR(p1),"h1_diLeptonDeltaR_"+params->suffix, "dilepton #Delta R;#Delta R;N_{evts}", 18, 0., 4.5, eventWeight,"Lepton");  
}

void higgsAnalyzer::StandardPlots(TLorentzVector p1, TLorentzVector p2, TLorentzVector gamma, float eventWeight,string tag, string folder)
{
  TLorentzVector diLep = p1+p2;
  TLorentzVector threeBody = p1+p2+gamma;

  hm->fill1DHist(p1.Pt(),"h1_leadLeptonStdPt"+tag+"_"+params->suffix, "p_{T} leading lepton;p_{T};N_{evts}", 28, 0., 140., eventWeight,folder);     
  hm->fill1DHist(p1.Eta(),"h1_leadLeptonStdEta"+tag+"_"+params->suffix, "#eta leading lepton;#eta;N_{evts}", 20, -2.5, 2.5, eventWeight,folder);    
  hm->fill1DHist(p1.Phi(),"h1_leadLeptonStdPhi"+tag+"_"+params->suffix, "#phi leading lepton;#phi;N_{evts}", 20, -TMath::Pi(), TMath::Pi(), eventWeight,folder);    

  hm->fill1DHist(p2.Pt(),"h1_trailingLeptonStdPt"+tag+"_"+params->suffix, "p_{T} trailing lepton;p_{T};N_{evts}", 20, 0., 100., eventWeight,folder); 
  hm->fill1DHist(p2.Eta(),"h1_trailingLeptonStdEta"+tag+"_"+params->suffix, "#eta trailing lepton;#eta;N_{evts}", 20, -2.5, 2.5, eventWeight,folder);
  hm->fill1DHist(p2.Phi(),"h1_trailingLeptonStdPhi"+tag+"_"+params->suffix, "#phi trailing lepton;#phi;N_{evts}", 20, -TMath::Pi(), TMath::Pi(), eventWeight,folder);

  hm->fill1DHist(gamma.Pt(),"h1_photonPt"+tag+"_"+params->suffix, "p_{T} gamma;p_{T};N_{evts}", 16, 0., 80., eventWeight,folder); 
  hm->fill1DHist(gamma.Eta(),"h1_photonEta"+tag+"_"+params->suffix, "#eta gamma;#eta;N_{evts}", 20, -2.5, 2.5, eventWeight,folder);//20
  hm->fill1DHist(gamma.Phi(),"h1_photonPhi"+tag+"_"+params->suffix, "#phi gamma;#phi;N_{evts}", 20, -TMath::Pi(), TMath::Pi(), eventWeight,folder);//18

  hm->fill1DHist(diLep.Pt(),"h1_diLepPt"+tag+"_"+params->suffix, "p_{T} Z;p_{T};N_{evts}", 20, 0., 100., eventWeight,folder);     
  hm->fill1DHist(diLep.Eta(),"h1_diLepEta"+tag+"_"+params->suffix, "#eta Z;#eta;N_{evts}", 18, -2.5, 2.5, eventWeight,folder);    
  hm->fill1DHist(diLep.Phi(),"h1_diLepPhi"+tag+"_"+params->suffix, "#phi Z;#phi;N_{evts}", 18, -TMath::Pi(), TMath::Pi(), eventWeight,folder);    
  hm->fill1DHist(diLep.M(),"h1_diLepMass"+tag+"_"+params->suffix, "M_{Z};M (GeV);N_{evts}", 50, 66, 116, eventWeight,folder);    

  hm->fill1DHist(threeBody.Pt(),"h1_threeBodyPt"+tag+"_"+params->suffix, "p_{T} 3body;p_{T};N_{evts}", 20, 0., 100., eventWeight,folder);     
  hm->fill1DHist(threeBody.Eta(),"h1_threeBodyEta"+tag+"_"+params->suffix, "#eta 3body;#eta;N_{evts}", 18, -2.5, 2.5, eventWeight,folder);    
  hm->fill1DHist(threeBody.Phi(),"h1_threeBodyPhi"+tag+"_"+params->suffix, "#phi 3body;#phi;N_{evts}", 18, -TMath::Pi(), TMath::Pi(), eventWeight,folder);    
  hm->fill1DHist(threeBody.M(),"h1_threeBodyMass"+tag+"_"+params->suffix, "M_{3body};M (GeV);N_{evts}", 150, 50, 200, eventWeight,folder);    


  hm->fill1DHist(primaryVtx->GetSize(),"h1_pvMultCopy"+tag+"_"+params->suffix, "Multiplicity of PVs;N_{PV};N_{evts}", 25, 0.5, 25.5, eventWeight, folder);
  for(int i = 0; i < 64; ++i) {
    unsigned long iHLT = 0x0; 
    iHLT = 0x01 << i;  
    if ((triggerStatus & iHLT) == iHLT) hm->fill1DHist(i+1,"h1_triggerStatus"+tag+"_"+params->suffix, "Triggers (no weight)", 64, 0.5, 64.5,1,"Misc");  
  } 
}

void higgsAnalyzer::StandardPlots(TLorentzVector p1, TLorentzVector p2, float eventWeight,string tag, string folder)
{
  TLorentzVector diLep = p1+p2;

  hm->fill1DHist(p1.Pt(),"h1_leadLeptonStdPt"+tag+"_"+params->suffix, "p_{T} leading lepton;p_{T};N_{evts}", 28, 0., 140., eventWeight,folder);     
  hm->fill1DHist(p1.Eta(),"h1_leadLeptonStdEta"+tag+"_"+params->suffix, "#eta leading lepton;#eta;N_{evts}", 20, -2.5, 2.5, eventWeight,folder);    
  hm->fill1DHist(p1.Phi(),"h1_leadLeptonStdPhi"+tag+"_"+params->suffix, "#phi leading lepton;#phi;N_{evts}", 20, -TMath::Pi(), TMath::Pi(), eventWeight,folder);    

  hm->fill1DHist(p2.Pt(),"h1_trailingLeptonStdPt"+tag+"_"+params->suffix, "p_{T} trailing lepton;p_{T};N_{evts}", 20, 0., 100., eventWeight,folder); 
  hm->fill1DHist(p2.Eta(),"h1_trailingLeptonStdEta"+tag+"_"+params->suffix, "#eta trailing lepton;#eta;N_{evts}", 20, -2.5, 2.5, eventWeight,folder);
  hm->fill1DHist(p2.Phi(),"h1_trailingLeptonStdPhi"+tag+"_"+params->suffix, "#phi trailing lepton;#phi;N_{evts}", 20, -TMath::Pi(), TMath::Pi(), eventWeight,folder);

  hm->fill1DHist(diLep.Pt(),"h1_diLepPt"+tag+"_"+params->suffix, "p_{T} Z;p_{T};N_{evts}", 20, 0., 100., eventWeight,folder);     
  hm->fill1DHist(diLep.Eta(),"h1_diLepEta"+tag+"_"+params->suffix, "#eta Z;#eta;N_{evts}", 18, -2.5, 2.5, eventWeight,folder);    
  hm->fill1DHist(diLep.Phi(),"h1_diLepPhi"+tag+"_"+params->suffix, "#phi Z;#phi;N_{evts}", 18, -TMath::Pi(), TMath::Pi(), eventWeight,folder);    
  hm->fill1DHist(diLep.M(),"h1_diLepMass"+tag+"_"+params->suffix, "M_{Z};M (GeV);N_{evts}", 50, 66, 116, eventWeight,folder);    
  hm->fill1DHist(diLep.M(),"h1_diLepMassLow"+tag+"_"+params->suffix, "M_{Z};M (GeV);N_{evts}", 66, 0, 66, eventWeight,folder);    

  if(fabs(p1.Eta()) < 1.4442 && fabs(p2.Eta()) < 1.4442){
    hm->fill1DHist(diLep.M(),"h1_diLepMassEB-EB"+tag+"_"+params->suffix, "M_{Z} EB-EB;M (GeV);N_{evts}", 50, 66, 116, eventWeight,folder);    
  }else if (fabs(p1.Eta()) > 1.566 && fabs(p2.Eta()) < 1.566){
    hm->fill1DHist(diLep.M(),"h1_diLepMassEE-EE"+tag+"_"+params->suffix, "M_{Z} EE-EE;M (GeV);N_{evts}", 50, 66, 116, eventWeight,folder);    
  }else if (fabs(p1.Eta()) < 1.566 && fabs(p2.Eta()) < 1.566 && fabs(p1.Eta()) > 1.4442 && fabs(p2.Eta()) > 1.4442){
    hm->fill1DHist(diLep.M(),"h1_diLepMassET-ET"+tag+"_"+params->suffix, "M_{Z} ET-ET;M (GeV);N_{evts}", 50, 66, 116, eventWeight,folder);    
  }

  hm->fill1DHist(primaryVtx->GetSize(),"h1_pvMultCopy"+tag+"_"+params->suffix, "Multiplicity of PVs;N_{PV};N_{evts}", 25, 0.5, 25.5, eventWeight, folder);
  for(int i = 0; i < 64; ++i) {
    unsigned long iHLT = 0x0; 
    iHLT = 0x01 << i;  
    if ((triggerStatus & iHLT) == iHLT) hm->fill1DHist(i+1,"h1_triggerStatus"+tag+"_"+params->suffix, "Triggers (no weight)", 64, 0.5, 64.5,1,"Misc");  
  } 
}

void higgsAnalyzer::HighMassPlots(TLorentzVector p1, TLorentzVector p2, TLorentzVector gamma, float eventWeight,string tag, string folder)
{
  TLorentzVector diLep = p1+p2;
  TLorentzVector threeBody = p1+p2+gamma;


  hm->fill1DHist(gamma.Pt(),"h1_photonPtHigh"+tag+"_"+params->suffix, "p_{T} gamma;p_{T};N_{evts}", 50, 0.,500., eventWeight,folder); 
  hm->fill1DHist(diLep.Pt(),"h1_diLepPtHigh"+tag+"_"+params->suffix, "p_{T} Z;p_{T};N_{evts}", 50, 0., 500., eventWeight,folder);     
  hm->fill1DHist(threeBody.M(),"h1_threeBodyMassHigh"+tag+"_"+params->suffix, "M_{3body};M (GeV);N_{evts}", 130, 150, 800, eventWeight,folder);    
  hm->fill1DHist(threeBody.Pt(),"h1_threeBodyPtHigh"+tag+"_"+params->suffix, "pT_{3body};pT_{ll#gamma} (GeV);N_{evts}", 40, 0, 200, eventWeight,folder);    

  hm->fill2DHist(threeBody.M(),gamma.Pt(),"h2_threeBodyMassVphotonPt"+tag+"_"+params->suffix, "M_{3body} v pT_{#gamma};M_{ll#gamma} (GeV);pT_{#gamma} (GeV)", 130, 150, 800, 50, 0, 500, eventWeight,folder);    
  hm->fill2DHist(threeBody.M(),diLep.Pt(),"h2_threeBodyMassVdiLepPt"+tag+"_"+params->suffix, "M_{3body} v pT_{ll};M_{ll#gamma} (GeV);pT_{ll} (GeV)", 130, 150, 800, 50, 0, 500, eventWeight,folder);    
  hm->fill2DHist(gamma.Pt(),diLep.Pt(),"h2_photonPtVdiLepPt"+tag+"_"+params->suffix, "pT_{#gamma} v pT_{ll};pT_{#gamma} (GeV);pT_{ll} (GeV)", 50, 0, 500, 50, 0, 500, eventWeight,folder);    

}

void higgsAnalyzer::MVAPlots(mvaVarStruct _mvaVars, float _mvaVal, float eventWeight, string tag, string folder)
{
  hm->fill1DHist(_mvaVars._smallTheta,"h1_smallTheta"+tag+"_"+params->suffix, "smallTheta;;N_{evts}", 30, 0., 1., eventWeight,folder);     
  hm->fill1DHist(_mvaVars._bigTheta,"h1_bigTheta"+tag+"_"+params->suffix, "bigTheta;;N_{evts}", 30, 0., 1., eventWeight,folder);     
  hm->fill1DHist(_mvaVars._comPhi,"h1_comPhi"+tag+"_"+params->suffix, "comPhi;;N_{evts}", 30, -3.2, 3.2, eventWeight,folder);     
  hm->fill1DHist(_mvaVars._threeBodyPtOM,"h1_threeBodyPtOM"+tag+"_"+params->suffix, "threeBodyPtOM;;N_{evts}", 30, 0., 1., eventWeight,folder);     
  hm->fill1DHist(_mvaVars._R9,"h1_R9"+tag+"_"+params->suffix, "R9;;N_{evts}", 30, 0., 1.1, eventWeight,folder);     
  hm->fill1DHist(_mvaVars._sieip,"h1_sieip"+tag+"_"+params->suffix, "sieip;;N_{evts}", 30, -1., 1., eventWeight,folder);     
  hm->fill1DHist(_mvaVars._sipip,"h1_sipip"+tag+"_"+params->suffix, "sipip;;N_{evts}", 30, 0., 0.007, eventWeight,folder);     
  hm->fill1DHist(_mvaVars._SCRawEOPt,"h1_SCRawEOPt"+tag+"_"+params->suffix, "SCRawEOPt;;N_{evts}", 30, 0.9, 7., eventWeight,folder);     
  hm->fill1DHist(_mvaVars._SCPSEOPt,"h1_SCPSEOPt"+tag+"_"+params->suffix, "SCPSEOPt;;N_{evts}", 30, 0., 0.8, eventWeight,folder);     
  hm->fill1DHist(_mvaVars._e2x2O5x5,"h1_e2x2O5x5"+tag+"_"+params->suffix, "e2x2O5x5;;N_{evts}", 30, 0., 2., eventWeight,folder);     
  hm->fill1DHist(_mvaVal,"h1_mvaVal"+tag+"_"+params->suffix, "mvaVal;;N_{evts}", 30, -1., 1., eventWeight,folder);     
}



void higgsAnalyzer::AnglePlots(ZGAngles &zga,float eventWeight, string folder, string tag)
{
  hm->fill1DHist(zga.costheta_lp,"h1_costhetaLP"+tag+"_"+params->suffix, "Cos(#theta) positive lepton;cos(#theta);N_{evts}", 50, -1., 1., eventWeight,folder);     
  hm->fill1DHist(zga.costheta_lm,"h1_costhetaLM"+tag+"_"+params->suffix, "Cos(#theta) negative lepton;cos(#theta);N_{evts}", 50, -1., 1., eventWeight,folder);     
  hm->fill1DHist(zga.phi,"h1_phi"+tag+"_"+params->suffix, "#phi positive lepton;#phi;N_{evts}", 50, -TMath::Pi(), TMath::Pi(), eventWeight,folder);     
  hm->fill1DHist(zga.cosTheta,"h1_costhetaZG"+tag+"_"+params->suffix, "Cos(#Theta) ZG system;cos(#Theta);N_{evts}", 50, -1., 1., eventWeight,folder);     
  hm->fill1DHist(zga.costheta_lm+zga.costheta_lp,"h1_costhetaBoth"+tag+"_"+params->suffix, "Cos(#theta) of both lepton;cos(#theta);N_{evts}", 50, -1.1, 1.1, eventWeight,folder);     
}

void higgsAnalyzer::DileptonBasicPlots(TLorentzVector ZP4, float eventWeight)
{
  hm->fill1DHist(ZP4.Mt(),"h1_diLeptonTransMass_"+params->suffix, "M_{T,ll};M_{T,ll};N_{evts}", 100, 55., 255., eventWeight,"Lepton");
  hm->fill1DHist(ZP4.M(),"h1_diLeptonMass_"+params->suffix, "M_{ll}; M_{ll};N_{evts}", 40, 70., 110., eventWeight,"Lepton");     
  hm->fill1DHist(ZP4.Pt(),"h1_diLeptonQt_"+params->suffix, "q_{T};Q_{T};N_{evts}", 50, 0., 500., eventWeight,"Lepton");       
}

void higgsAnalyzer::GenPlots(vector<TCGenParticle> Zs, vector<TCGenParticle> leps, vector<TCGenParticle> phots, vector<TCGenParticle> Hs, TLorentzVector ZP4, TLorentzVector GP4, float eventWeight)
{
  //cout<<"gen function has activated"<<endl;
  if (Zs.size()>0){
    //cout<<"yarp there's some Zs"<<endl;
    hm->fill1DHist(Zs[0].M(),"h1_genZMass_"+params->suffix, "GEN M_{Z}; M_{Z};N_{evts}", 40, 70., 110., eventWeight,"GenLvl");     
    hm->fill1DHist(Zs[0].Pt(),"h1_genZQt_"+params->suffix, "q_{T};Q_{T};N_{evts}", 50, 0., 500., eventWeight,"GenLvl");       
  }
  if (Hs.size()>0){
    //cout<<"yarp there's some Zs"<<endl;
    hm->fill1DHist(Hs[0].M(),"h1_genHMass_"+params->suffix, "GEN M_{H}; M_{H};N_{evts}", 90, 90., 190., eventWeight,"GenLvl");     
    hm->fill1DHist(Hs[0].Pt(),"h1_genHQt_"+params->suffix, "H p_{T};Q_{T};N_{evts}", 50, 0., 500., eventWeight,"GenLvl");       
  }

  vector<TCGenParticle>::iterator lepIt;
  TCGenParticle zMuon1;
  TCGenParticle zMuon2;
  bool oneSet = false;

  if (leps.size() > 1){
    for (lepIt=leps.begin(); lepIt<leps.end(); lepIt++){
      if (oneSet && (*lepIt).Mother() && abs((*lepIt).Mother()->GetPDGId()) == 23){
        zMuon2 = *lepIt;
        break;
      }
      if (!oneSet && (*lepIt).Mother() && abs((*lepIt).Mother()->GetPDGId()) == 23){
        zMuon1 = *lepIt;
        oneSet = true;
      }
    }
    //cout<<"yarp there's some MUOOOOONs"<<endl;
    hm->fill1DHist((zMuon1+zMuon2).M(),"h1_genDiLeptonMass_"+params->suffix, "M_{ll} (pre FSR); M_{ll};N_{evts}", 40, 70., 110., eventWeight,"GenLvl");
    hm->fill1DHist((zMuon1+zMuon2).Pt(),"h1_genDiLeptonPt_"+params->suffix, "pT_{ll} (pre FSR); M_{ll};N_{evts}", 50, 0., 500., eventWeight,"GenLvl");
    hm->fill1DHist((zMuon1+zMuon2).M()-ZP4.M(),"h1_genDiLeptonMassRes_"+params->suffix, "Gen-Reco No Gamma M_{ll}; #Delta M_{ll};N_{evts}", 40, -10., 10., eventWeight,"GenLvl");

    /// combinatorix time

    if(params->suffix == "HZG125Signal"){
      //cout<<"SIGNAL GEN SHIT"<<endl;
      TCGenParticle hPhot;
      vector<TCGenParticle>::iterator photIt;
      for (photIt=leps.begin(); photIt<leps.end(); photIt++){
        //cout<<(*photIt).Mother()<<endl;
        if ((*photIt).Mother() && (*photIt).Mother()->GetPDGId()==23){
          hPhot = *photIt;
          break;
        }
      }
      //cout<<"End PhotoLoop"<<endl;
      //cout<<"Gen: "<<(Hs[0]).M()<<endl;
      //cout<<"Reco: "<<(ZP4+GP4).M()<<endl;
      hm->fill1DHist((Hs[0]).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassRes_"+params->suffix, "Gen-Reco M_{ll#gamma} Total Acc; #Delta M_{ll};N_{evts}", 40, -20., 20., eventWeight,"GenLvl");
      // gamma barrel, Z barrel
      if (fabs(GP4.Eta())<1.4442 && fabs(ZP4.Eta())<1.4442){
        hm->fill1DHist((Hs[0]).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResBB_"+params->suffix, "Gen-Reco M_{ll#gamma} Barrel Only; #Delta M_{ll};N_{evts}", 40, -20., 20., eventWeight,"GenLvl");
      }
      // gamma barrel, Z endcap
      if (fabs(GP4.Eta())<1.4442 && fabs(ZP4.Eta())>1.566){
        hm->fill1DHist((Hs[0]).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResBE_"+params->suffix, "Gen-Reco M_{ll#gamma} #gamma Barrel, diLep Endcap; #Delta M_{ll};N_{evts}", 40, -20., 20., eventWeight,"GenLvl");
      }
      // gamma endcap, Z barrel
      if (fabs(GP4.Eta())>1.566 && fabs(ZP4.Eta())<1.4442){
        hm->fill1DHist((Hs[0]).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResEB_"+params->suffix, "Gen-Reco M_{ll#gamma} #gamma Endcap, diLep Barrel; #Delta M_{ll};N_{evts}", 40, -20., 20., eventWeight,"GenLvl");
      }
      // gamma endcap, Z endcap
      if (fabs(GP4.Eta())>1.566 && fabs(ZP4.Eta())>1.566){
        hm->fill1DHist((Hs[0]).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResEE_"+params->suffix, "Gen-Reco M_{ll#gamma} Endcap Only; #Delta M_{ll};N_{evts}", 40, -20., 20., eventWeight,"GenLvl");
      }
      // gamma endcap, Z both
      if (fabs(GP4.Eta())>1.566){
        hm->fill1DHist((Hs[0]).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResE_"+params->suffix, "Gen-Reco M_{ll#gamma} #gamma Endcap, diLep Both; #Delta M_{ll};N_{evts}", 40, -20., 20., eventWeight,"GenLvl");
      }
      // gamma barrel, Z both
      if (fabs(GP4.Eta())<1.4442){
        hm->fill1DHist((Hs[0]).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResB_"+params->suffix, "Gen-Reco M_{ll#gamma} #gamma Barrel, diLep Both; #Delta M_{ll};N_{evts}", 40, -20., 20., eventWeight,"GenLvl");
      }
    }



    else{
      hm->fill1DHist((zMuon1+zMuon2).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassRes_"+params->suffix, "Gen-Reco M_{ll#gamma} Total Acc; #Delta M_{ll};N_{evts}", 40, -10., 10., eventWeight,"GenLvl");
      // gamma barrel, Z barrel
      if (fabs(GP4.Eta())<1.4442 && fabs(ZP4.Eta())<1.4442){
        hm->fill1DHist((zMuon1+zMuon2).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResBB_"+params->suffix, "Gen-Reco M_{ll#gamma} Barrel Only; #Delta M_{ll};N_{evts}", 40, -10., 10., eventWeight,"GenLvl");
      }
      // gamma barrel, Z endcap
      if (fabs(GP4.Eta())<1.4442 && fabs(ZP4.Eta())>1.566){
        hm->fill1DHist((zMuon1+zMuon2).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResBE_"+params->suffix, "Gen-Reco M_{ll#gamma} #gamma Barrel, diLep Endcap; #Delta M_{ll};N_{evts}", 40, -10., 10., eventWeight,"GenLvl");
      }
      // gamma endcap, Z barrel
      if (fabs(GP4.Eta())>1.566 && fabs(ZP4.Eta())<1.4442){
        hm->fill1DHist((zMuon1+zMuon2).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResEB_"+params->suffix, "Gen-Reco M_{ll#gamma} #gamma Endcap, diLep Barrel; #Delta M_{ll};N_{evts}", 40, -10., 10., eventWeight,"GenLvl");
      }
      // gamma endcap, Z endcap
      if (fabs(GP4.Eta())>1.566 && fabs(ZP4.Eta())>1.566){
        hm->fill1DHist((zMuon1+zMuon2).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResEE_"+params->suffix, "Gen-Reco M_{ll#gamma} Endcap Only; #Delta M_{ll};N_{evts}", 40, -10., 10., eventWeight,"GenLvl");
      }
      // gamma endcap, Z both
      if (fabs(GP4.Eta())>1.566){
        hm->fill1DHist((zMuon1+zMuon2).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResE_"+params->suffix, "Gen-Reco M_{ll#gamma} #gamma Endcap, diLep Both; #Delta M_{ll};N_{evts}", 40, -10., 10., eventWeight,"GenLvl");
      }
      // gamma barrel, Z both
      if (fabs(GP4.Eta())<1.4442){
        hm->fill1DHist((zMuon1+zMuon2).M()-(ZP4+GP4).M(),"h1_genGamDiLeptonMassResB_"+params->suffix, "Gen-Reco M_{ll#gamma} #gamma Barrel, diLep Both; #Delta M_{ll};N_{evts}", 40, -10., 10., eventWeight,"GenLvl");
      }
    }

  }

  if (phots.size() > 1){
    hm->fill1DHist(phots[0].Pt(),"h1_genLeadPhotonPt_"+params->suffix, "p_{T} gamma;p_{T};N_{evts}", 16, 0., 80., eventWeight,"GenLvl");
  }

}


float higgsAnalyzer::CalculateTransMass(TLorentzVector p1, TLorentzVector p2)
{
  //float transE    = sqrt(p1.Pt()*p1.Pt() + pow(91.2,2)) + sqrt(p2.Pt()*p2.Pt() + pow(91.2,2)); //p2.M()*p2.M());
  float transE    = sqrt(p1.Pt()*p1.Pt() + p2.M()*p2.M()) + sqrt(p2.Pt()*p2.Pt() +p2.M()*p2.M());
  float transPt   = (p1 + p2).Pt();
  float transMass = sqrt(transE*transE - transPt*transPt);

  return transMass;
}

float higgsAnalyzer::CalculateTransMassAlt(TLorentzVector p1, TLorentzVector p2)
{
  float transMass = sqrt(2*p2.Pt()*p1.Pt() * (1 - cos(fabs(p2.DeltaPhi(p1)))));
  return transMass;
}

bool higgsAnalyzer::CosmicMuonFilter(TCMuon muon1, TCMuon muon2)
{
  float dimuonAngle = muon1.Angle(muon2.Vect());
  if (fabs(TMath::Pi() - dimuonAngle)  < 0.05)
    return true;
  else
    return false;
}


float higgsAnalyzer::Dz(TVector3 objVtx, TLorentzVector objP4, TVector3 vtx)
{
  float vx  = objVtx.x(), vy = objVtx.y(), vz = objVtx.z();
  float px  = objP4.Px(), py = objP4.Py();
  float pz  = objP4.Pz(), pt = objP4.Pt();
  float pvx = vtx.x(), pvy = vtx.y(), pvz = vtx.z();

  float dZ =  (vz-pvz)-((vx-pvx)*px +(vy-pvy)*py)/pt*(pz/pt);
  return dZ;
}

float higgsAnalyzer::Dxy(TVector3 objVtx, TLorentzVector objP4, TVector3 vtx)
{
  float vx  = objVtx.x(), vy = objVtx.y();
  float px  = objP4.Px(), py = objP4.Py(), pt = objP4.Pt();
  float pvx = vtx.x(), pvy = vtx.y();

  double dXY =  (-(vx-pvx)*py + (vy-pvy)*px)/pt;
  return dXY;
}

float higgsAnalyzer::GetPhotonMass()
{
  float photonMass = 91.2;
  if (params->selection == "eGamma") {
    //        TH1D  *h_mass = (TH1D*)reweightFile.Get("h1_diElectronMass");
    //        photonMass = h_mass->GetRandom();
  }
  if (params->selection == "muGamma") {
    //        TH1D  *h_mass = (TH1D*)reweightFile.Get("h1_diMuonMass");
    //        photonMass = h_mass->GetRandom();
  }
  return photonMass;
}

//////////////////////////////
// ZGamma Functions !!!!!   //
//////////////////////////////


float higgsAnalyzer::CalculateX2(TLorentzVector p1, TLorentzVector p2)
{
  float Et = sqrt(p1.Pt()*p1.Pt() + p1.M()*p1.M()) + sqrt(p2.Pt()*p2.Pt() + p2.M()*p2.M()); //same points as above.
  float X2 = (Et/7000.)*(exp(-p1.Eta())+exp(-p2.Eta()));
  return X2;
}

float higgsAnalyzer::CalculateX1(TLorentzVector p1,TLorentzVector p2)
{
  float Et = sqrt(p1.Pt()*p1.Pt() + p1.M()*p1.M() + sqrt(p2.Pt()*p2.Pt() + p2.M()*p2.M())); //uses p.M() in both cases - should i change this for the photon, also uses theoretical z mass.
  float X1 = (Et/7000.)*(exp(p1.Eta())+exp(p2.Eta()));
  return X1;
}


float higgsAnalyzer::CalculateM12sqrd(TLorentzVector p1, TLorentzVector p2)
{
  float Et1Et2 =p1.Pt()*p2.Pt();
  float M12sqrd = 2*Et1Et2*(cosh(p1.Eta()-p2.Eta())-cos(p1.DeltaPhi(p2)));
  return M12sqrd;
}


//////////////////////////////////
// Lumi Reweighing (old values) //
//////////////////////////////////


void higgsAnalyzer::LumiXSWeight(double * _LumiXSWeight){
  if (!isRealData){
    if(params->period.find("2011") != string::npos){

      if(params->suffix.find("gg") != string::npos){
        if(params->suffix.find("120")!=string::npos) *_LumiXSWeight = 99990/(16.65*0.00111*0.10098*1000);
        if(params->suffix.find("125")!=string::npos) *_LumiXSWeight = 95193/(15.32*0.00154*0.10098*1000);
        if(params->suffix.find("130")!=string::npos) *_LumiXSWeight = 99992/(14.16*0.00195*0.10098*1000);
        if(params->suffix.find("135")!=string::npos) *_LumiXSWeight = 99995/(13.11*0.00227*0.10098*1000);
        if(params->suffix.find("140")!=string::npos) *_LumiXSWeight = 99988/(12.18*0.00246*0.10098*1000);
        if(params->suffix.find("145")!=string::npos) *_LumiXSWeight = 99997/(11.33*0.00248*0.10098*1000);
        if(params->suffix.find("150")!=string::npos) *_LumiXSWeight = 99998/(10.58*0.00231*0.10098*1000);

      }else if(params->suffix.find("vbf") != string::npos){
        if(params->suffix.find("120")!=string::npos) *_LumiXSWeight = 99885/(1.269*0.00111*0.10098*1000);
        if(params->suffix.find("125")!=string::npos) *_LumiXSWeight = 99890/(1.211*0.00154*0.10098*1000);
        if(params->suffix.find("130")!=string::npos) *_LumiXSWeight = 99899/(1.154*0.00195*0.10098*1000);
        if(params->suffix.find("135")!=string::npos) *_LumiXSWeight = 99913/(1.100*0.00227*0.10098*1000);
        if(params->suffix.find("140")!=string::npos) *_LumiXSWeight = 99882/(1.052*0.00246*0.10098*1000);
        if(params->suffix.find("145")!=string::npos) *_LumiXSWeight = 99893/(1.023*0.00248*0.10098*1000);
        if(params->suffix.find("150")!=string::npos) *_LumiXSWeight = 99892/(0.9800*0.00231*0.10098*1000);

      }else if(params->suffix.find("tth") != string::npos){
        if(params->suffix.find("120")!=string::npos) *_LumiXSWeight = 99999/(0.0976*0.00111*0.10098*1000);
        if(params->suffix.find("125")!=string::npos) *_LumiXSWeight = 99999/(0.0863*0.00154*0.10098*1000);
        if(params->suffix.find("130")!=string::npos) *_LumiXSWeight = 100076/(0.0766*0.00195*0.10098*1000);
        if(params->suffix.find("135")!=string::npos) *_LumiXSWeight = 99999/(0.0681*0.00227*0.10098*1000);
        if(params->suffix.find("140")!=string::npos) *_LumiXSWeight = 99999/(0.0607*0.00246*0.10098*1000);
        if(params->suffix.find("145")!=string::npos) *_LumiXSWeight = 100284/(0.0544*0.00248*0.10098*1000);
        if(params->suffix.find("150")!=string::npos) *_LumiXSWeight = 100000/(0.0487*0.00231*0.10098*1000);

      }else if(params->suffix.find("wh") != string::npos){
        if(params->suffix.find("120")!=string::npos) *_LumiXSWeight = 65152/(0.6561*0.00111*0.10098*1000);
        if(params->suffix.find("125")!=string::npos) *_LumiXSWeight = 65507/(0.5729*0.00154*0.10098*1000);
        if(params->suffix.find("130")!=string::npos) *_LumiXSWeight = 65659/(0.5008*0.00195*0.10098*1000);
        if(params->suffix.find("135")!=string::npos) *_LumiXSWeight = 65461/(0.4390*0.00227*0.10098*1000);
        if(params->suffix.find("140")!=string::npos) *_LumiXSWeight = 65491/(0.3857*0.00246*0.10098*1000);
        if(params->suffix.find("145")!=string::npos) *_LumiXSWeight = 65491/(0.3406*0.00248*0.10098*1000);
        if(params->suffix.find("150")!=string::npos) *_LumiXSWeight = 65205/(0.3001*0.00231*0.10098*1000);

      }else if(params->suffix.find("zh") != string::npos){
        if(params->suffix.find("120")!=string::npos) *_LumiXSWeight = 34294/(0.3598*0.00111*0.10098*1000);
        if(params->suffix.find("125")!=string::npos) *_LumiXSWeight = 34492/(0.3158*0.00154*0.10098*1000);
        if(params->suffix.find("130")!=string::npos) *_LumiXSWeight = 34340/(0.2778*0.00195*0.10098*1000);
        if(params->suffix.find("135")!=string::npos) *_LumiXSWeight = 34538/(0.2453*0.00227*0.10098*1000);
        if(params->suffix.find("140")!=string::npos) *_LumiXSWeight = 34049/(0.2172*0.00246*0.10098*1000);
        if(params->suffix.find("145")!=string::npos) *_LumiXSWeight = 34653/(0.1930*0.00248*0.10098*1000);
        if(params->suffix.find("150")!=string::npos) *_LumiXSWeight = 34794/(0.1713*0.00231*0.10098*1000);
      }
      if (params->selection=="mumuGamma") *_LumiXSWeight = 5.05/(*_LumiXSWeight);
      if (params->selection=="eeGamma") *_LumiXSWeight = 4.98/(*_LumiXSWeight);

    }else if(params->period.find("2012") != string::npos){

      if(params->suffix.find("gg") != string::npos){
        if(params->suffix.find("120")!=string::npos) *_LumiXSWeight = 99992/(21.13*0.00111*0.10098*1000);
        //if(params->suffix.find("125")!=string::npos) *_LumiXSWeight = 99991/(19.52*0.00154*0.100974*1000);
        if(params->suffix.find("125")!=string::npos) *_LumiXSWeight = 100000/(19.52*0.00154*0.100974*1000);
        if(params->suffix.find("130")!=string::npos) *_LumiXSWeight = 99991/(18.07*0.00195*0.10098*1000);
        if(params->suffix.find("135")!=string::npos) *_LumiXSWeight = 99996/(16.79*0.00227*0.10098*1000);
        if(params->suffix.find("140")!=string::npos) *_LumiXSWeight = 96994/(15.63*0.00246*0.10098*1000);
        if(params->suffix.find("145")!=string::npos) *_LumiXSWeight = 99990/(14.59*0.00248*0.10098*1000);
        if(params->suffix.find("150")!=string::npos) *_LumiXSWeight = 99991/(13.65*0.00231*0.10098*1000);

      }else if(params->suffix.find("vbf") != string::npos){
        if(params->suffix.find("120")!=string::npos) *_LumiXSWeight = 99886/(1.649*0.00111*0.10098*1000);
        if(params->suffix.find("125")!=string::npos) *_LumiXSWeight = 99885/(1.578*0.00154*0.10098*1000);
        if(params->suffix.find("130")!=string::npos) *_LumiXSWeight = 99899/(1.511*0.00195*0.10098*1000);
        if(params->suffix.find("135")!=string::npos) *_LumiXSWeight = 99004/(1.448*0.00227*0.10098*1000);
        if(params->suffix.find("140")!=string::npos) *_LumiXSWeight = 99890/(1.389*0.00246*0.10098*1000);
        if(params->suffix.find("145")!=string::npos) *_LumiXSWeight = 99888/(1.333*0.00248*0.10098*1000);
        if(params->suffix.find("150")!=string::npos) *_LumiXSWeight = 99893/(1.280*0.00231*0.10098*1000);

      }else if(params->suffix.find("tth") != string::npos){
        if(params->suffix.find("120")!=string::npos) *_LumiXSWeight = 100080/(0.1470*0.00111*0.10098*1000);
        if(params->suffix.find("125")!=string::npos) *_LumiXSWeight = 100048/(0.1302*0.00154*0.10098*1000);
        if(params->suffix.find("130")!=string::npos) *_LumiXSWeight = 100368/(0.1157*0.00195*0.10098*1000);
        if(params->suffix.find("135")!=string::npos) *_LumiXSWeight = 100130/(0.1031*0.00227*0.10098*1000);
        if(params->suffix.find("140")!=string::npos) *_LumiXSWeight = 100340/(0.09207*0.00246*0.10098*1000);
        if(params->suffix.find("145")!=string::npos) *_LumiXSWeight = 97869/(0.08246*0.00248*0.10098*1000);
        if(params->suffix.find("150")!=string::npos) *_LumiXSWeight = 100048/(0.07403*0.00231*0.10098*1000);

      }else if(params->suffix.find("wh") != string::npos){
        if(params->suffix.find("120")!=string::npos) *_LumiXSWeight = 656153/(0.7966*0.00111*0.10098*1000);
        if(params->suffix.find("125")!=string::npos) *_LumiXSWeight = 656101/(0.6966*0.00154*0.10098*1000);
        if(params->suffix.find("130")!=string::npos) *_LumiXSWeight = 614920/(0.6095*0.00195*0.10098*1000);
        if(params->suffix.find("135")!=string::npos) *_LumiXSWeight = 328690/(0.5351*0.00227*0.10098*1000);
        if(params->suffix.find("140")!=string::npos) *_LumiXSWeight = 653556/(0.4713*0.00246*0.10098*1000);
        if(params->suffix.find("145")!=string::npos) *_LumiXSWeight = 652944/(0.4164*0.00248*0.10098*1000);
        if(params->suffix.find("150")!=string::npos) *_LumiXSWeight = 652973/(0.3681*0.00231*0.10098*1000);

      }else if(params->suffix.find("zh") != string::npos){
        if(params->suffix.find("120")!=string::npos) *_LumiXSWeight = 344220/(0.4483*0.00111*0.10098*1000);
        if(params->suffix.find("125")!=string::npos) *_LumiXSWeight = 344143/(0.3943*0.00154*0.10098*1000);
        if(params->suffix.find("130")!=string::npos) *_LumiXSWeight = 323352/(0.3473*0.00195*0.10098*1000);
        if(params->suffix.find("135")!=string::npos) *_LumiXSWeight = 173822/(0.3074*0.00227*0.10098*1000);
        if(params->suffix.find("140")!=string::npos) *_LumiXSWeight = 346650/(0.2728*0.00246*0.10098*1000);
        if(params->suffix.find("145")!=string::npos) *_LumiXSWeight = 347056/(0.2424*0.00248*0.10098*1000);
        if(params->suffix.find("150")!=string::npos) *_LumiXSWeight = 347459/(0.2159*0.00231*0.10098*1000);
      }
      if (params->selection=="mumuGamma"){
        if (params->abcd == "AB") *_LumiXSWeight = 5.3185/(*_LumiXSWeight);
        else if (params->abcd == "CD") *_LumiXSWeight = 14.299/(*_LumiXSWeight);
        else if (params->abcd == "ABCD") *_LumiXSWeight = 19.6175/(*_LumiXSWeight);
        else cout<<"abcd not specified"<<endl;
      }else if (params->selection=="eeGamma"){
        if (params->abcd == "AB") *_LumiXSWeight = 5.3185/(*_LumiXSWeight);
        else if (params->abcd == "CD") *_LumiXSWeight = 14.301/(*_LumiXSWeight);
        else if (params->abcd == "ABCD") *_LumiXSWeight = 19.6195/(*_LumiXSWeight);
        else cout<<"abcd not specified"<<endl;
      }
    }
  }
}

TMVA::Reader* higgsAnalyzer::MVAInitializer(){
  // -----------------  MVA stuff -----------------------------------------------------------
  tmvaReader = new TMVA::Reader("!Color:!Silent");

  //add  variables... some are exclusive to particular sample/jet multi discriminator
  //
  if(params->doAltMVA){
    tmvaReader->AddVariable("comPhi", &(mvaVars._comPhi));
    tmvaReader->AddVariable("threeBodyPtOM", &(mvaVars._threeBodyPtOM));
    tmvaReader->AddVariable("l2Eta", &(mvaVars._l2Eta));
    tmvaReader->AddVariable("GEta", &(mvaVars._GEta));
    tmvaReader->AddVariable("bigTheta", &(mvaVars._bigTheta));
    tmvaReader->AddVariable("smallTheta", &(mvaVars._smallTheta));
    tmvaReader->AddVariable("l1Eta", &(mvaVars._l1Eta));
  }else{
    //if (params->selection =="mumuGamma"){
    //  tmvaReader->AddVariable("comPhi", &(mvaVars._comPhi));
    //  tmvaReader->AddVariable("threeBodyPtOM", &(mvaVars._threeBodyPtOM));
    //  tmvaReader->AddVariable("l2Eta", &(mvaVars._l2Eta));
    //  tmvaReader->AddVariable("GEta", &(mvaVars._GEta));
    //  tmvaReader->AddVariable("bigTheta", &(mvaVars._bigTheta));
    //  tmvaReader->AddVariable("l1Eta", &(mvaVars._l1Eta));
    //}else if (params->selection=="eeGamma"){
      tmvaReader->AddVariable("threeBodyPtOM", &(mvaVars._threeBodyPtOM));
      tmvaReader->AddVariable("l2Eta", &(mvaVars._l2Eta));
      tmvaReader->AddVariable("GEta", &(mvaVars._GEta));
      tmvaReader->AddVariable("bigTheta", &(mvaVars._bigTheta));
      tmvaReader->AddVariable("smallTheta", &(mvaVars._smallTheta));
      tmvaReader->AddVariable("l1Eta", &(mvaVars._l1Eta));
    //}
  }
  
  //tmvaReader->AddVariable("medisc", &(mvaVars._medisc));
  //tmvaReader->AddVariable("comPhi", &(mvaVars._comPhi));
  //tmvaReader->AddVariable("GPtOM", &(mvaVars._GPtOM));
  //tmvaReader->AddVariable("diffZGvectorOM", &(mvaVars._diffZGvectorOM));
  //tmvaReader->AddVariable("ZPtOM", &(mvaVars._ZPtOM));
  //tmvaReader->AddVariable("R9var", &(mvaVars._R9));
  //tmvaReader->AddVariable("sieip", &(mvaVars._sieip));
  //tmvaReader->AddVariable("sipip", &(mvaVars._sipip));
  //tmvaReader->AddVariable("SCRawE", &(mvaVars._SCRawE));
  //tmvaReader->AddVariable("SCPSE", &(mvaVars._SCPSE));
  //tmvaReader->AddVariable("e5x5", &(mvaVars._e5x5));
  //tmvaReader->AddVariable("e2x2", &(mvaVars._e2x2));
  //tmvaReader->AddVariable("SCRawEOPt", &(mvaVars._SCRawEOPt));
  //tmvaReader->AddVariable("SCPSEOPt", &(mvaVars._SCPSEOPt));
  //tmvaReader->AddVariable("e2x2O5x5", &(mvaVars._e2x2O5x5));
  //tmvaReader->AddVariable("ZEta", &(mvaVars._ZEta));
  //tmvaReader->AddVariable("threeBodyEta", &(mvaVars._threeBodyEta));
  //tmvaReader->AddVariable("GPtOHPt", &(mvaVars._GPtOHPt));
  
  tmvaReader->AddSpectator("threeBodyMass",&(mvaVars._threeBodyMass));

  // Book the methods
  // for testing we will set only the BDT and hotwire this loop

  int discr = BDTG;
  //int discr = BDT;
  //int discr = MLPBNN;

  for (int mh = 0; mh < 1; ++mh) {

    TString label = TString::Format("%s_%s_%s_ggM%i_%s", mvaInits.discrMethodName[discr].Data(), mvaInits.discrSelection.Data(), mvaInits.discrSampleName.Data(),
        mvaInits.mvaHiggsMassPoint[mh], mvaInits.discrSuffixName.Data());

    TString weightFile = TString::Format("%sdiscr_%s_%s_ggM%i_%s__%s.weights.xml",
        mvaInits.weightsDir.Data(), mvaInits.discrSelection.Data(), mvaInits.discrSampleName.Data(), mvaInits.mvaHiggsMassPoint[mh], mvaInits.discrSuffixName.Data(), mvaInits.discrMethodName[discr].Data());

    tmvaReader->BookMVA(label.Data(), weightFile.Data());
    

  }
  return tmvaReader;
  // ------------------ End of MVA stuff --------------------------------------------------------------------------
}

float higgsAnalyzer::MVACalculator (mvaInitStruct inits, TMVA::Reader* _tmvaReader){

  // -------------------------- MVA stuff -------------------------------------------
  // the sequence of cuts is a bit different for the pre-selection
  // thi si the latest where we can plug in the BDT for now
  // *additional* cuts for the BDT pre-selection on top of the ones already applied
  // assign values to the variables used in the BDT


  // get the MVA discriminators for the considered methods


  //                    [mva method][higgs mass point]
  Float_t tmvaValue[3] = {-999};

  int discr = BDTG; // use only this one for now
  //int discr = BDT; // use only this one for now
  //int discr = MLPBNN; // use only this one for now


  TString label = TString::Format("%s_%s_%s_ggM%i_%s", inits.discrMethodName[discr].Data(), inits.discrSelection.Data(), inits.discrSampleName.Data(),
      inits.mvaHiggsMassPoint[0], inits.discrSuffixName.Data());
  tmvaValue[discr] = _tmvaReader->EvaluateMVA(label.Data());
  return tmvaValue[discr];


}


float higgsAnalyzer::MEDiscriminator(TCPhysObject lepton1, TCPhysObject lepton2, TLorentzVector gamma){
  //modified from kevin kelly

  hzgamma_event_type hzgamma_event;

  float dXsec_ZGam_MCFM = 0.;
  float dXsec_HZGam_MCFM = 0.;
  float Discriminant = 0.;
  //float logBkg(0.), logSig(0.);
  
  auto_ptr<TLorentzVector> pl1(new TLorentzVector(lepton1));
  auto_ptr<TLorentzVector> pl2(new TLorentzVector(lepton2));
  auto_ptr<TLorentzVector> pg(new TLorentzVector(gamma));
  TLorentzVector psum = *pl1 + *pl2 + *pg;

  TVector3 bv = -psum.BoostVector();
  pl1->Boost(bv); pl2->Boost(bv); pg->Boost(bv);

  if (lepton1.Charge() == -1){
    hzgamma_event.p[0].SetPxPyPzE(pl1->Px(), pl1->Py(), pl1->Pz(), pl1->Energy());
    hzgamma_event.p[1].SetPxPyPzE(pl2->Px(), pl2->Py(), pl2->Pz(), pl2->Energy());
    hzgamma_event.p[2].SetPxPyPzE(pg->Px(), pg->Py(), pg->Pz(), pg->Energy());
  }else{
    hzgamma_event.p[1].SetPxPyPzE(pl1->Px(), pl1->Py(), pl1->Pz(), pl1->Energy());
    hzgamma_event.p[0].SetPxPyPzE(pl2->Px(), pl2->Py(), pl2->Pz(), pl2->Energy());
    hzgamma_event.p[2].SetPxPyPzE(pg->Px(), pg->Py(), pg->Pz(), pg->Energy());
  }

  if (params->selection == "mumuGamma"){
    hzgamma_event.PdgCode[0] = 13;
    hzgamma_event.PdgCode[1] = -13;
    hzgamma_event.PdgCode[2] = 22;
  }else{
    hzgamma_event.PdgCode[0] = 11;
    hzgamma_event.PdgCode[1] = -11;
    hzgamma_event.PdgCode[2] = 22;
  }

  //float zmass = (hzgamma_event.p[0]+hzgamma_event.p[1]).M();
  //float gammass = (hzgamma_event.p[2]).M();
  float zgammass = (hzgamma_event.p[0]+hzgamma_event.p[1]+hzgamma_event.p[2]).M();

  Xcal2->SetHiggsMass(zgammass);
  Xcal2->SetMatrixElement(TVar::MCFM);

  // hacky bullshit to prevent XsecCalc from blowing up stdout
  fpos_t pos;
  fflush(stdout);
  fgetpos(stdout, &pos);
  int fd = dup(fileno(stdout));
  char fname[] = "garbage.txt";
  freopen(fname, "a+", stdout);   
  printf("inside file op");  

  dXsec_ZGam_MCFM = Xcal2->XsecCalc(TVar::qqb_zgam, TVar::QQB, hzgamma_event,false);
  dXsec_HZGam_MCFM = Xcal2->XsecCalc(TVar::gg_hzgam, TVar::GG, hzgamma_event,false);

  fflush(stdout);
  dup2(fd,fileno(stdout));
  close(fd);
  clearerr(stdout);
  fsetpos(stdout, &pos);
  // hacky bullshit end 

  //logBkg = -log10(dXsec_ZGam_MCFM);
  //logSig = -log10(dXsec_HZGam_MCFM);
  Discriminant = -log(dXsec_ZGam_MCFM/(dXsec_ZGam_MCFM+dXsec_HZGam_MCFM));

  return Discriminant;

}
void higgsAnalyzer::PhotonR9Corrector(TCPhoton& ph){
  //old R9 correction
  float R9Cor;
  if (params->PU == "S10"){
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
          R9Cor = 0.000740 + ph.R9()*1.00139; 
        }else{
          R9Cor = -0.000399 + ph.R9()*1.00016;
        }
      }
    }
  }
  ph.SetR9(R9Cor);
}

bool higgsAnalyzer::GoodLeptonsCat(const TCMuon& m1, const TCMuon& m2){
  bool _goodLep = false;
  _goodLep = (fabs(m1.Eta()) < 0.9 || fabs(m2.Eta()) < 0.9) && (fabs(m1.Eta()) < 2.1 && fabs(m2.Eta()) < 2.1);
  return _goodLep;
}

bool higgsAnalyzer::GoodLeptonsCat(const TCElectron& e1, const TCElectron& e2){
  bool _goodLep = false;
  _goodLep = (fabs(e1.SCEta()) < 1.4442 && fabs(e2.SCEta()) < 1.4442);
  return _goodLep;
}

bool higgsAnalyzer::GoodLeptonsCat(const TCPhysObject& m1, const TCPhysObject& m2){
  bool _goodLep = false;
  _goodLep = (fabs(m1.Eta()) < 0.9 || fabs(m2.Eta()) < 0.9) && (fabs(m1.Eta()) < 2.1 && fabs(m2.Eta()) < 2.1);
  return _goodLep;
}

bool higgsAnalyzer::GoodLeptonsCat(const float SCEta1, const float SCEta2){
  bool _goodLep = false;
  _goodLep = (fabs(SCEta1) < 1.4442 && fabs(SCEta2) < 1.4442);
  return _goodLep;
}

bool higgsAnalyzer::SpikeVeto(const TCPhoton& ph){
  bool veto = false;
  //  cout<<"SCeta: "<<ph.SCEta()<<" SCphi: "<<ph.SCPhi()<<endl;
  float dEta = ph.SCEta()-(-1.76);
  float dPhi = TVector2::Phi_mpi_pi(ph.SCPhi()-1.37);
  float spikeDR = sqrt(dEta*dEta+dPhi*dPhi);
  //cout<<"dEta: "<<dEta<<" dPhi: "<<dPhi<<" spikeDr: "<<spikeDR<<endl;
  if (spikeDR < 0.05){
    veto = true;
    return veto;
  }
  dEta = ph.SCEta()-(2.37);
  dPhi = TVector2::Phi_mpi_pi(ph.SCPhi()-2.69);
  spikeDR = sqrt((dEta*dEta)+(dPhi*dPhi));
  if (spikeDR < 0.05){
    veto = true;
    return veto;
  }

  return veto;
}

void higgsAnalyzer::UniversalEnergyCorrector(TCPhoton& ph, vector<TCGenParticle>& _genPhotons){
  // Section for photon energy/momentum corrections.  NOTE: this will change the pt and thus ID/ISO of photon


  float corrPhoPt = -1;
  int periodNum   = -1;
  if (params->period.find("2011") != string::npos) periodNum = 2011;
  if (params->period.find("2012") != string::npos) periodNum = 2012;
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

void higgsAnalyzer::UniversalEnergyCorrector(TCMuon& mu){

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

void higgsAnalyzer::UniversalEnergyCorrector(TCElectron& el){
  if (params->period.find("2012") != string::npos){
    if (el.RegressionMomCombP4().E() != 0){
      el.SetPtEtaPhiM(el.RegressionMomCombP4().Pt(),el.RegressionMomCombP4().Eta(),el.RegressionMomCombP4().Phi(),0.000511);
    }
    if (eventNumber == params->EVENTNUMBER){
      cout<< "electron after cor: "<<TCPhysObject(el)<<endl;
    }
  }
}

void higgsAnalyzer::BadLeptonPrune(vector<TCMuon>& myLeptons, const ParticleSelector::genHZGParticles& myGens)
{
  if (!myGens.lm || !myGens.lp) {
    myLeptons.clear();
    return;
  }
  vector<TCMuon>::iterator lep = myLeptons.begin();
  while(lep != myLeptons.end()){
    if (lep->Charge() == -1 && lep->DeltaR(*(myGens.lm)) > 0.02){
      lep = myLeptons.erase(lep);
    }else if(lep->Charge() == 1 && lep->DeltaR(*(myGens.lp)) > 0.02){
      lep = myLeptons.erase(lep);
    }else{
      lep++;
    }
  }
  return;
}
      
void higgsAnalyzer::BadLeptonPrune(vector<TCElectron>& myLeptons, const ParticleSelector::genHZGParticles& myGens)
{
  if (!myGens.lm || !myGens.lp) {
    myLeptons.clear();
    return;
  }
  vector<TCElectron>::iterator lep = myLeptons.begin();
  while(lep != myLeptons.end()){
    if (lep->Charge() == -1 && lep->DeltaR(*(myGens.lm)) > 0.02){
      lep = myLeptons.erase(lep);
    }else if(lep->Charge() == 1 && lep->DeltaR(*(myGens.lp)) > 0.02){
      lep = myLeptons.erase(lep);
    }else{
      lep++;
    }
  }
  return;
}

float higgsAnalyzer::ElectronMCScale(float eta, float pt){
  /*
   MC->Data scale correction
 $0 < |\eta| < 0.4$ & $0.998107$ \pm $-1$ \\
 $0.4 < |\eta| < 0.8$ & $0.99943$ \pm $-1$ \\
 $0.8 < |\eta| < 1.2$ & $1.00714$ \pm $-1$ \\
 $1.2 < |\eta| < 1.4442$ & $1.01949$ \pm $-1$ \\
 $1.4442 < |\eta| < 2.1$ & $0.999455$ \pm $-1$ \\
 $2.1 < |\eta| < 2.5$ & $0.995461$ \pm $-1$ \\
 
   MC->Data resolution correction [GeV]
 0 < |\eta| < 0.4 & $0.0298524$ \pm $0.00240817$ \\
 0.4 < |\eta| < 0.8 & $0.0399948$ \pm $0.00203437$ \\
 0.8 < |\eta| < 1.2 & $0.036199$ \pm $0.0105744$ \\
 1.2 < |\eta| < 1.4442 & $0.0676282$ \pm $4.55263$ \\
 1.4442 < |\eta| < 2.1 & $0.125098$ \pm $0.00575003$ \\
 2.1 < |\eta| < 2.5 & $0.359072$ \pm $0.00185872$ \\
 */

  float scaleWeights[6] = {0.998107, 0.99943, 1.00714, 1.01949, 0.999455, 0.995461};
  float sigmaWeights[6] = {0.0298524, 0.0399948, 0.036199, 0.0676282, 0.125098, 0.359072};
  float ptScale = 1.0;
  float ptSmear = 1.0;

  if (fabs(eta) < 0.4){
    ptScale = scaleWeights[0];
    ptSmear = rnGenerator->Gaus(1,(sigmaWeights[0])/pt);
  }else if(fabs(eta) >= 0.4 && fabs(eta) < 0.8){
    ptScale = scaleWeights[1];
    ptSmear = rnGenerator->Gaus(1,(sigmaWeights[1])/pt);
  }else if(fabs(eta) >= 0.8 && fabs(eta) < 1.2){
    ptScale = scaleWeights[2];
    ptSmear = rnGenerator->Gaus(1,(sigmaWeights[2])/pt);
  }else if(fabs(eta) >= 1.2 && fabs(eta) < 1.4442){
    ptScale = scaleWeights[3];
    ptSmear = rnGenerator->Gaus(1,(sigmaWeights[3])/pt);
  }else if(fabs(eta) >= 1.4442 && fabs(eta) < 2.1){
    ptScale = scaleWeights[4];
    ptSmear = rnGenerator->Gaus(1,(sigmaWeights[4])/pt);
  }else if(fabs(eta) >= 2.1 && fabs(eta) < 2.5){
    ptScale = scaleWeights[5];
    ptSmear = rnGenerator->Gaus(1,(sigmaWeights[5])/pt);
  }
  
  //cout<<ptSmear<<endl;
  return ptScale*ptSmear;
}

