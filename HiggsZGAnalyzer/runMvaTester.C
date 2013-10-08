
  #include <iostream>
  #include <fstream>
  #include <string>
  #include <vector>
  #include <cstdlib>

  using namespace std;

  void runMvaTester() {

    gROOT->SetMacroPath(".:../src/:../interface/:../plugins/");
    gROOT->LoadMacro("EGammaMvaEleEstimator.cc++g");
    gROOT->ProcessLine(".x mvaTester.C++g");


    TStopwatch timer;
    timer.Start();

    cout << "\n\nDone!" << endl;
    cout << "CPU Time : " << timer.CpuTime() << endl;
    cout << "RealTime : " << timer.RealTime() << endl;
    cout << "\n";
  }

