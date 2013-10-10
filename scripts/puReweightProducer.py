#! /usr/bin/env python

import sys
sys.argv.append('-b')
import ROOT as r
from decimal import *

if len(sys.argv) != 5:
  print "usage: ./puReweightProducer DataPU.root MCPU.root outputName.root"
  sys.exit()

r.gStyle.SetOptStat(0)
dataFile  = r.TFile(sys.argv[1], 'OPEN')
mcFile    = r.TFile(sys.argv[2], 'OPEN')
canvas    = r.TCanvas()

histData  = dataFile.Get('pileup')
histMC    = mcFile.Get('pileup')
#histMC    = mcFile.Get('mcPU')

histData.Sumw2()

histData.SetLineColor(4);
histData.Rebin(5)
histMC.SetLineColor(2);
histMC.Rebin(5)
#histData.SetBins(170,6,40)
#histMC.SetBins(170,6,40)
histData.Scale(1/histData.Integral())
histMC.Scale(1/histMC.Integral())

print histMC.GetNbinsX(), histMC.GetXaxis().GetXmin(), histMC.GetXaxis().GetXmax()

histData.Draw('HIST')
histMC.Draw('HIST SAME')
canvas.SaveAs('puPVMult.png')

outFile = r.TFile(sys.argv[3], 'RECREATE')
#puReweightHist = r.TH1D("pileupWeights", "PU reweight factors", 170, 6, 40)
puReweightHist = r.TH1D("pileupWeights", "PU reweight factors", 100, 0, 100)
puReweightHist.Divide(histData, histMC)
#puReweightHist.Rebin(5)

outFile.Write()
outFile.Close()
raw_input('Hit any key to continue')
