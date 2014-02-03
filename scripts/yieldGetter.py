#!/usr/bin/env python
from ROOT import *
import numpy as np

def LumiXSScale(name,lepton,year,thisFile):
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
  scaleDict['2012']['gg']['135'] = 16.79*0.00227*0.10098*1000

  initEvents = thisFile.GetDirectory('Misc').Get('h1_acceptanceByCut_'+name).Integral(1,1)
  if 'Signal' in name:
    sig = name[10:].partition('M')[0]
    mass = name[10:].partition('M')[-1][0:3]
    scale = initEvents/scaleDict[year][sig][mass]
  else:
    scale = initEvents/scaleDict[year][name]
  scale = lumi/scale
  return scale


def DoYield(fileName):
  print fileName
  signal = 'M125'
  '''Get full yield breakdown for all categories, data, signal'''
  leptons = ['MuMu','EE']
  dataTypes = ['Data','Signal']
  for lep in leptons:
    print lep
    thisFile = TFile('../HiggsZGAnalyzer/batchHistos/higgsHistograms_'+lep+'2012ABCD_'+fileName+'.root')
    for dataType in dataTypes:
      if dataType == 'Data': suffix = '_DATA'
      elif dataType == 'Signal': suffix = '_Signal2012gg'+signal
      rawTotal = thisFile.Get('h1_InvariantMass'+suffix).Integral()
      scaledTotal =
      print rawTotal
      for i in range(1,10):
        if thisFile.Get('h1_threeBodyMassCAT'+str(i)+suffix):
          print thisFile.Get('h1_threeBodyMassCAT'+str(i)+suffix)


if __name__ == '__main__':
 DoYield('02-03-14')

