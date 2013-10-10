#!/usr/bin/env python

import ROOT, sys
from ROOT import RooNDKeysPdf, RooDataSet, RooWorkspace, \
     RooPlot, RooHistPdf, RooAddPdf, RooRealVar, RooArgSet, RooFit, \
     TFile, TTree, RooArgList, RooMinuit, RooThresholdCategory, RooProfileLL, \
     TGraphAsymmErrors, RooMinimizer, RooRealConstant
from math import sqrt

from optparse import OptionParser

#have to make very fine hist pdf from Keys Pdf to save time, then linearly/quadratically interpolate histo

def main(options,args):

    eta_bins = [[0.0,1.4442]] # try to split material dependence
#                [1.560,2.5]]

    pt_bins = [[12, 15],
               [15, 20]]

    #get input workspace
    input = TFile.Open(options.inputFile)    
    ws = input.Get(options.workspaceName).Clone()
    input.Close()

    #ws.Print("v")

    sieie = RooRealVar(options.showerShapeName,options.showerShapeName,0)
    pt = RooRealVar(options.ptName,options.ptName,0)
    eta = RooRealVar(options.etaName,options.etaName,0)    
    chIso = RooRealVar(options.isoName,options.isoName,0)

    vars = RooArgSet()
    vars.add(sieie)
    vars.add(pt)
    vars.add(eta)
    vars.add(chIso)    

    if options.MCFakeRate:
        #chIso = RooRealVar(options.isoName, 'phHIso', 100, 0, 20)
        fake = RooRealVar('FakePho','',-0.5,1.5)
        vars.add(fake)

    data = None

    if options.weight is not None:
        weight = RooRealVar(options.weight,'',.5)
        vars.add(weight)
        data = RooDataSet('data','All Input Data',
                          vars,
                          RooFit.ImportFromFile(options.dataFile,
                                                options.dataTreeName),
                          RooFit.WeightVar(options.weight)
                          )
    else:
        data = RooDataSet('data','All Input Data',
                          vars,
                          RooFit.ImportFromFile(options.dataFile,
                                                options.dataTreeName)
                          )

    data.Print("v")
    
    # put in the raw datasets, no cuts or manipulation
    getattr(ws,'import')(data,
                         RooFit.RenameVariable(options.showerShapeName,'Pho_SigmaIEtaIEta'),
                         RooFit.RenameVariable(options.ptName,'Pho_Pt'),
                         RooFit.RenameVariable(options.etaName,'Pho_Eta'))

    sieie = ws.var('Pho_SigmaIEtaIEta')
    sieie.setRange('barrelSel',0,0.011)
    sieie.setRange('endcapSel',0,0.033)

    output = TFile.Open(options.outputFile,'RECREATE')
    output.cd()

    #histograms
    fakeRate   = []
    nFakes     = []
    nTot       = []
    fakeRateMC = []
    #counters
    totBkg     = [] #central value in each bin
    totBkgUp   = [] #upper error
    totBkgLo   = [] #lower error
    ietabin = 0

    #setup histograms and counters
    for i in range(len(eta_bins)):
        fakeRate.append(TGraphAsymmErrors())
        fakeRate[i].SetName('fakeRate_etabin%d'%i)

        nFakes.append(TGraphAsymmErrors())
        nFakes[i].SetName('nFakes_etabin%d'%i)

        nTot.append(TGraphAsymmErrors())
        
        nTot[i].SetName('nTot_etabin%d'%i)

        if options.MCFakeRate:
            fakeRateMC.append(TGraphAsymmErrors())
            fakeRateMC[i].SetName('fakeRateMC_etabin%d'%i)

        totBkg.append(0)
        totBkgUp.append(0)
        totBkgLo.append(0)

    #loop through bins making all fits
    for etabin in eta_bins:
        #pt bin number for TGraphs
        iptbin = 0
        
        for ptbin in pt_bins:
            
            if 'abs(Pho_Eta) < 1.4442':
                phoselsig = ' abs(Pho_Eta) > %f  && abs(Pho_Eta) < %f && Pho_Pt > %f && Pho_Pt < %f'%(etabin[0],
                                                                                                     etabin[1],
                                                                                                     ptbin[0],
                                                                                                      ptbin[1])
       #         if options.MCFakeRate:
                phoselbkg = 'phoCHIso > 2 && phoCHIso < 6 && abs(Pho_Eta) > %f  && abs(Pho_Eta) < %f && Pho_Pt > %f && Pho_Pt < %f'%(etabin[0],
                                                                                                                                      etabin[1],
                                                                                                                                      ptbin[0],
                                                                                                                                      ptbin[1])
                
            phosel= phoselsig or phoselbkg
            #phosel = 'phoselsig || phoselbkg'
            #phosel = 'abs(Pho_Eta) > %f  && abs(Pho_Eta) < %f && Pho_Pt > %f && Pho_Pt < %f'%(etabin[0],
            #    etabin[1],
            #   ptbin[0],
            #  ptbin[1])
            postfix = '_Eta_%.4f_%.4f_Pt_%.2f_%.2f'%(etabin[0],etabin[1],ptbin[0],ptbin[1])


            binData = ws.data('data').reduce(RooFit.Cut(phosel),
                                             RooFit.Name('binData'+postfix),
                                             RooFit.Title('Data Events'))        
                        
            #save it all in the workspace
            getattr(ws,'import')(binData)            
            
            #create the PDFs we are going to fit!
            options.pdfModule.makePdf(ws,etabin,postfix,options.extraInput)
            output.cd()
            
            ws.factory('RooExtendPdf::fullPDFExt'+postfix+'('+
                       'fullPDF'+postfix+',nTot'+postfix+'[2000,1e-8,10000])')

            
            bNLL = ws.pdf('fullPDFExt'+postfix).createNLL(ws.data('binData'+postfix),                                                          
                                                          RooFit.Extended(True), RooFit.NumCPU(4))
                        
            bFit = RooMinuit(bNLL)
        

            #bFit.setVerbose(True)
            if ws.data('binData'+postfix).sumEntries() > 0:
                bFit.setPrintLevel(-1)
                bFit.migrad()
                bFit.hesse()
                bFit.setPrintLevel(0)
                bFit.minos(RooArgSet(ws.var('fBkg'+postfix)))
                
                bRes = bFit.save('binFitResult'+postfix)
                getattr(ws,'import')(bRes)
                
                #plot fits on top of data
                bFrame = sieie.frame()
                bFrame.SetName('binFrame'+postfix)
                
                binData.plotOn(bFrame)
                ws.pdf('fullPDFExt'+postfix).plotOn(bFrame, RooFit.LineColor(ROOT.kYellow))
                #ws.pdf('fullPDFExt'+postfix).plotOn(bFramea)
                ws.pdf('fullPDFExt'+postfix).plotOn(bFrame,
                                                    RooFit.Components('bkgTemplatePDF'+postfix),
                                                    RooFit.LineColor(ROOT.kBlue))        
                ws.pdf('fullPDFExt'+postfix).plotOn(bFrame,
                                                    RooFit.Components('sigTemplatePDF'+postfix),
                                                    RooFit.LineColor(ROOT.kRed))        

                ws.pdf('fullPDFExt'+postfix).plotOn(bFrame,
                                                    RooFit.Components('fullPDF'+postfix),
                                                    RooFit.LineColor(ROOT.kGreen))        
                bFrame.Write()

                bSigInt = None
                bBkgInt = None
                
                #make plot of fake rate in signal region as a function of pT
                #< 0.011 (EB), <0.030 (EE)
                if etabin[0] >= 1.560:
                    bSigInt = ws.pdf('sigTemplatePDF'+
                                     postfix).createIntegral(RooArgSet(sieie),
                                                             RooArgSet(sieie),
                                                             'endcapSel')
                    bBkgInt = ws.pdf('bkgTemplatePDF'+
                                     postfix).createIntegral(RooArgSet(sieie),
                                                             RooArgSet(sieie),
                                                             'endcapSel')
                else:
                    bSigInt = ws.pdf('sigTemplatePDF'+
                                     postfix).createIntegral(RooArgSet(sieie),
                                                             RooArgSet(sieie),
                                                             'barrelSel')
                    bBkgInt = ws.pdf('bkgTemplatePDF'+
                                     postfix).createIntegral(RooArgSet(sieie),
                                                             RooArgSet(sieie),
                                                             'barrelSel')
                
                nEventsB = ws.var('nTot'+postfix).getVal()                
                nSignalB = (1.0 - ws.var('fBkg'+postfix).getVal())*nEventsB*bSigInt.getVal()
                nBkgB    = ws.var('fBkg'+postfix).getVal()*nEventsB*bBkgInt.getVal()
                nBkgBUp  = ws.var('fBkg'+postfix).getErrorHi()*nEventsB*bBkgInt.getVal()
                nBkgBLo  = -ws.var('fBkg'+postfix).getErrorLo()*nEventsB*bBkgInt.getVal()
                print "Events Sig Bkg: %.3f + %.3f - %.3f"%(nEventsB, nSignalB, nBkgB)

                trash = (1.0 - ws.var('fBkg'+postfix).getVal())
                trash1 = bSigInt.getVal()
                trash2 = bBkgInt.getVal()
                trash3 = ws.var('fBkg'+postfix).getVal()
                print nEventsB
                print trash
                print trash1
                print trash2
                print trash3

                if nBkgBLo == 0.0: #catch cases when minos dies on lower limits
                    parb_err = ws.var('fBkg'+postfix).getError()*nEventsB*bBkgInt.getVal()
                    nBkgBLo = -max(nBkgB - parb_err,-nBkgB)
                    
                totBkg[ietabin]   += nBkgB
                totBkgUp[ietabin] = sqrt(nBkgBUp*nBkgBUp + totBkgUp[ietabin]*totBkgUp[ietabin])
                totBkgLo[ietabin] = sqrt(nBkgBLo*nBkgBLo + totBkgLo[ietabin]*totBkgLo[ietabin])
                
                print "Background Events: %.3f + %.3f - %.3f"%(nBkgB,nBkgBUp,nBkgBLo)
                fakeRate[ietabin].SetPoint(iptbin,(float(ptbin[1])+float(ptbin[0]))/2,nBkgB/(nSignalB+nBkgB))
                fakeRate[ietabin].SetPointError(iptbin,
                                                (float(ptbin[1])+float(ptbin[0]))/2 - float(ptbin[0]),
                                                float(ptbin[1]) - (float(ptbin[1])+float(ptbin[0]))/2,
                                                -ws.var('fBkg'+postfix).getErrorLo(),
                                                ws.var('fBkg'+postfix).getErrorHi())
                
                #nFakes[ietabin].SetPoint(iptbin,(float(ptbin[1])+float(ptbin[0]))/2,nBkgB/(float(ptbin[1]) - float(ptbin[0])))                
                nFakes[ietabin].SetPoint(iptbin,(float(ptbin[1])+float(ptbin[0]))/2,nBkgB)                
                nFakes[ietabin].SetPointError(iptbin,
                                              (float(ptbin[1])+float(ptbin[0]))/2 - float(ptbin[0]),
                                              float(ptbin[1]) - (float(ptbin[1])+float(ptbin[0]))/2,
                                              nBkgBLo/(float(ptbin[1]) - float(ptbin[0])),
                                              nBkgBUp/(float(ptbin[1]) - float(ptbin[0])))
                
                #nTot[ietabin].SetPoint(iptbin,(float(ptbin[1])+float(ptbin[0]))/2,nEventsB/(float(ptbin[1]) - float(ptbin[0])))
                nTot[ietabin].SetPoint(iptbin,(float(ptbin[1])+float(ptbin[0]))/2,nEventsB)

                nTot[ietabin].SetPointError(iptbin,
                                            (float(ptbin[1])+float(ptbin[0]))/2 - float(ptbin[0]),
                                            float(ptbin[1]) - (float(ptbin[1])+float(ptbin[0]))/2,
                                            ws.var('nTot'+postfix).getError()/(float(ptbin[1]) - float(ptbin[0])),
                                            ws.var('nTot'+postfix).getError()/(float(ptbin[1]) - float(ptbin[0])))
            
                if options.MCFakeRate:
                    fr = 0
                    if etabin[0] > 1.560:
                        fr = (ws.data('binData'+postfix).sumEntries('FakePho > 0.5 && Pho_SigmaIEtaIEta < 0.030')/
                              ws.data('binData'+postfix).sumEntries('FakePho > -1 && Pho_SigmaIEtaIEta < 0.030'))
                    else:
                        fr = (ws.data('binData'+postfix).sumEntries('FakePho > 0.5 && Pho_SigmaIEtaIEta < 0.011')/
                              ws.data('binData'+postfix).sumEntries('FakePho > -1 && Pho_SigmaIEtaIEta < 0.011'))
                    
                    fakeRateMC[ietabin].SetPoint(iptbin,(float(ptbin[1])+float(ptbin[0]))/2,fr)            
                    fakeRateMC[ietabin].SetPointError(iptbin,
                                                      (float(ptbin[1])+float(ptbin[0]))/2 - float(ptbin[0]),
                                                      float(ptbin[1]) - (float(ptbin[1])+float(ptbin[0]))/2,
                                                      0,
                                                      0)       
        
            iptbin += 1
        ietabin += 1
        
    for i in range(len(eta_bins)):
        print " %.4f < |Eta| < %.4f  Total Background = %.3f +/- (%.3f,%.3f)"%(eta_bins[i][0],eta_bins[i][1],
                                                                               totBkg[i],totBkgUp[i],totBkgLo[i])
        
        fakeRate[i].Write()
        nFakes[i].Write()
        nTot[i].Write()
        
        if options.MCFakeRate:
            fakeRateMC[i].Write()
    
    ws.Write()
    output.Close()  
    

if __name__ == '__main__':
    parser = OptionParser(description='%prog : J/Psi Polarization Fitter.',
                          usage='makeSmoothTemplates.py -o templates.root -i signal.root -b bkg.root')
    parser.add_option('-d',dest='dataFile',help='Name of the input selected data.')
    parser.add_option('-i',dest='inputFile',help='Name of the file containing the input RooWorkspace')
    parser.add_option('-w',dest='workspaceName',help='Name of the workspace in the file (defaults to filename without ".root".')
    parser.add_option('-o',dest='outputFile',help='Name of the output file.')
    parser.add_option('-p',dest='pdfModule',default='simpleTemplate',help='the python module with code to make the pdfs')
    parser.add_option('-e',dest='extraInput',help='extra input for the pdf module')
        
    parser.add_option('--dataTreeName',dest='dataTreeName',default='selzgam_nosihih',help='Name of the TTree in the data file.')
    parser.add_option('--showerShapeName',dest='showerShapeName',default='phosihih',help='Name of the leaf in the input data file.')
    parser.add_option('--ptName',dest='ptName',default='phoCorEt',help='Name of the leaf in the input data file.')
    parser.add_option('--etaName',dest='etaName',default='phoSCEta',help='Name of the leaf in the input data file.')
    parser.add_option('--weight',dest='weight',help='The name of the weight variable, for use with MC')
    parser.add_option('--doMCTruthRate',dest='MCFakeRate',default=False,action='store_true',help='Calculate the true fake rate from MC')
    parser.add_option('--isoName',dest='isoName',default='phoCHIso',help='Name of the leaf in the input data file.')
  
    (options,args) = parser.parse_args()

    miss_options = False

    if options.dataFile is None:
        print 'Need to specify -d'
        miss_options=True
        
    if options.inputFile is None:
        print 'Need to specify -i'
        miss_options=True
    elif options.workspaceName is None:
        options.workspaceName = options.inputFile[:options.inputFile.find('.root')]        
    if options.outputFile is None:
        print 'Need to specify -o'
        miss_options=True

    #get the module that will make our pdf
    __import__(options.pdfModule)
    options.pdfModule = sys.modules[options.pdfModule]
    
    if miss_options:
        exit(1)

    main(options,args)
