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

  params.reset(new Parameters());

  params->selection      = selection;
  params->period         = period;
  params->JC_LVL         = 0;
  params->abcd           = abcd;
  params->suffix         = suffix;
  params->dataname       = dataname;
  params->jobCount       = count;


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

  // Initialize utilities and selectors here //
  cuts.reset(new Cuts());
  cuts->InitEA(params->period);
  weighter.reset(new WeightUtils(*params, isRealData, runNumber));
  triggerSelector.reset(new TriggerSelector(params->selection, params->period, *triggerNames));
  rmcor2011.reset(new rochcor_2011(229));
  rmcor2012.reset(new rochcor2012(229));
  rEl.reset(new TRandom3(1234));
  rMuRun.reset(new TRandom3(187));
  phoCorrector.reset(new zgamma::PhosphorCorrectionFunctor("../plugins/PHOSPHOR_NUMBERS_EXPFIT_ERRORS.txt", true));
  Xcal2.reset(new TEvtProb);
  particleSelector.reset(new ParticleSelector(*params, *cuts, isRealData, runNumber, *rEl));
  dumper.reset(new Dumper(*params,*cuts,*particleSelector));


  //genHZG = {0,0,0,0,0,0};

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
  histoFile->mkdir("PreSel", "PreSel");
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

  ///////////////////////
  // ZGAngles MVA init //
  ///////////////////////

  // TMVA weights directory
  mvaInits.weightsDir = "../mva/testWeights/";

  // here we will use only BDTG... but will keep the structure 
  mvaInits.discrMethodName[0] = "MLPBNN";
  mvaInits.discrMethodName[1] = "BDTG";
  mvaInits.discrMethodName[2] = "BDT";

  mvaInits.discrSelection = params->selection;
  mvaInits.discrSampleName = "allBG";
  //mvaInits.discrSuffixName = "anglesOnly";
  //mvaInits.discrSuffixName = "newAnglesR9";
  //mvaInits.discrSuffixName = "01-29-14_v0905";
  mvaInits.discrSuffixName = "02-01-14";


  mvaInits.mvaHiggsMassPoint[0] = 125;

  mvaInits.bdtCut[0] = -0.1;
  if (params->doAnglesMVA) tmvaReader = MVAInitializer();
}



Bool_t higgsAnalyzer::Process(Long64_t entry)
{

  GetEntry(entry,1);
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
  dumper->SetRho(rhoFactor);
  dumper->SetRun(runNumber);
  dumper->SetEvent(eventNumber);
  dumper->SetLumi(lumiSection);

  ///////////////////
  // Gen Particles //
  ///////////////////
  

  vector<TCGenParticle> vetoPhotons;
  particleSelector->CleanUpGen(genHZG);
  //genHZG = {0,0,0,0,0,0};
  if(!isRealData){
    ///////// load all the relevent particles into a struct /////////
    particleSelector->FindGenParticles(*genParticles, vetoPhotons, genHZG);

    ///////// whzh decomposition /////////////////

    if (params->suffix.find("zh") != string::npos && genHZG.w) return kTRUE;
    if (params->suffix.find("wh") != string::npos && !genHZG.w) return kTRUE;

    ///////// gen angles, plots before any kinematic/fiducial cleaning //////////////

    ZGLabVectors genLevelInputs;
    ZGAngles     genLevelOutputs;


    if(genHZG.lp && genHZG.lm && genHZG.g){
      if(genHZG.lp->Pt() > 8 && genHZG.lm->Pt() > 8){
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
        //cout<<"costheta_lm: "<<genLevelOutputs.costheta_lm<<"\tcostheta_lp: "<<genLevelOutputs.costheta_lp<<"\tphi: "<<genLevelOutputs.phi<<"\tcosTheta: "<<genLevelOutputs.cosTheta<<"\tcosThetaG: "<<genLevelOutputs.cosThetaG<<endl;
      }
    }

    /*
    //////////// DYJets Gamma Veto ////////////
    vector<TCGenParticle>::iterator testIt;

    if (params->DYGammaVeto && (params->suffix.find("DYJets") != string::npos)){
      if (vetoPhotons.size() > 0){
        //cout<<"photon mother ID:"<<endl;
        for (testIt=vetoPhotons.begin(); testIt<vetoPhotons.end(); testIt++){
            // if the photon's mother and grandmother is a lepton, kill it
          if ((*testIt).Mother() && (abs((*testIt).Mother()->GetPDGId()) == 11 || abs((*testIt).Mother()->GetPDGId()) == 13 || abs((*testIt).Mother()->GetPDGId()) == 15) && ((*testIt).Mother()->Mother())
              && (abs((*testIt).Mother()->Mother()->GetPDGId()) == 11 || abs((*testIt).Mother()->Mother()->GetPDGId()) == 13 || abs((*testIt).Mother()->Mother()->GetPDGId()) == 15) ) return kTRUE; 
            // if the photon's mother is a lepton, and the grandmother is a Z or W, kill it
          if ((*testIt).Mother() && (abs((*testIt).Mother()->GetPDGId()) == 11 || abs((*testIt).Mother()->GetPDGId()) == 13 || abs((*testIt).Mother()->GetPDGId()) == 15) && ((*testIt).Mother()->Mother())
              && (abs((*testIt).Mother()->Mother()->GetPDGId()) == 23 || abs((*testIt).Mother()->Mother()->GetPDGId()) == 24) ) return kTRUE;
            // if the photon's mother is a photon, and the grandmother is an electron, kill it 
          if ((*testIt).Mother() && abs((*testIt).Mother()->GetPDGId()) == 22 && ((*testIt).Mother()->Mother()) && abs((*testIt).Mother()->Mother()->GetPDGId()) == 11) return kTRUE;
            // if the photon's mother is a gluon (?!) or a quark, kill it
          if ((*testIt).Mother() && (abs((*testIt).Mother()->GetPDGId()) == 21 || abs((*testIt).Mother()->GetPDGId()) < 7)) return kTRUE;
        }


      }
    }
    */

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
/*
  cout<<"new trig event"<<endl;
  for (int i = 0; i <  triggerObjects->GetSize(); ++i) {
    TCTriggerObject* thisTrigObj = (TCTriggerObject*) triggerObjects->At(i);    
    cout<<" HLT: "<<thisTrigObj->GetHLTName()<<endl;
    cout<<" module: "<<thisTrigObj->GetModuleName()<<endl;
    cout<<" id: "<<thisTrigObj->GetId()<<endl;
    thisTrigObj->Print();
    cout<<endl;
  }
*/
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
  vector<TCPrimaryVtx> pvVect;
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
  vector<TCElectron> electronsIDIsoUnCor;
  TCElectron * cloneElectron(0);
  //int eleCount = 0;


  for (int i = 0; i <  recoElectrons->GetSize(); ++i) {
    TCElectron* thisElec = (TCElectron*) recoElectrons->At(i);    



    // Section for electron energy/momentum corrections.  NOTE: this will change the pt and thus ID/ISO of electron

    thisElec->SetPtEtaPhiM(thisElec->Pt(),thisElec->Eta(),thisElec->Phi(),0.000511);
    

    // loose cuts on electron for diLepton requirements

    

    if(params->doEleMVA){
      bool passAll = false;

      //dumper->MVADumper(*thisElec, myMVATrig.get()); 

      if (particleSelector->PassElectronID(*thisElec, cuts->mvaPreElID, *recoMuons)) electronsID.push_back(*thisElec);			
      /// low pt
      if (thisElec->Pt() < 20 && particleSelector->PassElectronID(*thisElec, cuts->mvaPreElID, *recoMuons) && thisElec->MvaID_Old() > -0.5 && particleSelector->PassElectronIso(*thisElec, cuts->looseElIso, cuts->EAEle)){
        passAll = true;
      /// high pt
      }else if (thisElec->Pt() > 20 && particleSelector->PassElectronID(*thisElec, cuts->mvaPreElID, *recoMuons) && thisElec->MvaID_Old() > -0.90 && particleSelector->PassElectronIso(*thisElec, cuts->looseElIso, cuts->EAEle)){
        passAll = true;
      }
      //cout<<"MVA OLD: "<<thisElec->MvaID_Old()<<" MVA HZZ: "<<thisElec->MvaID()<<endl;
      /*
      if (thisElec->Pt() > 5 && thisElec->ConversionMissHits() <=1) electronsID.push_back(*thisElec);			
      /// inner barrel, low pt
      if (thisElec->Pt() > 5 && thisElec->Pt() < 10){
          if (fabs(thisElec->SCEta()) < 0.8
                  && thisElec->MvaID() > 0.47
                  && particleSelector->PassElectronIso(*thisElec, cuts->looseElIso, cuts->EAEle)){
              passAll = true;
      /// outer barrel, low pt
          }else if(fabs(thisElec->SCEta()) > 0.8 && fabs(thisElec->SCEta()) < 1.479
                  && thisElec->MvaID() > 0.004
                  && particleSelector->PassElectronIso(*thisElec, cuts->looseElIso, cuts->EAEle)){
              passAll = true;
      /// endcap, low pt
          }else if(fabs(thisElec->SCEta()) > 1.479
                  && thisElec->MvaID() > 0.295
                  && particleSelector->PassElectronIso(*thisElec, cuts->looseElIso, cuts->EAEle)){
              passAll = true;
          }
      }else if (thisElec->Pt() > 10){
      /// inner barrel, high pt
          if (fabs(thisElec->SCEta()) < 0.8
                  && thisElec->MvaID() > -0.34
                  && particleSelector->PassElectronIso(*thisElec, cuts->looseElIso, cuts->EAEle)){
              passAll = true;
      /// outer barrel, high pt
          }else if(fabs(thisElec->SCEta()) > 0.8 && fabs(thisElec->SCEta()) < 1.479
                  && thisElec->MvaID() > -0.65
                  && particleSelector->PassElectronIso(*thisElec, cuts->looseElIso, cuts->EAEle)){
              passAll = true;
      /// endcap, high pt
          }else if(fabs(thisElec->SCEta()) > 1.479
                  && thisElec->MvaID() > 0.6
                  && particleSelector->PassElectronIso(*thisElec, cuts->looseElIso, cuts->EAEle)){
              passAll = true;
          }

      }
      */

      if (passAll){
        cloneElectron = thisElec;
        electronsIDIso.push_back(*thisElec);			
        if (params->EVENTNUMBER == eventNumber) cout<<"regE: "<<thisElec->IdMap("EnergyRegression")<<" regE-p: "<<thisElec->RegressionMomCombP4().E()<<endl;
        if (params->engCor || params->doEleReg) electronsIDIsoUnCor.push_back(*cloneElectron);
      }
                                                                  

    }else{
        /* old style
      if(params->engCor){
        float energyElCor;
        if ( params->period.find("2011") != string::npos ){
          energyElCor = correctedElectronEnergy( thisElec->E(), thisElec->SCEta(), thisElec->R9(), runNumber, 0, "2011", !isRealData, rEl.get() );
        }else{
          energyElCor = correctedElectronEnergy( thisElec->E(), thisElec->SCEta(), thisElec->R9(), runNumber, 0, "HCP2012", !isRealData, rEl.get() );
        }
        float newPt = sqrt((pow(energyElCor,2)-pow(0.000511,2))/pow(cosh(thisElec->Eta()),2));
        cloneElectron = thisElec;
        thisElec->SetPtEtaPhiM(newPt,thisElec->Eta(),thisElec->Phi(),0.000511);
      }
      */
      if(params->engCor){
        float energyElCor;
        if ( params->period.find("2011") != string::npos ){
          energyElCor = correctedElectronEnergy( thisElec->E(), thisElec->SCEta(), thisElec->R9(), runNumber, 0, "2011", !isRealData, rEl.get() );
            float newPt = sqrt((pow(energyElCor,2)-pow(0.000511,2))/pow(cosh(thisElec->Eta()),2));
            cloneElectron = thisElec;
            thisElec->SetPtEtaPhiM(newPt,thisElec->Eta(),thisElec->Phi(),0.000511);
        }
      }
      dumper->ElectronDump(*thisElec, *recoMuons, false);
      if (particleSelector->PassElectronID(*thisElec, cuts->looseElID, *recoMuons)) electronsID.push_back(*thisElec);			
      if (particleSelector->PassElectronID(*thisElec, cuts->looseElID, *recoMuons) && particleSelector->PassElectronIso(*thisElec, cuts->looseElIso, cuts->EAEle)){
        electronsIDIso.push_back(*thisElec);			
        if (params->engCor) electronsIDIsoUnCor.push_back(*cloneElectron);
      }
    } 

    // 3rd lepton veto

  }

  sort(electronsID.begin(), electronsID.end(), P4SortCondition);
  sort(electronsIDIso.begin(), electronsIDIso.end(), P4SortCondition);


  ///////////
  // muons //
  ///////////

  vector<TCMuon> muonsID;
  vector<TCMuon> muonsIDIso;
  vector<TCMuon> muonsIDIsoUnCor;
  TLorentzVector tmpMuCor;
  TCMuon* cloneMuon(0);

  /*


     float EAMu[6] = {     //combined rel ISO, 2012 Data, 1.0 GeV
     0.340, //         eta < 1.0
     0.310, // 1.0   < eta < 1.5
     0.315, // 1.5   < eta < 2.0
     0.415, // 2.0   < eta < 2.2
     0.658, // 2.2   < eta < 2.3
     0.405, // 2.3   < eta < 2.4
     };
     */



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
      cloneMuon = thisMuon;
      thisMuon->SetPtEtaPhiM(tmpMuCor.Pt(), tmpMuCor.Eta(), tmpMuCor.Phi(), tmpMuCor.M());
    }


    // tight muon id

    dumper->MuonDump(*thisMuon, false);

    if (particleSelector->PassMuonID(*thisMuon, cuts->tightMuID)){
      muonsID.push_back(*thisMuon);
    }

    //tight ID and Iso

    if (params->doLooseMuIso){
      if (particleSelector->PassMuonID(*thisMuon, cuts->tightMuID) && particleSelector->PassMuonIso(*thisMuon, cuts->looseMuIso)){
        muonsIDIso.push_back(*thisMuon);
        if (params->engCor) muonsIDIsoUnCor.push_back(*cloneMuon);
      }
    }else{
      if (particleSelector->PassMuonID(*thisMuon, cuts->tightMuID) && particleSelector->PassMuonIso(*thisMuon, cuts->tightMuIso)){
        muonsIDIso.push_back(*thisMuon);
        if (params->engCor) muonsIDIsoUnCor.push_back(*cloneMuon);
      }
    }

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
  vector<TCPhoton> photonsIDIsoUnCor; 
  TCPhoton* clonePhoton;


  if (params->selection == "mumuGamma" || params->selection == "eeGamma") {
    for (Int_t i = 0; i < recoPhotons->GetSize(); ++i) {
      //cout<<endl;
      //cout<<"new photon!!!!!!!"<<endl;
      vector<float> TrkIsoVec;
      TCPhoton* thisPhoton = (TCPhoton*) recoPhotons->At(i);

      if (params->spikeVeto && (params->period == "2012A_Jul13" || params->period == "2012A_Aug06rec" || params->period == "2012B_Jul13")){
      //  cout<<"SCeta: "<<thisPhoton->SCEta()<<" SCphi: "<<thisPhoton->SCPhi()<<endl;
        float dEta = thisPhoton->SCEta()-(-1.76);
        float dPhi = TVector2::Phi_mpi_pi(thisPhoton->SCPhi()-1.37);
        float spikeDR = sqrt(dEta*dEta+dPhi*dPhi);
        //cout<<"dEta: "<<dEta<<" dPhi: "<<dPhi<<" spikeDr: "<<spikeDR<<endl;
        if (spikeDR < 0.05) continue;
        dEta = thisPhoton->SCEta()-(2.37);
        dPhi = TVector2::Phi_mpi_pi(thisPhoton->SCPhi()-2.69);
        spikeDR = sqrt((dEta*dEta)+(dPhi*dPhi));
        if (spikeDR < 0.05) continue;
      }

      // Section for photon energy/momentum corrections.  NOTE: this will change the pt and thus ID/ISO of photon
      
      if(params->engCor){
        //old R9 correction
        if (params->suffix.find("S10") != string::npos){
          R9Cor = thisPhoton->R9();
          if (params->doR9Cor){
            if (params->suffix != "DATA"){
              if (params->period == "2011"){
                if (fabs(thisPhoton->SCEta()) < 1.479){
                  R9Cor = thisPhoton->R9()*1.0048;
                }else{
                  R9Cor = thisPhoton->R9()*1.00492;
                }
              } else if (params->period == "2012"){
                if (fabs(thisPhoton->SCEta()) < 1.479){
                  R9Cor = thisPhoton->R9()*1.0045 + 0.0010;
                }else{
                  R9Cor = thisPhoton->R9()*1.0086 - 0.0007;
                }
              }
            }
          }
        //new R9 correction
        }else{
          R9Cor = thisPhoton->R9();
          if (params->doR9Cor){
            if (params->suffix != "DATA"){
              if (params->period == "2011"){
                if (fabs(thisPhoton->SCEta()) < 1.479){
                  R9Cor = thisPhoton->R9()*1.00153 + 0.000854;
                }else{
                  R9Cor = thisPhoton->R9()*1.00050+ 0.001231;
                }
              } else if (params->period == "2012"){
                if (fabs(thisPhoton->SCEta()) < 1.479){
                  R9Cor = thisPhoton->R9()*1.00139 + 0.000740;
                }else{
                  R9Cor = thisPhoton->R9()*1.00016- 0.000399;
                }
              }
            }
          }
        }

        clonePhoton = thisPhoton;
        float corrPhoPt = -1;
        int periodNum   = -1;
        if (params->period.find("2011") != string::npos) periodNum = 2011;
        if (params->period.find("2012") != string::npos) periodNum = 2012;
        if(!isRealData && thisPhoton->Pt()>10.){
          TCGenParticle goodGenPhoton;
          float testDr = 9999;
          int vetoPos = -1;
          //cout<<"veto photon size: "<<vetoPhotons.size()<<endl;
          for (UInt_t j = 0; j<vetoPhotons.size(); j++){
            //if (vetoPhotons[j].Mother() && fabs(vetoPhotons[j].Mother()->GetPDGId()) == 22) cout<<"mother: "<<vetoPhotons[j].Mother()->GetPDGId()<<endl;
            //if(thisPhoton->DeltaR(vetoPhotons[j]) < 0.2 && vetoPhotons[j].GetStatus()==1 && vetoPhotons[j].Mother() && fabs(vetoPhotons[j].Mother()->GetPDGId()) == 22){
            if(vetoPhotons[j].Mother() && (fabs(vetoPhotons[j].Mother()->GetPDGId()) == 25 || fabs(vetoPhotons[j].Mother()->GetPDGId()) == 22) && vetoPhotons[j].GetStatus()==1){
              if(thisPhoton->DeltaR(vetoPhotons[j])<testDr){
                goodGenPhoton = vetoPhotons[j];
                testDr = thisPhoton->DeltaR(vetoPhotons[j]);
                vetoPos = j;
                //cout<<"testdr: "<<testDr<<endl;
              }
            }
          }
          if (testDr < 0.2){
            corrPhoPt = phoCorrector->GetCorrEtMC(R9Cor, periodNum, thisPhoton->Pt(), thisPhoton->Eta(), goodGenPhoton.E());
            //cout<<"uncor pt: "<<thisPhoton->Pt()<<" cor pt: "<<corrPhoPt<<" gen pt: "<<vetoPhotons[vetoPos].Pt()<<endl;
            thisPhoton->SetPtEtaPhiM(corrPhoPt,thisPhoton->Eta(),thisPhoton->Phi(),0.0);
          }
          //else cout<<" no match, pt: "<<thisPhoton->Pt()<<endl;
        }else if (isRealData && thisPhoton->Pt()>10.){
          corrPhoPt = phoCorrector->GetCorrEtData(thisPhoton->R9(), periodNum, thisPhoton->Pt(), thisPhoton->Eta());
          thisPhoton->SetPtEtaPhiM(corrPhoPt,thisPhoton->Eta(),thisPhoton->Phi(),0.0);
        }
       
      }


      ////// Currently Using Cut-Based Photon ID, 2012

      dumper->PhotonDump(*thisPhoton); 
      if (particleSelector->PassPhotonID(*thisPhoton, cuts->mediumPhID)) photonsID.push_back(*thisPhoton);
      if (particleSelector->PassPhotonID(*thisPhoton, cuts->mediumPhID) && particleSelector->PassPhotonIso(*thisPhoton, cuts->mediumPhIso, cuts->EAPho)){
        //standard selection photons
        photonsIDIso.push_back(*thisPhoton);
        if (params->engCor) photonsIDIsoUnCor.push_back(*clonePhoton);
      }
      else if (particleSelector->PassPhotonID(*thisPhoton, cuts->loosePhID) && particleSelector->PassPhotonIso(*thisPhoton, cuts->mediumPhIso, cuts->EAPho)){
        photonsLIDMIso.push_back(*thisPhoton);
      }
      else if (particleSelector->PassPhotonID(*thisPhoton, cuts->mediumPhID) && particleSelector->PassPhotonIso(*thisPhoton, cuts->loosePhIso, cuts->EAPho)){
        photonsMIDLIso.push_back(*thisPhoton);
      }
      else if (particleSelector->PassPhotonID(*thisPhoton, cuts->loosePhID) && particleSelector->PassPhotonIso(*thisPhoton, cuts->loosePhIso, cuts->EAPho)){
        photonsLIDLIso.push_back(*thisPhoton);
      }
      else photonsNoIDIso.push_back(*thisPhoton);





    }
    //cout<<"debug0"<<endl;
    //return kTRUE;
    sort(photonsID.begin(), photonsID.end(), P4SortCondition);
    sort(photonsIDIso.begin(), photonsIDIso.end(), P4SortCondition);
    sort(photonsLIDMIso.begin(), photonsLIDMIso.end(), P4SortCondition);
    sort(photonsMIDLIso.begin(), photonsMIDLIso.end(), P4SortCondition);
    sort(photonsLIDLIso.begin(), photonsLIDLIso.end(), P4SortCondition);
    sort(photonsNoIDIso.begin(), photonsNoIDIso.end(), P4SortCondition);
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
  int            lepton1int =-1;
  TCPhysObject   lepton2;
  int            lepton2int =-1;
  TLorentzVector uncorLepton1;
  TLorentzVector uncorLepton2;

  if (params->selection == "electron" || params->selection == "eeGamma") {

    //////////////////////
    // 2 good electrons //
    //////////////////////
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
    if (params->engCor || params->doEleReg) goodZ = particleSelector->FindGoodZElectron(electronsIDIso,electronsIDIsoUnCor,lepton1,lepton2,uncorLepton1,uncorLepton2,ZP4,SCetaEl1,SCetaEl2,lepton1int,lepton2int);
    else goodZ = particleSelector->FindGoodZElectron(electronsIDIso,lepton1,lepton2,ZP4,SCetaEl1,SCetaEl2,lepton1int,lepton2int);
    if (eventNumber == params->EVENTNUMBER) cout<<"el num: "<<electronsIDIso.size()<<" goodZ: "<<goodZ<<endl;
    if (!goodZ) return kTRUE;
    if (!isRealData){ 
      if (params->doScaleFactors){
        eventWeight   *= weighter->ElectronTriggerWeight(lepton1, lepton2);
        if (params->period.find("2011") != string::npos){
          eventWeight   *= getEfficiencyWeight( &lepton1, CorrectionType::CENTRAL, atoi(params->period.c_str()));
          eventWeight   *= getEfficiencyWeight( &lepton2, CorrectionType::CENTRAL, atoi(params->period.c_str()));
        }else if (params->period.find("2012") != string::npos){
          eventWeight   *= weighter->ElectronSelectionWeight(lepton1);
          eventWeight   *= weighter->ElectronSelectionWeight(lepton2);
        }
      }
      eventWeightTrig   *= weighter->ElectronTriggerWeight(lepton1, lepton2);
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
    if (params->engCor) goodZ = particleSelector->FindGoodZMuon(muonsIDIso,muonsIDIsoUnCor,lepton1,lepton2,uncorLepton1,uncorLepton2,ZP4,lepton1int,lepton2int);
    else goodZ = particleSelector->FindGoodZMuon(muonsIDIso,lepton1,lepton2,ZP4,lepton1int,lepton2int);
    if (eventNumber == params->EVENTNUMBER) cout<<"goodZ?: "<<goodZ<<endl;
    if (!goodZ) return kTRUE;
    if (eventNumber == params->EVENTNUMBER) cout<<"goodZ?: "<<goodZ<<endl;
    if (!isRealData){ 
      if (params->doScaleFactors){
        eventWeight   *= weighter->MuonTriggerWeight(lepton1, lepton2);
        eventWeight   *= weighter->MuonSelectionWeight(lepton1);
        eventWeight   *= weighter->MuonSelectionWeight(lepton2);
      }
      eventWeightTrig   *= weighter->MuonTriggerWeight(lepton1, lepton2);
      eventWeightLep   *= weighter->MuonSelectionWeight(lepton1);
      eventWeightLep   *= weighter->MuonSelectionWeight(lepton2);
    }
    //cout<<eventWeight<<endl;
  }

  hm->fill1DHist(8,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(8,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[7];

  //////////////////////////////////////////////////////////////
  // lepton, gamma, Z plots directly after diLepton selection //
  //////////////////////////////////////////////////////////////



  if (photonsIDIso.size()>0 && ZP4.M() > cuts->zMassLow ){
    StandardPlots(lepton1,lepton2,photonsIDIso[0],eventWeight,"PreSel", "PreSel");
  }else if (ZP4.M() > cuts->zMassLow){
    TLorentzVector noPhoton;
    noPhoton.SetPtEtaPhiE(0.00001,0,0,0.00001);
    StandardPlots(lepton1,lepton2,noPhoton,eventWeight,"PreSel", "PreSel");
  }

  if (ZP4.M() > cuts->zMassLow) hm->fill1DHist(primaryVtx->GetSize(),"h1_pvMultPreSel_"+params->suffix, "Multiplicity of PVs;N_{PV};N_{evts}", 25, 0.5, 25.5, eventWeight,"PreSel");


  ////////////
  // Z mass //
  ////////////

  if (eventNumber == params->EVENTNUMBER) cout<<ZP4.M()<<endl;
  if ((ZP4.M() < cuts->zMassLow || ZP4.M() > cuts->zMassHigh)) return kTRUE;  
  hm->fill1DHist(9,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(9,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[8];
  hm->fill1DHist(ZP4.M(),"h1_diLeptonMassPreSel_"+params->suffix, "M_{ll}; M_{ll};N_{evts}", 40, 70., 110.,eventWeight,"PreSel");


  if (params->dumps){
    for (unsigned int i = 0; i < photonsIDIso.size(); ++i) {
      dumper->PhotonDump2(photonsIDIso[i], lepton1, lepton2);
    }
  }


  ////////////////////////////
  //**ZGamma** Gamma Energy //
  ////////////////////////////


  TCPhoton GP4_LIDMIso;
  TCPhoton GP4_MIDLIso;
  TCPhoton GP4_LIDLIso;
  TCPhoton GP4_NoIDIso;

  float R9Cor_LIDMIso, R9Cor_MIDLIso, R9Cor_LIDLIso, R9Cor_NoIDIso;
  R9Cor_LIDMIso = R9Cor_MIDLIso = R9Cor_LIDLIso = R9Cor_NoIDIso = -999;
  float GP4scEta_LIDMIso, GP4scEta_MIDLIso, GP4scEta_LIDLIso, GP4scEta_NoIDIso;
  GP4scEta_LIDMIso = GP4scEta_MIDLIso = GP4scEta_LIDLIso = GP4scEta_NoIDIso = -999;
  bool goodPhoton, goodPhoton_LIDMIso, goodPhoton_MIDLIso, goodPhoton_LIDLIso, goodPhoton_NoIDIso;
  goodPhoton = goodPhoton_LIDMIso = goodPhoton_MIDLIso = goodPhoton_LIDLIso = goodPhoton_NoIDIso = false;

  if (!params->doPhotonPurityStudy){
    if (photonsIDIso.size() < 1) return kTRUE;
    goodPhoton = particleSelector->FindGoodPhoton(photonsIDIso, GP4, lepton1, lepton2, R9Cor, GP4scEta, vetoPhotons);
    if(!goodPhoton) return kTRUE;

    if (params->doScaleFactors){
        eventWeight   *= weighter->GammaSelectionWeight(GP4, GP4scEta);
        if (params->suffix == "DYJets") eventWeight   *= weighter->PhotonFakeWeight(GP4.Eta(), GP4.Pt()); 
    }
    eventWeightPho   *= weighter->GammaSelectionWeight(GP4, GP4scEta);
  }else{

    /////////////////////////
    // photon purity study //
    /////////////////////////


    if (photonsIDIso.size() > 0){
      goodPhoton = particleSelector->FindGoodPhoton(photonsIDIso, GP4, lepton1, lepton2, R9Cor, GP4scEta, vetoPhotons);
    }
    if (photonsLIDMIso.size() > 0){
      goodPhoton_LIDMIso = particleSelector->FindGoodPhoton(photonsLIDMIso, GP4_LIDMIso, lepton1, lepton2, R9Cor_LIDMIso, GP4scEta_LIDMIso, vetoPhotons);
    }
    if (photonsMIDLIso.size() > 0){
      goodPhoton_MIDLIso = particleSelector->FindGoodPhoton(photonsMIDLIso, GP4_MIDLIso, lepton1, lepton2, R9Cor_MIDLIso, GP4scEta_MIDLIso, vetoPhotons);
    }
    if (photonsLIDLIso.size() > 0){
      goodPhoton_LIDLIso = particleSelector->FindGoodPhoton(photonsLIDLIso, GP4_LIDLIso, lepton1, lepton2, R9Cor_LIDLIso, GP4scEta_LIDLIso, vetoPhotons);
    }
    if (photonsNoIDIso.size() > 0){
      goodPhoton_NoIDIso = particleSelector->FindGoodPhoton(photonsNoIDIso, GP4_NoIDIso, lepton1, lepton2, R9Cor_NoIDIso, GP4scEta_NoIDIso, vetoPhotons);
    }

    if (!goodPhoton && !goodPhoton_LIDMIso && !goodPhoton_MIDLIso && !goodPhoton_LIDLIso && !goodPhoton_NoIDIso) return kTRUE;
    if (!goodPhoton){
      if (goodPhoton_LIDMIso){
        GP4 = GP4_LIDMIso;
        R9Cor = R9Cor_LIDMIso;
        GP4scEta = GP4scEta_LIDMIso;
      }else if (goodPhoton_MIDLIso){
        GP4 = GP4_MIDLIso;
        R9Cor = R9Cor_MIDLIso;
        GP4scEta = GP4scEta_MIDLIso;
      }else if (goodPhoton_LIDLIso){
        GP4 = GP4_LIDLIso;
        R9Cor = R9Cor_LIDLIso;
        GP4scEta = GP4scEta_LIDLIso;
      }else{
        GP4 = GP4_NoIDIso;
        R9Cor = R9Cor_NoIDIso;
        GP4scEta = GP4scEta_NoIDIso;
      }
    }
  }


  hm->fill1DHist(10,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(10,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[9];

  ///////////////////////////////
  //**ZGamma** Photon DR Stuff //
  ///////////////////////////////

  if(params->selection == "mumuGamma" || params->selection == "eeGamma")
  {
    // Make an FSR plot for 1337 normalization of the ZGToMuMuG set

    if (ZP4.M() > cuts->zMassLow && (GP4.DeltaR(lepton1)<cuts->dR || GP4.DeltaR(lepton2)<cuts->dR)){
      hm->fill1DHist(ZP4.M(),"h1_diLeptonMassPreSelFSR_"+params->suffix, "M_{ll}; M_{ll};N_{evts}", 30, 50., 80.,eventWeight,"PreSel");
    }

  }
  hm->fill1DHist(11,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5, eventWeight,"Misc");
  hm->fill1DHist(11,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
  ++nEvents[10];

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
    goodLep = (fabs(lepton1.Eta()) < 0.9 || fabs(lepton2.Eta()) < 0.9) && (fabs(lepton1.Eta()) < 2.1 && fabs(lepton2.Eta()) < 2.1);
  }else if (params->selection == "eeGamma"){
    goodLep = (fabs(SCetaEl1) < 1.4442 && fabs(SCetaEl2) < 1.4442);
  }
  // If lep1 or lep2 is in 0.9 and both are in 2.1 //
  if (isVBF){
    catNum = 5;
  }else if (goodLep && (fabs(GP4scEta) < 1.4442) ){
    if (R9Cor >= 0.94){
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
  R9var           = R9Cor;
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
  if (params->suffix.find("ggM125") != string::npos) scaleFactor *= 19.672/(unskimmedEventsTotal/(19.52*0.00154*0.10098*1000));
  if (params->suffix == "DYJets") scaleFactor *= 19.672/(unskimmedEventsTotal/(3503.71*1000));
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

  if (params->doAnglesMVA){
    float mvaVal = MVACalculator(mvaInits, tmvaReader);
    //scaled shapes, tuned on 135, no window, trying multicats
    if (params->selection == "mumuGamma"){
      if (catNum ==1){
        if (mvaVal <0.16) catNum = 6;
      }else if (catNum==2){
        if (mvaVal < -0.33) catNum = 7; 
      }else if (catNum==3){
        if (mvaVal < -0.13) catNum = 8;
      }else if (catNum==4){
        if (mvaVal < -0.53) catNum = 9;
      }
    }else if (params->selection == "eeGamma"){
      if (catNum ==1){
        if (mvaVal < 0.13) catNum = 6;
      }else if (catNum==2){
        if (mvaVal < -0.29) catNum = 7;
      }else if (catNum==3){
        if (mvaVal < -0.16) catNum = 8;
      }else if (catNum==4){
        if (mvaVal < -0.58) catNum = 9;
      }
    }
    /*
    if (params->selection == "mumuGamma"){
      if (catNum ==1){
        if (mvaVal <0.16) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.33) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.13) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.53) return kTRUE;
      }
    }else if (params->selection == "eeGamma"){
      if (catNum ==1){
        if (mvaVal < 0.13) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.29) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.16) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.58) return kTRUE;
      }
    }
    //MoreShapes
    if (params->selection == "mumuGamma"){
      if (catNum ==1){
        if (mvaVal <0.2) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.067) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.022) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.36) return kTRUE;
      }
    }else if (params->selection == "eeGamma"){
      if (catNum ==1){
        if (mvaVal < 0.29) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.13) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.24) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.42) return kTRUE;
      }
    }
    //nv9.5
    if (params->selection == "mumuGamma"){
      if (catNum ==1){
        if (mvaVal <0.11) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.4) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.22) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.56) return kTRUE;
      }
    }else if (params->selection == "eeGamma"){
      if (catNum ==1){
        if (mvaVal < 0.067) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.44) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.31) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.4) return kTRUE;
      }
    }

    //andy weight
    if (params->selection == "mumuGamma"){
      if (catNum ==1){
        if (mvaVal <0.13) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.44) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.11) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.47) return kTRUE;
      }
    }else if (params->selection == "eeGamma"){
      if (catNum ==1){
        if (mvaVal < 0.0) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.29) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.18) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.47) return kTRUE;
      }
    }
    //newest, no andy weight
    if (params->selection == "mumuGamma"){
      if (catNum ==1){
        if (mvaVal <0.13) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.44) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.11) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.47) return kTRUE;
      }
    }else if (params->selection == "eeGamma"){
      if (catNum ==1){
        if (mvaVal < 0.0) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.29) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.18) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.42) return kTRUE;
      }
    }
    // test version for photon pt, don't use it's shit
    if (params->selection == "mumuGamma"){
      if (catNum ==1){
        if (mvaVal <0.29) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.18) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.089) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.33) return kTRUE;
      }
    }else if (params->selection == "eeGamma"){
      if (catNum ==1){
        if (mvaVal < -0.067) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.42) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.2) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.53) return kTRUE;
      }
    }
     // newAnglesR9
    if (params->selection == "mumuGamma"){
      if (catNum ==1){
        if (mvaVal <0.089) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.49) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.22) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.67) return kTRUE;
      }
    }else if (params->selection == "eeGamma"){
      if (catNum ==1){
        if (mvaVal < -0.067) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.42) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.2) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.53) return kTRUE;
      }
    }
    */
    /*
    if (params->selection == "mumuGamma"){
      if (catNum ==1){
        if (mvaVal < -0.2) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.36) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.64) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.29) return kTRUE;
      }
    }else if (params->selection == "eeGamma"){
      if (catNum ==1){
        if (mvaVal < -0.11) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.42) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.67) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.36) return kTRUE;
      }
    }
    */
    /*
    if (params->selection == "mumuGamma"){
      if (catNum ==1){
        if (mvaVal < -0.13) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.33) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.62) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.31) return kTRUE;
      }
    }else if (params->selection == "eeGamma"){
      if (catNum ==1){
        if (mvaVal < -0.11) return kTRUE;
      }else if (catNum==2){
        if (mvaVal < -0.42) return kTRUE;
      }else if (catNum==3){
        if (mvaVal < -0.67) return kTRUE;
      }else if (catNum==4){
        if (mvaVal < -0.36) return kTRUE;
      }
    }
    */
    hm->fill2DHist((GP4+ZP4).M(),mvaVal,"h2_MassVsMVACAT"+str(catNum)+"_"+params->suffix,"Mass vs MVA output (BTDG); m_{ll#gamma}; MVA Disc", 90,100,190,90,-1,1,eventWeight,"MVAPlots");
    hm->fill2DHist((GP4+ZP4).M(),mvaVal,"h2_MassVsMVA_"+params->suffix,"Mass vs MVA output (BTDG); m_{ll#gamma}; MVA Disc", 90,100,190,90,-1,1,eventWeight,"MVAPlots");
  }


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

    if (params->dumps){
      if (params->selection == "mumuGamma"){
        dumper->MuonDump(muonsIDIso[lepton1int],true);
        dumper->MuonDump(muonsIDIso[lepton2int],true);

      } else if (params->selection == "eeGamma"){
        dumper->ElectronDump(electronsIDIso[lepton1int],*recoMuons,true);
        dumper->ElectronDump(electronsIDIso[lepton2int],*recoMuons,true);
      }
    }


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


    hm->fill1DHist((ZP4+GP4).M(),"h1_InvariantMassReco1GevCAT"+str(catNum)+"FULLRANGE_"+params->suffix,"Invariant Mass (H->Z#gamma);Mass (GeV);Entries",90,90,190,eventWeight);
    StandardPlots(lepton1,lepton2,GP4,eventWeight,"CAT"+str(catNum)+"", "CAT"+str(catNum)+"");
    hm->fill1DHist(R9, "h1_R9CAT"+str(catNum)+"_"+params->suffix,"R9;R9;Entries",100,0,1,eventWeight);
    hm->fill1DHist(R9Cor, "h1_R9CorCAT"+str(catNum)+"_"+params->suffix,"R9Cor;R9Cor;Entries",100,0,1,eventWeight);
    hm->fill2DHist(lepton1.Eta(),lepton2.Eta(),"h2_dilepEtaCAT"+str(catNum)+"_"+params->suffix,"Dilepton Eta CAT"+str(catNum)+"; Eta (leading); Eta (trailing)", 50,-2.5,2.5,50,-2.5,2.5,eventWeight,"CAT"+str(catNum)+"");
    hm->fill2DHist((GP4+ZP4).M(),MEdisc,"h2_MassVsMECAT"+str(catNum)+"_"+params->suffix,"Mass vs ME; m_{ll#gamma}; ME Disc", 90,100,190,90,0,0.2,eventWeight,"MEPlots");
    hm->fill1DHist(MEdisc,"h1_MECAT"+str(catNum)+"_"+params->suffix,"ME Disc CAT:"+str(catNum)+";ME Disc;Entries", 45,0,0.2,eventWeight,"MEPlots");
    hm->fill1DHist(59+catNum,"h1_acceptanceByCut_"+params->suffix, "Weighted number of events passing cuts by cut; cut; N_{evts}", 100, 0.5, 100.5,eventWeight,"Misc");
    hm->fill1DHist(59+catNum,"h1_acceptanceByCutRaw_"+params->suffix, "Raw number of events passing cuts; cut; N_{evts}", 100, 0.5, 100.5,1,"Misc");
    ++nEvents[60+catNum];

      

    if (params->dataDumps && isRealData){
      dumper->DataDumper(lepton1, lepton2, GP4, R9Cor, GP4scEta, SCetaEl1,SCetaEl2);
    }

    if (params->dumps){
      dumper->FinalDumper(lepton1, lepton2, GP4, catNum);
    }

    unBinnedWeight = eventWeight;



    hm->fill1DHist(R9, "h1_R9Full_"+params->suffix,"R9;R9;Entries",100,0,1,eventWeight);
    hm->fill1DHist(R9Cor, "h1_R9CorFull_"+params->suffix,"R9Cor;R9Cor;Entries",100,0,1,eventWeight);
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
  if (params->doPhotonPurityStudy){
    if(goodPhoton) StandardPlots(lepton1,lepton2,GP4,eventWeight,"MIDMIso", "PhotonPurity");
    if(goodPhoton_LIDMIso) StandardPlots(lepton1,lepton2,GP4_LIDMIso,eventWeight,"LIDMIso", "PhotonPurity");
    if(goodPhoton_MIDLIso) StandardPlots(lepton1,lepton2,GP4_MIDLIso,eventWeight,"MIDLIso", "PhotonPurity");
    if(goodPhoton_LIDLIso) StandardPlots(lepton1,lepton2,GP4_LIDLIso,eventWeight,"LIDLIso", "PhotonPurity");
    if(goodPhoton_NoIDIso) StandardPlots(lepton1,lepton2,GP4_NoIDIso,eventWeight,"NoIDIso", "PhotonPurity");
  }

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


  //GenPlots(genZs,genMuons,genPhotons,genHs,ZP4,GP4,eventWeight);

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

  hm->writeHists();
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
  hm->fill1DHist(diLep.M(),"h1_diLepMass"+tag+"_"+params->suffix, "M_{Z};M (GeV);N_{evts}", 25, 66, 116, eventWeight,folder);    

  hm->fill1DHist(threeBody.Pt(),"h1_threeBodyPt"+tag+"_"+params->suffix, "p_{T} 3body;p_{T};N_{evts}", 20, 0., 100., eventWeight,folder);     
  hm->fill1DHist(threeBody.Eta(),"h1_threeBodyEta"+tag+"_"+params->suffix, "#eta 3body;#eta;N_{evts}", 18, -2.5, 2.5, eventWeight,folder);    
  hm->fill1DHist(threeBody.Phi(),"h1_threeBodyPhi"+tag+"_"+params->suffix, "#phi 3body;#phi;N_{evts}", 18, -TMath::Pi(), TMath::Pi(), eventWeight,folder);    
  hm->fill1DHist(threeBody.M(),"h1_threeBodyMass"+tag+"_"+params->suffix, "M_{3body};M (GeV);N_{evts}", 50, 100, 200, eventWeight,folder);    


  hm->fill1DHist(primaryVtx->GetSize(),"h1_pvMultCopy"+tag+"_"+params->suffix, "Multiplicity of PVs;N_{PV};N_{evts}", 25, 0.5, 25.5, eventWeight, folder);
  for(int i = 0; i < 64; ++i) {
    unsigned long iHLT = 0x0; 
    iHLT = 0x01 << i;  
    if ((triggerStatus & iHLT) == iHLT) hm->fill1DHist(i+1,"h1_triggerStatus"+tag+"_"+params->suffix, "Triggers (no weight)", 64, 0.5, 64.5,1,"Misc");  
  } 
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
  
  //tmvaReader->AddVariable("medisc", &(mvaVars._medisc));
  tmvaReader->AddVariable("smallTheta", &(mvaVars._smallTheta));
  tmvaReader->AddVariable("bigTheta", &(mvaVars._bigTheta));
  tmvaReader->AddVariable("comPhi", &(mvaVars._comPhi));
  //tmvaReader->AddVariable("GPtOM", &(mvaVars._GPtOM));
  //tmvaReader->AddVariable("diffZGvectorOM", &(mvaVars._diffZGvectorOM));
  tmvaReader->AddVariable("threeBodyPtOM", &(mvaVars._threeBodyPtOM));
  //tmvaReader->AddVariable("ZPtOM", &(mvaVars._ZPtOM));
  tmvaReader->AddVariable("GEta", &(mvaVars._GEta));
  tmvaReader->AddVariable("l1Eta", &(mvaVars._l1Eta));
  tmvaReader->AddVariable("l2Eta", &(mvaVars._l2Eta));
  tmvaReader->AddVariable("R9var", &(mvaVars._R9));
  tmvaReader->AddVariable("sieip", &(mvaVars._sieip));
  tmvaReader->AddVariable("sipip", &(mvaVars._sipip));
  //tmvaReader->AddVariable("SCRawE", &(mvaVars._SCRawE));
  //tmvaReader->AddVariable("SCPSE", &(mvaVars._SCPSE));
  //tmvaReader->AddVariable("e5x5", &(mvaVars._e5x5));
  //tmvaReader->AddVariable("e2x2", &(mvaVars._e2x2));
  tmvaReader->AddVariable("SCRawEOPt", &(mvaVars._SCRawEOPt));
  tmvaReader->AddVariable("SCPSEOPt", &(mvaVars._SCPSEOPt));
  tmvaReader->AddVariable("e2x2O5x5", &(mvaVars._e2x2O5x5));
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

    TString weightFile = TString::Format("%sdiscr_%s_%s_ggM%i_%s___%s.weights.xml",
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

  //             [higgs mass points]
  Bool_t passBdtCut[1] = {kFALSE};
  Bool_t passAllBdtCuts[1] = {kTRUE};


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






