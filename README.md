Welcome to the New HZG Analyzer, by Brian Pollack
=================================================

This branch is in production, intended to run on the bleeding edge NWU ntuples.
Significant updates are intended for readability, speed improvements, and workflow optimization.

Notes:
------
  * Be careful of vector<ROOTstuff>, especially TCPhoton, vectors do not clean properly
  * Valgrind claims TCPhoton has mismatched delete in destructor, I think this is a ROOT issue, not sure how to fix
  * There is now a segfault on termination of ROOT, debugging has not shown the cause, most likely some corrupted memory somewhere along the line

From Yanyan (compiling MCFM for MEM):
------------------------------------
Instruction on compiling the MCFM library for MCFM 6.6

1. Download MCFM and prepare the install makefile

    wget http://mcfm.fnal.gov/mcfm-6.6.tar.gz
    tar -xf MCFM-6.6.tar.gz
    cd MCFM-6.6/
    ./Install

2. Modify the following two lines in makefile to allow shared library to be read out

    FFLAGS  = -fno-automatic -fno-f2c -O0 -fPIC -g -I$(INCPATH) -I$(TENSORREDDIR)/Include -Iobj
    F90FLAGS = -fno-automatic -fno-f2c -O2 -fPIC -g -I$(INCPATH) -Iobj -Jobj

3. compile the package and export the libarry

    make
    cd obj
    g++ -Wl,-soname,libmcfm_6p6.so -shared -o libmcfm_6p6.so *.o  
