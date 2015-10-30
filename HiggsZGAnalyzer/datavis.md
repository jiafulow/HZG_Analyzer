# Skim nuTuples_v9.10_8TeV

  root -l -b -q 'run2.C("DYJetsToLL_M-50 DYJetsToLL_M-50_1 1")' >& skims_datavis/skim_DYJetsToLL_M-50_1.log &
  root -l -b -q 'run2.C("DYJetsToLL_M-50 DYJetsToLL_M-50_2 2")' >& skims_datavis/skim_DYJetsToLL_M-50_2.log &
  root -l -b -q 'run2.C("DYJetsToLL_M-50 DYJetsToLL_M-50_3 3")' >& skims_datavis/skim_DYJetsToLL_M-50_3.log &
  root -l -b -q 'run2.C("DYJetsToLL_M-50 DYJetsToLL_M-50_4 4")' >& skims_datavis/skim_DYJetsToLL_M-50_4.log &

  root -l -b -q 'run2.C("ZZ ZZ_1 1")' >& skims_datavis/skim_ZZ_1.log &
  root -l -b -q 'run2.C("ZZ ZZ_2 2")' >& skims_datavis/skim_ZZ_2.log &
  root -l -b -q 'run2.C("ZZ ZZ_3 3")' >& skims_datavis/skim_ZZ_3.log &
  root -l -b -q 'run2.C("ZZ ZZ_4 4")' >& skims_datavis/skim_ZZ_4.log &

  root -l -b -q 'run2.C("TTJets_FullLep TTJets_FullLep_1 1")'   >& skims_datavis/skim_TTJets_FullLep_1.log &
  root -l -b -q 'run2.C("TTJets_FullLep TTJets_FullLep_2 2")'   >& skims_datavis/skim_TTJets_FullLep_2.log &
  root -l -b -q 'run2.C("TTJets_Hadronic TTJets_Hadronic_1 1")' >& skims_datavis/skim_TTJets_Hadronic_1.log &
  root -l -b -q 'run2.C("TTJets_Hadronic TTJets_Hadronic_2 2")' >& skims_datavis/skim_TTJets_Hadronic_2.log &
  root -l -b -q 'run2.C("TTJets_SemiLep TTJets_SemiLep_1 1")'   >& skims_datavis/skim_TTJets_SemiLep_1.log &
  root -l -b -q 'run2.C("TTJets_SemiLep TTJets_SemiLep_2 2")'   >& skims_datavis/skim_TTJets_SemiLep_2.log &

# Make flat ntuples

  ./execBatch.sh DY_local skims_datavis_DYJetsToLL_M-50 DYJetsToLL_M-50 ABCD mumuGamma 2012 S10 amumuAnalyzer4
  ./execBatch.sh TT_local skims_datavis_TTJets_FullLep TTJets_FullLep_M-50 ABCD mumuGamma 2012 S10 amumuAnalyzer4
  ./execBatch.sh ZZ_local skims_datavis_ZZ ZZ ABCD mumuGamma 2012 S10 amumuAnalyzer4
