#!/usr/bin/env python
import sys
sys.argv.append('-b')
from ROOT import *

gROOT.ProcessLine('.L ./tdrstyle.C')
setTDRStyle()
TH1.SetDefaultSumw2(kTRUE)
TH2.SetDefaultSumw2(kTRUE)
TProfile.SetDefaultSumw2(kTRUE)

PlotList = ['h2_MassVsME']
catList = ['','CAT1','CAT2','CAT3','CAT4']

myFileData= TFile('../HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_ME_2Dv5.root','r')
myFileSignal= TFile('../HiggsZGAnalyzer/localHistos/higgsHistograms_ggM125_8TeV_pythia8_175_v2_mumuGamma_local.root','r')
can= TCanvas('can','canvas',800,600)
can.SetRightMargin(0.1)
can.cd()

for cat in catList:
  h2HistData = myFileData.Get(PlotList[0]+cat+'_DATA')
  h2HistData.Draw('colz')
  can.SaveAs('MEPlots/MassVsME_2D_Data'+cat+'.pdf')

  h2HistSignal = myFileSignal.Get(PlotList[0]+cat+'_Signal2012ggM125')
  h2HistSignal.Draw('colz')
  can.SaveAs('MEPlots/MassVsME_2D_Signal'+cat+'.pdf')

