#! /usr/bin/env python
import BatchMaster as b
import os

EOS         = '/eos/uscms/store/user/bpollack'
dCache      = '/pnfs/cms/WAX/11/store/user/bpollack'
t3storage   = '/tthome/bpollack/storage/'

outputPathFNAL  = '/uscms/home/bpollack/nobackup/BatchOutput'
outputPathNWU  = '/tthome/bpollack/BatchOutput'

os.system('tar -zcvf stageball.tar.gz higgsAnalyzer* ../src  otherHistos ../plugins ../interface ../hzgammaME ../mva/testWeights')

doMuMuGamma = True
doEEGamma = False
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
    configs.append(b.JobConfig('ggHZG_M125_pythia8_NLO', EOS+'/V08_01_8TeV/ggH_M125_p8175_crab', 5, 'Signal2012ggM125NLOp8 ABCD eeGamma 2012','eeGamma'))
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
  #configs.append(b.JobConfig('ggHZG_M125_pythia8_NLO', t3storage+'/V08_01_8TeV/ggHZG_M125_Pythia8_175_POWHEG_PDF7', 5, 'Signal2012ggM125NLOp8 ABCD mumuGamma 2012','mumuGamma'))
  configs.append(b.JobConfig('ggHZG_M125_pythia8_NLO', t3storage+'/V08_01_8TeV/ggHZG_M125_Pythia8_175_POWHEG_CRAB', 5, 'Signal2012ggM125NLOp8 ABCD mumuGamma 2012','mumuGamma'))
  configs.append(b.JobConfig('ggHZG_M125_pythia8_LO', t3storage+'/V08_01_8TeV/ggHZG_M125_Pythia8_175_LO', 5, 'Signal2012ggM125p8 ABCD mumuGamma 2012','mumuGamma'))
  configs.append(b.JobConfig('ggHZG_M125_pythia6', t3storage+'/V08_01_8TeV/ggH_M125_p6', 5, 'Signal2012ggM125p6 ABCD mumuGamma 2012','mumuGamma'))

  configs.append(b.JobConfig('Run2012A', t3storage+'/V08_01_8TeV/DoubleMu/Run2012A', 50, 'DATA ABCD mumuGamma 2012','mumuGamma'))
  configs.append(b.JobConfig('Run2012B', t3storage+'/V08_01_8TeV/DoubleMu/Run2012B_v2', 100, 'DATA ABCD mumuGamma 2012','mumuGamma'))
  configs.append(b.JobConfig('Run2012C', t3storage+'/V08_01_8TeV/DoubleMu/Run2012C_v2', 150, 'DATA ABCD mumuGamma 2012','mumuGamma'))
  configs.append(b.JobConfig('Run2012D', t3storage+'/V08_01_8TeV/DoubleMu/Run2012D_v2', 150, 'DATA ABCD mumuGamma 2012','mumuGamma'))

  configs.append(b.JobConfig('ZGToLLG', t3storage+'/V08_01_8TeV/ZGToLLG', 50, 'ZGToLLG ABCD mumuGamma 2012','mumuGamma'))
  configs.append(b.JobConfig('DYJets', t3storage+'/V08_01_8TeV/DYJetsToLL_M-50', 150, 'DYJets ABCD mumuGamma 2012','mumuGamma'))

  batcher = b.BatchMaster(configs, outputPathNWU,'execBatch.sh')
batcher.SubmitToLPC()
