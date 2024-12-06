#ifndef GALILEO_DETECTOR_H_INCLUDED
#define GALILEO_DETECTOR_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>

#include <TVector3.h>
#include <TMath.h>
class TGalileoDetector{

 public:
  TGalileoDetector(){

  }
  ~TGalileoDetector(){;}
 public:
  void SetName(std::string n) {name=n;};
  void SetPosition(float Theta, float Phi,float Mag=220)  {
    float theta_rad = Theta*TMath::DegToRad();
    float phi_rad = Phi*TMath::DegToRad();
    position.SetMagThetaPhi(Mag,theta_rad,phi_rad);
  }
  void AddCalibrationFactors(float cal){
    calibr_factors.push_back(cal);
  }
  void SetComptonSuppThr(float thres) {ComptonSuppresionThr=thres;}

 public:
  double GetThetaRad() {return position.Theta();}
  double GetThetaDeg() {return position.Theta()*TMath::RadToDeg();}
  TVector3 GetPosition() {return position;};
  std::vector<float> GetCalibrFactors() {return calibr_factors;};
  std::string GetName() {return name;}

 private:
  std::string name;
  float ComptonSuppresionThr;
  TVector3 position;
  std::vector<float> calibr_factors;
};

#endif
