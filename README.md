Welcome to the New HZG Analyzer, by Brian Pollack
=================================================

This branch is in production, intended to run on the bleeding edge NWU ntuples.
Significant updates are intended for readability, speed improvements, and workflow optimization.

Overview:
------
`HiggsZGAnalyzer` contains the main analyzer for H(A)->Zg, along with other analyzers.  All analyzers are meant to run on nuTuples.
`hzgammaME` contains MCFM libraries for computing matrix elements
`scripts` contains plotting scripts for use on analyzer outputs
`interface, src, plugins` contain C++ source code for all the dependencies required to run the analyzers.


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

3. Compile the package and export the libarry

        make
        cd obj
        g++ -Wl,-soname,libmcfm_6p6.so -shared -o libmcfm_6p6.so *.o  
