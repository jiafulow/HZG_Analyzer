#include "../interface/AnalysisParameters.h"

using namespace std;

Cuts::Cuts():
  leadJetPt(30),
  trailJetPt(30),
  leadMuPt(20),
  trailMuPt(10),
  leadElePt(20),
  trailElePt(10),
  gPtOverMass(15./110.),
  gPt(15),
  zMassLow(50),
  zMassHigh(999999),
  metLow(-9999),
  metHigh(9999),
  zgMassLow(100),
  zgMassHigh(190)
{}
//static const float jetPtCut[] = {30,30}, muPtCut[] = {20,10}, elePtCut[] = {20,10}, gammaPtCut[] = {15./110.,15.}, zMassCut[] = {50,99999.}, metCut[] = {-9999.,9999.},
 // metByQtCut[] = {-999.,999.}, bJetVeto = 9999, qtCut = 0., nJets[] = {0,999}, MassZG[] = {100.,190.}, SumEt = 99999., AngCut = 999,
  //M12Cut = 999999999999999999999, drCut = 0.4, MZpMZGcut = 185, VBFdeltaEta = 3.5, VBFdeltaPhi = 2.4, diJetMassCut = 500, Zeppenfeld = 2.5, R9Cut = 0.94; 

Cuts* Cuts::m_pInstance = NULL;

Cuts* Cuts::Instance()
{
  if (!m_pInstance) m_pInstance = new Cuts();
  return m_pInstance;
}


