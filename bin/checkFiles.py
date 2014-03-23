#!/usr/bin/env python
import sys, os
import subprocess

def checkFiles():
  if not os.path.isfile(os.getenv('CMSSW_BASE')+'/src/HZG_Analyzer/HiggsZGAnalyzer/.checkfile.txt'):
    print 'where is the .checkfile.txt?! you fucked something up, I aint checkin shit. fuck you.'
    return
  infile = open(os.getenv('CMSSW_BASE')+'/src/HZG_Analyzer/HiggsZGAnalyzer/.checkfile.txt','r')
  print
  analyzer = infile.readline()
  print analyzer
  for line in infile:
    selectionList = line.split()
    for dataType in selectionList[1:]:
      if os.environ.get('AT_NWU') == None:
        rootProc = subprocess.Popen(["bash -c 'wc -l <(ls -1 ~/nobackup/BatchOutput/{0}/{1})'".format(selectionList[0],dataType)],stdout=subprocess.PIPE, shell=True)
        resProc  = subprocess.Popen(["bash -c 'wc -l <(ls -1 ~/nobackup/BatchOutput/{0}/{1}/res)'".format(selectionList[0],dataType)],stdout=subprocess.PIPE, shell=True)
      else:
        rootProc = subprocess.Popen(["bash -c 'wc -l <(ls -1 ~/BatchOutput/{0}/{1})'".format(selectionList[0],dataType)],stdout=subprocess.PIPE, shell=True)
        resProc  = subprocess.Popen(["bash -c 'wc -l <(ls -1 ~/BatchOutput/{0}/{1}/res)'".format(selectionList[0],dataType)],stdout=subprocess.PIPE, shell=True)
      rootOut  = rootProc.communicate()[0]
      resOut   = resProc.communicate()[0]

      num1 = int(rootOut.rsplit(" ")[0])-3
      num2 = int(resOut.rsplit(" ")[0])
      print
      print selectionList[0], dataType
      if 'smzg' in analyzer:
        print ' ',num1,num2/3
        if num1 == num2/3: print '  ALL JOBS FINISHED'
        else: print "  SOME JOBS HAVEN'T FUCKING FINISHED GODDAMNIT!"
      else:
        print ' ',num1/4,num2/3
        if num1/4 == num2/3: print '  ALL JOBS FINISHED'
        else: print "  SOME JOBS HAVEN'T FUCKING FINISHED GODDAMNIT!"






if __name__=="__main__":
  checkFiles()
