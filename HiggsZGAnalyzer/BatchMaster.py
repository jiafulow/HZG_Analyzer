#! /usr/bin/env python

import sys, os, subprocess, fileinput, math, tempfile, shutil, glob
from collections import defaultdict

#Small modifications on original BatchMaster by Nate Odell

class JobConfig():
    '''Class for storing configuration for each dataset'''
    def __init__(self, dataName = 'TEST', inDir = '/eos/uscms/store/user/bpollack/V08_00_8TeV/ggHZG_M125_Pythia8_175_v3', nJobs = 2, arguments = 'Signal2012ggM125 ABCD mumuGamma 2012', selection = 'mumuGamma'):
        self._dataName  = dataName
        self._inDir     = inDir
        self._nJobs     = nJobs
        self._args      = arguments
        self._selection = selection


class BatchMaster():
    '''A tool for submitting batch jobs'''
    def __init__(self, configList, outDir = '.',execution = 'execBatch.csh'):
        self._current    = os.path.abspath('.')
        self._outDir     = outDir
        self._configList = configList
        self._executable = execution

    def MakeDirectory(self, filePath, clear = True):
        '''Create save path in case it doesn't already exist'''
        if not os.path.exists(filePath):
            os.system('mkdir -p '+filePath)
        if not os.path.exists(filePath+'/res'):
            os.system('mkdir -p '+filePath+'/res')
        elif clear:
            os.system('rm '+filePath+'/*')
            os.system('rm '+filePath+'/res/*')

    def SplitJobs(self, directory, nJobs):
        '''Split jobs by dataset'''
        fileList = os.listdir(directory)
        nFiles = len(fileList)

        if nJobs > nFiles:
            nJobs = nFiles
        nFilesPerJob = int(math.ceil(nFiles/nJobs))
        fileSplit = [fileList[i:i+nFilesPerJob] for i in range(0, len(fileList), nFilesPerJob)]

        return fileSplit

    def MakeExecutable(self, config, sourceFiles):
        '''Writes config into executable'''
        infile   = open(self._executable, 'r')
        exec_tmp = tempfile.NamedTemporaryFile(prefix = config._dataName+'_'+config._selection+'_', delete=False)

        for line in infile.readlines():
            if 'Leave this blank' not in line:
                exec_tmp.write(line)
            else:
                path = config._inDir
                for file in sourceFiles:
                  if 'pnfs' in path:
                    exec_tmp.write('echo '+repr('dcache:'+path+'/'+file)+' >> input.txt\n')
                  else:
                    exec_tmp.write('echo '+repr(path+'/'+file)+' >> input.txt\n')

        exec_tmp.seek(0)
        infile.close()
        return exec_tmp

    def MakeBatchConfig(self, config, nJob, exec_tmp):
        '''Write batch configuration file'''
        batch_submit_file = tempfile.NamedTemporaryFile(prefix = 'batch_submit_file', delete=False)
        batch_submit_file.write('Arguments  = {0} {1} {2}\n'.format(nJob, config._dataName, config._args))
        batch_submit_file.write('Executable = {0}\n'.format(exec_tmp.name))
        batch_submit_file.write('Should_Transfer_Files = YES\n')
        batch_submit_file.write('WhenToTransferOutput = ON_EXIT\n')
        #if os.environ.get('AT_NWU') == None:
        batch_submit_file.write('Transfer_Input_Files = stageball.tar.gz\n')
        batch_submit_file.write('universe              = vanilla\n')
        batch_submit_file.write('Requirements          = OpSys == "LINUX"&& (Arch != "DUMMY" )\n')
        #batch_submit_file.write('Requirements          = machine!="ttnode0004"\n')
        batch_submit_file.write('+LENGTH               = "LONG"\n')
        batch_submit_file.write('Output                = res/report_$(Cluster)_$(Process).stdout\n')
        batch_submit_file.write('Error                 = res/report_$(Cluster)_$(Process).stderr\n')
        batch_submit_file.write('Log                   = res/report_$(Cluster)_$(Process).log\n')
        batch_submit_file.write('notify_user           = FAKENAME@fake.notreal\n')
        batch_submit_file.write('Queue\n')
        batch_submit_file.seek(0)
        return batch_submit_file

    def RecordSelection(self):
      '''Output a small txt file so CheckFiles script knows who is running, who is finished, all that shit'''
      checkDict = defaultdict(list)
      for cfg in self._configList:
        checkDict[cfg._selection].append(cfg._dataName)

      check_txt = open('.checkfile.txt','w')
      check_txt.write('{0}\n'.format(self._configList[0]._args.split()[-1]))
      for sel in checkDict.keys():
        check_txt.write('{0}'.format(sel))
        for data in checkDict[sel]:
          check_txt.write(' {0}'.format(data))
        check_txt.write('\n')

      check_txt.close()



    def SubmitToLPC(self):
        '''Submits batch jobs to lpc batch'''
        self.RecordSelection()
        for j,cfg in enumerate(self._configList):
            self.MakeDirectory(self._outDir+'/'+cfg._selection+'/'+cfg._dataName, clear = True)

            sourceFiles = self.SplitJobs(cfg._inDir, cfg._nJobs)

            # copy files to staging so they cant be modified during submission
            if not os.path.exists(self._outDir+'/'+cfg._selection+'/'+cfg._dataName+'/stageball.tar.gz'):
              os.system('cp stageball.tar.gz '+self._outDir+'/'+cfg._selection+'/'+cfg._dataName+'/.' )
            if not os.path.exists(self._outDir+'/'+cfg._selection+'/'+cfg._dataName+'/'+self._executable):
              os.system('cp '+self._executable+' '+self._outDir+'/'+cfg._selection+'/'+cfg._dataName+'/.' )
            os.chdir(self._outDir+'/'+cfg._selection+'/'+cfg._dataName)

            for i, source in enumerate(sourceFiles):
                exec_tmp  = self.MakeExecutable(cfg, source)
                batch_tmp = self.MakeBatchConfig(cfg, i, exec_tmp)
                sub = subprocess.call('condor_submit ' + batch_tmp.name , shell=True)
                exec_tmp.close()
                batch_tmp.close()

                #sub.wait()
                #os.remove(exec_tmp.name)
                #os.remove(batch_tmp.name)

