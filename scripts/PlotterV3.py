#!/usr/bin/env python
import sys
sys.argv.append('-b')
from PlottingFunctions import *
from ROOT import *
import numpy as np

gROOT.ProcessLine('.L ./tdrstyle.C')
setTDRStyle()
TH1.SetDefaultSumw2(kTRUE)
TProfile.SetDefaultSumw2(kTRUE)

def ComparisonSuiteSignal():
  File1mu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu_p6.root")
  File2mu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu_p8.root")
  File3mu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/localHistos/higgsHistograms_ggM125_8TeV_pythia8_175_v2_mumuGamma_local.root")
  #File1el= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/localHistos/higgsHistograms_ggM125_8TeV_eeGamma_local.root")
  #File2el= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/localHistos/higgsHistograms_ggM125_8TeV_pythia8_175_v3_eeGamma_local.root")

  selection = ['mu']
  plotList = []
  labelList = ['pythia6_NLO','pythia8_LO','pythia8_NLO']
  colorList = [kRed,kBlue,kGreen+1]
  catList = ['CAT1','CAT2','CAT3','CAT4','','PreGen','ZGAngles','Vtx']
  distList = ['Pt','Eta','Phi','Mass']
  physList1 = ['Photon','LeadingLepton','TrailingLepton','DiLep','ThreeBody']
  physList2 = ['costhetaLM','phi','costhetaZG']
  physList3 = ['pvMult','nTracks','pvPosZ']
  can= TCanvas('can','canvas',800,600)
  can.cd()
  gStyle.SetOptStat(0)
  for sel in selection:
    if sel is 'mu': fileList=[File1mu,File2mu,File3mu]
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

              leg = TLegend(0.80,0.80,1.00,0.95,'',"brNDC")
              leg.SetBorderSize(1)
              leg.SetTextSize(0.03)
              leg.SetFillColor(0)
              leg.SetFillStyle(1001)
            plotList.append(thisPlot)

          RatioPlot(plotList[0:2], labelList[0:2], 'Pythia6vs8/'+sel+'_'+dist+'_'+cat+'_'+phys+'_ratio.pdf')
          ymax = max(map(lambda x:x.GetMaximum(),plotList))*1.2
          #ymin = min(map(lambda x:x.GetMinimum(),plotList))*0.9
          ymin = 0
          for i,plot in enumerate(plotList):
            if i == 0:
              plot.SetMaximum(ymax)
              plot.SetMinimum(ymin)
              plot.Draw('hist')
            else:
              plot.Draw('histsame')
            leg.AddEntry(plot,labelList[i],'l')
          leg.Draw()
          can.SaveAs('Pythia6vs8/'+sel+'_'+dist+'_'+cat+'_'+phys+'.pdf')
          can.Clear()



          plotList = []
  del can

def ComparisonSuiteBG():
  FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_10-10-13_fixPU.root")
  selection = ['mu']
  plotList = []
  labelList = ['DATA','DYJets','ZGToLLG']
  colorList = [kBlack,kGreen+1,kBlue]
  catList = ['CAT1','CAT2','CAT3','CAT4','CAT5','','Vtx']
  distList = ['Pt','Eta','Phi','Mass']
  physList1 = ['Photon','LeadingLepton','TrailingLepton','DiLep','ThreeBody']
  physList2 = ['costhetaLM','phi','costhetaZG']
  physList3 = ['pvMult','nTracks','pvPosZ']

  can= TCanvas('can','canvas',800,600)
  can.cd()
  gStyle.SetOptStat(0)
  for sel in selection:
    if sel is 'mu': thisFile=FileMu
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
          for i,label in enumerate(labelList):

            if phys == 'Photon':
              thisPlot = thisFile.GetDirectory(folder).Get('h1_photon'+dist+cat+'_'+label)
            if phys == 'LeadingLepton':
              thisPlot = thisFile.GetDirectory(folder).Get('h1_leadLeptonStd'+dist+cat+'_'+label)
            if phys == 'TrailingLepton':
              thisPlot = thisFile.GetDirectory(folder).Get('h1_trailingLeptonStd'+dist+cat+'_'+label)
            if phys == 'DiLep':
              thisPlot = thisFile.GetDirectory(folder).Get('h1_diLep'+dist+cat+'_'+label)
            if phys == 'ThreeBody':
              thisPlot = thisFile.GetDirectory(folder).Get('h1_threeBody'+dist+cat+'_'+label)
            elif phys not in physList1:
              thisPlot = thisFile.GetDirectory(folder).Get('h1_'+phys+'_'+label)



            print sel, cat, phys, dist, thisFile
            #if phys is not 'nTracks': thisPlot.Scale(1/thisPlot.Integral())
            thisPlot.SetLineColor(colorList[i])
            thisPlot.SetLineWidth(2)
            thisPlot.SetMarkerColor(colorList[i])
            thisPlot.SetMarkerStyle(20+i)
            if i == 0:
              leg = TLegend(0.80,0.80,1.00,0.95,'',"brNDC")
              leg.SetBorderSize(1)
              leg.SetTextSize(0.03)
              leg.SetFillColor(0)
              leg.SetFillStyle(1001)
              leg.AddEntry(thisPlot,labelList[i],'lep')
            else:
              thisPlot.SetFillStyle(1001)
              thisPlot.SetFillColor(colorList[i])
              initEvents = thisFile.GetDirectory('Misc').Get('h1_acceptanceByCut_'+label).Integral(1,1)
              scale = LumiXSScale('2012',sel,label,initEvents)
              thisPlot.Scale(scale)
              leg.AddEntry(thisPlot,labelList[i],'f')
            plotList.append(thisPlot)

          bgStack = THStack('bgs','bgs')
          bgStack.Add(plotList[1])
          bgStack.Add(plotList[2])
          if phys is 'pvMult':
            newPlotList = [plotList[2], plotList[0]]
          else:
            newPlotList = [bgStack, plotList[0]]
          ymax = max(map(lambda x:x.GetMaximum(),newPlotList))*1.2
          ymin = 0

          for i,plot in enumerate(newPlotList):
            if i == 0:
              plot.SetMaximum(ymax)
              plot.SetMinimum(ymin)
              if phys is 'pvMult': plot.Scale(1/plot.Integral())
              plot.Draw('hist')
              plot.GetYaxis().SetTitle(plotList[0].GetYaxis().GetTitle())
              plot.GetYaxis().SetTitleSize(0.06)
              plot.GetYaxis().CenterTitle()
              plot.GetXaxis().SetTitle(plotList[0].GetXaxis().GetTitle())
              plot.GetXaxis().SetTitleSize(0.05)
              #plot.GetYaxis().SetLabelSize(0.05)
              #plot.GetXaxis().SetLabelSize(0.05)
              if phys is 'nTracks':
                plot.GetYaxis().SetTitle("nTracks")
              else:
                plot.GetYaxis().SetTitle("Events")
              #plot.GetXaxis().SetTitle(dist)
              plot.SetTitle(sel+sel+' '+phys+' '+cat+' '+dist)
              plot.GetYaxis().SetTitleOffset(0.8)
            else:
              if phys is 'pvMult': plot.Scale(1/plot.Integral())
              plot.Draw('psame')
          leg.Draw()
          can.SaveAs('BGComparisons/'+sel+'_'+dist+'_'+cat+'_'+phys+'.pdf')
          can.Clear()



          plotList = []

def PhotonPurity():
  FileMu= TFile("/uscms_data/d2/bpollack/CMSSW_5_3_8_patch1/src/HZG_Analyzer/HiggsZGAnalyzer/batchHistos/higgsHistograms_MuMu2012ABCD_10-9-13.root")
  selection = ['mu']
  plotList = []
  labelList = ['DATA','DYJets','ZGToLLG']
  colorList = [kBlack,kGreen+1,kBlue]
  folder = 'PhotonPurity'
  distList = ['Pt','Eta','Phi','Mass']
  physList1 = ['Photon','LeadingLepton','TrailingLepton','DiLep','ThreeBody']
  physList2 = ['costhetaLM','phi','costhetaZG']
  physList3 = ['pvMult','nTracks','pvPosZ']

  can= TCanvas('can','canvas',800,600)
  can.cd()
  gStyle.SetOptStat(0)


if __name__=="__main__":
  print 'executing', sys.argv[1]
  if 'bg' == sys.argv[1]:
    ComparisonSuiteBG()
  elif 'sig' == sys.argv[1]:
    ComparisonSuiteSignal()

