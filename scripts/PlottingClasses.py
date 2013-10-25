#!/usr/bin/env python
import sys,os
#sys.argv.append('-b')
from ROOT import *
from collections import defaultdict
import numpy as np

#color dictionary
colorDict = {'DYJets':kGreen+1,'ZGToLLG':kBlue}

# class for multi-layered nested dictionaries, pretty cool
class AutoVivification(dict):
    """Implementation of perl's autovivification feature."""
    def __getitem__(self, item):
        try:
            return dict.__getitem__(self, item)
        except KeyError:
            value = self[item] = type(self)()
            return value

class Plotter:
  """Class to handle most standard plotting needs"""
  def __init__(self, thisFile, inFolder, outFolder, year, lepton, signal):
    self.thisFile = thisFile
    self.folder = inFolder
    self.directory = outFolder
    self.year = year
    self.lepton = lepton
    self.sigName = signal
    self.FolderDump()

  def FolderDump(self):
    '''Input file and folder name, output default dictionary of histogram lists. the key name is the distribution, the lists are all the samples for the given distribution'''
    folderDict = defaultdict(list)
    lok = self.thisFile.GetDirectory(self.folder).GetListOfKeys()
    for i in range(0, lok.GetEntries()):
      name = lok.At(i).GetName()
      key = name.split('_')[1]
      folderDict[key].append(self.thisFile.GetDirectory(self.folder).Get(name))
    self.folderDict = folderDict

  def LumiXSScale(self,name):
    '''Outputs scale for MC with respect to lumi and XS'''

    if name is 'DATA': return 1

    lumi = 0
    if self.lepton is 'mu': lumi = 19.672
    elif self.lepton is 'el': lumi = 19.711
    else: raise NameError('LumiXSScale lepton incorrect')

    scaleDict = AutoVivification()

    scaleDict['2012']['DYJets'] = 3503.71*1000
    scaleDict['2012']['ZGToLLG'] = 156.2*1000
    scaleDict['2012']['gg']['125'] = 19.52*0.00154*0.10098*1000

    initEvents = self.thisFile.GetDirectory('Misc').Get('h1_acceptanceByCut_'+name).Integral(1,1)
    if 'Signal' in name:
      sig = name[10:].partition('M')[0]
      mass = name[10:].partition('M')[-1][0:3]
      scale = initEvents/scaleDict[self.year][sig][mass]
    else:
      scale = initEvents/scaleDict[self.year][name]
    scale = lumi/scale
    return scale

  def GetDataHist(self,histList):
    dataHist = None
    for hist in histList:
      if 'DATA' in hist.GetName():
        dataHist = hist.Clone()
        break
    if not dataHist: raise NameError('No dataHist found in this list')
    dataHist.SetLineColor(kBlack)
    dataHist.SetMarkerColor(kBlack)
    dataHist.SetMarkerStyle(20)
    dataHist.SetFillStyle(0)
    return dataHist

  def GetBGHists(self, histList):
    bgList = [hist.Clone() for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]
    if len(bgList) == 0: raise NameError('No BG hists found in this list')
    bgList = sorted(bgList, key=lambda hist:hist.GetName()[-1], reverse=True)
    return bgList

  def GetSignalHist(self, histList):
    signalHist = None
    for hist in histList:
      if self.sigName in hist.GetName():
        signalHist= hist.Clone()
        break
    if not signalHist: raise NameError('No signalHist found in this list')
    signalHist.SetLineColor(kRed)
    signalHist.SetLineWidth(2)
    signalHist.SetFillStyle(0)
    return signalHist

  def MakeBGStack(self, bgList, dontStack = False):
    if dontStack:
      bgStack = bgList[0].Clone()
      bgStack.Reset()
      for hist in bgList:
        scale = LumiXSScale(label)
        hist.Scale(scale)
        bgStack.Add(hist)
      bgStack.SetFillColor(kBlue)
      self.leg.AddEntry(bgStack,'BG','f')
    else:
      bgStack = THStack('bgs','bgs')
      for hist in bgList:
        label = hist.GetName().split('_')[-1]
        hist.SetFillStyle(1001)
        hist.SetFillColor(colorDict[label])
        hist.SetLineColor(colorDict[label])
        scale = self.LumiXSScale(label)
        hist.Scale(scale)
        self.leg.AddEntry(hist,label,'f')
        bgStack.Add(hist)
    return bgStack

  def DrawHists(self, data,bg,signal, do2D = False):
    ymax = max(map(lambda x:x.GetMaximum(),[data,bg,signal]))*1.2
    ymin = 0

    bg.SetMaximum(ymax)
    bg.SetMinimum(ymin)
    if not do2D: bg.Draw('hist')
    else: bg.Draw('colz')
    bg.GetYaxis().SetTitle(data.GetYaxis().GetTitle())
    bg.GetYaxis().SetTitleSize(0.06)
    bg.GetYaxis().CenterTitle()
    bg.GetXaxis().SetTitle(data.GetXaxis().GetTitle())
    bg.GetXaxis().SetTitleSize(0.05)
    #bg.GetYaxis().SetLabelSize(0.05)
    #bg.GetXaxis().SetLabelSize(0.05)
    #bg.GetXaxis().SetTitle(dist)
    bg.SetTitle(self.lepton+self.lepton+' '+data.GetTitle())
    bg.GetYaxis().SetTitleOffset(0.82)

    if not do2D: data.Draw('pesame')
    else: data.Draw('boxsame')

    if not do2D: signal.Draw('histsame')
    else: signal.Draw('boxsame')

  def DataBGComp(self,histList):
    '''Give a list of histograms that contain the data and backgrounds, plot them and save them'''
    if len(histList) == 0: raise NameError('histList is empty')

    gStyle.SetOptStat(0)
    do2D = False
    if histList[0].GetName().split('_')[0] in ['h2','p']: do2D = True

    dataHist = self.GetDataHist(histList)
    bgList = self.GetBGHists(histList)
    signalHist = self.GetSignalHist(histList)

    if not os.path.isdir(self.directory):
      os.mkdir(self.directory)

    TH1.SetDefaultSumw2(kTRUE)
    TProfile.SetDefaultSumw2(kTRUE)

    # Make legend and canvas
    self.can= TCanvas('can','canvas',800,600)
    self.can.cd()
    if do2D:
      self.can.SetRightMargin(0.1)
      self.leg = TLegend(0.75,0.75,0.90,0.92,'',"brNDC")
    else:
      self.leg = TLegend(0.81,0.73,0.97,0.92,'',"brNDC")
    self.leg.SetBorderSize(1)
    self.leg.SetTextSize(0.03)
    self.leg.SetFillColor(0)
    self.leg.SetFillStyle(0)
    if do2D:
      self.leg.AddEntry(dataHist,'DATA','f')
    else:
      self.leg.AddEntry(dataHist,'DATA','lep')

    bgStack = self.MakeBGStack(bgList)

    scale = self.LumiXSScale(self.sigName)
    signalHist.Scale(scale*500)
    if do2D:
      self.leg.AddEntry(signalHist,'Signalx500','f')
    else:
      self.leg.AddEntry(signalHist,'Signalx500','l')

    self.DrawHists(dataHist,bgStack,signalHist,do2D)

    self.leg.Draw()
    self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+dataHist.GetName().split('_')[1]+'.pdf')
    self.can.IsA().Destructor(self.can)
