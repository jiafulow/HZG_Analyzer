#!/usr/bin/env python
import sys
sys.argv.append('-b')
from ROOT import *

gROOT.ProcessLine('.L ./tdrstyle.C')
setTDRStyle()
TH1.SetDefaultSumw2(kTRUE)
TH2.SetDefaultSumw2(kTRUE)
TProfile.SetDefaultSumw2(kTRUE)

PlotList = ['p_MassVsME_DATA','h2_MassVsME_DATA']

myFile = TFile('../HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_ME_plots.root','r')
can= TCanvas('trailingCan','canvas',800,600)
can.SetRightMargin(0.1)
can.cd()

profHist = myFile.Get(PlotList[0])
profHist.Draw()
can.SaveAs('MEPlots/MassVsME_profile.pdf')

h2Hist = myFile.Get(PlotList[1])
h2Hist.Draw('colz')
can.SaveAs('MEPlots/MassVsME_2D.pdf')

