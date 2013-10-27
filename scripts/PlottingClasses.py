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
    self.doProj = False

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

  def GetDataHist(self,histList,sigWindow = False):
    dataHist = None
    if self.doProj:
      for hist in histList:
        if 'DATA' in hist.GetName():
          if sigWindow: hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(sigWindow-3),hist.GetXaxis().FindBin(sigWindow+3))
          dataHist = hist.Clone().ProjectionY('proj'+hist.GetName())
          break
    else:
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

  def GetDataHistsMD(self,histList,sigWindow = False):
    dataHistM = None
    dataHistD = None
    for hist in histList:
      if 'DATA' in hist.GetName():
        if sigWindow: hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(sigWindow-3),hist.GetXaxis().FindBin(sigWindow+3))
        dataHistM = hist.Clone().ProjectionX('projM'+hist.GetName())
        dataHistD = hist.Clone().ProjectionY('projD'+hist.GetName())
        break
    if not dataHistM: raise NameError('No dataHist found in this list')
    dataHistM.SetLineColor(kBlack)
    dataHistM.SetMarkerColor(kBlack)
    dataHistM.SetMarkerStyle(20)
    dataHistM.SetFillStyle(0)
    dataHistD.SetLineColor(kBlack)
    dataHistD.SetMarkerColor(kBlack)
    dataHistD.SetMarkerStyle(20)
    dataHistD.SetFillStyle(0)
    return (dataHistM, dataHistD)

  def GetBGHists(self, histList,sigWindow = False):
    if self.doProj:
      if sigWindow:
        bgList = []
        for hist in histList:
          if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1):
            hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(sigWindow-3),hist.GetXaxis().FindBin(sigWindow+3))
            bgList.append(hist.Clone().ProjectionY('proj'+hist.GetName()))
      else:
        bgList = [hist.ProjectionY('proj'+hist.GetName()) for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]
    else:
      bgList = [hist.Clone() for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]
    if len(bgList) == 0: raise NameError('No BG hists found in this list')
    bgList = sorted(bgList, key=lambda hist:hist.GetName()[-1], reverse=True)
    return bgList

  def GetBGHistsMD(self, histList,sigWindow = False):
    if sigWindow:
      bgListM = []
      bgListD = []
      for hist in histList:
        if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1):
          hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(sigWindow-3),hist.GetXaxis().FindBin(sigWindow+3))
          bgListM.append(hist.Clone().ProjectionX('projM'+hist.GetName()))
          bgListD.append(hist.Clone().ProjectionY('projD'+hist.GetName()))
    else:
      bgListM = [hist.ProjectionX('projM'+hist.GetName()) for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]
      bgListD = [hist.ProjectionY('projD'+hist.GetName()) for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]

    if len(bgListM) == 0: raise NameError('No BG hists found in this list')
    bgListM = sorted(bgListM, key=lambda hist:hist.GetName()[-1], reverse=True)
    bgListD = sorted(bgListD, key=lambda hist:hist.GetName()[-1], reverse=True)
    return (bgListM, bgListD)

  def GetSignalHist(self, histList,sigWindow = False):
    signalHist = None
    if self.doProj:
      for hist in histList:
        if self.sigName in hist.GetName():
          if sigWindow: hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(sigWindow-3),hist.GetXaxis().FindBin(sigWindow+3))
          signalHist = hist.Clone().ProjectionY('proj'+hist.GetName())
          break
    else:
      for hist in histList:
        if self.sigName in hist.GetName():
          signalHist= hist.Clone()
          break
    if not signalHist: raise NameError('No signalHist found in this list')
    signalHist.SetLineColor(kRed)
    signalHist.SetLineWidth(2)
    signalHist.SetFillStyle(0)
    return signalHist

  def GetSignalHistsMD(self,histList, sigWindow=False)
    signalHistM = None
    signalHistD = None
    for hist in histList:
      if sigName in hist.GetName():
        if sigWindow: hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(sigWindow-3),hist.GetXaxis().FindBin(sigWindow+3))
        signalHistM = hist.Clone().ProjectionX('projM'+hist.GetName())
        signalHistD = hist.Clone().ProjectionY('projD'+hist.GetName())
      break
    if not signalHist: raise NameError('No signalHist found in this list')
    signalHistM.SetLineColor(kRed)
    signalHistM.SetLineWidth(2)
    signalHistM.SetFillStyle(1001)
    signalHistD.SetLineColor(kRed)
    signalHistD.SetLineWidth(2)
    signalHistD.SetFillStyle(1001)
    return (signalHistM, signalHistD)


  def MakeBGStack(self, bgList, dontStack = False):
    if dontStack:
      bgStack = bgList[0].Clone()
      bgStack.Reset()
      for hist in bgList:
        label = hist.GetName().split('_')[-1]
        scale = self.LumiXSScale(label)
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

    bgStack = self.MakeBGStack(bgList,do2D)

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

  def DataBGComp2DProj(self,histList, sigWindow = False):
    '''Project the Y-axis of 2D hists.  If a sigWindow is specified, the assumption is that the x-axis is the 3body mass'''
    if len(histList) == 0: raise NameError('histList is empty')
    gStyle.SetOptStat(0)
    if histList[0].GetName().split('_')[0] != 'h2':
      print 'skipping 2D proj for', histList[0].GetName()
      return

    self.doProj = True

    dataHist = self.GetDataHist(histList,sigWindow)
    bgList =self.GetBGHists(histList,sigWindow)
    signalHist = self.GetSignalHist(histList,sigWindow)


    if not os.path.isdir(self.directory):
      os.mkdir(self.directory)

    TH1.SetDefaultSumw2(kTRUE)
    TProfile.SetDefaultSumw2(kTRUE)

    # Make canvas and legend
    self.can= TCanvas('can','canvas',800,600)
    self.can.cd()
    self.leg = TLegend(0.81,0.73,0.97,0.92,'',"brNDC")
    self.leg.SetBorderSize(1)
    self.leg.SetTextSize(0.03)
    self.leg.SetFillColor(0)
    self.leg.SetFillStyle(0)

    self.leg.AddEntry(dataHist,'DATA','lep')

    # Set the bg histograms
    bgStack = self.MakeBGStack(bgList)

    # Set the signal histograms
    scale = self.LumiXSScale(self.sigName)
    signalHist.Scale(scale*500)
    self.leg.AddEntry(signalHist,'Signalx500','l')

    self.DrawHists(dataHist,bgStack,signalHist)

    self.leg.Draw()
    if sigWindow:
      self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+dataHist.GetName().split('_')[1]+'projWindow.pdf')
    else:
      self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+dataHist.GetName().split('_')[1]+'proj.pdf')
    self.can.IsA().Destructor(self.can)
    self.doProj = False

  def ROCcurves(self, histList,sigWindow=False):
    '''Give a list of histograms, calculate ROC curve for that distribution.  Need 2D hist, mass vs disc for proper significance calculation'''
    if len(histList) == 0: raise NameError('histList is empty')
    gStyle.SetOptStat(0)
    if histList[0].GetName().split('_')[0] != 'h2':
      print 'skipping ROC curve for', histList[0].GetName()
      return

    dataHistM,dataHistD = self.GetDataHistsMD(histList,sigWindow)
    bgListM,bgListD = self.GetBGHistsMD(histList,sigWindow)
    signalHistM, signalHistD = self.GetSignalHistsMD(histList,sigWindow)


    if not os.path.isdir(directory):
      os.mkdir(directory)

    TH1.SetDefaultSumw2(kTRUE)
    TProfile.SetDefaultSumw2(kTRUE)

    self.can= TCanvas('can','canvas',800,600)
    self.can.cd()
    self.leg = TLegend(0.81,0.73,0.97,0.92,'',"brNDC")
    self.leg.SetBorderSize(1)
    self.leg.SetTextSize(0.03)
    self.leg.SetFillColor(0)
    self.leg.SetFillStyle(0)

    # Set the bg histograms
    bgStack = bgList[0].Clone()
    bgStack.Reset()
    for hist in bgList:
      label = hist.GetName().split('_')[-1]
      hist.SetFillStyle(1001)
      hist.SetFillColor(colorDict[label])
      hist.SetLineColor(colorDict[label])
      initEvents = thisFile.GetDirectory('Misc').Get('h1_acceptanceByCut_'+label).Integral(1,1)
      scale = LumiXSScale(year,lepton,label,initEvents)
      hist.Scale(scale)
      #hist.Scale(scale*1.40)
      bgStack.Add(hist)

    bgStackM = THStack('bgsM','bgs')
    for hist in bgListM:
      label = hist.GetName().split('_')[-1]
      hist.SetFillStyle(1001)
      hist.SetFillColor(colorDict[label])
      hist.SetLineColor(colorDict[label])
      initEvents = thisFile.GetDirectory('Misc').Get('h1_acceptanceByCut_'+label).Integral(1,1)
      scale = LumiXSScale(year,lepton,label,initEvents)
      hist.Scale(scale)
      #hist.Scale(scale*1.40)
      leg.AddEntry(hist,label,'f')
      bgStackM.Add(hist)


    # Set the signal histograms
    signalHist.SetLineColor(kRed)
    signalHist.SetFillStyle(0)
    label = sigName
    initEvents = thisFile.GetDirectory('Misc').Get('h1_acceptanceByCut_'+label).Integral(1,1)
    scale = LumiXSScale(year,lepton,label,initEvents)
    signalHist.Scale(scale)
    signalHistM.SetLineColor(kRed)
    signalHistM.SetFillStyle(0)
    signalHistM.Scale(scale*100)
    leg.AddEntry(signalHistM,'Signalx100','l')


    #temp switch, use data instead of MC:
    #bgStack = dataHist

    bestCut = None
    bestBGEff= None
    bestSignif = 0
    rocCurve = TProfile('rocCurve',signalHist.GetTitle()+' ROC;BG eff;Signal eff',signalHist.GetNbinsX(),0,1,0,1)
    signifPlot = TProfile('signifPlot',signalHist.GetTitle()+' Signif;MEDisc;s/#sqrt{s+b}',signalHist.GetNbinsX(),signalHist.GetBinLowEdge(1),signalHist.GetBinLowEdge(1+signalHist.GetNbinsX()))

    for bin in range(1,signalHist.GetNbinsX()+1):
      #print bin, signalHist.GetBinLowEdge(bin), signalHist.GetBinContent(bin), signalHist.Integral(bin,signalHist.GetNbinsX())
      bgYield = bgStack.Integral(bin, bgStack.GetNbinsX())
      sigYield = signalHist.Integral(bin, signalHist.GetNbinsX())
      rocCurve.Fill(bgYield/bgStack.Integral(), sigYield/signalHist.Integral())
      if sigYield+bgYield > 0:
        if sigYield/sqrt(sigYield+bgYield) > bestSignif:
          bestSignif = sigYield/sqrt(sigYield+bgYield)
          bestCut = signalHist.GetBinLowEdge(bin)
          bestBGEff = bgYield/bgStack.Integral()
        signifPlot.Fill(signalHist.GetBinLowEdge(bin),sigYield/sqrt(sigYield+bgYield))
      else:
        signifPlot.Fill(signalHist.GetBinLowEdge(bin),0)

    #reversed
    #for bin in range(0,signalHist.GetNbinsX()):
      #print bin, signalHist.GetBinLowEdge(bin), signalHist.GetBinContent(bin), signalHist.Integral(bin,signalHist.GetNbinsX())
      #bgYield = bgStack.Integral(1, bgStack.GetNbinsX()-bin)
      #sigYield = signalHist.Integral(1, signalHist.GetNbinsX()-bin)
      #rocCurve.Fill(bgYield/bgStack.Integral(), sigYield/signalHist.Integral())
      #if sigYield+bgYield > 0:
        #if sigYield/sqrt(sigYield+bgYield) > bestSignif:
          #bestSignif = sigYield/sqrt(sigYield+bgYield)
          #bestCut = signalHist.GetBinLowEdge(signalHist.GetNbinsX()-bin)
          #bestBGEff = bgYield/bgStack.Integral()
        #signifPlot.Fill(signalHist.GetBinLowEdge(signalHist.GetNbinsX()-bin),sigYield/sqrt(sigYield+bgYield))
      #else:
        #signifPlot.Fill(signalHist.GetBinLowEdge(signalHist.GetNbinsX()-bin),0)


    rocCurve.GetYaxis().SetTitleSize(0.06)
    rocCurve.GetYaxis().CenterTitle()
    rocCurve.GetXaxis().SetTitleSize(0.05)
    rocCurve.GetYaxis().SetTitleOffset(0.82)
    rocCurve.Draw('pe')
    line = TLine(bestBGEff,rocCurve.GetMinimum(),bestBGEff,rocCurve.GetMaximum()*1.05)
    line.SetLineColor(kRed+1)
    line.SetLineStyle(2)
    line.SetLineWidth(2)
    line.Draw()

    if sigWindow: can.SaveAs(directory+'/'+lepton+lepton+'_'+signalHist.GetName().split('_')[1]+'_ROC_window.pdf')
    else: can.SaveAs(directory+'/'+lepton+lepton+'_'+signalHist.GetName().split('_')[1]+'_ROC.pdf')

    signifPlot.SetMarkerColor(kRed)
    signifPlot.GetYaxis().SetTitleSize(0.06)
    signifPlot.GetYaxis().CenterTitle()
    signifPlot.GetXaxis().SetTitleSize(0.05)
    signifPlot.GetYaxis().SetTitleOffset(0.82)
    signifPlot.Draw('pe')
    print bestCut,signifPlot.GetMinimum(),bestCut,signifPlot.GetMaximum()
    line = TLine(bestCut,signifPlot.GetMinimum(),bestCut,signifPlot.GetMaximum()*1.05)
    line.SetLineColor(kRed+1)
    line.SetLineWidth(2)
    line.SetLineStyle(2)
    line.Draw()

    if sigWindow: can.SaveAs(directory+'/'+lepton+lepton+'_'+signalHist.GetName().split('_')[1]+'_Signif_Window.pdf')
    else: can.SaveAs(directory+'/'+lepton+lepton+'_'+signalHist.GetName().split('_')[1]+'_Signif.pdf')

    ymax = max(map(lambda x:x.GetMaximum(),[bgStackM,signalHistM]))*1.2
    ymin = 0

    if sigWindow:
      tempHist = TH1F('tmp',lepton+lepton+' '+bgList[0].GetTitle()+' '+'Mass Proj',90,100,190)
      tempHist.SetMaximum(ymax)
      tempHist.SetMinimum(ymin)
      tempHist.GetYaxis().SetTitle('N_{evts}')
      tempHist.GetYaxis().SetTitleSize(0.06)
      tempHist.GetYaxis().CenterTitle()
      tempHist.GetXaxis().SetTitle(bgListM[0].GetXaxis().GetTitle())
      tempHist.GetXaxis().SetTitleSize(0.05)
      #tempHist.GetYaxis().SetLabelSize(0.05)
      #tempHist.GetXaxis().SetLabelSize(0.05)
      #tempHist.GetXaxis().SetTitle(dist)
      tempHist.SetTitle(lepton+lepton+' '+bgList[0].GetTitle()+' '+'Mass Proj')
      tempHist.GetYaxis().SetTitleOffset(0.82)

      tempHist.Draw()
    bgStackM.SetMaximum(ymax)
    bgStackM.SetMinimum(ymin)
    if sigWindow:
      bgStackM.Draw('histsame')
    else:
      bgStackM.Draw('hist')
    bgStackM.GetYaxis().SetTitle('N_{evts}')
    bgStackM.GetYaxis().SetTitleSize(0.06)
    bgStackM.GetYaxis().CenterTitle()
    bgStackM.GetXaxis().SetTitle(bgListM[0].GetXaxis().GetTitle())
    bgStackM.GetXaxis().SetTitleSize(0.05)
    #bgStackM.GetYaxis().SetLabelSize(0.05)
    #bgStackM.GetXaxis().SetLabelSize(0.05)
    #bgStackM.GetXaxis().SetTitle(dist)
    bgStackM.SetTitle(lepton+lepton+' '+bgList[0].GetTitle()+' '+'Mass Proj')
    bgStackM.GetYaxis().SetTitleOffset(0.82)

    signalHistM.Draw('histsame')

    leg.Draw()
    if sigWindow: can.SaveAs(directory+'/'+lepton+lepton+'_'+signalHistM.GetName().split('_')[1]+'_MassCheck_Window.pdf')
    else: can.SaveAs(directory+'/'+lepton+lepton+'_'+signalHistM.GetName().split('_')[1]+'_MassCheck.pdf')
    can.Clear()
    leg.Clear()

    print bestSignif, bestCut

    bgStackD = THStack('bgsD','bgs')
    for hist in bgList:
      label = hist.GetName().split('_')[-1]
      hist.SetFillStyle(1001)
      hist.SetFillColor(colorDict[label])
      hist.SetLineColor(colorDict[label])
      initEvents = thisFile.GetDirectory('Misc').Get('h1_acceptanceByCut_'+label).Integral(1,1)
      scale = LumiXSScale(year,lepton,label,initEvents)
      #hist.Scale(scale)
      hist.Scale(scale)
      leg.AddEntry(hist,label,'f')
      bgStackD.Add(hist)

    signalHist.Scale(100)
    leg.AddEntry(signalHist,'Signalx100','l')

    ymax = max(map(lambda x:x.GetMaximum(),[bgStackD,signalHist]))*1.2
    ymin = 0

    bgStackD.SetMaximum(ymax)
    bgStackD.SetMinimum(ymin)
    bgStackD.Draw('hist')
    bgStackD.GetYaxis().SetTitle('N_{evts}')
    bgStackD.GetYaxis().SetTitleSize(0.06)
    bgStackD.GetYaxis().CenterTitle()
    bgStackD.GetXaxis().SetTitle(bgListM[0].GetXaxis().GetTitle())
    bgStackD.GetXaxis().SetTitleSize(0.05)
    #bgStackD.GetYaxis().SetLabelSize(0.05)
    #bgStackD.GetXaxis().SetLabelSize(0.05)
    #bgStackD.GetXaxis().SetTitle(dist)
    bgStackD.SetTitle(lepton+lepton+' '+bgList[0].GetTitle()+' '+'Mass Proj')
    bgStackD.GetYaxis().SetTitleOffset(0.82)

    signalHist.Draw('histsame')
    line = TLine(bestCut,bgStackD.GetMinimum(),bestCut,ymax*1.05)
    line.SetLineColor(kRed+1)
    line.SetLineWidth(2)
    line.SetLineStyle(2)
    line.Draw()

    leg.Draw()
    if sigWindow: can.SaveAs(directory+'/'+lepton+lepton+'_'+signalHist.GetName().split('_')[1]+'_DiscCheck_Window.pdf')
    else: can.SaveAs(directory+'/'+lepton+lepton+'_'+signalHist.GetName().split('_')[1]+'_DiscCheck.pdf')
    can.Clear()
    leg.Clear()

    can.IsA().Destructor(can)
