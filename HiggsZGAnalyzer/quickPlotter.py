#! /usr/bin/env python

from ROOT import *
gROOT.SetBatch()
#gROOT.ProcessLine('.L ../scripts/tdrstyle.C')
#setTDRStyle()

muonFile = TFile('otherHistos/MuHLTEfficiencies_Run_2012ABCD_53X_DR03-2.root')

c = TCanvas()
c.SetRightMargin(0.15)
gStyle.SetPaintTextFormat("1.3f")
#gDirectory.ls("-d")

dictOfHistNames = {'DATA_Mu17Mu8_Tight_Mu1_20ToInfty_&_Mu2_20ToInfty_with_STAT_uncrt':'muon_trig_data_eff_high',
    'DATA_Mu17Mu8_Tight_Mu1_10To20_&_Mu2_20ToInfty_with_STAT_uncrt':'muon_trig_data_eff_low',
    'MC_Mu17Mu8_Tight_Mu1_20ToInfty_&_Mu2_20ToInfty_with_STAT_uncrt':'muon_trig_mc_eff_high',
    'MC_Mu17Mu8_Tight_Mu1_10To20_&_Mu2_20ToInfty_with_STAT_uncrt':'muon_trig_mc_eff_low'}

dictOfTitles = {'DATA_Mu17Mu8_Tight_Mu1_20ToInfty_&_Mu2_20ToInfty_with_STAT_uncrt':'DATA Muon Trigger Efficiency: Trailing p_{T} > 20 GeV',
    'DATA_Mu17Mu8_Tight_Mu1_10To20_&_Mu2_20ToInfty_with_STAT_uncrt':'DATA Muon Trigger Efficiency: 10 < Trailing p_{T} < 20 GeV',
    'MC_Mu17Mu8_Tight_Mu1_20ToInfty_&_Mu2_20ToInfty_with_STAT_uncrt':'MC Muon Trigger Efficiency: Trailing p_{T} > 20 GeV',
    'MC_Mu17Mu8_Tight_Mu1_10To20_&_Mu2_20ToInfty_with_STAT_uncrt':'MC Muon Trigger Efficiency: 10 < Trailing p_{T} < 20 GeV'}

for fileName in dictOfHistNames:
  histo = muonFile.Get(fileName)
  histo.SetTitle(dictOfTitles[fileName])
  histo.SetMarkerSize(2.0)
  histo.GetZaxis().SetRangeUser(0.75,1.0)
  histo.GetZaxis().SetTitle('Efficiency')
  histo.GetZaxis().SetTitleOffset(1.1)
  histo.GetYaxis().SetTitle('Leading Lepton |#eta|')
  histo.GetXaxis().SetTitle('Trailing Lepton |#eta|')


  histo.Draw('colztext')
  c.Print(dictOfHistNames[fileName]+'.pdf')


