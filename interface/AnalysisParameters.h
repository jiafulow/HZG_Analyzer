#ifndef ANALYSISPARAMETERS_H
#define ANALYSISPARAMETERS_H

#include <stddef.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


class Cuts{
  public:
    static Cuts* Instance();
    const float leadJetPt, trailJetPt, leadMuPt, trailMuPt, leadElePt, trailElePt, gPtOverMass, gPt, zMassLow, zMassHigh, metLow, metHigh, zgMassLow, zgMassHigh ;
  private:
    Cuts();
    static Cuts* m_pInstance;
};





#endif
