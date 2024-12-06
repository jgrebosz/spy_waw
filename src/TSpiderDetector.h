#ifndef SPIDER_DETECTOR_H_INCLUDED
#define SPIDER_DETECTOR_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>

#include <TVector3.h>
#include <TMath.h>
class TSpiderDetector{

 public:
  TSpiderDetector(){

  }
  ~TSpiderDetector(){;}
 public:
  void SetName(std::string n) {name=n;};
  void SetRing(int r) {ring =r;}
  void SetPosition(float Theta, float Phi,float Mag=82)  {
    float theta_rad = Theta*TMath::DegToRad();
    float phi_rad = Phi*TMath::DegToRad();
    position.SetMagThetaPhi(Mag,theta_rad,phi_rad);
  }
  void AddCalibrationFactors(float cal){
    calibr_factors.push_back(cal);
  }
  void SetGoodEventThr(float thr_low, float thr_hig) {GoodEventThr = std::make_pair(thr_low,thr_hig);}

 public:
  double GetThetaRad() {return position.Theta();}
  double GetThetaDeg() {return position.Theta()*TMath::RadToDeg();}
  TVector3 GetPosition() {return position;};
  int GetRing() {return ring;}
  std::vector<float> GetCalibrFactors() {return calibr_factors;};
  std::string GetName() {return name;}
  std::pair<float,float> GetGoodEvent() {return GoodEventThr;}

 private:
  int ring;
  std::string name;
  std::pair<float,float> GoodEventThr;
  TVector3 position;
  std::vector<float> calibr_factors;
};

#endif
