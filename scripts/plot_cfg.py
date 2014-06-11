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
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_ME_2Dv5.root")
    #FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-18-13_anglesOnly.root")
  else:
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

def ROCPlotter(suffix = '05-07-14_PhoMVA'):
  if os.environ.get('AT_NWU') == None:
    #FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-18-13_anglesOnly.root")
    #FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-18-13_anglesOnly.root")
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_11-29-13_plusPtTest.root")
    FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-29-13_plusPtTest.root")
  else:
    #FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_02-04-14.root")
    #FileEl= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_02-04-14.root")
    FileMu= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_"+suffix+".root")
    FileEl= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_"+suffix+".root")

  plotter = Plotter(FileMu, 'MVAPlots', 'ROC_MVA_'+suffix, '2012','mu','Signal2012ggM123')
  for key in plotter.folderDict.keys():
    if 'CAT5' in key: continue
    plotter.ROCcurves(plotter.folderDict[key])
    plotter.ROCcurves(plotter.folderDict[key],123)
  #plotterEl = Plotter(FileEl, 'MVAPlots', 'ROC_MVA_'+suffix, '2012','el','Signal2012ggM123')
  #for key in plotterEl.folderDict.keys():
    #if 'CAT5' in key: continue
    #plotterEl.ROCcurves(plotterEl.folderDict[key])
    #plotterEl.ROCcurves(plotterEl.folderDict[key],123)

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
    FileEl1= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_04-1-14_PhoMVA.root")
    FileEl2= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_04-1-14_PhoKinMVA.root")
    FileMu1= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_04-1-14_PhoMVA.root")
    FileMu2= TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_04-1-14_PhoKinMVA.root")
    #FileMu = TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_02-12-14_PUTest.root")
    #FileEl = TFile("/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_02-12-14_PUTest.root")

  #plotter = Plotter(FileMu, 'Vtx', 'PUCheck_NoRD', '2012','mu','Signal2012ggM125p8')
  #plotterMu = Plotter([FileMu1,FileMu2], 'pT-Eta-Phi', 'ntuple8v9', '2012','mu','Signal2012ggM125NLOp8')
  #plotterMu = Plotter([FileMu1,FileMu2], 'ZGamma', 'ntuple8v9', '2012','mu','Signal2012ggM125NLOp8')
  #for key in plotterMu.folderDict[0].keys():
    #plotterMu.RatioPlot(key,['Signal2012ggM125NLOp8','Signal2012ggM125NLOp8'],['nv8.2','nv9.4'])
    #plotterMu.RatioPlot(key,['DATA','DATA'],['nv8.2','nv9.4'])

  #plotterEl = Plotter([FileEl1,FileEl2], 'pT-Eta-Phi', 'ntuple8v9', '2012','el','Signal2012ggM125NLOp8')
  folders = ['CAT1','CAT2','CAT3','CAT4','CAT5','CAT6','CAT7','CAT8','CAT9']
  for folder in folders:
    plotterEl = Plotter(FileEl2, folder, 'PhoKinMVARatio', '2012','el','Signal2012ggM125')
    for key in plotterEl.folderDict.keys():
      plotterEl.RatioPlot(key,['Signal2012ggM125','bg'],['Signal','bg'],True)
      #plotterEl.RatioPlot(key,['DATA','DATA'],['phoMVA','mediumWP'],True)
    plotterMu = Plotter(FileMu2, folder, 'PhoKinMVARatio', '2012','mu','Signal2012ggM125')
    for key in plotterMu.folderDict.keys():
      plotterMu.RatioPlot(key,['Signal2012ggM125','bg'],['Signal','bg'],True)
      #plotterMu.RatioPlot(key,['DATA','DATA'],['phoMVA','mediumWP'],True)

  '''
  folders = ['Vtx']
  for folder in folders:
    plotterEl = Plotter(FileEl, folder, 'NewPuRD1', '2012','el','Signal2012ggM125')
    for key in plotterEl.folderDict.keys():
      plotterEl.RatioPlot(key,['DATA','Signal'],['Data','Signal'],True)
    plotterMu = Plotter(FileMu, folder, 'NewPuRD1', '2012','mu','Signal2012ggM125')
    for key in plotterMu.folderDict.keys():
      plotterMu.RatioPlot(key,['DATA','Signal'],['Data','Signal'],True)
  '''

def DoAll():
  if os.environ.get('AT_NWU'):
    mainPath = '/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_'
    suffix = '06-11-14_ZGToLLGTest3'
    headDir = 'Full_'+suffix
    if not os.path.isdir(headDir):
      os.mkdir(headDir)
    FileMu = TFile(mainPath+'MuMu2012ABCD_'+suffix+'.root','OPEN')
    #FileEl = TFile(mainPath+'EE2012ABCD_'+suffix+'.root','OPEN')
    folders = ['ZGAngles', 'ZGAngles_RECO','MVAPlots','pT-Eta-Phi','PreSelDiLep','PreSelThreeBody','PreSelDiLepNoW','PreSelThreeBodyNoW']
    #folders = ['PreSelDiLep']
    #folders = ['ZGamma','CAT1','CAT2','CAT3','CAT4','CAT5','CAT6','CAT7','CAT8','CAT9','pT-Eta-Phi','MVAPlots','Misc','ZGAngles_RECO','PreSel']
    for folder in folders:
     # plotterEl = Plotter(FileEl, folder, headDir+'/'+folder, '2012','el','Signal2012ggM125')
     # for key in plotterEl.folderDict.keys():
     #   plotterEl.RatioPlot(key,['DATA','Signal'],['Data','Signal'],True)
     #   plotterEl.RatioPlot(key,['DATA','BG'],['Data','BG'],False)
     #   plotterEl.directory = headDir+'/'+folder
     #   plotterEl.DataBGComp(plotterEl.folderDict[key])
     #   plotterEl.DataBGComp2DProj(plotterEl.folderDict[key])
      plotterMu = Plotter(FileMu, folder, headDir+'/'+folder, '2012','mu','Signal2012ggM125')
      for key in plotterMu.folderDict.keys():
        #plotterMu.RatioPlot(key,['Signal','BG'],['Signal','BG'],True)
        #plotterMu.RatioPlot(key,['DATA','BG'],['DATA','BG'],True)
        plotterMu.RatioPlot(key,['DATA','BG'],['DATA','BG'],False)
        #plotterMu.RatioPlot(key,['DYToMuMu','ZGToLLG'],['DYToMuMu','ZGToLLG'],False)
        #plotterMu.RatioPlot(key,['Signal','ZGToLLG'],['Signal','ZGToLLG'],True)
        #plotterMu.RatioPlot(key,['Signal','DYJets'],['Signal','DYJets'],True)
        plotterMu.directory = headDir+'/'+folder
        plotterMu.DataBGComp(plotterMu.folderDict[key])
        plotterMu.DataBGComp2DProj(plotterMu.folderDict[key])
  else: print 'this is not set up for FNAL'

def DoMulti():
  if os.environ.get('AT_NWU'):
    mainPath = '/tthome/bpollack/CMSSW_5_3_11_patch6/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_'
    suffix1 = suffix2 = suffix3 = suffix4 = '04-28-14_PhoMVA'
    headDir = '_'.join(['Multi',suffix1,suffix2,suffix3,suffix4])
    if not os.path.isdir(headDir):
      os.mkdir(headDir)
    FileMu1 = TFile(mainPath+'MuMu2012ABCD_'+suffix1+'.root','OPEN')
    FileMu2 = TFile(mainPath+'MuMu2012ABCD_'+suffix2+'.root','OPEN')
    FileMu3 = TFile(mainPath+'MuMu2012ABCD_'+suffix3+'.root','OPEN')
    FileMu4 = TFile(mainPath+'MuMu2012ABCD_'+suffix4+'.root','OPEN')
    files = [FileMu1,FileMu2,FileMu3,FileMu4]
    #FileEl1 = TFile(mainPath+'EE2012ABCD_'+suffix1+'.root','OPEN')
    #FileEl2 = TFile(mainPath+'EE2012ABCD_'+suffix2+'.root','OPEN')
    #FileEl3 = TFile(mainPath+'EE2012ABCD_'+suffix3+'.root','OPEN')
    #FileEl = TFile(mainPath+'EE2012ABCD_'+suffix+'.root','OPEN')
    #folders = ['ZGamma','CAT1','CAT2','CAT3','CAT4','CAT5','pT-Eta-Phi','ZGAngles_RECO']
    folders = ['CAT1','CAT2','CAT3','CAT4']

    plotterMu = Plotter(files, folders, headDir, '2012','mu','Signal2012ggM125')
    #for key in plotterMu.folderDict[0].keys():
      #print key
    keys = ['mvaVal']*4
    plotterMu.MultiPlots([a+b for a,b in zip(keys,folders)],['DATA','DATA','DATA','DATA'],folders,True)
  else: print 'this is not set up for FNAL'


if __name__=="__main__":
  print 'executing', sys.argv[1]
  if 'plot' == sys.argv[1].lower():
    GenericPlotter(sys.argv[2], sys.argv[3])
  elif 'roc' == sys.argv[1].lower():
    if sys.argv[2] != '-b': ROCPlotter(sys.argv[2])
    else: ROCPlotter()
  elif 'ratio' == sys.argv[1].lower():
    RatioPlotter()
  elif 'all' == sys.argv[1].lower():
    DoAll()
  elif 'multi' == sys.argv[1].lower():
    DoMulti()
