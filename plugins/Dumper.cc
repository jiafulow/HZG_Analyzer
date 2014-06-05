#include "../interface/Dumper.h"

Dumper::Dumper(const Parameters& params, const Cuts& cuts, const ParticleSelector& psel):
  _parameters(params),
  _cuts(cuts),
  _psel(psel)
{
  InitDumps();
}

void Dumper::SetRho(float rho){
  _rhoFactor = rho;
}

void Dumper::SetRun(int runNumber){
  _runNumber = runNumber;
}

void Dumper::SetEvent(int eventNumber){
  _eventNumber = eventNumber;
}

void Dumper::SetPv(const TVector3& pv){
  _pv = pv;
}

void Dumper::SetLumi(int lumiSection){
  _lumiSection = lumiSection;
}

void Dumper::InitDumps(){
  muonDump = false;
  electronDump = false;
  if (_parameters.dumps){
    if (_parameters.selection == "mumuGamma") muonDump = true;
    if (_parameters.selection == "eeGamma") electronDump= true;
    
    string subtag;
    if (_parameters.doPhoMVA){
      subtag = "PhoMVA";
    }else{
      subtag = "Proper";
    }

    char buffer[512];
    sprintf(buffer, "dumps/%s_%s_%s_lepDump1.txt",_parameters.selection.c_str(),_parameters.suffix.c_str(),subtag.c_str());
    lepDump1.open(buffer);
    sprintf(buffer, "dumps/%s_%s_%s_lepDump2.txt",_parameters.selection.c_str(),_parameters.suffix.c_str(),subtag.c_str());
    lepDump2.open(buffer);
    sprintf(buffer, "dumps/%s_%s_%s_phoDump1.txt",_parameters.selection.c_str(),_parameters.suffix.c_str(),subtag.c_str());
    phoDump1.open(buffer);
    sprintf(buffer, "dumps/%s_%s_%s_phoDump2.txt",_parameters.selection.c_str(),_parameters.suffix.c_str(),subtag.c_str());
    phoDump2.open(buffer);
    sprintf(buffer, "dumps/%s_%s_%s_finalDump.txt",_parameters.selection.c_str(),_parameters.suffix.c_str(),subtag.c_str());
    finalDump.open(buffer);
  }
}


void Dumper::ElectronDump(const TCElectron& el, const TClonesArray& recoMuons, int dnum)
{
  if(!electronDump) return;

  ofstream* dump;
  if (dnum == 1){
    dump = &lepDump1;
  } else if (dnum == 2){
    dump = &lepDump2;
  }else{
    dump = &finalDump;
  }

  *dump << "runNumber: " <<  _runNumber << " eventNumber: " << _eventNumber << " ELECTRON: " << el
       << endl;
}

void Dumper::MuonDump(const TCMuon& mu, int dnum) 
{
  if(!muonDump) return;

  ofstream* dump;
  if (dnum == 1){
    dump = &lepDump1;
  } else if (dnum == 2){
    dump = &lepDump2;
  }else{
    dump = &finalDump;
  }

  *dump << "runNumber: " <<  _runNumber << " eventNumber: " << _eventNumber << " MUON: " << mu
       << endl;
}

void Dumper::PhotonDump(const TCPhoton& ph, int dnum) 
{
  if(!_parameters.dumps) return;
  float chEA,nhEA,phEA,tmpEta;
  tmpEta = ph.SCEta();


  if (fabs(tmpEta) < 1.0){
    chEA = _cuts.EAPho[0][0];
    nhEA = _cuts.EAPho[0][1];
    phEA = _cuts.EAPho[0][2];
  }else if (fabs(tmpEta) < 1.479){
    chEA = _cuts.EAPho[1][0];
    nhEA = _cuts.EAPho[1][1];
    phEA = _cuts.EAPho[1][2];
  }else if (fabs(tmpEta) < 2.0){
    chEA = _cuts.EAPho[2][0];
    nhEA = _cuts.EAPho[2][1];
    phEA = _cuts.EAPho[2][2];
  }else if (fabs(tmpEta) < 2.2){
    chEA = _cuts.EAPho[3][0];
    nhEA = _cuts.EAPho[3][1];
    phEA = _cuts.EAPho[3][2];
  }else if (fabs(tmpEta) < 2.3){ 
    chEA = _cuts.EAPho[4][0];
    nhEA = _cuts.EAPho[4][1];
    phEA = _cuts.EAPho[4][2];
  }else if (fabs(tmpEta) < 2.4){
    chEA = _cuts.EAPho[5][0];
    nhEA = _cuts.EAPho[5][1];
    phEA = _cuts.EAPho[5][2];
  }else{                                  
    chEA = _cuts.EAPho[6][0];
    nhEA = _cuts.EAPho[6][1];
    phEA = _cuts.EAPho[6][2];
  }

  ofstream* dump;
  if (dnum == 1){
    dump = &phoDump1;
  } else if (dnum == 2){
    dump = &phoDump2;
  }else{
    dump = &finalDump;
  }

  *dump << "runNumber: " <<  _runNumber << " eventNumber: " << _eventNumber << " PHOTON: " << ph
       << endl;

}


void Dumper::CloseDumps(){
  if(_parameters.dumps){
    lepDump1.close();
    lepDump2.close();
    phoDump1.close();
    phoDump2.close();
    finalDump.close();
  }
}
