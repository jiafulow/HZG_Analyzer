#!/usr/bin/env python

import ROOT
#RooNDKeysPdf, http://root.cern.ch/root/html/RooNDKeysPdf.html
from ROOT import RooNDKeysPdf, RooDataSet, RooWorkspace, \
     RooPlot, RooHistPdf, RooAddPdf, RooRealVar, RooArgSet, RooFit, RooCategory, \
     TFile, TTree, RooArgList, RooDataHist

#Optparse supports short style options like -x, long style options like --xhtml and positional arguments, also makes it easy to add default options and help text. 
from optparse import OptionParser

#have to make very fine hist pdf from Keys Pdf to save time, then linearly interpolate histo
def unify(input):
    if isinstance(input,RooDataSet):
        return input.__class__.__bases__[0]
    if isinstance(input,RooNDKeysPdf):
        return input

def main(options,args):

    pt_bins = [[12, 15],
               [15, 20]]
       
    eta_bins = [[0.0,1.4442]] # try to split material dependence
#                [1.560,2.5]]

    sieie = RooRealVar(options.showerShapeName,'#sigma_{i #eta i #eta}',.1,0,.15)
    sieie.setBins(200)
    pt    = RooRealVar(options.ptName,'Photon p_{T}',50,10,1000)
    eta   = RooRealVar(options.etaName,'Photon #eta',0.0,-3.0,3.0)
    chIso = RooRealVar(options.isoName, 'phoCHIso', 100, 0, 20)
    fakeMom = RooRealVar(options.momName, 'phohasFakemom', 0,1)
    
    #containor for the root variables
    vars = RooArgSet()
    vars.add(sieie)
    vars.add(pt)
    vars.add(eta)    
    vars.add(chIso)
    vars.add(fakeMom)

    allSig = None
    allBkg = None
    if options.weight is not None:
        weight = RooRealVar(options.weight,'',1)
        vars.add(weight)
        allSig = RooDataSet('allSig','All Signal Template Events',
                            vars,
                            RooFit.ImportFromFile(options.signalInput,
                                                  options.sigTreeName),
                            RooFit.WeightVar(options.weight)
                            )

        if options.useFakeMCBkg:
            fake = RooRealVar('Fake','',0.5,1.5)
            #RooRealVar isSidebands("isSidebands","isSidebands",0,1);
            #fake = RooRealVar(options.momName,"phohasFakemom",0,1)
            vars.add(fake)

        weight.setVal(1)

        ctauStates = RooCategory('ctauRegion','Cut Region in lifetime')
        ctauStates.defineType('phohasFakemom',0)
        #ctauStates.defineType('nonPrompt',1)
        
        allBkg = RooDataSet('allBkg','All Background Template Events',
                            RooArgSet(ctauStates,vars),
                            RooFit.ImportFromFile(options.backgroundInput,
                                                  options.bkgTreeName),
                            RooFit.WeightVar(options.weight)
                            )
    else:
        allSig = RooDataSet('allSig','All Signal Template Events',
                            vars,
                            RooFit.ImportFromFile(options.signalInput,
                                                  options.sigTreeName)
                            )
        
        allBkg = RooDataSet('allBkg','All Background Template Events',
                        vars,
                        RooFit.ImportFromFile(options.backgroundInput,
                                              options.bkgTreeName)
                        )
      
    output = TFile.Open(options.outputFile,'RECREATE')
    ws = RooWorkspace(options.outputFile[:options.outputFile.find('.root')],'Template Workspace')

    # put in the raw datasets, no cuts or manipulation
    getattr(ws,'import')(allSig,
                         RooFit.RenameVariable(options.showerShapeName,'Pho_SigmaIEtaIEta'),
                         RooFit.RenameVariable(options.ptName,'Pho_Pt'),
                         #RooFit.RenameVariable(options.isoName,'phoCHIso'),
                         RooFit.RenameVariable(options.etaName,'Pho_Eta'))
    getattr(ws,'import')(allBkg,
                         RooFit.RenameVariable(options.showerShapeName,'Pho_SigmaIEtaIEta'),
                         RooFit.RenameVariable(options.ptName,'Pho_Pt'),
                         #RooFit.RenameVariable(options.isoName,'phoCHIso'),
                         RooFit.RenameVariable(options.etaName,'Pho_Eta'))


    #loop through bins making all templates
    for etabin in eta_bins:
        for ptbin in pt_bins:
            
            if 'abs(Pho_Eta) < 1.4442':
                phoselsig = 'abs(Pho_Eta) > %f  && abs(Pho_Eta) < %f && Pho_Pt > %f && Pho_Pt < %f'%(etabin[0],
                                                                                              etabin[1],
                                                                                              ptbin[0],
                                                                                              ptbin[1])
#                if fake:
                phoselbkg = 'phoCHIso < 2  && abs(Pho_Eta) > %f  && abs(Pho_Eta) < %f && Pho_Pt > %f && Pho_Pt < %f'%(etabin[0],
                                                                                                         etabin[1],
                                                                                                         ptbin[0],
                                                                                                         ptbin[1])
                    
            postfix = '_Eta_%.4f_%.4f_Pt_%.2f_%.2f'%(etabin[0],etabin[1],ptbin[0],ptbin[1])
       
            binSig = ws.data('allSig').reduce(RooFit.Cut(phoselsig),
                                              RooFit.Name('sigEta'+postfix),
                                              RooFit.Title('Signal Template Events'))        
                        
            binBkg = ws.data('allBkg').reduce(RooFit.Cut(phoselbkg),
                                              RooFit.Name('bkgEta'+postfix),
                                              RooFit.Title('Background Template Events'))
                        
            #save it all in the workspace
            getattr(ws,'import')(binSig)
            getattr(ws,'import')(binBkg)            
            
            smoothingPars = RooArgList(ws.var('Pho_SigmaIEtaIEta')) #ws.var('Pho_Pt'),
            print 'Making binSigTemplate'+postfix+' with '+str(binSig.numEntries())+' events.'
            bSigTempl = None
            if( binSig.numEntries() <= 80000 ):
                bSigTempl = RooNDKeysPdf('binSigTemplate'+postfix,'',smoothingPars,binSig,"am")
            else:
                bSigTempl = super(RooDataSet,binSig)
                
            print 'Making binBkgTemplate'+postfix+' with '+str(binBkg.numEntries())+' events.'
            bBkgTempl = None
            if( binBkg.numEntries() <= 80000 ):
                bBkgTempl = RooNDKeysPdf('binBkgTemplate'+postfix,'',smoothingPars,binBkg,"am")
            else:
                bBkgTempl = super(RooDataSet,binBkg)
            
            #finely binned histograms so that we can store the results of the smoothing
            #These will be linearly interpolated by RooHistPdf
            bsTHist = bSigTempl.createHistogram('binSigSmoothedHist'+postfix,
                                                ws.var('Pho_SigmaIEtaIEta'),
                                                RooFit.Binning(options.nBins,0,.15) )
            getattr(ws,'import')(bsTHist)            
            
            bbTHist = bBkgTempl.createHistogram('binBkgSmoothedHist'+postfix,
                                                ws.var('Pho_SigmaIEtaIEta'),
                                                RooFit.Binning(options.nBins,0,.15) )
            getattr(ws,'import')(bbTHist)
                        
            #make RooDataHists for consuption by RooHistPdf Later
            bsTDHist = RooDataHist('binSigSmoothedDataHist'+postfix,
                                   '',
                                   RooArgList(ws.var('Pho_SigmaIEtaIEta')),
                                   ws.obj('binSigSmoothedHist'+postfix+'__Pho_SigmaIEtaIEta'))
            getattr(ws,'import')(bsTDHist)
                        
            bbTDHist = RooDataHist('binBkgSmoothedDataHist'+postfix,
                                   '',
                                   RooArgList(ws.var('Pho_SigmaIEtaIEta')),
                                   ws.obj('binBkgSmoothedHist'+postfix+'__Pho_SigmaIEtaIEta'))
            getattr(ws,'import')(bbTDHist)
    
    ws.Write()
    output.Close()  
    

if __name__ == '__main__':
    parser = OptionParser(description='%prog : J/Psi Polarization Fitter.',
                          usage='makeSmoothTemplates.py -o templates.root -i signal.root -b bkg.root')
    parser.add_option('-b',dest='backgroundInput',help='Name of the background template file.')
    parser.add_option('-s',dest='signalInput',help='Name of the signal template file.')
    parser.add_option('-o',dest='outputFile',help='Name of the output file.')
    parser.add_option('-n',dest='nBins',default='600',help='Number of bins to use in smoothed template')

    parser.add_option('--sigTreeName',dest='sigTreeName',default='SignalTree',help='Name of the TTree in the signal file.')
    parser.add_option('--bkgTreeName',dest='bkgTreeName',default='BackgroundTree',help='Name of the TTree in the background file.')
    parser.add_option('--useFakeMCBkg',dest='useFakeMCBkg',action="store_true",default=False,help='Require "isFake".')
    parser.add_option('--weight',dest='weight',help='use MC weight')

    parser.add_option('--showerShapeName',dest='showerShapeName',default='phosihih',help='Name of the leaf in the input data file.')
    parser.add_option('--ptName',dest='ptName',default='phoCorEt',help='Name of the leaf in the input data file.')
    parser.add_option('--etaName',dest='etaName',default='phoSCEta',help='Name of the leaf in the input data file.')
    parser.add_option('--isoName',dest='isoName',default='phoCHIso',help='Name of the leaf in the input data file.')
    parser.add_option('--momName',dest='momName',default='phohasFakemom',help='Name of the leaf in the input data file.')

    (options,args) = parser.parse_args()

    miss_options = False

    if options.backgroundInput is None:
        print 'Need to specify -b'
        miss_options=True
    if options.signalInput is None:
        print 'Need to specify -s'
        miss_options=True
    if options.outputFile is None:
        print 'Need to specify -o'
        miss_options=True
    
    options.nBins = int(options.nBins)
        
    if miss_options:
        exit(1)

    main(options,args)
