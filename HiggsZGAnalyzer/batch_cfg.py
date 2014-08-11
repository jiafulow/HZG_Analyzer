#! /usr/bin/env python
import BatchMaster as b
import os

EOS         = '/eos/uscms/store/user/bpollack'
dCache      = '/pnfs/cms/WAX/11/store/user/bpollack'
t3storage   = '/tthome/share/noobs/'

outputPathFNAL  = '/uscms/home/bpollack/nobackup/BatchOutput'
outputPathNWU  = '/tthome/bpollack/BatchOutput'

analyzer = 'higgsAnalyzer'
#analyzer = 'smzgAnalyzer'
#analyzer = 'eeSelector'

os.system('tar -zcvf stageball.tar.gz {0}* ../src  otherHistos ../plugins ../interface ../mva/testWeights* ../txtFiles ../input.DAT ../process.DAT ../Pdfdata ../br.sm*'.format(analyzer))

doMuMuGamma = True
doEEGamma = True
doMuEGamma = False
do7Tev = False
configs = []

#different pathnames and executables for NWU or FNAL
if os.environ.get('AT_NWU') == None:
  print 'lol'

else:
  if doMuMuGamma:
    if not do7Tev:
      #configs.append(b.JobConfig('ggHZG_M120', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M120_RD1', 5, 'Signal2012ggM120 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('ggHZG_M123', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M123_RD1', 5, 'Signal2012ggM123 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('ggHZG_M125', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M125_RD1', 5, 'Signal2012ggM125 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('ggHZG_M130', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M130_RD1', 5, 'Signal2012ggM130 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('ggHZG_M135', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M135_RD1', 5, 'Signal2012ggM135 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('ggHZG_M140', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M140_RD1', 5, 'Signal2012ggM140 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('ggHZG_M145', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M145_RD1', 5, 'Signal2012ggM145 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('ggHZG_M150', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M150_RD1', 5, 'Signal2012ggM150 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('ggHZG_M155', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M155_RD1', 5, 'Signal2012ggM155 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('ggHZG_M160', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M160_RD1', 5, 'Signal2012ggM160 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))

      configs.append(b.JobConfig('ggHZG_M200', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M200_S10', 5, 'Signal2012ggM200 ABCD mumuGamma 2012 S10 {0}'.format(analyzer),'mumuGamma'))
      configs.append(b.JobConfig('ggHZG_M250', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M250_S10', 5, 'Signal2012ggM250 ABCD mumuGamma 2012 S10 {0}'.format(analyzer),'mumuGamma'))
      configs.append(b.JobConfig('ggHZG_M275', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M275_S10', 5, 'Signal2012ggM275 ABCD mumuGamma 2012 S10 {0}'.format(analyzer),'mumuGamma'))
      configs.append(b.JobConfig('ggHZG_M300', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M300_S10', 5, 'Signal2012ggM300 ABCD mumuGamma 2012 S10 {0}'.format(analyzer),'mumuGamma'))
      configs.append(b.JobConfig('ggHZG_M400', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M400_S10', 5, 'Signal2012ggM400 ABCD mumuGamma 2012 S10 {0}'.format(analyzer),'mumuGamma'))
      configs.append(b.JobConfig('ggHZG_M500', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M500_S10', 5, 'Signal2012ggM500 ABCD mumuGamma 2012 S10 {0}'.format(analyzer),'mumuGamma'))

      #configs.append(b.JobConfig('vbfHZG_M120', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M120_RD1', 5, 'Signal2012vbfM120 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('vbfHZG_M125', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M125_RD1', 5, 'Signal2012vbfM125 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('vbfHZG_M130', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M130_RD1', 5, 'Signal2012vbfM130 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('vbfHZG_M135', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M135_RD1', 5, 'Signal2012vbfM135 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('vbfHZG_M140', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M140_RD1', 5, 'Signal2012vbfM140 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('vbfHZG_M145', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M145_RD1', 5, 'Signal2012vbfM145 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('vbfHZG_M150', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M150_RD1', 5, 'Signal2012vbfM150 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('vbfHZG_M155', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M155_RD1', 5, 'Signal2012vbfM155 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('vbfHZG_M160', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M160_RD1', 5, 'Signal2012vbfM160 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))

      #configs.append(b.JobConfig('whHZG_M120', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M120_RD1', 5, 'Signal2012whM120 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('whHZG_M125', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M125_RD1', 5, 'Signal2012whM125 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('whHZG_M130', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M130_RD1', 5, 'Signal2012whM130 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('whHZG_M135', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M135_RD1', 5, 'Signal2012whM135 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('whHZG_M140', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M140_RD1', 5, 'Signal2012whM140 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('whHZG_M145', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M145_RD1', 5, 'Signal2012whM145 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('whHZG_M150', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M150_RD1', 5, 'Signal2012whM150 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('whHZG_M155', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M155_RD1', 5, 'Signal2012whM155 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('whHZG_M160', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M160_RD1', 5, 'Signal2012whM160 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))

      #configs.append(b.JobConfig('zhHZG_M120', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M120_RD1', 5, 'Signal2012zhM120 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('zhHZG_M125', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M125_RD1', 5, 'Signal2012zhM125 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('zhHZG_M130', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M130_RD1', 5, 'Signal2012zhM130 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('zhHZG_M135', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M135_RD1', 5, 'Signal2012zhM135 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('zhHZG_M140', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M140_RD1', 5, 'Signal2012zhM140 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('zhHZG_M145', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M145_RD1', 5, 'Signal2012zhM145 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('zhHZG_M150', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M150_RD1', 5, 'Signal2012zhM150 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('zhHZG_M155', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M155_RD1', 5, 'Signal2012zhM155 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('zhHZG_M160', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M160_RD1', 5, 'Signal2012zhM160 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))

      #configs.append(b.JobConfig('tthHZG_M120', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M120_RD1', 5, 'Signal2012tthM120 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('tthHZG_M125', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M125_RD1', 5, 'Signal2012tthM125 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('tthHZG_M130', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M130_RD1', 5, 'Signal2012tthM130 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('tthHZG_M135', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M135_RD1', 5, 'Signal2012tthM135 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('tthHZG_M140', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M140_RD1', 5, 'Signal2012tthM140 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('tthHZG_M145', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M145_RD1', 5, 'Signal2012tthM145 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('tthHZG_M150', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M150_RD1', 5, 'Signal2012tthM150 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('tthHZG_M155', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M155_RD1', 5, 'Signal2012tthM155 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('tthHZG_M160', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M160_RD1', 5, 'Signal2012tthM160 ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))

      configs.append(b.JobConfig('Run2012A', t3storage+'/nuTuples_v9.8_8TeV/Data/DoubleMu_Run2012A_v2', 50, 'DATA ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      configs.append(b.JobConfig('Run2012B', t3storage+'/nuTuples_v9.8_8TeV/Data/DoubleMu_Run2012B', 100, 'DATA ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      configs.append(b.JobConfig('Run2012C', t3storage+'/nuTuples_v9.8_8TeV/Data/DoubleMu_Run2012C', 150, 'DATA ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      configs.append(b.JobConfig('Run2012D', t3storage+'/nuTuples_v9.8_8TeV/Data/DoubleMu_Run2012D', 150, 'DATA ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))

      configs.append(b.JobConfig('ZGToLLG', t3storage+'/nuTuples_v9.8_8TeV/MC/ZGToLLG_RD1', 50, 'ZGToLLG ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      #configs.append(b.JobConfig('DYJets', t3storage+'/nuTuples_v9.8_8TeV/MC/DYJetsToLL_M-50_RD1', 150, 'DYJets ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
      configs.append(b.JobConfig('DYToMuMu', t3storage+'/nuTuples_v9.8_8TeV/MC/DYToMuMu_M-20_RD1', 200, 'DYToMuMu ABCD mumuGamma 2012 RD1 {0}'.format(analyzer),'mumuGamma'))
    else:
      configs.append(b.JobConfig('Run2011A', t3storage+'/nuTuples_v9.8_7TeV/Data/DoubleMu_Run2011A', 100, 'DATA AB mumuGamma 2011 RD1 {0}'.format(analyzer),'mumuGamma'))
      configs.append(b.JobConfig('Run2011B', t3storage+'/nuTuples_v9.8_7TeV/Data/DoubleMu_Run2011B', 100, 'DATA AB mumuGamma 2011 RD1 {0}'.format(analyzer),'mumuGamma'))

  if doEEGamma:
    #configs.append(b.JobConfig('ggHZG_M120', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M120_RD1', 5, 'Signal2012ggM120 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('ggHZG_M123', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M123_RD1', 5, 'Signal2012ggM123 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('ggHZG_M125', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M125_RD1', 5, 'Signal2012ggM125 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('ggHZG_M130', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M130_RD1', 5, 'Signal2012ggM130 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('ggHZG_M135', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M135_RD1', 5, 'Signal2012ggM135 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('ggHZG_M140', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M140_RD1', 5, 'Signal2012ggM140 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('ggHZG_M145', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M145_RD1', 5, 'Signal2012ggM145 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('ggHZG_M150', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M150_RD1', 5, 'Signal2012ggM150 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('ggHZG_M155', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M155_RD1', 5, 'Signal2012ggM155 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('ggHZG_M160', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M160_RD1', 5, 'Signal2012ggM160 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))

    configs.append(b.JobConfig('ggHZG_M200', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M200_S10', 5, 'Signal2012ggM200 ABCD eeGamma 2012 S10 {0}'.format(analyzer),'eeGamma'))
    configs.append(b.JobConfig('ggHZG_M250', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M250_S10', 5, 'Signal2012ggM250 ABCD eeGamma 2012 S10 {0}'.format(analyzer),'eeGamma'))
    configs.append(b.JobConfig('ggHZG_M275', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M275_S10', 5, 'Signal2012ggM275 ABCD eeGamma 2012 S10 {0}'.format(analyzer),'eeGamma'))
    configs.append(b.JobConfig('ggHZG_M300', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M300_S10', 5, 'Signal2012ggM300 ABCD eeGamma 2012 S10 {0}'.format(analyzer),'eeGamma'))
    configs.append(b.JobConfig('ggHZG_M400', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M400_S10', 5, 'Signal2012ggM400 ABCD eeGamma 2012 S10 {0}'.format(analyzer),'eeGamma'))
    configs.append(b.JobConfig('ggHZG_M500', t3storage+'/nuTuples_v9.8_8TeV/MC/ggHZG_M500_S10', 5, 'Signal2012ggM500 ABCD eeGamma 2012 S10 {0}'.format(analyzer),'eeGamma'))

    #configs.append(b.JobConfig('vbfHZG_M120', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M120_RD1', 5, 'Signal2012vbfM120 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('vbfHZG_M125', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M125_RD1', 5, 'Signal2012vbfM125 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('vbfHZG_M130', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M130_RD1', 5, 'Signal2012vbfM130 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('vbfHZG_M135', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M135_RD1', 5, 'Signal2012vbfM135 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('vbfHZG_M140', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M140_RD1', 5, 'Signal2012vbfM140 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('vbfHZG_M145', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M145_RD1', 5, 'Signal2012vbfM145 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('vbfHZG_M150', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M150_RD1', 5, 'Signal2012vbfM150 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('vbfHZG_M155', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M155_RD1', 5, 'Signal2012vbfM155 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('vbfHZG_M160', t3storage+'/nuTuples_v9.8_8TeV/MC/vbfHZG_M160_RD1', 5, 'Signal2012vbfM160 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))

    #configs.append(b.JobConfig('whHZG_M120', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M120_RD1', 5, 'Signal2012whM120 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('whHZG_M125', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M125_RD1', 5, 'Signal2012whM125 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('whHZG_M130', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M130_RD1', 5, 'Signal2012whM130 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('whHZG_M135', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M135_RD1', 5, 'Signal2012whM135 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('whHZG_M140', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M140_RD1', 5, 'Signal2012whM140 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('whHZG_M145', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M145_RD1', 5, 'Signal2012whM145 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('whHZG_M150', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M150_RD1', 5, 'Signal2012whM150 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('whHZG_M155', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M155_RD1', 5, 'Signal2012whM155 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('whHZG_M160', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M160_RD1', 5, 'Signal2012whM160 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))

    #configs.append(b.JobConfig('zhHZG_M120', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M120_RD1', 5, 'Signal2012zhM120 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('zhHZG_M125', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M125_RD1', 5, 'Signal2012zhM125 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('zhHZG_M130', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M130_RD1', 5, 'Signal2012zhM130 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('zhHZG_M135', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M135_RD1', 5, 'Signal2012zhM135 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('zhHZG_M140', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M140_RD1', 5, 'Signal2012zhM140 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('zhHZG_M145', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M145_RD1', 5, 'Signal2012zhM145 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('zhHZG_M150', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M150_RD1', 5, 'Signal2012zhM150 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('zhHZG_M155', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M155_RD1', 5, 'Signal2012zhM155 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('zhHZG_M160', t3storage+'/nuTuples_v9.8_8TeV/MC/whzhHZG_M160_RD1', 5, 'Signal2012zhM160 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))

    #configs.append(b.JobConfig('tthHZG_M120', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M120_RD1', 5, 'Signal2012tthM120 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('tthHZG_M125', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M125_RD1', 5, 'Signal2012tthM125 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('tthHZG_M130', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M130_RD1', 5, 'Signal2012tthM130 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('tthHZG_M135', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M135_RD1', 5, 'Signal2012tthM135 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('tthHZG_M140', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M140_RD1', 5, 'Signal2012tthM140 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('tthHZG_M145', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M145_RD1', 5, 'Signal2012tthM145 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('tthHZG_M150', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M150_RD1', 5, 'Signal2012tthM150 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('tthHZG_M155', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M155_RD1', 5, 'Signal2012tthM155 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('tthHZG_M160', t3storage+'/nuTuples_v9.8_8TeV/MC/tthHZG_M160_RD1', 5, 'Signal2012tthM160 ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))


    configs.append(b.JobConfig('Run2012A', t3storage+'/nuTuples_v9.8_8TeV/Data/DoubleElectron_Run2012A', 50, 'DATA ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    configs.append(b.JobConfig('Run2012B', t3storage+'/nuTuples_v9.8_8TeV/Data/DoubleElectron_Run2012B', 100, 'DATA ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    configs.append(b.JobConfig('Run2012C', t3storage+'/nuTuples_v9.8_8TeV/Data/DoubleElectron_Run2012C', 150, 'DATA ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    configs.append(b.JobConfig('Run2012D', t3storage+'/nuTuples_v9.8_8TeV/Data/DoubleElectron_Run2012D', 150, 'DATA ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))

    configs.append(b.JobConfig('ZGToLLG', t3storage+'/nuTuples_v9.8_8TeV/MC/ZGToLLG_RD1', 50, 'ZGToLLG ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('DYJets', t3storage+'/nuTuples_v9.8_8TeV/MC/DYJetsToLL_M-50_RD1', 150, 'DYJets ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    configs.append(b.JobConfig('DYToEE', t3storage+'/nuTuples_v9.8_8TeV/MC/DYToEE_M-20_RD1', 200, 'DYToEE ABCD eeGamma 2012 RD1 {0}'.format(analyzer),'eeGamma'))
    #configs.append(b.JobConfig('DYJets', t3storage+'/nuTuples_v9.8_8TeV/MC_skimmed/DYJetsToLL_M-50_noSkim', 150, 'DYJets ABCD eeGamma 2012 S10 {0}'.format(analyzer),'eeGamma'))

  if doMuEGamma:
    configs.append(b.JobConfig('Run2012A', t3storage+'/nuTuples_v9.8_8TeV/Data/MuEG_Run2012A', 50, 'DATA ABCD mueGamma 2012 RD1 {0}'.format(analyzer),'mueGamma'))
    configs.append(b.JobConfig('Run2012B', t3storage+'/nuTuples_v9.8_8TeV/Data/MuEG_Run2012B', 100, 'DATA ABCD mueGamma 2012 RD1 {0}'.format(analyzer),'mueGamma'))
    configs.append(b.JobConfig('Run2012C', t3storage+'/nuTuples_v9.8_8TeV/Data/MuEG_Run2012C', 150, 'DATA ABCD mueGamma 2012 RD1 {0}'.format(analyzer),'mueGamma'))
    configs.append(b.JobConfig('Run2012D', t3storage+'/nuTuples_v9.8_8TeV/Data/MuEG_Run2012D', 150, 'DATA ABCD mueGamma 2012 RD1 {0}'.format(analyzer),'mueGamma'))
    configs.append(b.JobConfig('ZtoJPsiGamma','/tthome/bpollack/storage/nuTuples_v9.7_8TeV/MC/ZtoJPsiGamma', 10, 'ZtoJPsiGamma ABCD mueGamma 2012 S10 {0}'.format(analyzer),'mueGamma'))

  batcher = b.BatchMaster(configs, outputPathNWU,'execBatch.sh')
batcher.SubmitToLPC()
