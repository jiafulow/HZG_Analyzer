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
    self.can = None
    self.myCut = None

  def FolderDump(self):
    '''Input file and folder name, output default dictionary of histogram lists. the key name is the distribution, the lists are all the samples for the given distribution'''
    if type(self.thisFile) != list:
        folderDict = defaultdict(list)
        lok = self.thisFile.GetDirectory(self.folder).GetListOfKeys()
        for i in range(0, lok.GetEntries()):
          name = lok.At(i).GetName()
          key = name.split('_')[1]
          folderDict[key].append(self.thisFile.GetDirectory(self.folder).Get(name))
        self.folderDict = folderDict
    else:
        folderDict = []
        for aFile in self.thisFile:
            lok = aFile.GetDirectory(self.folder).GetListOfKeys()
            folderDict.append(defaultdict(list))
            for i in range(0, lok.GetEntries()):
              name = lok.At(i).GetName()
              key = name.split('_')[1]
              folderDict[-1][key].append(aFile.GetDirectory(self.folder).Get(name))
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

  def ChooseTwoHists(self,chooseNames,histList, histList2 = None):
    outList = []
    if chooseNames[0].lower() == 'bg' or chooseNames[0].lower() == 'background':
      bgList = self.GetBGHists(histList)
      bgHist = bgList[0].Clone()
      bgHist.Reset()
      for hist in bgList:
        label = hist.GetName().split('_')[-1]
        scale = self.LumiXSScale(label)
        hist.Scale(scale)
        bgHist.Add(hist)
      outList.append(bgHist)
    else:
      for hist in histList:
        if chooseNames[0] in hist.GetName():
          outList.append(hist.Clone())
          break

    if chooseNames[1].lower() == 'bg' or chooseNames[1].lower() == 'background':
      if histList2 == None: histList2 = histList
      bgList = self.GetBGHists(histList2)
      bgHist = bgList[0].Clone()
      bgHist.Reset()
      for hist in bgList:
        label = hist.GetName().split('_')[-1]
        scale = self.LumiXSScale(label)
        hist.Scale(scale)
        bgHist.Add(hist)
      outList.append(bgHist)
    else:
      for hist in histList2:
        if chooseNames[1] in hist.GetName():
          outList.append(hist.Clone())
          break

    if len(outList) != 2: raise NameError('ChooseTwoHists gave wrong list length: {0}'.format(len(outList)))

    outList[0].Scale(1/outList[0].Integral())
    outList[1].Scale(1/outList[1].Integral())

    ymax = max(map(lambda x:x.GetMaximum(),outList))*1.1
    ymin = 0
    outList[0].SetMaximum(ymax)
    outList[0].SetMinimum(ymin)
    outList[0].SetLineColor(kRed)
    outList[0].SetLineWidth(2)
    outList[0].GetYaxis().SetTitleOffset(0.82)
    outList[0].GetYaxis().SetTitleSize(0.06)
    outList[0].GetYaxis().CenterTitle()
    outList[0].GetXaxis().SetTitleSize(0.05)

    outList[1].SetLineColor(kBlue)
    outList[1].SetLineWidth(2)
    outList[1].Scale(1/outList[1].Integral())
    return outList


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
        dataHistM = hist.Clone().ProjectionX('projM'+hist.GetName(),1,hist.GetNbinsY(),'[mycut]')
        dataHistD = hist.Clone().ProjectionY('projD'+hist.GetName(),1,hist.GetNbinsX(),'[mycut]')
        break
    if not dataHistM: raise NameError('No dataHist found in this list')
    dataHistM.SetLineColor(kBlack)
    dataHistM.SetMarkerColor(kBlack)
    dataHistM.SetMarkerStyle(20)
    dataHistM.SetFillStyle(0)
    dataHistM.GetYaxis().SetTitle('Entries')
    dataHistM.SetTitle('Mass')
    dataHistD.SetLineColor(kBlack)
    dataHistD.SetMarkerColor(kBlack)
    dataHistD.SetMarkerStyle(20)
    dataHistD.SetFillStyle(0)
    dataHistD.GetYaxis().SetTitle('Entries')
    dataHistD.SetTitle('Discriminator')
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
    bgListM = [hist.Clone().ProjectionX('projM'+hist.GetName(),1,hist.GetNbinsY(),'[mycut]') for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]
    bgListD = [hist.Clone().ProjectionY('projD'+hist.GetName(),1,hist.GetNbinsX(),'[mycut]') for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]
    #bgListM = [hist.ProjectionX('projM'+hist.GetName(),1,hist.GetNbinsY(),'') for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]
    #bgListD = [hist.ProjectionY('projD'+hist.GetName(),1,hist.GetNbinsX(),'') for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]

    if len(bgListM) == 0: raise NameError('No BG hists found in this list')
    bgListM = sorted(bgListM, key=lambda hist:hist.GetName()[-1], reverse=True)
    bgListD = sorted(bgListD, key=lambda hist:hist.GetName()[-1], reverse=True)
    for i,hist in enumerate(bgListM):
      bgListM[i].GetYaxis().SetTitle('Entries')
      bgListD[i].GetYaxis().SetTitle('Entries')
      bgListM[i].SetTitle('Mass')
      bgListD[i].SetTitle('Discriminator')
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

  def GetSignalHistsMD(self,histList, sigWindow=False):
    signalHistM = None
    signalHistD = None
    for hist in histList:
      if self.sigName in hist.GetName():
        signalHistM = hist.Clone().ProjectionX('projM'+hist.GetName(),1,hist.GetNbinsY(),'[mycut]')
        signalHistD = hist.Clone().ProjectionY('projD'+hist.GetName(),1,hist.GetNbinsX(),'[mycut]')
        break
    if not signalHistM: raise NameError('No signalHist found in this list')
    signalHistM.SetLineColor(kRed)
    signalHistM.SetLineWidth(2)
    signalHistM.SetFillStyle(1001)
    signalHistD.SetLineColor(kRed)
    signalHistD.SetLineWidth(2)
    signalHistD.SetFillStyle(1001)
    return (signalHistM, signalHistD)


  def MakeBGStack(self, bgList, leg = None, dontStack = False):
    if dontStack:
      bgStack = bgList[0].Clone()
      bgStack.Reset()
      for hist in bgList:
        hist = hist.Clone()
        label = hist.GetName().split('_')[-1]
        scale = self.LumiXSScale(label)
        hist.Scale(scale)
        bgStack.Add(hist)
      bgStack.SetFillColor(kBlue)
      if leg: leg.AddEntry(bgStack,'BG','f')
    else:
      bgStack = THStack('stack'+bgList[0].GetName(),'bgs')
      for hist in bgList:
        hist = hist.Clone()
        label = hist.GetName().split('_')[-1]
        hist.SetFillStyle(1001)
        hist.SetFillColor(colorDict[label])
        hist.SetLineColor(colorDict[label])
        scale = self.LumiXSScale(label)
        hist.Scale(scale)
        if leg: leg.AddEntry(hist,label,'f')
        bgStack.Add(hist)
        print hist.GetName()
        print hist.Integral(0,hist.GetNbinsX())
        print 'bin1:',hist.GetBinContent(1)
        #raw_input()
    return bgStack

  def MakeLegend(self,x1 = None, y1 = None, x2 = None, y2 = None):
    if x1 == y1 == x2 == y2 == None:
      x1 = 0.81
      y1 = 0.73
      x2 = 0.97
      y2 = 0.92
    leg = TLegend(x1,y1,x2,y2,'',"brNDC")
    leg.SetBorderSize(1)
    leg.SetTextSize(0.03)
    leg.SetFillColor(0)
    leg.SetFillStyle(0)
    return leg

  def DrawHists(self, data,bg,signal, do2D = False, lineX = None, lineY = None):
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
    #bg.GetXaxis().SetRangeUser(100,200)

    if not do2D: data.Draw('pesame')
    else: data.Draw('boxsame')

    if not do2D: signal.Draw('histsame')
    else: signal.Draw('boxsame')

    if lineX:
      print 'lineX',lineX
      print 'lineStats', lineX,bg.GetMinimum(),lineX,bg.GetMaximum()*1.05
      line = TLine(lineX,bg.GetMinimum(),lineX,ymax*1.05)
      line.SetLineColor(kRed+1)
      line.SetLineWidth(2)
      line.SetLineStyle(2)
      line.Draw()
      SetOwnership(line,0)



  def MakeDrawROCandSignif(self,signalHist,bgStack,sigWindow=None):
    '''Make the ROC and significance plots, and return the best values'''
    bestCut = None
    bestBGEff= None
    bestSigEff= None
    bestSignif = 0
    rocCurve = TProfile('rocCurve',signalHist.GetTitle()+' ROC;BG eff;Signal eff',signalHist.GetNbinsX(),0,1,0,1)
    signifPlot = TProfile('signifPlot',signalHist.GetTitle()+' Signif;MEDisc;s/#sqrt{s+b}',signalHist.GetNbinsX(),signalHist.GetBinLowEdge(1),signalHist.GetBinLowEdge(1+signalHist.GetNbinsX()))

    for bin in range(1,signalHist.GetNbinsX()+1):
      #print bin, signalHist.GetBinLowEdge(bin), signalHist.GetBinContent(bin), signalHist.Integral(bin,signalHist.GetNbinsX())
      bgYield = bgStack.Integral(bin, bgStack.GetNbinsX())
      sigYield = signalHist.Integral(bin, signalHist.GetNbinsX())
      rocCurve.Fill(bgYield/bgStack.Integral(), sigYield/signalHist.Integral())
      if bin == 1:
        initialSignif = sigYield/sqrt(sigYield+bgYield)
      if sigYield+bgYield > 0:
        if (sigYield/sqrt(sigYield+bgYield) > bestSignif) and (sigYield>0.6*signalHist.Integral(1, signalHist.GetNbinsX())):
          bestSignif = sigYield/sqrt(sigYield+bgYield)
          bestCut = signalHist.GetBinLowEdge(bin)
          bestBGEff = bgYield/bgStack.Integral()
          bestSigEff = sigYield/signalHist.Integral()
        signifPlot.Fill(signalHist.GetBinLowEdge(bin),sigYield/sqrt(sigYield+bgYield))
      else:
        signifPlot.Fill(signalHist.GetBinLowEdge(bin),0)

    percentImprovement = (bestSignif-initialSignif)*100/initialSignif

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

    if sigWindow: self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+signalHist.GetName().split('_')[1]+'_ROC_window.pdf')
    else: self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+signalHist.GetName().split('_')[1]+'_ROC.pdf')

    signifPlot.SetMarkerColor(kRed)
    signifPlot.GetYaxis().SetTitleSize(0.05)
    signifPlot.GetYaxis().CenterTitle()
    signifPlot.GetXaxis().SetTitle(signalHist.GetXaxis().GetTitle())
    signifPlot.GetXaxis().SetTitleSize(0.05)
    signifPlot.GetYaxis().SetTitleOffset(1.04)
    signifPlot.GetYaxis().SetLabelSize(0.045)
    signifPlot.Draw('pe')
    line = TLine(bestCut,signifPlot.GetMinimum(),bestCut,signifPlot.GetMaximum()*1.05)
    line.SetLineColor(kRed+1)
    line.SetLineWidth(2)
    line.SetLineStyle(2)
    line.Draw()
    signifStats = TPaveText(bestCut,signifPlot.GetMaximum()*0.02,bestCut+0.05,signifPlot.GetMaximum()*0.25)
    #SetOwnership(signifStats,False)
    signifStats.SetBorderSize(0)
    signifStats.SetFillStyle(0)
    signifStats.SetFillColor(0)
    signifStats.SetTextFont(42)
    signifStats.SetTextSize(0.035)
    signifStats.AddText('Discrim Cut: {0:.2}'.format(bestCut))
    signifStats.AddText('Significance: {0:.2}'.format(bestSignif))
    signifStats.AddText('Improvement: {0:.2}%'.format(percentImprovement))
    signifStats.AddText('BG Eff: {0:.2}'.format(bestBGEff))
    signifStats.AddText('Sig Eff: {0:.2}'.format(bestSigEff))
    signifStats.Draw()


    if sigWindow: self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+signalHist.GetName().split('_')[1]+'_Signif_Window.pdf')
    else: self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+signalHist.GetName().split('_')[1]+'_Signif.pdf')

    return (bestCut,bestBGEff,bestSignif,percentImprovement)


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
      leg = self.MakeLegend(0.75,0.75,0.90,0.92)
    else:
      leg = self.MakeLegend()
    if do2D:
      leg.AddEntry(dataHist,'DATA','f')
    else:
      leg.AddEntry(dataHist,'DATA','lep')

    bgStack = self.MakeBGStack(bgList,leg,do2D)

    scale = self.LumiXSScale(self.sigName)
    signalHist.Scale(scale*100)
    if do2D:
      leg.AddEntry(signalHist,'Signalx100','f')
    else:
      leg.AddEntry(signalHist,'Signalx100','l')

    self.DrawHists(dataHist,bgStack,signalHist,do2D)

    leg.Draw()
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
    leg = self.MakeLegend()

    leg.AddEntry(dataHist,'DATA','lep')

    # Set the bg histograms
    bgStack = self.MakeBGStack(bgList,leg)

    # Set the signal histograms
    scale = self.LumiXSScale(self.sigName)
    signalHist.Scale(scale*500)
    leg.AddEntry(signalHist,'Signalx500','l')

    self.DrawHists(dataHist,bgStack,signalHist)

    leg.Draw()
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
    if sigWindow:
      self.myCut = TCutG('mycut',5)
      self.myCut.SetPoint(0,sigWindow-4,-2)
      self.myCut.SetPoint(1,sigWindow-4,2)
      self.myCut.SetPoint(2,sigWindow+4,2)
      self.myCut.SetPoint(3,sigWindow+4,-2)
      self.myCut.SetPoint(4,sigWindow-4,-2)


    for hist in histList:
      print hist.GetName()
    dataHistM,dataHistD = self.GetDataHistsMD(histList,sigWindow)
    bgListM,bgListD = self.GetBGHistsMD(histList,sigWindow)
    signalHistM, signalHistD = self.GetSignalHistsMD(histList,sigWindow)


    if not os.path.isdir(self.directory):
      os.mkdir(self.directory)

    TH1.SetDefaultSumw2(kTRUE)
    TH2.SetDefaultSumw2(kTRUE)
    TProfile.SetDefaultSumw2(kTRUE)

    self.can= TCanvas('can'+self.lepton,'canvas',800,600)
    self.can.cd()
    legM = self.MakeLegend()
    legD = self.MakeLegend()
    legM.AddEntry(dataHistM,'DATA','lep')
    legD.AddEntry(dataHistD,'DATA','lep')

    # Set the bg histograms

    print 'mass'
    bgStackM = self.MakeBGStack(bgListM,legM)
    print 'disc'
    bgStackD = self.MakeBGStack(bgListD,legD)
    bgStackROC = self.MakeBGStack(bgListD,None,True)

    scale = self.LumiXSScale(self.sigName)
    signalHistM.Scale(scale*100)
    signalHistD.Scale(scale)

    #temp switch, use data instead of MC:
    #bgStack = dataHist

    #make ROC and signif plots and get the best values
    cutVal,BGeffVal,signifVal,signifChange = self.MakeDrawROCandSignif(signalHistD,bgStackROC,sigWindow)

    print 'cutVal:', cutVal
    print 'BGeffVal:', BGeffVal
    print 'signifVal:', signifVal
    print 'signifChange:', signifChange

    legM.AddEntry(signalHistM,'Signalx100','l')
    legD.AddEntry(signalHistD,'Signalx100','l')

    self.DrawHists(dataHistM,bgStackM,signalHistM,False)
    legM.Draw()

    if sigWindow: self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+signalHistM.GetName().split('_')[1]+'_MassCheck_Window.pdf')
    else: self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+signalHistM.GetName().split('_')[1]+'_MassCheck.pdf')
    self.can.Clear()
    signalHistD.Scale(100)

    self.DrawHists(dataHistD,bgStackD,signalHistD,False,cutVal)
    legD.Draw()

    if sigWindow: self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+signalHistD.GetName().split('_')[1]+'_DiscCheck_Window.pdf')
    else: self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+signalHistD.GetName().split('_')[1]+'_DiscCheck.pdf')
    self.can.Clear()

    dataHistD.IsA().Destructor(dataHistD)
    dataHistM.IsA().Destructor(dataHistM)
    signalHistD.IsA().Destructor(signalHistD)
    signalHistM.IsA().Destructor(signalHistM)
    for hist in bgListM:
      hist.IsA().Destructor(hist)
    for hist in bgListD:
      hist.IsA().Destructor(hist)
    if sigWindow:
      self.myCut.Clear()

    #self.can.IsA().Destructor(self.can)

  def RatioPlot(self, key, chooseNames, legendNames):
    '''Get two plots, normalize them, compare with ratio'''
    if type(self.thisFile) != list:
        histList = self.folderDict[key]
        if len(histList) == 0: raise NameError('histList is empty')
        gStyle.SetOptStat(0)
        if histList[0].GetName().split('_')[0] == 'h2':
          print 'skipping ratio plot for', histList[0].GetName()
          return

        compHists = self.ChooseTwoHists(chooseNames, histList)
    else:
        histList1 = self.folderDict[0][key]
        histList2 = self.folderDict[1][key]
        if len(histList1) == 0: raise NameError('histList1 is empty')
        if len(histList2) == 0: raise NameError('histList2 is empty')
        gStyle.SetOptStat(0)
        if histList1[0].GetName().split('_')[0] == 'h2':
          print 'skipping ratio plot for', histList1[0].GetName()
          return
        if histList2[0].GetName().split('_')[0] == 'h2':
          print 'skipping ratio plot for', histList2[0].GetName()
          return

        compHists = self.ChooseTwoHists(chooseNames,histList1,histList2)

    if not os.path.isdir(self.directory):
      os.mkdir(self.directory)

    TH1.SetDefaultSumw2(kTRUE)
    TProfile.SetDefaultSumw2(kTRUE)

    self.can= TCanvas('ratioCan','canvas',800,600)
    self.can.cd()
    leg = self.MakeLegend(0.85,0.83,1.00,1.00)

    pad1 = TPad('pad1', '', 0.00, 0.30, 1.0, 0.98, 0)
    #SetOwnership(pad1,False)
    pad1.SetBottomMargin(0.03)
    pad1.SetLeftMargin(0.09)
    pad1.SetRightMargin(0.018)
    pad2 = TPad('pad2', '', 0.00, 0.02, 1.0, 0.32, 0)
    #SetOwnership(pad2,False)
    pad2.SetTopMargin(0.)
    pad2.SetBottomMargin(0.25)
    pad2.SetLeftMargin(0.09)
    pad2.SetRightMargin(0.018)
    pad2.SetGrid(2,2)

    self.can.cd()
    pad1.Draw()
    pad2.Draw()
    pad1.cd()
    if type(compHists[0]) is TProfile:
      h1 = compHists[0].ProjectionX()
      h2 = compHists[1].ProjectionX()
      ratio = h1.Clone()
      ratio.Divide(h2.Clone())
      h1.IsA().Destructor(h1)
      h2.IsA().Destructor(h2)
    else:
      ratio = compHists[0].Clone()
      ratio.Divide(compHists[1])
    for i,plot in enumerate(compHists):
      if i == 0:
        plot.Draw('hist')
      else:
        plot.Draw('histsame')
      leg.AddEntry(plot,legendNames[i],'l')
    leg.Draw()
    pad2.cd()
    ratio.SetTitle('')
    ratio.SetLineColor(kBlack)
    ratio.SetMarkerColor(kBlack)
    ratio.GetYaxis().SetTitle('#frac{'+legendNames[0]+'}{'+legendNames[1]+'}')
    ratio.GetYaxis().CenterTitle()
    ratio.GetXaxis().SetTitleSize(0.12)
    ratio.GetXaxis().SetLabelSize(0.12)
    ratio.GetYaxis().SetTitleSize(0.08)
    ratio.GetYaxis().SetTitleOffset(0.38)
    ratio.GetYaxis().SetLabelSize(0.09)
    ratio.GetYaxis().SetNdivisions(506)
    ratio.SetMinimum(-0.1)
    #ratio.SetMaximum(min(ratio.GetMaximum()*1.5,4))
    ratio.SetMaximum(1.8)
    ratio.Draw()
    line = TLine(ratio.GetBinLowEdge(1),1.00,ratio.GetBinLowEdge(ratio.GetNbinsX()+ 1),1.00)
    line.SetLineColor(kRed)
    line.SetLineWidth(2)
    line.Draw()
    self.can.cd()
    zero = TPaveText(0.07,0.305,0.087,0.335)
    #SetOwnership(zero,False)
    zero.SetBorderSize(0)
    zero.SetFillStyle(1001)
    zero.SetFillColor(0)
    zero.SetTextFont(42)
    zero.SetTextSize(0.035)
    zero.AddText('0')
    zero.Draw()
    self.can.SaveAs(self.directory+'/'+self.lepton+self.lepton+'_'+compHists[0].GetName().split('_')[1]+'.pdf')
    self.can.IsA().Destructor(self.can)
