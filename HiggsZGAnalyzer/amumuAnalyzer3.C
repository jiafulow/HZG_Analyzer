#define amumuAnalyzer3_cxx
// The class definition in amumuAnalyzer3.h has been generated automatically
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
// Root > T->Process("amumuAnalyzer3.C")
// Root > T->Process("amumuAnalyzer3.C","some options")
// Root > T->Process("amumuAnalyzer3.C+")
//

#include "amumuAnalyzer3.h"
#include <TH2.h>
#include <TStyle.h>


void amumuAnalyzer3::Begin(TTree * /*tree*/)
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
  string selection = "mumuGamma";

  params.reset(new Parameters());

  params->selection      = selection;
  //params->period         = period;
  //params->JC_LVL         = 0;
  //params->abcd           = abcd;
  params->suffix         = suffix;
  params->dataname       = dataname;
  params->jobCount       = njob;
  //params->dumps          = true;
  //params->doScaleFactors = true;
  
  amumuFile.reset(new TFile(("amumuFile_"+params->dataname+"_"+params->selection+"_"+params->jobCount+".root").c_str(), "RECREATE"));
  amumuFile->cd();
  amumuTree.reset(new TTree(("amumuTree_"+params->suffix).c_str(),"three body mass values"));
  hm.reset(new HistManager(amumuFile.get()));
  
}

void amumuAnalyzer3::SlaveBegin(TTree * /*tree*/)
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();

}

Bool_t amumuAnalyzer3::Process(Long64_t entry)
{
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // It can be passed to either amumuAnalyzer3::GetEntry() or TBranch::GetEntry()
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
  
  b_lhe_nup->GetEntry(entry);
  
  hm->fill1DHist(lhe_nup, "h1_lhe_nup_"+params->suffix, "; LHE nup;N_{evts}", 20, 0., 20., 1);
  
  b_genParticles->GetEntry(entry);
  
  vector<TCGenParticle> genZs;
  for (int i = 0; i < genParticles->GetSize(); ++i) {
    const TCGenParticle* thisPart = (TCGenParticle*) genParticles->At(i);
    if (abs(thisPart->GetPDGId()) == 23 && thisPart->GetStatus() == 3) genZs.push_back(*thisPart);
    
    //if (entry==0) {
    //    std::cout << "PDG: " << thisPart->GetPDGId() << " st: " << thisPart->GetStatus() << " mass: " << thisPart->M() << " parton: " << thisPart->IsParton() << " momId: " << thisPart->MotherId() << std::endl;
    //}
  }
  assert(genZs.size() == 1);
  
  if (lhe_nup == 5) {  // 0 jet
    hm->fill1DHist(genZs.at(0).Pt(), "h1_genZ_pt_nup5_"+params->suffix, "; genZ p_{T} [GeV]", 100, 0, 200., 1);
  } else if (lhe_nup == 6) {  // 1 jet
    hm->fill1DHist(genZs.at(0).Pt(), "h1_genZ_pt_nup6_"+params->suffix, "; genZ p_{T} [GeV]", 100, 0, 200., 1);
  } else if (lhe_nup == 7) {  // 2 jets
    hm->fill1DHist(genZs.at(0).Pt(), "h1_genZ_pt_nup7_"+params->suffix, "; genZ p_{T} [GeV]", 100, 0, 200., 1);  
  } else if (lhe_nup == 8) {  // 3 jets
    hm->fill1DHist(genZs.at(0).Pt(), "h1_genZ_pt_nup8_"+params->suffix, "; genZ p_{T} [GeV]", 100, 0, 200., 1);
  } else if (lhe_nup == 9) {  // 4 jets
    hm->fill1DHist(genZs.at(0).Pt(), "h1_genZ_pt_nup9_"+params->suffix, "; genZ p_{T} [GeV]", 100, 0, 200., 1);
  } else if (lhe_nup >= 10) {  // 5 or more jets
    hm->fill1DHist(genZs.at(0).Pt(), "h1_genZ_pt_nup10_"+params->suffix, "; genZ p_{T} [GeV]", 100, 0, 200., 1);
  } else {
    std::cout << "Unexpected lhe_nup: " << lhe_nup << std::endl;
  }
  
  return kTRUE;

}

void amumuAnalyzer3::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

void amumuAnalyzer3::Terminate()
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.
  
  cout << "TOTAL NUMBER OF FILES: " << fileCount << " and they have this many events: " << unskimmedEventsTotal << endl;
  cout << "Passed: " << skimmedEventsTotal << "/" << unskimmedEventsTotal << endl;

  amumuFile->Write();
  amumuFile->Close();
}
