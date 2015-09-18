#!/usr/bin/env python

from collections import OrderedDict
import split

skimdir = "skims/"
outdir = "skims/nuTuples_v9.10_8TeV/MC/"
srcdir = "skims/nuTuples_v9.10_8TeV/sourceFiles/"
ext = ".txt"

datasets = OrderedDict([
    ("DYJetsToLL_M-10To50", 2),
    ("DY1JetsToLL_M-10To50", 2),
    ("DY2JetsToLL_M-10To50", 2),
    ("DYJetsToLL_M-50", 4),
    ("DY1JetsToLL_M-50", 4),
    ("DY2JetsToLL_M-50", 4),
    ("DY3JetsToLL_M-50", 2),
    ("DY4JetsToLL_M-50", 2),
    ("WJetsToLNu", 2),
    ("TTJets_FullLep", 2),
    ("TTJets_Hadronic", 2),
    ("TTJets_SemiLep", 2),
    ("WW", 2),
    ("WZ", 2),
    ("ZZ", 4),
    ("WWJetsTo2L2Nu", 2),
    ("WZJetsTo2L2Q", 1),
    ("WZJetsTo3LNu", 1),
    ("ZZJetsTo2L2Nu", 1),
    ("ZZJetsTo2L2Q", 1),
    ("ZZJetsTo4L", 2),
    ("T_s", 1),
    ("T_t", 1),
    ("T_tW", 1),
    ("Tbar_s", 1),
    ("Tbar_t", 1),
    ("Tbar_tW", 1),
    ("TT_FCNH_M-30", 1),
    ("TTH_M-30", 1),
])

def skim():
    
    for k, v in datasets.iteritems():
        if v != 1:
            split.split(srcdir+k+ext, v, verbose=False)
    
        if v != 1:
            for i in xrange(v):
                print "  root -l -b -q 'run2.C(\"{0} {0}_{1} {1}\")' >& {2}skim_{0}_{1}.log &".format(k, i+1, skimdir)
        else:
            print "  root -l -b -q 'run2.C(\"{0} {0} {1}\")' >& {2}skim_{0}.log &".format(k, v, skimdir)
    
    
    print "\nOutput files go to:"
    print outdir

if __name__=="__main__":
    
    skim()
