#!/usr/bin/env python
import sys,os
from PlottingClasses import *
from ROOT import *
import numpy as np
from tempfile import mkstemp
from shutil import move
from collections import OrderedDict

gROOT.ProcessLine('.L ./tdrstyle.C')
setTDRStyle()
gROOT.SetBatch(True)


def GenericPlotter(inFolder, outFolder):
  if os.environ.get('AT_NWU') == None:
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_ME_2Dv5.root")
    #FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-18-13_anglesOnly.root")
  else:
    FileMu = TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_02-01-14_Cut.root")
    FileEl = TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_02-01-14_Cut.root")

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
  #FileMu= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_02-04-14.root")
  #FileEl= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_02-04-14.root")
  FileMu= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_"+suffix+".root")
  FileEl= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_"+suffix+".root")

  plotter = Plotter(FileMu, 'MVAPlots', 'ROC_MVA_'+suffix, '2012','mu','Signal2012ggM123')
  for key in plotter.folderDict.keys():
    if 'CAT5' in key: continue
    plotter.ROCcurves(plotter.folderDict[key])
    plotter.ROCcurves(plotter.folderDict[key],123)
  plotterEl = Plotter(FileEl, 'MVAPlots', 'ROC_MVA_'+suffix, '2012','el','Signal2012ggM123')
  for key in plotterEl.folderDict.keys():
    if 'CAT5' in key: continue
    plotterEl.ROCcurves(plotterEl.folderDict[key])
    plotterEl.ROCcurves(plotterEl.folderDict[key],123)

def RatioPlotter():
  if os.environ.get('AT_NWU') == None:
    #FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_signalComp.root")
    FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_signalTest.root")
    #FileEl= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_11-20-13_anglesOnly.root")
  else:
    #FileMu1= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-13-14.root")
    #FileMu2= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_01-21-14_v9_4.root")
    #FileEl1= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_01-13-14.root")
    #FileEl2= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_01-21-14_v9_4.root")
    FileEl1= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_04-1-14_PhoMVA.root")
    FileEl2= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_04-1-14_PhoKinMVA.root")
    FileMu1= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_04-1-14_PhoMVA.root")
    FileMu2= TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_04-1-14_PhoKinMVA.root")
    #FileMu = TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_02-12-14_PUTest.root")
    #FileEl = TFile("/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_EE2012ABCD_02-12-14_PUTest.root")

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

  #folders = ['Vtx']
  #for folder in folders:
  #  plotterEl = Plotter(FileEl, folder, 'NewPuRD1', '2012','el','Signal2012ggM125')
  #  for key in plotterEl.folderDict.keys():
  #    plotterEl.RatioPlot(key,['DATA','Signal'],['Data','Signal'],True)
  #  plotterMu = Plotter(FileMu, folder, 'NewPuRD1', '2012','mu','Signal2012ggM125')
  #  for key in plotterMu.folderDict.keys():
  #    plotterMu.RatioPlot(key,['DATA','Signal'],['Data','Signal'],True)

def DoAll(suffix):
  if os.environ.get('AT_NWU'):
    mainPath = '/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_'
    headDir = 'Full_'+suffix
    if not os.path.isdir(headDir):
      os.mkdir(headDir)
    if 'HighMass' in suffix: testMass = 'M200'
    else: testMass = 'M125'

    FileMu = FileEl = None
    if os.path.isfile(mainPath+'MuMu2012ABCD_'+suffix+'.root'):
      FileMu = TFile(mainPath+'MuMu2012ABCD_'+suffix+'.root','OPEN')
      if (FileMu.GetSize() == -1):
        raise IOError(mainPath+'MuMu2012ABCD_'+suffix+'.root not found')
    if os.path.isfile(mainPath+'EE2012ABCD_'+suffix+'.root'):
      FileEl = TFile(mainPath+'EE2012ABCD_'+suffix+'.root','OPEN')
      if (FileEl.GetSize() == -1):
        raise IOError(mainPath+'EE2012ABCD_'+suffix+'.root not found')

    #folders = ['ZGAngles_RECO','MVAPlots','pT-Eta-Phi','PreSelDiLep','PreSelThreeBody','PreSelDiLepNoW','PreSelThreeBodyNoW']
    #folders = ['pT-Eta-Phi','HighMass','MVAPlots','ZGAngles_RECO','ZGamma','PreSelDiLep','PreSelThreeBody','PreGen']
    #folders = ['pT-Eta-Phi','HighMass','MVAPlots','ZGAngles_RECO','ZGamma','PreSelThreeBody','PreGen']
    folders = ['pT-Eta-Phi','MVAPlots','ZGAngles_RECO','ZGamma','PreSelThreeBody','PreGen']
    #folders = ['pT-Eta-Phi']
    #folders = ['ZGamma','CAT1','CAT2','CAT3','CAT4','CAT5','CAT6','CAT7','CAT8','CAT9','pT-Eta-Phi','MVAPlots','ZGAngles_RECO','PreSelDiLep','PreSelThreeBody']
    if FileEl != None:
      for folder in folders:
        plotterEl = Plotter(FileEl, folder, headDir+'/'+folder, '2012','el','Signal2012gg'+testMass)
        for key in plotterEl.folderDict.keys():
          if 'diLepMassLowPreSelDiLep' in key: continue

          print 'el', folder, key, testMass
          if folder != 'PreGen':
            plotterEl.RatioPlot(key,['Signal2012gg'+testMass,'BG'],['Signal2012gg'+testMass,'BG'],True, False)
            if 'threeBodyMassHigh' in key:
              plotterEl.RatioPlot(key,['Signal2012gg'+testMass,'BG'],['Signal2012gg'+testMass,'BG'],False, False)
              #raw_input('this is the threeBodyMassHigh, signal/BG')
          #plotterEl.RatioPlot(key,['Signal2012ggM200','Signal2012ggM500'],['200','500'],True,False)
          if testMass != 'M125':
            try:
              plotterEl.RatioPlot(key,['Signal2012gg'+testMass,'Signal2012ggM125'],[testMass,'M125'],True, False)
            except:
              pass
          #plotterEl.RatioPlot(key,['BG','Signal'],['BG','Signal'],True, False)
          #plotterEl.RatioPlot(key,['DATA','BG'],['DATA','BG'],True)
          if folder != 'PreGen':
            if 'MassHigh' in key:
              plotterEl.RatioPlot(key,['DATA','BG'],['DATA','BG'],False,True)
            elif 'pvMult' in key:
              plotterEl.RatioPlot(key,['DATA','BG'],['DATA','BG'],True,False)
            else:
              plotterEl.RatioPlot(key,['DATA','BG'],['DATA','BG'],False,False)
              #if folder == 'PreSelDiLep' and key == 'diLepMassPreSelDiLep': raw_input()
          #plotterEl.RatioPlot(key,['DYToElEl','ZGToLLG'],['DYToElEl','ZGToLLG'],False)
          #plotterEl.RatioPlot(key,['Signal','ZGToLLG'],['Signal','ZGToLLG'],True)
          #plotterEl.RatioPlot(key,['Signal','DYJets'],['Signal','DYJets'],True)
          #plotterEl.directory = headDir+'/'+folder
          plotterEl.DataBGComp(plotterEl.folderDict[key])
          #plotterEl.DataBGComp2DProj(plotterEl.folderDict[key])
    if FileMu != None:
      for folder in folders:
        plotterMu = Plotter(FileMu, folder, headDir+'/'+folder, '2012','mu','Signal2012gg'+testMass)
        for key in plotterMu.folderDict.keys():
          if 'diLepMassLowPreSelDiLep' in key: continue
          print 'mu', folder, key, testMass
          if folder != 'PreGen': plotterMu.RatioPlot(key,['Signal2012gg'+testMass,'BG'],['Signal2012gg'+testMass,'BG'],True,False)
          #plotterMu.RatioPlot(key,['Signal2012ggM200','Signal2012ggM500'],['200','500'],True,False)
          if testMass != 'M125':
            try:
              plotterMu.RatioPlot(key,['Signal2012gg'+testMass,'Signal2012ggM125'],[testMass,'M125'],True, False)
            except:
              pass
          #plotterMu.RatioPlot(key,['DATA','BG'],['DATA','BG'],True)
          if folder != 'PreGen':
            if 'MassHigh' in key:
              plotterMu.RatioPlot(key,['DATA','BG'],['DATA','BG'],False,True)
            elif 'pvMult' in key:
              plotterMu.RatioPlot(key,['DATA','BG'],['DATA','BG'],True,False)
            else:
              plotterMu.RatioPlot(key,['DATA','BG'],['DATA','BG'],False,False)
          #plotterMu.RatioPlot(key,['DYToMuMu','ZGToLLG'],['DYToMuMu','ZGToLLG'],False)
          #plotterMu.RatioPlot(key,['Signal','ZGToLLG'],['Signal','ZGToLLG'],True)
          #plotterMu.RatioPlot(key,['Signal','DYJets'],['Signal','DYJets'],True)
          #plotterMu.directory = headDir+'/'+folder
          plotterMu.DataBGComp(plotterMu.folderDict[key])
          #plotterMu.DataBGComp2DProj(plotterMu.folderDict[key])
  else: print 'this is not set up for FNAL'

def DoMulti(suffix1, nS1, folder1, nF1, **kwargs):
  """There are 3 items that can be extended: the suffixes, the folders, and the keys.
  The suffixes determine the files, the folders determine the folder per file (number of files must equal number of folders).
  The total number of files (and folders): N = sum(suffix_i*nS_i,i=1,i=n).
  The keys are looped over, where each key is called N times, where N = N_suffixes = N_folders.  The syntax must be preserved."""

  if os.environ.get('AT_NWU'):
    mainPath = '/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_'

    suffixes = OrderedDict({suffix1:int(nS1)})

    moreSuffix = dict((k, v) for k, v in kwargs.iteritems() if 'suffix' in k)
    moreNs = dict((k, v) for k, v in kwargs.iteritems() if 'nS' in k)
    for i in range(2,len(moreSuffix)+2):
      suffixes[moreSuffix['suffix'+str(i)]] = moreNs['nS'+str(i)]

    headDir = '_'.join(['Multi']+[str(suffixes[k])+'x'+k for k in suffixes])
    if not os.path.isdir(headDir):
      os.mkdir(headDir)

    filesMu = []
    filesEl = []
    for suffix in suffixes:
      filesMu += [TFile(mainPath+'MuMu2012ABCD_'+suffix+'.root','OPEN')]*suffixes[suffix]
      filesEl += [TFile(mainPath+'EE2012ABCD_'+suffix+'.root','OPEN')]*suffixes[suffix]

    folders = [folder1]*int(nF1)

    moreFolder = dict((k, v) for k, v in kwargs.iteritems() if 'folder' in k)
    moreNf = dict((k, v) for k, v in kwargs.iteritems() if 'nF' in k)
    for i in range(2,len(moreFolder)+2):
      folders += [moreFolder['folder'+str(i)]]*moreNf['nF'+str(i)]

    print len(filesMu), len(folders)
    plotterMu = Plotter(filesMu, folders, headDir, '2012','mu','Signal2012ggM125')
    plotterEl = Plotter(filesEl, folders, headDir, '2012','el','Signal2012ggM125')
    #chooseNames = ['BG','Signal2012ggM200','Signal2012ggM250','Signal2012ggM300','Signal2012ggM350','Signal2012ggM400','Signal2012ggM450','Signal2012ggM500']
    chooseNames = ['BG','Signal2012ggM200','Signal2012ggM250','Signal2012ggM300','Signal2012ggM350','Signal2012ggM400','Signal2012ggM450','Signal2012ggM500','Signal2012ggM800','Signal2012ggM1200']
    #chooseNames = ['BG','Signal2012ggM200Narrow','Signal2012ggM250Narrow','Signal2012ggM300Narrow','Signal2012ggM350Narrow','Signal2012ggM400Narrow','Signal2012ggM450Narrow','Signal2012ggM500Narrow','Signal2012ggM800Narrow','Signal2012ggM1200Narrow']
    #chooseNames = ['BG','Signal2012ggM200','Signal2012ggM500']
    #keys = ['photonPtHigh','diLepPtHigh','threeBodyMassHigh']
    for key in plotterEl.folderDict[0].keys():
    #for key in keys:
      key = [key]*len(folders)
      if 'Mass' in key[0]:
        plotterMu.MultiPlots(key,chooseNames,chooseNames,True,True)
        plotterEl.MultiPlots(key,chooseNames,chooseNames,True,True)
      else:
        plotterMu.MultiPlots(key,chooseNames,chooseNames,True)
        plotterEl.MultiPlots(key,chooseNames,chooseNames,True)

  else: print 'this is not set up for FNAL'

def DoAcc(suffix):
  if os.environ.get('AT_NWU'):
    mainPath = '/tthome/bpollack/CMSSW_6_1_1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_'
    headDir = 'AccEff_'+suffix
    if not os.path.isdir(headDir):
      os.mkdir(headDir)
    if 'HighMass' in suffix: testMass = 'M200'
    else: testMass = 'M125'

    FileMu = FileEl = None
    if os.path.isfile(mainPath+'MuMu2012ABCD_'+suffix+'.root'):
      FileMu = TFile(mainPath+'MuMu2012ABCD_'+suffix+'.root','OPEN')
      if (FileMu.GetSize() == -1):
        raise IOError(mainPath+'MuMu2012ABCD_'+suffix+'.root not found')
    if os.path.isfile(mainPath+'EE2012ABCD_'+suffix+'.root'):
      FileEl = TFile(mainPath+'EE2012ABCD_'+suffix+'.root','OPEN')
      if (FileEl.GetSize() == -1):
        raise IOError(mainPath+'EE2012ABCD_'+suffix+'.root not found')

    massList = [200,250,300,350,400,450,500,800,1200]
    if FileEl != None:
      plotterEl = Plotter(FileEl, 'Misc', headDir+'/acc', '2012','el','Signal2012gg'+testMass)
      plotterEl.AcceptancePlot(massList,False)
      plotterEl.AcceptancePlot(massList,True)
    if FileMu != None:
      plotterMu = Plotter(FileMu, 'Misc', headDir+'/acc', '2012','mu','Signal2012gg'+testMass)
      plotterMu.AcceptancePlot(massList,False)
      plotterMu.AcceptancePlot(massList,True)

def update_default(args):
  #Create temp file
  fh, abs_path = mkstemp()
  new_file = open(abs_path,'w')
  try:
    old_file = open('.scripts_hist')
  except IOError:
    old_file = open('.scripts_hist','w')
    old_file.write('last good script calls:\n')
    old_file.close()
    old_file = open('.scripts_hist')

  rep = False
  for line in old_file:
    if len(args[0:2]) == len([x for x in line if x in args[0:2]]):
      rep = True
      new_file.write(' '.join(args)+'\n')
    else:
      new_file.write(line)
  if not rep:
    new_file.write(' '.join(args)+'\n')

  #close temp file
  new_file.close()
  os.close(fh)
  old_file.close()
  #Remove original file
  os.remove('.scripts_hist')
  #Move new file
  move(abs_path, '.scripts_hist')

if __name__=="__main__":
  print len(sys.argv)
  if len(sys.argv) < 3:
    raise NameError('bad call')

  print 'executing', sys.argv[1]
  if 'plot' == sys.argv[1].lower():
    GenericPlotter(sys.argv[2], sys.argv[3])
  elif 'roc' == sys.argv[1].lower():
    if sys.argv[2] != '-b': ROCPlotter(sys.argv[2])
    else: ROCPlotter()
  elif 'ratio' == sys.argv[1].lower():
    RatioPlotter()
  elif 'all' == sys.argv[1].lower():
    DoAll(sys.argv[2])
    update_default(sys.argv)
  elif 'multi' == sys.argv[1].lower():
    DoMulti(*sys.argv[2:])
    update_default(sys.argv)
  elif 'acc' == sys.argv[1].lower():
    DoAcc(*sys.argv[2:])
    update_default(sys.argv)






