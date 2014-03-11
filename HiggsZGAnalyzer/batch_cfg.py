#! /usr/bin/env python
import BatchMaster as b
import os

EOS         = '/eos/uscms/store/user/bpollack'
dCache      = '/pnfs/cms/WAX/11/store/user/bpollack'
t3storage   = '/tthome/bpollack/storage/'

outputPathFNAL  = '/uscms/home/bpollack/nobackup/BatchOutput'
outputPathNWU  = '/tthome/bpollack/BatchOutput'

os.system('tar -zcvf stageball.tar.gz higgsAnalyzer* ../src  otherHistos ../plugins ../interface ../mva/testWeights ../txtFiles ../input.DAT ../process.DAT ../Pdfdata ../br.sm*')

doMuMuGamma = True
doEEGamma = True
configs = []

#different pathnames and executables for NWU or FNAL
if os.environ.get('AT_NWU') == None:
  if doMuMuGamma:
    #configs.append(b.JobConfig('ggHZG_M125_pythia8_153', EOS+'/V08_01_8TeV/ggH_M125_p8153_crab', 5, 'Signal2012ggM125v153p8 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('ggHZG_M125_pythia8_NLO', EOS+'/V08_01_8TeV/ggH_M125_p8175_v4', 5, 'Signal2012ggM125NLOp8 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('ggHZG_M125_pythia8_LO', EOS+'/V08_01_8TeV/ggHZG_M125_Pythia8_175_LO', 5, 'Signal2012ggM125p8 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('ggHZG_M125_pythia6', dCache+'/V08_01_8TeV/ggH_M125_p6', 5, 'Signal2012ggM125p6 ABCD mumuGamma 2012','mumuGamma'))

    configs.append(b.JobConfig('Run2012A', dCache+'/V08_01_8TeV/DoubleMu/Run2012A', 50, 'DATA ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('Run2012B', dCache+'/V08_01_8TeV/DoubleMu/Run2012B_v2', 100, 'DATA ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('Run2012C', dCache+'/V08_01_8TeV/DoubleMu/Run2012C_v2', 150, 'DATA ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('Run2012D', dCache+'/V08_01_8TeV/DoubleMu/Run2012D_v2', 150, 'DATA ABCD mumuGamma 2012','mumuGamma'))

    configs.append(b.JobConfig('ZGToLLG', dCache+'/V08_01_8TeV/ZGToLLG', 50, 'ZGToLLG ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('DYJets', dCache+'/V08_01_8TeV/DYJetsToLL_M-50', 150, 'DYJets ABCD mumuGamma 2012','mumuGamma'))

  if doEEGamma:
    configs.append(b.JobConfig('ggHZG_M125_pythia8_153', EOS+'/V08_01_8TeV/ggH_M125_p8153_crab', 5, 'Signal2012ggM125v153p8 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('ggHZG_M125_pythia8_NLO', EOS+'/V08_01_8TeV/ggH_M125_p8175_v4', 5, 'Signal2012ggM125NLOp8 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('ggHZG_M125_pythia8_LO', EOS+'/V08_01_8TeV/ggHZG_M125_Pythia8_175_LO', 5, 'Signal2012ggM125p8 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('ggHZG_M125_pythia6', dCache+'/V08_01_8TeV/ggH_M125_p6', 5, 'Signal2012ggM125p6 ABCD eeGamma 2012','eeGamma'))

    configs.append(b.JobConfig('Run2012A', dCache+'/V08_01_8TeV/DoubleEl/Run2012A', 50, 'DATA ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('Run2012B', dCache+'/V08_01_8TeV/DoubleEl/Run2012B_v2', 100, 'DATA ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('Run2012C', dCache+'/V08_01_8TeV/DoubleEl/Run2012C_v4', 150, 'DATA ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('Run2012D', dCache+'/V08_01_8TeV/DoubleEl/Run2012D_v3', 150, 'DATA ABCD eeGamma 2012','eeGamma'))

    configs.append(b.JobConfig('ZGToLLG', dCache+'/V08_01_8TeV/ZGToLLG', 50, 'ZGToLLG ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('DYJets', dCache+'/V08_01_8TeV/DYJetsToLL_M-50', 150, 'DYJets ABCD eeGamma 2012','eeGamma'))

  batcher = b.BatchMaster(configs, outputPathFNAL,'execBatch.csh')

else:
  if doMuMuGamma:
    configs.append(b.JobConfig('ggHZG_M120', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M120_RD1', 5, 'Signal2012ggM120 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('ggHZG_M125', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M125_RD1', 5, 'Signal2012ggM125 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('ggHZG_M130', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M130_RD1', 5, 'Signal2012ggM130 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('ggHZG_M135', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M135_RD1', 5, 'Signal2012ggM135 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('ggHZG_M140', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M140_RD1', 5, 'Signal2012ggM140 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('ggHZG_M145', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M145_RD1', 5, 'Signal2012ggM145 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('ggHZG_M150', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M150_RD1', 5, 'Signal2012ggM150 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('ggHZG_M155', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M155_RD1', 5, 'Signal2012ggM155 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('ggHZG_M160', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M160_RD1', 5, 'Signal2012ggM160 ABCD mumuGamma 2012','mumuGamma'))

    configs.append(b.JobConfig('vbfHZG_M120', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M120_RD1', 5, 'Signal2012vbfM120 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('vbfHZG_M125', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M125_RD1', 5, 'Signal2012vbfM125 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('vbfHZG_M130', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M130_RD1', 5, 'Signal2012vbfM130 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('vbfHZG_M135', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M135_RD1', 5, 'Signal2012vbfM135 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('vbfHZG_M140', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M140_RD1', 5, 'Signal2012vbfM140 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('vbfHZG_M145', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M145_RD1', 5, 'Signal2012vbfM145 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('vbfHZG_M150', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M150_RD1', 5, 'Signal2012vbfM150 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('vbfHZG_M155', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M155_RD1', 5, 'Signal2012vbfM155 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('vbfHZG_M160', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M160_RD1', 5, 'Signal2012vbfM160 ABCD mumuGamma 2012','mumuGamma'))

    configs.append(b.JobConfig('whzhHZG_M120', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M120_RD1', 5, 'Signal2012whzhM120 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('whzhHZG_M125', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M125_RD1', 5, 'Signal2012whzhM125 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('whzhHZG_M130', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M130_RD1', 5, 'Signal2012whzhM130 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('whzhHZG_M135', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M135_RD1', 5, 'Signal2012whzhM135 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('whzhHZG_M140', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M140_RD1', 5, 'Signal2012whzhM140 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('whzhHZG_M145', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M145_RD1', 5, 'Signal2012whzhM145 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('whzhHZG_M150', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M150_RD1', 5, 'Signal2012whzhM150 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('whzhHZG_M155', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M155_RD1', 5, 'Signal2012whzhM155 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('whzhHZG_M160', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M160_RD1', 5, 'Signal2012whzhM160 ABCD mumuGamma 2012','mumuGamma'))

    configs.append(b.JobConfig('tthHZG_M120', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M120_RD1', 5, 'Signal2012tthM120 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('tthHZG_M125', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M125_RD1', 5, 'Signal2012tthM125 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('tthHZG_M130', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M130_RD1', 5, 'Signal2012tthM130 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('tthHZG_M135', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M135_RD1', 5, 'Signal2012tthM135 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('tthHZG_M140', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M140_RD1', 5, 'Signal2012tthM140 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('tthHZG_M145', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M145_RD1', 5, 'Signal2012tthM145 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('tthHZG_M150', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M150_RD1', 5, 'Signal2012tthM150 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('tthHZG_M155', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M155_RD1', 5, 'Signal2012tthM155 ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('tthHZG_M160', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M160_RD1', 5, 'Signal2012tthM160 ABCD mumuGamma 2012','mumuGamma'))

    configs.append(b.JobConfig('Run2012A', t3storage+'/nuTuples_v9.6_8TeV/Data/DoubleMu_Run2012A', 50, 'DATA ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('Run2012B', t3storage+'/nuTuples_v9.6_8TeV/Data/DoubleMu_Run2012B', 100, 'DATA ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('Run2012C', t3storage+'/nuTuples_v9.6_8TeV/Data/DoubleMu_Run2012C', 150, 'DATA ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('Run2012D', t3storage+'/nuTuples_v9.6_8TeV/Data/DoubleMu_Run2012D', 150, 'DATA ABCD mumuGamma 2012','mumuGamma'))

    configs.append(b.JobConfig('ZGToLLG', t3storage+'/nuTuples_v9.6_8TeV/MC/ZGToLLG_RD1', 50, 'ZGToLLG ABCD mumuGamma 2012','mumuGamma'))
    configs.append(b.JobConfig('DYJets', t3storage+'/nuTuples_v9.6_8TeV/MC/DYJetsToLL_M-50_RD1', 150, 'DYJets ABCD mumuGamma 2012','mumuGamma'))

  if doEEGamma:
    configs.append(b.JobConfig('ggHZG_M120', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M120_RD1', 5, 'Signal2012ggM120 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('ggHZG_M125', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M125_RD1', 5, 'Signal2012ggM125 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('ggHZG_M130', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M130_RD1', 5, 'Signal2012ggM130 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('ggHZG_M135', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M135_RD1', 5, 'Signal2012ggM135 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('ggHZG_M140', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M140_RD1', 5, 'Signal2012ggM140 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('ggHZG_M145', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M145_RD1', 5, 'Signal2012ggM145 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('ggHZG_M150', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M150_RD1', 5, 'Signal2012ggM150 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('ggHZG_M155', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M155_RD1', 5, 'Signal2012ggM155 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('ggHZG_M160', t3storage+'/nuTuples_v9.6_8TeV/MC/ggHZG_M160_RD1', 5, 'Signal2012ggM160 ABCD eeGamma 2012','eeGamma'))

    configs.append(b.JobConfig('vbfHZG_M120', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M120_RD1', 5, 'Signal2012vbfM120 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('vbfHZG_M125', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M125_RD1', 5, 'Signal2012vbfM125 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('vbfHZG_M130', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M130_RD1', 5, 'Signal2012vbfM130 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('vbfHZG_M135', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M135_RD1', 5, 'Signal2012vbfM135 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('vbfHZG_M140', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M140_RD1', 5, 'Signal2012vbfM140 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('vbfHZG_M145', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M145_RD1', 5, 'Signal2012vbfM145 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('vbfHZG_M150', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M150_RD1', 5, 'Signal2012vbfM150 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('vbfHZG_M155', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M155_RD1', 5, 'Signal2012vbfM155 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('vbfHZG_M160', t3storage+'/nuTuples_v9.6_8TeV/MC/vbfHZG_M160_RD1', 5, 'Signal2012vbfM160 ABCD eeGamma 2012','eeGamma'))

    configs.append(b.JobConfig('whzhHZG_M120', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M120_RD1', 5, 'Signal2012whzhM120 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('whzhHZG_M125', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M125_RD1', 5, 'Signal2012whzhM125 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('whzhHZG_M130', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M130_RD1', 5, 'Signal2012whzhM130 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('whzhHZG_M135', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M135_RD1', 5, 'Signal2012whzhM135 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('whzhHZG_M140', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M140_RD1', 5, 'Signal2012whzhM140 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('whzhHZG_M145', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M145_RD1', 5, 'Signal2012whzhM145 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('whzhHZG_M150', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M150_RD1', 5, 'Signal2012whzhM150 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('whzhHZG_M155', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M155_RD1', 5, 'Signal2012whzhM155 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('whzhHZG_M160', t3storage+'/nuTuples_v9.6_8TeV/MC/whzhHZG_M160_RD1', 5, 'Signal2012whzhM160 ABCD eeGamma 2012','eeGamma'))

    configs.append(b.JobConfig('tthHZG_M120', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M120_RD1', 5, 'Signal2012tthM120 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('tthHZG_M125', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M125_RD1', 5, 'Signal2012tthM125 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('tthHZG_M130', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M130_RD1', 5, 'Signal2012tthM130 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('tthHZG_M135', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M135_RD1', 5, 'Signal2012tthM135 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('tthHZG_M140', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M140_RD1', 5, 'Signal2012tthM140 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('tthHZG_M145', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M145_RD1', 5, 'Signal2012tthM145 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('tthHZG_M150', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M150_RD1', 5, 'Signal2012tthM150 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('tthHZG_M155', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M155_RD1', 5, 'Signal2012tthM155 ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('tthHZG_M160', t3storage+'/nuTuples_v9.6_8TeV/MC/tthHZG_M160_RD1', 5, 'Signal2012tthM160 ABCD eeGamma 2012','eeGamma'))


    configs.append(b.JobConfig('Run2012A', t3storage+'/nuTuples_v9.6_8TeV/Data/DoubleElectron_Run2012A', 50, 'DATA ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('Run2012B', t3storage+'/nuTuples_v9.6_8TeV/Data/DoubleElectron_Run2012B', 100, 'DATA ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('Run2012C', t3storage+'/nuTuples_v9.6_8TeV/Data/DoubleElectron_Run2012C', 150, 'DATA ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('Run2012D', t3storage+'/nuTuples_v9.6_8TeV/Data/DoubleElectron_Run2012D', 150, 'DATA ABCD eeGamma 2012','eeGamma'))

    configs.append(b.JobConfig('ZGToLLG', t3storage+'/nuTuples_v9.6_8TeV/MC/ZGToLLG_RD1', 50, 'ZGToLLG ABCD eeGamma 2012','eeGamma'))
    configs.append(b.JobConfig('DYJets', t3storage+'/nuTuples_v9.6_8TeV/MC/DYJetsToLL_M-50_RD1', 150, 'DYJets ABCD eeGamma 2012','eeGamma'))

  batcher = b.BatchMaster(configs, outputPathNWU,'execBatch.sh')
batcher.SubmitToLPC()
