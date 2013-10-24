#!/usr/bin/env python
import sys,os
sys.argv.append('-b')
from ROOT import *
import numpy as np

gROOT.Reset()
gStyle.SetOptStat(0)
gStyle.SetPalette(1)
gStyle.SetCanvasColor(33)
gStyle.SetFrameFillColor(18)
f2 = TF2("f2","xygaus + xygaus(5) + xylandau(10)",-4,4,-4,4)
params = np.array([130,-1.4,1.8,1.5,1, 150,2,0.5,-2,0.5, 3600,-2,0.7,-3,0.3])
f2.SetParameters(params)
h2 = TH2F("h2","xygaus + xygaus(5) + xylandau(10)",20,-4,4,20,-4,4)
h2.SetFillColor(46)
h2.FillRandom("f2",40000)

can= TCanvas('can','canvas',800,600)
can.cd()

myCut = TCutG('myCut',5)
myCut.SetPoint(0,-1,-1)
myCut.SetPoint(1,-1,1)
myCut.SetPoint(2,1,1)
myCut.SetPoint(3,1,-1)
myCut.SetPoint(4,-1,-1)

h2.Draw('colz[myCut]')
myCut.Draw()
can.SaveAs('testCut1.pdf')

h2Proj = h2.ProjectionX('noCut')
h2ProjCut = h2.ProjectionX('yesCut',1,h2.GetNbinsX(),'[myCut]')

h2Proj.Draw('hist')
can.SaveAs('testCut2.pdf')

h2ProjCut.Draw('hist')
can.SaveAs('testCut3.pdf')

