#!/usr/bin/env python
import sys
sys.argv.append('-b')
from ROOT import *
import numpy as np

gROOT.ProcessLine('.L ./tdrstyle.C')
setTDRStyle()
TH1.SetDefaultSumw2(kTRUE)
TProfile.SetDefaultSumw2(kTRUE)

def ComparisonSuite():
  File1mu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/localHistos/higgsHistograms_ggM125_8TeV_mumuGamma_local.root")
  File2mu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/localHistos/higgsHistograms_ggM125_8TeV_pythia8_175_v2_mumuGamma_local.root")
  File1el= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/localHistos/higgsHistograms_ggM125_8TeV_eeGamma_local.root")
  File2el= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/localHistos/higgsHistograms_ggM125_8TeV_pythia8_175_v3_eeGamma_local.root")

  selection = ['el']
  plotList = []
  labelList = ['pythia6','pythia8']
  colorList = [kRed,kBlue]
  catList = ['CAT1','CAT2','CAT3','CAT4','','PreGen','ZGAngles','Vtx']
  distList = ['Pt','Eta','Phi','Mass']
  physList1 = ['Photon','LeadingLepton','TrailingLepton','DiLep','ThreeBody']
  physList2 = ['costhetaLM','phi','costhetaZG']
  physList3 = ['pvMult','nTracks','pvPosZ']
  can= TCanvas('trailingCan','canvas',800,600)
  pad1 = TPad('pad1', '', 0.00, 0.30, 1.0, 0.98, 0)
  pad1.SetBottomMargin(0.03)
  pad1.SetLeftMargin(0.09)
  pad1.SetRightMargin(0.018)
  pad2 = TPad('pad2', '', 0.00, 0.02, 1.0, 0.32, 0)
  pad2.SetTopMargin(0.)
  pad2.SetBottomMargin(0.25)
  pad2.SetLeftMargin(0.09)
  pad2.SetRightMargin(0.018)
  pad2.SetGrid(2,2)
  can.cd()
  gStyle.SetOptStat(0)
  for sel in selection:
    if sel is 'mu': fileList=[File1mu,File2mu]
    elif sel is 'el': fileList=[File1el,File2el]
    for cat in catList:
      folder = cat
      if cat == '': folder = 'pT-Eta-Phi'
      elif cat == 'ZGAngles': physListTmp = physList2
      elif cat == 'Vtx': physListTmp = physList3
      else: physListTmp = physList1
      for phys in physListTmp:
        if phys not in physList1: distListTmp = ['']
        elif phys is 'DiLep' or phys is 'ThreeBody': distListTmp = distList
        else: distListTmp = distList[0:3]

        for dist in distListTmp:
          for i,thisFile in enumerate(fileList):
            if i == 0: suffix = ''
            elif i ==1: suffix = ''

            if phys == 'Photon':
              thisPlot = thisFile.GetDirectory(folder).Get('h1_photon'+dist+cat+'_Signal2012ggM125'+suffix)
            if phys == 'LeadingLepton':
              thisPlot = thisFile.GetDirectory(folder).Get('h1_leadLeptonStd'+dist+cat+'_Signal2012ggM125'+suffix)
            if phys == 'TrailingLepton':
              thisPlot = thisFile.GetDirectory(folder).Get('h1_trailingLeptonStd'+dist+cat+'_Signal2012ggM125'+suffix)
            if phys == 'DiLep':
              thisPlot = thisFile.GetDirectory(folder).Get('h1_diLep'+dist+cat+'_Signal2012ggM125'+suffix)
            if phys == 'ThreeBody':
              thisPlot = thisFile.GetDirectory(folder).Get('h1_threeBody'+dist+cat+'_Signal2012ggM125'+suffix)
            elif phys not in physList1:
              thisPlot = thisFile.GetDirectory(folder).Get('h1_'+phys+'_Signal2012ggM125'+suffix)



            print sel, cat, phys, dist, thisFile
            if phys is not 'nTracks': thisPlot.Scale(1/thisPlot.Integral())
            thisPlot.SetLineColor(colorList[i])
            thisPlot.SetLineWidth(2)
            thisPlot.SetMarkerColor(colorList[i])
            thisPlot.SetMarkerStyle(20+i)
            if i == 0:
              thisPlot.GetYaxis().SetTitleSize(0.06)
              thisPlot.GetYaxis().CenterTitle()
              thisPlot.GetXaxis().SetTitleSize(0.05)
              #thisPlot.GetYaxis().SetLabelSize(0.05)
              #thisPlot.GetXaxis().SetLabelSize(0.05)
              if phys is 'nTracks':
                thisPlot.GetYaxis().SetTitle("nTracks")
              else:
                thisPlot.GetYaxis().SetTitle("Events")
              #thisPlot.GetXaxis().SetTitle(dist)
              thisPlot.SetTitle(sel+sel+' '+phys+' '+cat+' '+dist)
              thisPlot.SetTitleOffset(0.8,"Y")

              leg = TLegend(0.90,0.83,1.00,1.00,'',"brNDC")
              leg.SetBorderSize(1)
              leg.SetTextSize(0.03)
              leg.SetFillColor(0)
              leg.SetFillStyle(1001)
            plotList.append(thisPlot)

          ymax = max(map(lambda x:x.GetMaximum(),plotList))*1.1
          #ymin = min(map(lambda x:x.GetMinimum(),plotList))*0.9
          ymin = 0
          can.cd()
          pad1.Draw()
          pad2.Draw()
          pad1.cd()
          if phys is 'nTracks':
            h1 = plotList[0].ProjectionX()
            h2 = plotList[1].ProjectionX()
            ratio = h1.Clone()
            ratio.Divide(h2.Clone())
            del h1
            del h2
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
            leg.AddEntry(plot,labelList[i],'l')
          leg.Draw()
          pad2.cd()
          ratio.SetTitle('')
          ratio.SetLineColor(kBlack)
          ratio.SetMarkerColor(kBlack)
          ratio.GetYaxis().SetTitle('p6/p8')
          ratio.GetXaxis().SetTitleSize(0.12)
          ratio.GetXaxis().SetLabelSize(0.12)
          ratio.GetYaxis().SetTitleSize(0.12)
          ratio.GetYaxis().SetTitleOffset(0.40)
          ratio.GetYaxis().SetLabelSize(0.09)
          ratio.GetYaxis().SetNdivisions(506)
          ratio.SetMinimum(-0.1)
          ratio.SetMaximum(min(ratio.GetMaximum()*1.5,4))
          ratio.Draw()
          line = TLine(ratio.GetBinLowEdge(1),1.00,ratio.GetBinLowEdge(ratio.GetNbinsX()+ 1),1.00)
          line.SetLineColor(kRed)
          line.SetLineWidth(2)
          line.Draw()
          can.cd()
          zero = TPaveText(0.07,0.305,0.087,0.335)
          zero.SetBorderSize(0)
          zero.SetFillStyle(1001)
          zero.SetFillColor(0)
          zero.SetTextFont(42)
          zero.SetTextSize(0.035)
          zero.AddText('0')
          zero.Draw()
          can.SaveAs('Pythia6vs8/'+sel+'_'+dist+'_'+cat+'_'+phys+'.pdf')
          pad1.Clear()
          pad2.Clear()
          plotList = []
          ratio.Clear()
  del pad1
  del pad2
  del can

if __name__=="__main__":
  ComparisonSuite()
