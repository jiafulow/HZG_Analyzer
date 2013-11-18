#!/usr/bin/env python
import sys,os
sys.argv.append('-b')
from PlottingClasses import *
from ROOT import *
import numpy as np

gROOT.ProcessLine('.L ./tdrstyle.C')
setTDRStyle()

def GenericPlotter(inFolder, outFolder):
  if os.environ.get('AT_NWU') == None:
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-18-13_anglesOnly.root")
  else:
    FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_10-31-13.root")
    #FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-4-13.root")

  plotter = Plotter(FileMu, inFolder, outFolder, '2012','mu','Signal2012ggM125p8')
  for key in plotter.folderDict.keys():
    plotter.DataBGComp(plotter.folderDict[key])
    plotter.DataBGComp2DProj(plotter.folderDict[key])
    #splotter.DataBGComp2DProj(plotter.folderDict[key],125)

def ROCPlotter():
  if os.environ.get('AT_NWU') == None:
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-18-13.root")
  else:
    FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-12-13.root")

  plotter = Plotter(FileMu, 'MVAPlots', 'ROC_MVA_anglesOnly', '2012','mu','Signal2012ggM125p8')
  for key in plotter.folderDict.keys():
    plotter.ROCcurves(plotter.folderDict[key])
    plotter.ROCcurves(plotter.folderDict[key],125)

def RatioPlotter():
  if os.environ.get('AT_NWU') == None:
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_signalComp.root")
  else:
    FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-6-13.root")

  #plotter = Plotter(FileMu, 'Vtx', 'PUCheck_NoRD', '2012','mu','Signal2012ggM125p8')
  plotter = Plotter(FileMu, 'PreGen', 'Ratio', '2012','mu','Signal2012ggM125p8')
  for key in plotter.folderDict.keys():
    plotter.RatioPlot(plotter.folderDict[key],['Signal2012ggM125p6','Signal2012ggM125v153p8'],['p6 NLO','p8 NLO'])
    #plotter.RatioPlot(plotter.folderDict[key],['DATA','DYJets'],['DATA','DYJets'])
  #plotter.RatioPlot(plotter.folderDict['pvMult'],['DATA','DYJets'],['DATA','DYJets'])


if __name__=="__main__":
  print 'executing', sys.argv[1]
  if 'plot' == sys.argv[1]:
    GenericPlotter(sys.argv[2], sys.argv[3])
  elif 'ROC' == sys.argv[1]:
    ROCPlotter()
  elif 'Ratio' == sys.argv[1]:
    RatioPlotter()
