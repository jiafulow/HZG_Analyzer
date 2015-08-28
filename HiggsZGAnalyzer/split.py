#!/usr/bin/env python

import os
import sys
from math import ceil

"""
Split a text file into many text files -- how cool is that!?
"""

def split(verbose=True):
    
    if len(sys.argv) != 3:
        print "usage: ./split.py filename n"
        exit(1)
    
    filename = sys.argv[1]
    n = int(sys.argv[2])
    
    if n <= 0:
        raise Exception("n must be >= 1")
    
    base, ext = os.path.splitext(filename)
    
    # Read
    with open(filename) as f:
        lines = f.readlines()
    
    nlines = len(lines)
    
    if n > nlines:
        n = nlines
    
    m = int(ceil(float(nlines)/n))
    
    if verbose:
        print "Input file {0} has {1} lines".format(filename, nlines)
        print "Will split into {0} files, <= {1} lines per file.".format(n, m)
    
    # Write
    for i in xrange(n):
        filename1 = "{0}_{1}{2}".format(base, i, ext)
        
        with open(filename1, "w") as f:
            writeme = lines[i:i+m]
            for line in writeme:
                f.write(line)
    
    if verbose:
        print "DONE"


if __name__=="__main__":
    
    split()
