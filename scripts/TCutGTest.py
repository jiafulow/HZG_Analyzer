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
f2_1 = TF2("f2_1","xygaus + xygaus(5) + xylandau(10)",-4,4,-4,4)
f2_2 = TF2("f2_2","xygaus + xygaus(5) + xylandau(10)",-4,4,-4,4)
params_1 = np.array([130,-1.4,1.8,1.5,1, 150,2,0.5,-2,0.5, 3600,-2,0.7,-3,0.3])
params_2 = np.array([130,-2.4,0.8,1.0,1, 150,3,0.2,-2.6,0.1, 3600,-3,0.6,-2,0.1])
f2_1.SetParameters(params_1)
f2_2.SetParameters(params_2)
h2_1 = TH2F("h2_1","xygaus + xygaus(5) + xylandau(10)",20,-4,4,20,-4,4)
h2_2 = TH2F("h2_2","xygaus + xygaus(5) + xylandau(10)",20,-4,4,20,-4,4)
h2_1.SetFillColor(46)
h2_1.FillRandom("f2_1",40000)
h2_2.SetFillColor(47)
h2_2.FillRandom("f2_2",40000)

can= TCanvas('can','canvas',800,600)
can.cd()

myCut = TCutG('mycut',5)
myCut.SetPoint(0,-1,-1)
myCut.SetPoint(1,-1,1)
myCut.SetPoint(2,1,1)
myCut.SetPoint(3,1,-1)
myCut.SetPoint(4,-1,-1)

h2_1.Draw('colz[mycut]')
myCut.Draw()
can.SaveAs('testCut1.pdf')

h2Proj = h2.ProjectionX('noCut')
h2ProjCut = h2.ProjectionX('yesCut',1,h2.GetNbinsX(),'[mycut]')

h2Proj.Draw('hist')
can.SaveAs('testCut2.pdf')

h2ProjCut.Draw('hist')
can.SaveAs('testCut3.pdf')

