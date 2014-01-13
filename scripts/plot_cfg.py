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
    #FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-20-13_anglesOnly.root")
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_ME_2Dv5.root")
    #FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-20-13_anglesOnly.root")
    #FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-18-13_anglesOnly.root")
    #FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-18-13_anglesOnly.root")
  else:
    #FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_10-31-13_MECuts.root")
    FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-13-14_Cut.root")

  plotterMu = Plotter(FileMu, inFolder, outFolder, '2012','mu','Signal2012ggM125NLOp8')
  for key in plotterMu.folderDict.keys():
    plotterMu.DataBGComp(plotterMu.folderDict[key])
    plotterMu.DataBGComp2DProj(plotterMu.folderDict[key])
    #splotter.DataBGComp2DProj(plotter.folderDict[key],125)
  #plotterEl = Plotter(FileEl, inFolder, outFolder, '2012','el','Signal2012ggM125p8')
  #for key in plotterEl.folderDict.keys():
    #plotterEl.DataBGComp(plotterEl.folderDict[key])
    #plotterEl.DataBGComp2DProj(plotterEl.folderDict[key])
    #splotter.DataBGComp2DProj(plotter.folderDict[key],125)

def ROCPlotter():
  if os.environ.get('AT_NWU') == None:
    #FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-18-13_anglesOnly.root")
    #FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-18-13_anglesOnly.root")
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-29-13_plusPtTest.root")
    FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-29-13_plusPtTest.root")
  else:
    #FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-12-13.root")
    FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-13-14.root")
    FileEl= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_01-13-14.root")

  plotter = Plotter(FileMu, 'MVAPlots', 'ROC_MVA_01-13-14', '2012','mu','Signal2012ggM125NLOp8')
  for key in plotter.folderDict.keys():
    plotter.ROCcurves(plotter.folderDict[key])
    plotter.ROCcurves(plotter.folderDict[key],125)
  #plotterEl = Plotter(FileEl, 'MVAPlots', 'ROC_MVA_newAnglesR9', '2012','el','Signal2012ggM125NLOp8')
  #for key in plotterEl.folderDict.keys():
    #plotterEl.ROCcurves(plotterEl.folderDict[key])
    #plotterEl.ROCcurves(plotterEl.folderDict[key],125)

def RatioPlotter():
  if os.environ.get('AT_NWU') == None:
    #FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_signalComp.root")
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_signalTest.root")
    #FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-20-13_anglesOnly.root")
  else:
    FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_EmVeto.root")

  #plotter = Plotter(FileMu, 'Vtx', 'PUCheck_NoRD', '2012','mu','Signal2012ggM125p8')
  plotterMu = Plotter(FileMu, 'pT-Eta-Phi', 'EmVetoTest', '2012','mu','Signal2012ggM125NLOp8')
  for key in plotterMu.folderDict.keys():
    plotterMu.RatioPlot(plotterMu.folderDict[key],['Signal2012ggM125p6','Signal2012ggM125NLOEmVetop8'],['p6 NLO','p8 NLO EmVeto'])

  #plotterEl = Plotter(FileEl, 'ZGAngles_RECO', 'Ratio_PostMVA', '2012','el','Signal2012ggM125p8')
  #for key in plotterEl.folderDict.keys():
  #  plotterEl.RatioPlot(plotterEl.folderDict[key],['bg','Signal2012ggM125p8'],['bg','p8 NLO'])


if __name__=="__main__":
  print 'executing', sys.argv[1]
  if 'plot' == sys.argv[1]:
    GenericPlotter(sys.argv[2], sys.argv[3])
  elif 'ROC' == sys.argv[1]:
    ROCPlotter()
  elif 'Ratio' == sys.argv[1]:
    RatioPlotter()
