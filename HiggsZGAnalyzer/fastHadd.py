#!/usr/bin/env python
import sys, os, glob, subprocess

#useage:
# ./fastHadd.py tag

def fastHadd():
  if len(sys.argv) != 2:
    print 'usage: ./fastHadd.py tag'

  if not os.path.isfile('.checkfile.txt'):
    print 'where is the .checkfile.txt?! you fucked something up, I aint hadding shit. fuck you, do it yourself.'
    return

  infile = open('.checkfile.txt','r')
  analyzer = infile.readline()

  for line in infile:
    selectionList = line.split()

    if '2011' in selectionList[1]:
      year = '2011AB'
    else:
      year = '2012ABCD'

    if selectionList[0] == 'mumuGamma':
      leptonA = 'MuMu'
      leptonB = 'mumuGamma'
    elif selectionList[0] == 'eeGamma':
      leptonA = 'EE'
      leptonB = 'eeGamma'
    elif selectionList[0] == 'mueGamma':
      leptonA = 'MuE'
      leptonB = 'mueGamma'
    tag = sys.argv[1]

    if 'smzg' in analyzer:
      if not os.path.isdir('liteFiles'): os.mkdir('liteFiles')
      os.system('./hadd.py liteFiles/liteFile_'+leptonA+year+'_'+tag+'.root '+leptonB+' liteFile {0}'.format(' '.join(selectionList[1:])))

    elif 'tnp' in analyzer:
      if not os.path.isdir('tnpFiles'): os.mkdir('tnpFiles')
      os.system('./hadd.py tnpFiles/tnpFile_'+leptonA+year+'_'+tag+'.root '+leptonB+' tnpFile {0}'.format(' '.join(selectionList[1:])))

    elif 'amumu' in analyzer:
      if not os.path.isdir('amumuFiles'): os.mkdir('amumuFiles')
      os.system('./hadd.py amumu/amumuFile_'+leptonA+year+'_'+tag+'.root '+leptonB+' amumuFile {0}'.format(' '.join(selectionList[1:])))


    elif 'eeSelector' in analyzer:
      if not os.path.isdir('eleFiles'): os.mkdir('eleFiles')
      os.system('./hadd.py eleFiles/eleFile_'+leptonA+year+'_'+tag+'.root '+leptonB+' eleFile {0}'.format(' '.join(selectionList[1:])))

    else:
      if os.environ.get('AT_NWU') == None:
        os.system('./hadd.py ~/afsHome/public/m_llgFile_'+leptonA+year+'_'+tag+'.root '+leptonB+' m_llgFile {0}'.format(' '.join(selectionList[1:])))
      else:
        if not os.path.isdir('massTrees'): os.mkdir('massTrees')
        os.system('./hadd.py massTrees/m_llgFile_'+leptonA+year+'_'+tag+'.root '+leptonB+' m_llgFile {0}'.format(' '.join(selectionList[1:])))
      if not os.path.isdir('batchHistos'): os.mkdir('batchHistos')
      os.system('./hadd.py batchHistos/higgsHistograms_'+leptonA+year+'_'+tag+'.root '+leptonB+' Histograms {0}'.format(' '.join(selectionList[1:])))
      if not os.path.isdir('mvaFiles'): os.mkdir('mvaFiles')
      if 'ggHZG_M123' in selectionList:
        print 'signal mva'
        os.system('./hadd.py mvaFiles/higgsTraining_'+leptonA+year+'_'+tag+'_signal.root '+leptonB+' Training {0}'.format(' '.join(['ggHZG_M123'])))
        os.system('./hadd.py mvaFiles/higgsSample_'+leptonA+year+'_'+tag+'_signal.root '+leptonB+' Sample {0}'.format(' '.join(['ggHZG_M123'])))
      if [bg for bg in ['DYJets','ZGToLLG'] if bg in selectionList]:
        print 'bg mva'
        bgList = filter(lambda bg: bg in ['DYJets','ZGToLLG'],selectionList)
        os.system('./hadd.py mvaFiles/higgsTraining_'+leptonA+year+'_'+tag+'_bg.root '+leptonB+' Training {0}'.format(' '.join(bgList)))
        os.system('./hadd.py mvaFiles/higgsSample_'+leptonA+year+'_'+tag+'_bg.root '+leptonB+' Sample {0}'.format(' '.join(bgList)))

  dataDumpList = glob.glob('dumps/dataDump*.txt')
  if len(dataDumpList) >0:
    with open('dumps/CombDataDump_'+leptonB+'_'+tag+'.txt','w') as f:
      for dump in dataDumpList:
        with open(dump) as ftemp:
          for line in ftemp:
            f.write(line)
    f.close()

    os.system('rm dumps/dataDump*.txt')


if __name__=="__main__":
  fastHadd()
  os.system('rm /tmp/batch_submit*')
  os.system('rm /tmp/*Gamma*')

#hadd -f otherHistos/eleSmear2011.root ~/nobackup/BatchOutput/eeGamma_Combined/eleSmearFile_*
#./hadd.py ForStoyan/higgsHistograms_MuMu2011_Limits_Nominal_LowR9.root mumuGamma File Run2011A Run2011B ggHZG_M120 ggHZG_M125 ggHZG_M130 ggHZG_M135 ggHZG_M140 vbfHZG_M120 vbfHZG_M125 vbfHZG_M130 vbfHZG_M135 vbfHZG_M140

