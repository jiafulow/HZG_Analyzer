#!/usr/bin/env python
from ROOT import *
import numpy as np
import sys
from PlottingClasses import AutoVivification

def LumiXSScale(name,lepton,year,thisFile):
  '''Outputs scale for MC with respect to lumi and XS'''

  if name is 'DATA': return 1

  lumi = 0
  if lepton is 'MuMu': lumi = 19.672
  elif lepton is 'EE': lumi = 19.711
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


def DoYield(fileName, secondName = None):
  print fileName
  signal = 'M125'
  '''Get full yield breakdown for all categories, data, signal'''
  leptons = ['MuMu','EE']
  dataTypes = ['Data','Signal']
  for lep in leptons:
    print '  ',lep
    thisFile = TFile('../HiggsZGAnalyzer/batchHistos/higgsHistograms_'+lep+'2012ABCD_'+fileName+'.root')
    if secondName:
      secondFile = TFile('../HiggsZGAnalyzer/batchHistos/higgsHistograms_'+lep+'2012ABCD_'+secondName+'.root')
    for dataType in dataTypes:
      if dataType == 'Data': suffix = 'DATA'
      elif dataType == 'Signal': suffix = 'Signal2012gg'+signal
      print '    ',suffix
      rawTotal = thisFile.Get('h1_InvariantMass_'+suffix).Integral()
      scaledTotal = rawTotal*LumiXSScale(suffix, lep, '2012', thisFile)
      print '      ','rawTotal: {0:.0f} scaledTotal: {1:.2f}'.format(rawTotal,scaledTotal)
# count number of cats
      catTotal = 0
      for i in range(1,10):
        if thisFile.Get('h1_threeBodyMassCAT'+str(i)+'_'+suffix): catTotal +=1
# do the cats
      rawTotalCat = [-9999]
      for i in range(1,10):
        if thisFile.Get('h1_threeBodyMassCAT'+str(i)+'_'+suffix):
          rawTotalCat.append(thisFile.Get('h1_threeBodyMassCAT'+str(i)+'_'+suffix).Integral())
          scaledTotalCat = rawTotalCat[i]*LumiXSScale(suffix, lep, '2012', thisFile)
          percentage = (rawTotalCat[i]/rawTotal)*100
          if secondName:
            rawTotalCatSecond = secondFile.Get('h1_threeBodyMassCAT'+str(i)+'_'+suffix).Integral()
            percentageChange = (rawTotalCat[i]/rawTotalCatSecond)*100

          if secondName:
            print '      ','CAT{0} rawTotal: {1:4.0f} scaledTotal: {2:7.2f} percent: {3:4.1f}% percentCh: {4:.1f}%'.format(i,rawTotalCat[i],scaledTotalCat,percentage,percentageChange)
          else:
            print '      ','CAT{0} rawTotal: {1:4.0f} scaledTotal: {2:7.2f} percent: {3:.1f}%'.format(i,rawTotalCat[i],scaledTotalCat,percentage)
      if catTotal == 9:
        for i in range(1,10):
          if i < 5:
            print '        ','CAT{0} temp card scale: {1:.3f}'.format(i,rawTotalCat[i]/(rawTotalCat[i+5]+rawTotalCat[i]))
          elif i == 5:
            print '        ','CAT{0} temp card scale: {1:.3f}'.format(i,rawTotalCat[i]/(rawTotalCat[i]))
          else:
            print '        ','CAT{0} temp card scale: {1:.3f}'.format(i,rawTotalCat[i]/(rawTotalCat[i-5]+rawTotalCat[i]))



if __name__ == '__main__':
  name = str(sys.argv[1])
  secondName = None
  if len(sys.argv)>2: secondName = sys.argv[2]
  DoYield(name, secondName)


