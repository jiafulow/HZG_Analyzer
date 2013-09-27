#! /usr/bin/env python
import BatchMaester as b
import os

EOS         = '/eos/uscms/store/user'

outputPath  = '/uscms/home/bpollack/nobackup/BatchOutput'

configs = []
configs.append(b.JobConfig('ggHZG_M125', EOS+'/bpollack/V08_00_8TeV/ggHZG_M125_Pythia8_175_v3', 2, 'Signal2012ggM125 ABCD mumuGamma 2012','mumuGamma'))
configs.append(b.JobConfig('ggHZG_M125', EOS+'/bpollack/V08_00_8TeV/ggHZG_M125_Pythia8_175_v3', 2, 'Signal2012ggM125 ABCD mumuGamma 2012','mumuGamma'))

#os.system('tar -zcvf stageball.tar.gz higgsAnalyzer_Template.C higgsAnalyzer.h ../src  otherHistos/*.root ../plugins ../interface ../hzgammaME')

batcher = b.BatchMaester(configs, outputPath,'execBatch.csh')
batcher.SubmitToLPC()
