#!/usr/bin/env python
import sys
sys.argv.append('-b')
from ROOT import *

gROOT.ProcessLine('.L ./tdrstyle.C')
setTDRStyle()
TH1.SetDefaultSumw2(kTRUE)
TH2.SetDefaultSumw2(kTRUE)
TProfile.SetDefaultSumw2(kTRUE)

PlotList = ['p_MassVsME_','h2_MassVsME_']

myFileData= TFile('../HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_ME_plots.root','r')
myFileSignal= TFile('../HiggsZGAnalyzer/localHistos/higgsHistograms_ggM125_8TeV_pythia8_175_v2_eeGamma_local.root','r')
can= TCanvas('can','canvas',800,600)
can.SetRightMargin(0.1)
can.cd()

profHistData = myFileData.Get(PlotList[0]+'DATA')
profHistData.Draw()
profHistSignal = myFileSignal.Get(PlotList[0]+'Signal2012ggM125')
profHistSignal.SetLineColor(kRed)
profHistSignal.Draw('same')

can.SaveAs('MEPlots/MassVsME_profile.pdf')

h2HistData = myFileData.Get(PlotList[1]+'DATA')
h2HistData.Draw('colz')
can.SaveAs('MEPlots/MassVsME_2D_Data.pdf')

h2HistSignal = myFileSignal.Get(PlotList[1]+'Signal2012ggM125')
h2HistSignal.Draw('colz')
can.SaveAs('MEPlots/MassVsME_2D_Signal.pdf')
