#include <TMVA/Reader.h>

//______________________________________________________________________________
bool HggPreselection(int i)
{
  /* Preselection of photon candidates for the identification with MVA.
   *
   * Returns true if candidate passes the preselection or false otherwise.
   *
   * i = index of a photon candidate to consider.
   */

  if (phoEt[i] < 15 || phoEt[i] > 200) return false;
  if (phoEleVeto[i] == 1) return false;

  // ECAL barrel
  if (fabs(phoSCEta[i]) < 1.479) {
    if (phoSigmaIEtaIEta[i] > 0.014) return false;
    if (phoR9[i] > 0.9) {
      if (phoHoverE12[i] > 0.082) return false;
    } else {
      if (phoHoverE12[i] > 0.075) return false;
    }
  // ECAL endcaps
  } else {
    if (phoSigmaIEtaIEta[i] > 0.034) return false;
    if (phoHoverE12[i] > 0.075) return false;
  }

  // isolation
  if (phoR9[i] > 0.9) {
    if (phoHcalIsoDR03[i] - 0.005 * phoEt[i] > 50) return false;
    if (phoTrkIsoHollowDR03[i] - 0.002 * phoEt[i] > 50) return false;
    if (phoCiCPF4chgpfIso02[i][0] > 4) return false;
  } else {
    if (phoHcalIsoDR03[i] - 0.005 * phoEt[i] > 4) return false;
    if (phoTrkIsoHollowDR03[i] - 0.002 * phoEt[i] > 4) return false;
    if (phoCiCPF4chgpfIso02[i][0] > 4) return false;
  }

  // preselection successful
  return true;
}

//______________________________________________________________________________
float PhotonIDMVA(int i)
{
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
  int iBE = (fabs(phoSCEta[i]) < 1.479) ? 0 : 1;

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
      tmvaReader[0]->BookMVA("BDT", "/afs/cern.ch/user/r/rslu/public/photonIDMVA_2014/EB_BDT.weights.xml");
    else
      tmvaReader[1]->BookMVA("BDT", "/afs/cern.ch/user/r/rslu/public/photonIDMVA_2014/EE_BDT.weights.xml");

  } // one-time initialization

  // set MVA variables
  phoPhi_ = phoPhi[i];
  phoR9_ = phoR9[i];
  phoSigmaIEtaIEta_ = phoSigmaIEtaIEta[i];
  phoSigmaIEtaIPhi_ = phoSigmaIEtaIPhi[i];
  phoS4_ = phoE2x2[i]/phoE5x5[i];
  phoS13_ = phoE1x3[i]/phoE5x5[i];
  phoS25_ = phoE2x5Max[i]/phoE5x5[i];
  phoSCEta_ = phoSCEta[i];
  phoSCRawE_ = phoSCRawE[i];
  phoSCEtaWidth_ = phoSCEtaWidth[i];
  phoSCPhiWidth_ = phoSCPhiWidth[i];
  rho2012_ = rho2012;
  phoPFPhoIso_ = phoPFPhoIso[i];
  phoPFChIso_ = phoPFChIso[i];
  phoESEnToRawE_ = phoESEn[i]/phoSCRawE[i];
  phoESEffSigmaRR_x_= phoESEffSigmaRR_x[i];
  phoEt_ = phoEt[i];
  phoEta_ = phoEta[i];

  // evaluate largest isolation value
  phoPFChIsoWorst_ = 0;
  for (size_t k = 0; k < phoCiCPF4chgpfIso03[i].size(); ++k)
    if (phoPFChIsoWorst_ < phoCiCPF4chgpfIso03[i][k])
      phoPFChIsoWorst_ = phoCiCPF4chgpfIso03[i][k];

  return tmvaReader[iBE]->EvaluateMVA("BDT");
}

void SteeringFunction()
{

  // loop over photon candidates
  for (int i = 0; i < nPho; ++i) {
    if (fabs(phoSCEta[i]) > 2.5) continue;

    // exclude ECAL's barrel-endcaps crack regions
    if (fabs(phoSCEta[i]) >= 1.4442 && fabs(phoSCEta[i]) <= 1.566) continue;

    // for MC: correct R9; ECAL barrel vs ECAL endcaps
    if (isMC) {
      if (fabs(phoSCEta[i]) < 1.479)
         phoR9[i] = 1.00139 * phoR9[i] + 0.000740;
      else
         phoR9[i] = 1.00016 * phoR9[i] - 0.000399;
    }

    // MVA ID (applied before the energy scale correction)
    float mvaout = -9;
    if (HggPreselection(i)) mvaout = PhotonIDMVA(i);

    // ...
  }

}
