# a function that creates a very simple sum of raw templates
import ROOT
from ROOT import RooNDKeysPdf, RooDataSet, RooWorkspace, \
     RooPlot, RooHistPdf, RooAddPdf, RooRealVar, RooArgSet, RooFit, \
     TFile, TTree, RooArgList, RooMinuit, RooThresholdCategory

from optparse import OptionParser

def makePdf(ws,bin,postfix,args):
	
	#create the PDFs we are going to fit!
	ws.factory('RooHistPdf::sigTemplatePDF'+postfix+'({Pho_SigmaIEtaIEta},'+
		   'binSigSmoothedDataHist'+postfix+',2)')
	ws.factory('RooHistPdf::bkgTemplatePDF'+postfix+'({Pho_SigmaIEtaIEta},'+
		   'binBkgSmoothedDataHist'+postfix+',2)')               
	ws.factory('SUM::fullPDF'+postfix+'('+
		   'fBkg'+postfix+'[0.5,1e-8,1]*bkgTemplatePDF'+postfix+
		   ', sigTemplatePDF'+postfix+')')
        
