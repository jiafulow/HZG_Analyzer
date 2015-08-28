#define amumuAnalyzer2_cxx
// The class definition in amumuAnalyzer2.h has been generated automatically
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
// Root > T->Process("amumuAnalyzer2.C")
// Root > T->Process("amumuAnalyzer2.C","some options")
// Root > T->Process("amumuAnalyzer2.C+")
//

#include "amumuAnalyzer2.h"
#include <TH2.h>
#include <TStyle.h>


void amumuAnalyzer2::Begin(TTree * /*tree*/)
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  /////////////////////////////
  //Specify parameters here. //
  /////////////////////////////

  TString option = GetOption();
  string suffix = static_cast<string>(static_cast<TObjString*>(option.Tokenize(" ")->At(0))->GetString());
  string dataname = static_cast<string>(static_cast<TObjString*>(option.Tokenize(" ")->At(1))->GetString());
  string njob = static_cast<string>(static_cast<TObjString*>(option.Tokenize(" ")->At(2))->GetString());
  string outdir = static_cast<string>(static_cast<TObjString*>(option.Tokenize(" ")->At(3))->GetString());

  params.reset(new Parameters());

  //params->selection      = selection;
  //params->period         = period;
  //params->JC_LVL         = 0;
  //params->abcd           = abcd;
  //params->suffix         = suffix;
  //params->dataname       = dataname;
  //params->jobCount       = count;
  //params->dumps          = true;
  //params->doScaleFactors = true;
  
  outdir = outdir + suffix;
  if (gSystem->AccessPathName(outdir.c_str()))
    gSystem->mkdir(outdir.c_str());
  newFile = new TFile((outdir+"/nuTuple_" + njob + "_1_ABC.root").c_str(), "RECREATE");
  newFile->mkdir("ntupleProducer")->cd();

}

void amumuAnalyzer2::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();

}

Bool_t amumuAnalyzer2::Process(Long64_t entry)
{
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // It can be passed to either amumuAnalyzer2::GetEntry() or TBranch::GetEntry()
  // to read either all or the required parts of the data. When processing
  // keyed objects with PROOF, the object is already loaded and is available
  // via the fObject pointer.
  //
  // This function should contain the "body" of the analysis. It can contain
  // simple or elaborate selection criteria, run algorithms on the data
  // of the event and typically fill histograms.
  //
  // The processing can be stopped by calling Abort().
  //
  // Use fStatus to set the return value of TTree::Process().
  //
  // The return value is currently not used.
  
  //GetEntry(entry, 1);
  
  ///////////
  // Muons //
  ///////////
  
  b_recoMuons->GetEntry(entry);
  
  int nmuons = 0;
  int nmuons_pos = 0;
  int nmuons_neg = 0;
  for (Int_t i = 0; i < recoMuons->GetSize(); ++ i) {
    const TCMuon* thisMuon = (TCMuon*) recoMuons->At(i);
    
    if (thisMuon->Pt() > 15. && fabs(thisMuon->Eta()) < 2.4) {
      if (thisMuon->Charge() == 1) {
        nmuons_pos += 1;
      } else if (thisMuon->Charge() == -1) {
        nmuons_neg += 1;
      }
      nmuons += 1;
    }
  }
  
  //bool passMuon = (nmuons >= 2);
  bool passMuon = (nmuons_pos >= 1 && nmuons_neg >= 1);

  //////////
  // Jets //
  //////////
  
  b_recoJets->GetEntry(entry);
  
  int njets = 0;
  for (Int_t i = 0; i < recoJets->GetSize(); ++i) {
    const TCJet* thisJet = (TCJet*) recoJets->At(i);
    
    if (thisJet->Pt() > 20. && fabs(thisJet->Eta()) < 4.7) {
      njets += 1;
    }
  }

  bool passJets = (njets >= 2);
  
  ////////////
  // Select //
  ////////////
  
  if (passMuon && passJets) {
    GetEntry(entry, 1);
    newEventTree->Fill();
    skimmedEventsTotal += 1;
    
    return kTRUE;

  } else {
    return kFALSE;
  }

}

void amumuAnalyzer2::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

void amumuAnalyzer2::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.
  
  cout << "TOTAL NUMBER OF FILES: " << fileCount << " and they have this many events: " << unskimmedEventsTotal << endl;
  cout << "Passed: " << skimmedEventsTotal << "/" << unskimmedEventsTotal << endl;

  // Remake the numOfEvents
  new_h1_numOfEvents = new TH1F("numOfEvents", "total number of events, unskimmed", 1,0,1);
  new_h1_numOfEvents->SetBinContent(1,unskimmedEventsTotal);
  
  // Remake the jobTree
  newJobTree = new TTree("jobTree", "jobTree");
  UInt_t nEvents = unskimmedEventsTotal;
  vector<string> triggerPaths = *triggerNames;
  newJobTree->Branch("nEvents", &nEvents, "nEvents/i");
  newJobTree->Branch("triggerNames", "vector<string>", &triggerPaths);
  newJobTree->Fill();

  newFile->Write();
  newFile->Close();
}
