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
    #FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-22-14.root")
    #FileEl= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_01-22-14.root")
    #FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-22-14_AndyWeightCut.root")
    #FileEl= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_01-22-14_AndyWeightCut.root")

    #FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-24-14_ME.root")

    #FileEl= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_01-22-14_OldMVA.root")
    #FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-13-14.root")
    #FileEl= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_01-13-14.root")
    #FileMu = TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_12-4-13_newAnglesR9.root")
    #FileEl = TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_12-4-13_newAnglesR9.root")
    FileMu = TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_02-01-14_Cut.root")
    FileEl = TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_02-01-14_Cut.root")

  plotterMu = Plotter(FileMu, inFolder, outFolder, '2012','mu','Signal2012ggM125')
  for key in plotterMu.folderDict.keys():
    #plotterMu.DataBGComp(plotterMu.folderDict[key],'bg',doLeg = False)
    #plotterMu.DataBGComp(plotterMu.folderDict[key],'Signal',doLeg = False)
    plotterMu.DataBGComp(plotterMu.folderDict[key])
    plotterMu.DataBGComp2DProj(plotterMu.folderDict[key])
    #plotter.DataBGComp2DProj(plotter.folderDict[key],125)
  plotterEl = Plotter(FileEl, inFolder, outFolder, '2012','el','Signal2012ggM125')
  for key in plotterEl.folderDict.keys():
    plotterEl.DataBGComp(plotterEl.folderDict[key])
    plotterEl.DataBGComp2DProj(plotterEl.folderDict[key])
    plotterEl.DataBGComp2DProj(plotterEl.folderDict[key],125)

def ROCPlotter():
  if os.environ.get('AT_NWU') == None:
    #FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-18-13_anglesOnly.root")
    #FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-18-13_anglesOnly.root")
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-29-13_plusPtTest.root")
    FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-29-13_plusPtTest.root")
  else:
    #FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_02-04-14.root")
    #FileEl= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_02-04-14.root")
    FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_02-18-14.root")
    FileEl= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_02-18-14.root")

  plotter = Plotter(FileMu, 'MVAPlots', 'ROC_MVA_02-18-14', '2012','mu','Signal2012ggM135')
  for key in plotter.folderDict.keys():
    plotter.ROCcurves(plotter.folderDict[key])
    plotter.ROCcurves(plotter.folderDict[key],135)
  plotterEl = Plotter(FileEl, 'MVAPlots', 'ROC_MVA_02-18-14', '2012','el','Signal2012ggM135')
  for key in plotterEl.folderDict.keys():
    plotterEl.ROCcurves(plotterEl.folderDict[key])
    plotterEl.ROCcurves(plotterEl.folderDict[key],135)

def RatioPlotter():
  if os.environ.get('AT_NWU') == None:
    #FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_signalComp.root")
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_signalTest.root")
    #FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-20-13_anglesOnly.root")
  else:
    #FileMu1= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-13-14.root")
    #FileMu2= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-21-14_v9_4.root")
    #FileEl1= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_01-13-14.root")
    #FileEl2= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_01-21-14_v9_4.root")
    #FileEl1= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_01-22-14_AndyWeight.root")
    #FileEl2= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_01-22-14.root")
    #FileMu1= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-22-14_AndyWeight.root")
    #FileMu2= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-22-14.root")
    FileMu = TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_02-12-14_PUTest.root")
    FileEl = TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_02-12-14_PUTest.root")

  #plotter = Plotter(FileMu, 'Vtx', 'PUCheck_NoRD', '2012','mu','Signal2012ggM125p8')
  #plotterMu = Plotter([FileMu1,FileMu2], 'pT-Eta-Phi', 'ntuple8v9', '2012','mu','Signal2012ggM125NLOp8')
  #plotterMu = Plotter([FileMu1,FileMu2], 'ZGamma', 'ntuple8v9', '2012','mu','Signal2012ggM125NLOp8')
  #for key in plotterMu.folderDict[0].keys():
    #plotterMu.RatioPlot(key,['Signal2012ggM125NLOp8','Signal2012ggM125NLOp8'],['nv8.2','nv9.4'])
    #plotterMu.RatioPlot(key,['DATA','DATA'],['nv8.2','nv9.4'])

  #plotterEl = Plotter([FileEl1,FileEl2], 'pT-Eta-Phi', 'ntuple8v9', '2012','el','Signal2012ggM125NLOp8')
  #plotterEl = Plotter([FileEl1,FileEl2], 'ZGamma', 'ntuple8v9', '2012','el','Signal2012ggM125NLOp8')
  #for key in plotterEl.folderDict[0].keys():
    #plotterEl.RatioPlot(key,['Signal2012ggM125NLOp8','Signal2012ggM125NLOp8'],['nv8.2','nv9.4'])
    #plotterEl.RatioPlot(key,['DATA','DATA'],['nv8.2','nv9.4'])

  folders = ['Vtx']
  for folder in folders:
    plotterEl = Plotter(FileEl, folder, 'NewPuRD1', '2012','el','Signal2012ggM125')
    for key in plotterEl.folderDict.keys():
      plotterEl.RatioPlot(key,['DATA','Signal'],['Data','Signal'],True)
    plotterMu = Plotter(FileMu, folder, 'NewPuRD1', '2012','mu','Signal2012ggM125')
    for key in plotterMu.folderDict.keys():
      plotterMu.RatioPlot(key,['DATA','Signal'],['Data','Signal'],True)

def DoAll():
  if os.environ.get('AT_NWU'):
    mainPath = '/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_'
    suffix = '02-18-14'
    headDir = 'Full_'+suffix
    if not os.path.isdir(headDir):
      os.mkdir('Full_'+suffix)
    FileMu = TFile(mainPath+'MuMu2012ABCD_'+suffix+'.root','OPEN')
    FileEl = TFile(mainPath+'EE2012ABCD_'+suffix+'.root','OPEN')
    folders = ['ZGamma','CAT1','CAT2','CAT3','CAT4','pT-Eta-Phi','MVAPlots','Misc']
    for folder in folders:
      plotterEl = Plotter(FileEl, folder, headDir+'/'+folder+'_Ratio', '2012','el','Signal2012ggM125')
      for key in plotterEl.folderDict.keys():
        plotterEl.RatioPlot(key,['DATA','Signal'],['Data','Signal'],True)
        plotterEl.directory = headDir+'/'+folder
        plotterEl.DataBGComp(plotterEl.folderDict[key])
        plotterEl.DataBGComp2DProj(plotterEl.folderDict[key])
      plotterMu = Plotter(FileMu, folder, headDir+'/'+folder+'_Ratio', '2012','mu','Signal2012ggM125')
      for key in plotterMu.folderDict.keys():
        plotterMu.RatioPlot(key,['DATA','Signal'],['Data','Signal'],True)
        plotterMu.directory = headDir+'/'+folder
        plotterMu.DataBGComp(plotterMu.folderDict[key])
        plotterMu.DataBGComp2DProj(plotterMu.folderDict[key])
  else: print 'this is not set up for FNAL'



if __name__=="__main__":
  print 'executing', sys.argv[1]
  if 'plot' == sys.argv[1].lower():
    GenericPlotter(sys.argv[2], sys.argv[3])
  elif 'roc' == sys.argv[1].lower():
    ROCPlotter()
  elif 'ratio' == sys.argv[1].lower():
    RatioPlotter()
  elif 'all' == sys.argv[1].lower():
    DoAll()
