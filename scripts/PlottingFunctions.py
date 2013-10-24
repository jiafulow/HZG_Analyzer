#!/usr/bin/env python
import sys,os
#sys.argv.append('-b')
from ROOT import *
from collections import defaultdict
import numpy as np


# class for multi-layered nested dictionaries, pretty cool
class AutoVivification(dict):
    """Implementation of perl's autovivification feature."""
    def __getitem__(self, item):
        try:
            return dict.__getitem__(self, item)
        except KeyError:
            value = self[item] = type(self)()
            return value

def RatioPlot(plotList = [], titleList = [], saveName = 'lol'):
  '''Input list of two plots, two plot names, and a name to save the canvas'''
  TH1.SetDefaultSumw2(kTRUE)
  TProfile.SetDefaultSumw2(kTRUE)

  can= TCanvas('ratioCan','canvas',800,600)
  leg = TLegend(0.85,0.83,1.00,1.00,'',"brNDC")
  leg.SetBorderSize(1)
  leg.SetTextSize(0.03)
  leg.SetFillColor(0)
  leg.SetFillStyle(1001)

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

  ymax = max(map(lambda x:x.GetMaximum(),plotList))*1.1
  #ymin = min(map(lambda x:x.GetMinimum(),plotList))*0.9
  ymin = 0

  can.cd()
  pad1.Draw()
  pad2.Draw()
  pad1.cd()
  if type(plotList[0]) is TProfile:
    h1 = plotList[0].ProjectionX()
    h2 = plotList[1].ProjectionX()
    ratio = h1.Clone()
    ratio.Divide(h2.Clone())
    h1.IsA().Destructor(h1)
    h2.IsA().Destructor(h2)
  else:
    ratio = plotList[0].Clone()
    ratio.Divide(plotList[1])
  for i,plot in enumerate(plotList):
    if i == 0:
      plot.SetMaximum(ymax)
      plot.SetMinimum(ymin)
      plot.Draw('hist')
    else:
      plot.Draw('histsame')
    leg.AddEntry(plot,titleList[i],'l')
  leg.Draw()
  pad2.cd()
  ratio.SetTitle('')
  ratio.SetLineColor(kBlack)
  ratio.SetMarkerColor(kBlack)
  ratio.GetYaxis().SetTitle('#frac{'+titleList[0]+'}{'+titleList[1]+'}')
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
  can.cd()
  zero = TPaveText(0.07,0.305,0.087,0.335)
  #SetOwnership(zero,False)
  zero.SetBorderSize(0)
  zero.SetFillStyle(1001)
  zero.SetFillColor(0)
  zero.SetTextFont(42)
  zero.SetTextSize(0.035)
  zero.AddText('0')
  zero.Draw()
  can.SaveAs(saveName)
  can.IsA().Destructor(can)

def LumiXSScale(year,lepton,name,initialNevt):
  '''Outputs scale for MC with respect to lumi and XS'''

  if name is 'DATA': return 1

  lumi = 0
  if lepton is 'mu': lumi = 19.672
  elif lepton is 'el': lumi = 19.711
  else: raise NameError('LumiXSScale lepton incorrect')

  scaleDict = AutoVivification()

  scaleDict['2012']['DYJets'] = 3503.71*1000
  scaleDict['2012']['ZGToLLG'] = 156.2*1000
  scaleDict['2012']['gg']['125'] = 19.52*0.00154*0.10098*1000

  if 'Signal' in name:
    sig = name[10:].partition('M')[0]
    mass = name[10:].partition('M')[-1][0:3]
    scale = initialNevt/scaleDict[year][sig][mass]
  else:
    scale = initialNevt/scaleDict[year][name]
  scale = lumi/scale
  return scale

def FolderDump(tfile, folder):
  '''Input file and folder name, output default dictionary of histogram lists. the key name is the distribution, the lists are all the samples for the given distribution'''
  folderDict = defaultdict(list)
  lok = tfile.GetDirectory(folder).GetListOfKeys()
  for i in range(0, lok.GetEntries()):
    name = lok.At(i).GetName()
    key = name.split('_')[1]
    folderDict[key].append(tfile.GetDirectory(folder).Get(name))

  return folderDict

def DataBGComp(histList,directory,thisFile,year,lepton,sigName):
  '''Give a list of histograms that contain the data and backgrounds, plot them and save them'''
  if len(histList) == 0: raise NameError('histList is empty')
  gStyle.SetOptStat(0)
  do2D = False
  if histList[0].GetName().split('_')[0] in ['h2','p']: do2D = True

  dataHist = None
  for hist in histList:
    if 'DATA' in hist.GetName():
      dataHist = hist.Clone()
      break
  if not dataHist: raise NameError('No dataHist found in this list')

  bgList = [hist.Clone() for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]
  if len(bgList) == 0: raise NameError('No BG hists found in this list')
  bgList = sorted(bgList, key=lambda hist:hist.GetName()[-1], reverse=True)

  signalHist = None
  for hist in histList:
    if sigName in hist.GetName():
      signalHist= hist.Clone()
      break
  if not signalHist: raise NameError('No signalHist found in this list')
  signalHist.SetLineColor(kRed)
  signalHist.SetLineWidth(2)
  signalHist.SetFillStyle(1001)

  if not os.path.isdir(directory):
    os.mkdir(directory)

  TH1.SetDefaultSumw2(kTRUE)
  TProfile.SetDefaultSumw2(kTRUE)

  # Make canvas and legend
  can= TCanvas('ratioCan','canvas',800,600)
  if do2D:
    can.SetRightMargin(0.1)
    leg = TLegend(0.75,0.75,0.90,0.92,'',"brNDC")
  else:
    leg = TLegend(0.81,0.73,0.97,0.92,'',"brNDC")
  leg.SetBorderSize(1)
  leg.SetTextSize(0.03)
  leg.SetFillColor(0)
  leg.SetFillStyle(0)

  # Set the data histogram
  dataHist.SetLineColor(kBlack)
  dataHist.SetMarkerColor(kBlack)
  dataHist.SetMarkerStyle(20)
  dataHist.SetFillStyle(0)
  if do2D:
    leg.AddEntry(dataHist,'DATA','f')
  else:
    leg.AddEntry(dataHist,'DATA','lep')

  # Set the bg histograms
  if do2D:
    bgStack = bgList[0].Clone()
    for hist in bgList[1:]:
      bgStack.Add(hist)
    bgStack.SetFillColor(kBlue)
    leg.AddEntry(bgStack,'BG','f')
  else:
    bgStack = THStack('bgs','bgs')
    for hist in bgList:
      label = hist.GetName().split('_')[-1]
      hist.SetFillStyle(1001)
      hist.SetFillColor(colorDict[label])
      hist.SetLineColor(colorDict[label])
      initEvents = thisFile.GetDirectory('Misc').Get('h1_acceptanceByCut_'+label).Integral(1,1)
      scale = LumiXSScale(year,lepton,label,initEvents)
      #hist.Scale(scale)
      hist.Scale(scale*1.40)
      leg.AddEntry(hist,label,'f')
      bgStack.Add(hist)


  # Set the signal histograms
  signalHist.SetLineColor(kRed)
  signalHist.SetFillStyle(0)
  label = sigName
  initEvents = thisFile.GetDirectory('Misc').Get('h1_acceptanceByCut_'+label).Integral(1,1)
  scale = LumiXSScale(year,lepton,label,initEvents)
  signalHist.Scale(scale*500)
  if do2D:
    leg.AddEntry(signalHist,'Signalx500','f')
  else:
    leg.AddEntry(signalHist,'Signalx500','l')

  myCut = TCutG('myCut',5)
  myCut.SetPoint(0,122,0)
  myCut.SetPoint(1,122,0.2)
  myCut.SetPoint(2,128,0.2)
  myCut.SetPoint(3,128,0)
  myCut.SetPoint(4,122,0)


  ymax = max(map(lambda x:x.GetMaximum(),[dataHist,bgStack,signalHist]))*1.2
  ymin = 0

  bgStack.SetMaximum(ymax)
  bgStack.SetMinimum(ymin)
  if not do2D: bgStack.Draw('hist')
  else: bgStack.Draw('colz[myCut]')
  bgStack.GetYaxis().SetTitle(bgList[0].GetYaxis().GetTitle())
  bgStack.GetYaxis().SetTitleSize(0.06)
  bgStack.GetYaxis().CenterTitle()
  bgStack.GetXaxis().SetTitle(bgList[0].GetXaxis().GetTitle())
  bgStack.GetXaxis().SetTitleSize(0.05)
  #bgStack.GetYaxis().SetLabelSize(0.05)
  #bgStack.GetXaxis().SetLabelSize(0.05)
  #bgStack.GetXaxis().SetTitle(dist)
  bgStack.SetTitle(lepton+lepton+' '+dataHist.GetTitle())
  bgStack.GetYaxis().SetTitleOffset(0.82)

  if not do2D: dataHist.Draw('pesame')
  else: dataHist.Draw('boxsame')

  if not do2D: signalHist.Draw('histsame')
  else: signalHist.Draw('boxsame')

  leg.Draw()
  can.SaveAs(directory+'/'+lepton+lepton+'_'+dataHist.GetName().split('_')[1]+'.pdf')
  can.Clear()

def DataBGComp2DProj(histList,directory,thisFile,year,lepton,sigName,title,sigWindow = False):
  '''Give a list of histograms that contain the data and backgrounds, plot them and save them'''
  if len(histList) == 0: raise NameError('histList is empty')
  gStyle.SetOptStat(0)
  if histList[0].GetName().split('_')[0] != 'h2':
    print 'skipping 2D proj for', histList[0].GetName()
    return

  dataHist = None
  for hist in histList:
    if 'DATA' in hist.GetName():
      if sigWindow:
        hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(119),hist.GetXaxis().FindBin(131))
        dataHist = hist.Clone().ProjectionY('proj'+hist.GetName())
      else:
        dataHist = hist.Clone().ProjectionY('proj'+hist.GetName())
      break
  if not dataHist: raise NameError('No dataHist found in this list')

  if sigWindow:
    bgList = []
    for hist in histList:
      if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1):
        hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(119),hist.GetXaxis().FindBin(131))
        bgList.append(hist.Clone().ProjectionY('proj'+hist.GetName()))
  else:
    bgList = [hist.ProjectionY('proj'+hist.GetName()) for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]
  if len(bgList) == 0: raise NameError('No BG hists found in this list')
  bgList = sorted(bgList, key=lambda hist:hist.GetName()[-1], reverse=True)

  signalHist = None
  for hist in histList:
    if sigName in hist.GetName():
      if sigWindow:
        hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(119),hist.GetXaxis().FindBin(131))
        signalHist = hist.Clone().ProjectionY('proj'+hist.GetName())
      else:
        signalHist = hist.Clone().ProjectionY('proj'+hist.GetName())
      break
  if not signalHist: raise NameError('No signalHist found in this list')
  signalHist.SetLineColor(kRed)
  signalHist.SetLineWidth(2)
  signalHist.SetFillStyle(1001)

  if not os.path.isdir(directory):
    os.mkdir(directory)

  TH1.SetDefaultSumw2(kTRUE)
  TProfile.SetDefaultSumw2(kTRUE)

  # Make canvas and legend
  can= TCanvas('ratioCan','canvas',800,600)
  leg = TLegend(0.81,0.73,0.97,0.92,'',"brNDC")
  leg.SetBorderSize(1)
  leg.SetTextSize(0.03)
  leg.SetFillColor(0)
  leg.SetFillStyle(0)

  # Set the data histogram
  dataHist.SetLineColor(kBlack)
  dataHist.SetMarkerColor(kBlack)
  dataHist.SetMarkerStyle(20)
  dataHist.SetFillStyle(0)
  leg.AddEntry(dataHist,'DATA','lep')

  # Set the bg histograms
  bgStack = THStack('bgs','bgs')
  for hist in bgList:
    label = hist.GetName().split('_')[-1]
    hist.SetFillStyle(1001)
    hist.SetFillColor(colorDict[label])
    hist.SetLineColor(colorDict[label])
    initEvents = thisFile.GetDirectory('Misc').Get('h1_acceptanceByCut_'+label).Integral(1,1)
    scale = LumiXSScale(year,lepton,label,initEvents)
    hist.Scale(scale)
    leg.AddEntry(hist,label,'f')
    bgStack.Add(hist)


  # Set the signal histograms
  signalHist.SetLineColor(kRed)
  signalHist.SetFillStyle(0)
  label = sigName
  initEvents = thisFile.GetDirectory('Misc').Get('h1_acceptanceByCut_'+label).Integral(1,1)
  scale = LumiXSScale(year,lepton,label,initEvents)
  signalHist.Scale(scale*500)
  leg.AddEntry(signalHist,'Signalx500','l')


  ymax = max(map(lambda x:x.GetMaximum(),[dataHist,bgStack,signalHist]))*1.2
  ymin = 0

  bgStack.SetMaximum(ymax)
  bgStack.SetMinimum(ymin)
  bgStack.Draw('hist')
  bgStack.GetYaxis().SetTitle('N_{evts}')
  bgStack.GetYaxis().SetTitleSize(0.06)
  bgStack.GetYaxis().CenterTitle()
  bgStack.GetXaxis().SetTitle(bgList[0].GetXaxis().GetTitle())
  bgStack.GetXaxis().SetTitleSize(0.05)
  #bgStack.GetYaxis().SetLabelSize(0.05)
  #bgStack.GetXaxis().SetLabelSize(0.05)
  #bgStack.GetXaxis().SetTitle(dist)
  bgStack.SetTitle(lepton+lepton+' '+title)
  bgStack.GetYaxis().SetTitleOffset(0.82)

  dataHist.Draw('pesame')

  signalHist.Draw('histsame')

  leg.Draw()
  if sigWindow:
    can.SaveAs(directory+'/'+lepton+lepton+'_'+dataHist.GetName().split('_')[1]+'_projWindow.pdf')
  else:
    can.SaveAs(directory+'/'+lepton+lepton+'_'+dataHist.GetName().split('_')[1]+'_proj.pdf')
  can.Clear()

def ROCcurves(histList,directory,thisFile,year,lepton,sigName, sigWindow=True):
  '''Give a list of histograms, calculate ROC curve for that distribution.  Need 2D hist, mass vs disc for proper significance calculation'''
  if len(histList) == 0: raise NameError('histList is empty')
  gStyle.SetOptStat(0)

  if histList[0].GetName().split('_')[0] != 'h2':
    print 'skipping ROC curve for', histList[0].GetName()
    return

  dataHist = None
  for hist in histList:
    if 'DATA' in hist.GetName():
      if sigWindow:
        hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(121),hist.GetXaxis().FindBin(129))
        dataHist = hist.Clone().ProjectionY('projWin'+hist.GetName())
      else:
        dataHist = hist.Clone().ProjectionY('proj'+hist.GetName())
      break
  if not dataHist: raise NameError('No dataHist found in this list')

  myCut = TCutG('myCut',5)
  myCut.SetPoint(0,122,0)
  myCut.SetPoint(1,122,0.2)
  myCut.SetPoint(2,128,0.2)
  myCut.SetPoint(3,128,0)
  myCut.SetPoint(4,122,0)

  if sigWindow:
    bgList = []
    bgListM = []
    for hist in histList:
      if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1):
        hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(121),hist.GetXaxis().FindBin(129))
        #bgList.append(hist.Clone().ProjectionY('proj'+hist.GetName(), 1, hist.GetXaxis().GetNbins(), '[myCut]'))
        #bgListM.append(hist.Clone().ProjectionX('projM'+hist.GetName(), 1, hist.GetXaxis().GetNbins(), '[myCut]'))
        bgList.append(hist.Clone().ProjectionY('projWin'+hist.GetName()))
        bgListM.append(hist.Clone().ProjectionX('projWinM'+hist.GetName()))
  else:
    bgList = [hist.ProjectionY('proj'+hist.GetName()) for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]
    bgListM = [hist.ProjectionX('projM'+hist.GetName()) for hist in histList if (hist.GetName().find('DATA') == -1 and hist.GetName().find('Signal') == -1)]
  if len(bgList) == 0: raise NameError('No BG hists found in this list')
  bgList = sorted(bgList, key=lambda hist:hist.GetName()[-1], reverse=True)
  bgListM = sorted(bgListM, key=lambda hist:hist.GetName()[-1], reverse=True)

  signalHist = None
  signalHistM = None
  for hist in histList:
    if sigName in hist.GetName():
      if sigWindow:
        hist.GetXaxis().SetRange(hist.GetXaxis().FindBin(121),hist.GetXaxis().FindBin(129))
        signalHist = hist.Clone().ProjectionY('projWin'+hist.GetName())
        signalHistM = hist.Clone().ProjectionX('projWinM'+hist.GetName())
        #signalHist = hist.Clone().ProjectionY('proj'+hist.GetName(),1, hist.GetXaxis().GetNbins(), '[myCut]')
        #signalHistM = hist.Clone().ProjectionX('projM'+hist.GetName(),1, hist.GetXaxis().GetNbins(), '[myCut]')
      else:
        signalHist = hist.Clone().ProjectionY('proj'+hist.GetName())
        signalHistM = hist.Clone().ProjectionX('projM'+hist.GetName())
    break
  if not signalHist: raise NameError('No signalHist found in this list')
  signalHist.SetLineColor(kRed)
  signalHist.SetLineWidth(2)
  signalHist.SetFillStyle(1001)
  signalHistM.SetLineColor(kRed)
  signalHistM.SetLineWidth(2)
  signalHistM.SetFillStyle(1001)


  if not os.path.isdir(directory):
    os.mkdir(directory)

  TH1.SetDefaultSumw2(kTRUE)
  TProfile.SetDefaultSumw2(kTRUE)
  can= TCanvas('ratioCan','canvas',800,600)
  leg = TLegend(0.81,0.73,0.97,0.92,'',"brNDC")
  leg.SetBorderSize(1)
  leg.SetTextSize(0.03)
  leg.SetFillColor(0)
  leg.SetFillStyle(0)
  can.cd()

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
  signifPlot = TProfile('signifPlot',signalHist.GetTitle()+' Signif;MEDisc;#frac{s}{#sqrt{s+b}}',signalHist.GetNbinsX(),signalHist.GetBinLowEdge(1),signalHist.GetBinLowEdge(1+signalHist.GetNbinsX()))

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

  bgStackM.SetMaximum(ymax)
  bgStackM.SetMinimum(ymin)
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






colorDict = {'DYJets':kGreen+1,'ZGToLLG':kBlue}






