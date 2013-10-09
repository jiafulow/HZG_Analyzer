#!/usr/bin/env python
import sys, os, glob

#useage:
# ./fastHadd.py year lepton tag
if len(sys.argv) !=4:
  print 'you are doing this wrong'
else:

  year = sys.argv[1]
  if sys.argv[2].lower() == 'mumu':
    leptonA = 'MuMu'
    leptonB = 'mumuGamma'
  elif sys.argv[2].lower() == 'ee':
    leptonA = 'EE'
    leptonB = 'eeGamma'
  tag = sys.argv[3]

  selectionString = ''
  if 'A' in year:
    selectionString += ' Run2012A'
  if 'B' in year:
    selectionString += ' Run2012B'
  if 'C' in year:
    selectionString += ' Run2012C'
  if 'D' in year:
    selectionString += ' Run2012D'

  os.system('./hadd.py ForStoyan/higgsHistograms_'+leptonA+year+'_Limits_'+tag+'.root '+leptonB+' File '+selectionString+' DYJets ZGToLLG ggHZG_M125_pythia8_LO ggHZG_M125_pythia6')
  os.system('./hadd.py ~/afsHome/public/m_llgFile_'+leptonA+year+'_'+tag+'.root '+leptonB+' m_llgFile '+selectionString+' DYJets ZGToLLG ggHZG_M125_pythia8_LO ggHZG_M125_pythia6')
  os.system('./hadd.py batchHistos/higgsHistograms_'+leptonA+year+'_'+tag+'.root '+leptonB+' Histograms '+selectionString+' DYJets ZGToLLG ggHZG_M125_pythia8_LO ggHZG_M125_pythia6')

dataDumpList = glob.glob('dumps/dataDump*.txt')
if len(dataDumpList) >0:
  with open('dumps/CombDataDump_'+leptonB+'_'+tag+'.txt','w') as f:
    for dump in dataDumpList:
      with open(dump) as ftemp:
        for line in ftemp:
          f.write(line)
  f.close()

  os.system('rm dumps/dataDump*.txt')



#hadd -f otherHistos/eleSmear2011.root ~/nobackup/BatchOutput/eeGamma_Combined/eleSmearFile_*
#./hadd.py ForStoyan/higgsHistograms_MuMu2011_Limits_Nominal_LowR9.root mumuGamma File Run2011A Run2011B ggHZG_M120 ggHZG_M125 ggHZG_M130 ggHZG_M135 ggHZG_M140 vbfHZG_M120 vbfHZG_M125 vbfHZG_M130 vbfHZG_M135 vbfHZG_M140

