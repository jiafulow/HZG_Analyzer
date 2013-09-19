Dumper::InitDumps(){
  muonDump = false;
  electronDump = false;
  if (dumps){
    if (selection == "mumuGamma") muonDump = true;
    if (selection == "eeGamma") electronDump= true;
  }

  if (electronDump){
    elDump2.open("dumps/electronDump2_SUFFIX.txt");
    if (!elDump2.good()) cout << "ERROR: can't open file for writing." << endl;
    elDump2<<"run"             <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"             <<" "<<"dEtaIn"      <<" "<<"dPhiIn"
      <<" "<<"sigmaIetaIeta"   <<" "<<"HoE"         <<" "<<"ooemoop"
      <<" "<<"conPass"         <<" "<<"mHits"       <<" "<<"d0"
      <<" "<<"dz"              <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"           <<" "<<"combIso"     <<" "<<"rho"
      <<" "<<"EA"              <<" "<<"passID"      <<" "<<"passIso"
      <<endl<<endl;
    phDump1.open("dumps/photonDump1El_SUFFIX.txt");
    if (!phDump1.good()) cout << "ERROR: can't open file for writing." << endl;
    phDump2.open("dumps/photonDump2El_SUFFIX.txt");
    if (!phDump2.good()) cout << "ERROR: can't open file for writing." << endl;

    elDumpFinal.open("dumps/electronDumpFinal_SUFFIX.txt");
    if (!elDumpFinal.good()) cout << "ERROR: can't open file for writing." << endl;
    elDumpFinal<<"run"             <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"             <<" "<<"dEtaIn"      <<" "<<"dPhiIn"
      <<" "<<"sigmaIetaIeta"   <<" "<<"HoE"         <<" "<<"ooemoop"
      <<" "<<"conPass"         <<" "<<"mHits"       <<" "<<"d0"
      <<" "<<"dz"              <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"           <<" "<<"combIso"     <<" "<<"rho"
      <<" "<<"EA"              <<" "<<"passID"      <<" "<<"passIso"
      <<endl<<endl;

    if(doEleMVA){
      elDumpMVA.open("dumps/electronDumpMVA_SUFFIX.txt");
      if (!elDumpMVA.good()) cout << "ERROR: can't open file for writing." << endl;
    }
  }
  if (muonDump){
    muDump1.open("dumps/muonDump1_SUFFIX.txt");
    if (!muDump1.good()) cout << "ERROR: can't open file for writing." << endl;
    muDump1<<"run"       <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"       <<" "<<"isGlobal"    <<" "<<"isPf"
      <<" "<<"rChi2"     <<" "<<"nHits"       <<" "<<"nMatch"
      <<" "<<"d0"        <<" "<<"dz"          <<" "<<"nPixel"
      <<" "<<"nLayer"    <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"     <<" "<<"combIso"     <<" "<<"dB"
      <<" "<<"passID"    <<" "<<"passIso"
      <<endl<<endl;
    phDump1.open("dumps/photonDump1Mu_SUFFIX.txt");
    if (!phDump1.good()) cout << "ERROR: can't open file for writing." << endl;
    phDump2.open("dumps/photonDump2Mu_SUFFIX.txt");
    if (!phDump2.good()) cout << "ERROR: can't open file for writing." << endl;

    muDumpFinal.open("dumps/muonDumpFinal_SUFFIX.txt");
    if (!muDumpFinal.good()) cout << "ERROR: can't open file for writing." << endl;
    muDumpFinal<<"run"       <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"       <<" "<<"isGlobal"    <<" "<<"isPf"
      <<" "<<"rChi2"     <<" "<<"nHits"       <<" "<<"nMatch"
      <<" "<<"d0"        <<" "<<"dz"          <<" "<<"nPixel"
      <<" "<<"nLayer"    <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"     <<" "<<"combIso"     <<" "<<"dB"
      <<" "<<"passID"    <<" "<<"passIso"
      <<endl<<endl;
  }

  if (dumps){
    phDump1<<"run"             <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"              <<" "<<"CSElVeto"    <<" "<<"HoE"
      <<" "<<"sigmaIetaIeta"    <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"            <<" "<<"rho"         <<" "<<"chEA"
      <<" "<<"nhEA"             <<" "<<"phEA"        <<" "<<"passID"
      <<" "<<"passIso"
      <<endl<<endl;

    phDump2<<"run"             <<" "<<"event"       <<" "<<"pt"
      <<" "<<"eta"              <<" "<<"CSElVeto"    <<" "<<"HoE"
      <<" "<<"sigmaIetaIeta"    <<" "<<"chIso"       <<" "<<"nhIso"
      <<" "<<"phIso"            <<" "<<"rho"         <<" "<<"chEA"
      <<" "<<"nhEA"             <<" "<<"phEA"        <<" "<<"passID"
      <<" "<<"passIso"          <<" "<<"dr1"         <<" "<<"dr2"
      <<" "<<"pt/Mllg"
      <<endl<<endl;

    finalDump.open("dumps/finalDump_SELECTION_PERIOD_SUFFIX.txt");
  }

  if (dataDumps && suffix == "DATA"){
    dataDump.open("dataDump_DATANAME_COUNT.txt");
    if (!dataDump.good()) cout << "ERROR: can't open file for writing." << endl;
  }
