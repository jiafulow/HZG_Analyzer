#! /usr/bin/env python
import BatchMaster as b
import os

EOS         = '/eos/uscms/store/user/bpollack'
dCache      = '/pnfs/cms/WAX/11/store/user/bpollack'
t3storage   = '/tthome/bpollack/storage/'

#outputPath  = '/uscms/home/bpollack/nobackup/BatchOutput'
outputPath  = '/tthome/bpollack/BatchOutput'

configs = []
configs.append(b.JobConfig('ggHZG_M125_pythia8_LO', t3storage+'/V08_01_8TeV/ggHZG_M125_Pythia8_175_LO', 5, 'Signal2012ggM125p8 ABCD mumuGamma 2012','mumuGamma'))
#configs.append(b.JobConfig('ggHZG_M125_pythia6', t3storage+'/V08_01_8TeV/ggH_M125_p6', 5, 'Signal2012ggM125p6 ABCD mumuGamma 2012','mumuGamma'))

#configs.append(b.JobConfig('Run2012A', t3storage+'/V08_01_8TeV/DoubleMu/Run2012A', 50, 'DATA ABCD mumuGamma 2012','mumuGamma'))
#configs.append(b.JobConfig('Run2012B', t3storage+'/V08_01_8TeV/DoubleMu/Run2012B_v2', 100, 'DATA ABCD mumuGamma 2012','mumuGamma'))
#configs.append(b.JobConfig('Run2012C', t3storage+'/V08_01_8TeV/DoubleMu/Run2012C_v2', 150, 'DATA ABCD mumuGamma 2012','mumuGamma'))
#configs.append(b.JobConfig('Run2012D', t3storage+'/V08_01_8TeV/DoubleMu/Run2012D_v2', 150, 'DATA ABCD mumuGamma 2012','mumuGamma'))

#configs.append(b.JobConfig('ZGToLLG', t3storage+'/V08_01_8TeV/ZGToLLG', 50, 'ZGToLLG ABCD mumuGamma 2012','mumuGamma'))
#configs.append(b.JobConfig('DYJets', t3storage+'/V08_01_8TeV/DYJetsToLL_M-50', 150, 'DYJets ABCD mumuGamma 2012','mumuGamma'))


os.system('tar -zcvf stageball.tar.gz higgsAnalyzer_Template.C higgsAnalyzer.h ../src  otherHistos ../plugins ../interface ../hzgammaME')

if os.environ.get('AT_NWU') == None:
  batcher = b.BatchMaster(configs, outputPath,'execBatch.csh')
else:
  batcher = b.BatchMaster(configs, outputPath,'execBatch.sh')
batcher.SubmitToLPC()
