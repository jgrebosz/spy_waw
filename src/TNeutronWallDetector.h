#ifndef NW_DETECTOR_H_INCLUDED
#define NW_DETECTOR_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>

#include <TVector3.h>
#include <TMath.h>

class TNeutronWallDetector{
  
 public:
  TNeutronWallDetector(){
    name="";
    signals.clear();
    calibr_factors.clear();
  }
  ~TNeutronWallDetector(){
    name="";
    signals.clear();
    calibr_factors.clear();}
 public:
  void SetName(std::string n) {name=n;};
  void SetChannel(std::string sig, int ch) {signals[ch]=sig;};
  void SetPosition(float theta, float phi){
    position.SetMagThetaPhi(1,
			    theta*TMath::DegToRad(),
			    phi*TMath::DegToRad());
  }
  void AddCalibrationFactors(std::string sig, float cal){
    if(sig=="tof"){
      calibr_factors[0].push_back(cal);
    }
    else if(sig=="zco"){
      calibr_factors[1].push_back(cal);
    }
    else if(sig=="qvc"){
      calibr_factors[2].push_back(cal);
    }
    else{
      std::cerr << "Don't know what to do so I do nothing" << std::endl;
    }
  }
  
 public:
  std::string GetName() {return name;}
  std::map<int, std::vector<float> > GetCalibrFactor() {return calibr_factors;}
  std::string GetSignalType(int ch) {return signals[ch];}
  float GetThetaRad() {return position.Theta();}
  float GetThetaDeg() {return position.Theta()*TMath::RadToDeg();}
  float GetPhiRad() {return position.Phi();}
  float GetPhiDeg() {return position.Phi()*TMath::RadToDeg();}
  TVector3 GetPosition(){return position;}

  int FindChannel(int ch) {
    if(signals.find(ch)!=signals.end()){
      if(signals[ch]=="tof") return 0;
      else if(signals[ch]=="zco") return 1;
      else if(signals[ch]=="qvc") return 2;
      else return -10;
    }
    else return -10;
  }
  
 private:
  std::string name;
  TVector3 position;
  std::map<int,std::string> signals  ;
  std::map<int,std::vector<float> > calibr_factors;
};

#endif
