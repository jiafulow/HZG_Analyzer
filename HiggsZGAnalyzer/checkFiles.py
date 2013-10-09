#!/usr/bin/env python
import sys, os
import subprocess

def checkFiles():
  if not os.path.isfile('.checkfile.txt'):
    print 'where is the .checkfile.txt?! you fucked something up, I aint checkin shit. fuck you.'
    return
  infile = open('.checkfile.txt','r')
  for line in infile:
    selectionList = line.split()
    for dataType in selectionList[1:]:
      rootProc = subprocess.Popen(["bash -c 'wc -l <(ls -1 ~/nobackup/BatchOutput/{0}/{1})'".format(selectionList[0],dataType)],stdout=subprocess.PIPE, shell=True)
      rootOut  = rootProc.communicate()[0]
      resProc  = subprocess.Popen(["bash -c 'wc -l <(ls -1 ~/nobackup/BatchOutput/{0}/{1}/res)'".format(selectionList[0],dataType)],stdout=subprocess.PIPE, shell=True)
      resOut   = resProc.communicate()[0]

      num1 = int(rootOut.rsplit(" ")[0])-3
      num2 = int(resOut.rsplit(" ")[0])
      print
      print selectionList[0], dataType
      print ' ',num1/5,num2/3
      if num1/5 == num2/3: print '  ALL JOBS FINISHED'
      else: print "  SOME JOBS HAVEN'T FUCKING FINISHED GODDAMNIT!"






if __name__=="__main__":
  checkFiles()
