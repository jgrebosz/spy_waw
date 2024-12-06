#ifndef EUCLIDES_DETECTOR_H_INCLUDED
#define EUCLIDES_DETECTOR_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <TVector3.h>
#include <TMath.h>
class TEuclidesDetector{

 public:
  TEuclidesDetector(){

  }
  ~TEuclidesDetector(){;}
 public:
  void SetName(std::string n) {name=n;};
  void SetPosition(float Theta, float Phi, float Mag=60)  {
    float theta_rad = Theta*TMath::DegToRad();
    float phi_rad = Phi*TMath::DegToRad();
    position.SetMagThetaPhi(Mag,theta_rad,phi_rad);
  }
  void AddCalibrationFactors(int subdomain, float cal){
    calibr_factors[subdomain].push_back(cal);
  }
  void SetAlphaDEThres(float thr_low,float thr_up) {
    AlphaDEThres=std::make_pair(thr_low,thr_up);
  }

 public:
  double GetThetaRad() {return position.Theta();}
  double GetThetaDeg() {return position.Theta()*TMath::RadToDeg();}
  TVector3 GetPosition() {return position;};
  std::vector<float> GetCalibrFactor(int subdomain) {return calibr_factors[subdomain];};
  std::pair<float,float> GetAlphaThres(){return AlphaDEThres;}
  std::string GetName() {return name;}


 private:
  std::string name;
  std::pair<float,float> AlphaDEThres;
  TVector3 position;
  std::map<int,std::vector<float> > calibr_factors;
};

#endif
