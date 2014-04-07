#!/usr/bin/env python
import sys,os
#sys.argv.append('-b')
from ROOT import *


# Hi Mayda! Here is a simple analyzer for running over lite root trees and playing with 4-vectors
# You can use this script to make histograms and investigate the kinematics of the final state particles
# There is no detector information saved currently

#gROOT.SetBatch()

# Grab the file of interest
myFile = TFile('../HiggsZGAnalyzer/liteFiles/liteFile_MuMu2012ABCD_04-6-14_llg.root')

# Grab the tree in this file (there could be more than one, but in this file there is only one tree)
myTree = myFile.Get('zgTree_DATA')

# Define an empty histogram.  TH1F means a 1D histogram for floats.  The syntax is:
# TH1F(name, title;x;y, nBins, lowBin, highBin)

threeBodyMass = TH1F('threeBodyMass', 'Three-Body Mass;Mass (GeV);Entries', 75, 0, 150)
llMass = TH1F('llMass', 'Dilepton Mass;Mass (GeV);Entries', 1500, 0, 150)

# Now we will loop over each event, and grab the three objects I stored in the tree.
# These objects are all TLorentzVectors, so they have all the position, momentum, and energy information stored
# For reference: http://root.cern.ch/root/html/TLorentzVector.html

print 'Filling histograms ...'
for event in myTree:
  muonPos = event.muonPos
  muonNeg = event.muonNeg
  photon  = event.photon

  # adding two TLorentzVectors follows standard vector addition
  dilepton = muonPos + muonNeg
  threeBody = muonPos + muonNeg + photon

  #print 'photon pt:', photon.Pt()
  #print 'pos muon eta:', muonPos.Eta()
  #print 'dilepton mass', dilepton.M()

  # Fill our histogram after each event.
  llMass.Fill(dilepton.M())

  if abs(dilepton.M()-3.1) < 0.1:
    #Fill the three body mass if the dilepton event comes from an on-shell j/psi
    threeBodyMass.Fill(threeBody.M())

# We build a canvas so we can save our results
canvas = TCanvas('can','canvas',800,600)
canvas.cd()

# Lets look at what we made!
llMass.Draw()
raw_input("Hit 'Enter' to continue")

# Save as a pdf
canvas.SaveAs('dileptonMass.pdf')
# Save as a root file if we want to edit the plot later
#canvas.SaveAs('dileptonMass.root')

# Look at the same plot, zoomed in
llMass.GetXaxis().SetRangeUser(0,15)
llMass.Draw()
canvas.Update()
raw_input("Hit 'Enter' to continue")

threeBodyMass.Draw()
canvas.Update()
raw_input("Hit 'Enter' to continue")
