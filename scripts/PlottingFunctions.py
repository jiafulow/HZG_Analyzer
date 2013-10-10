#!/usr/bin/env python
import sys
#sys.argv.append('-b')
from ROOT import *


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

  scale = initialNevt/scaleDict[year][name]
  scale = lumi/scale
  return scale









