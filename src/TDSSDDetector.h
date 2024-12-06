#ifndef DSSD_DETECTOR_H_INCLUDED
#define DSSD_DETECTOR_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>

#include <TVector3.h>
#include <TMath.h>
class TDSSDDetector{

 public:
  TDSSDDetector(){

  }
  ~TDSSDDetector(){;}
 public:
  void SetName(std::string n) {name=n;};
  void SetRing(int r) {ring =r;}
  void SetPosition(float Theta, float Phi,float Mag=82)  {
    float theta_rad = Theta*TMath::DegToRad();
    float phi_rad = Phi*TMath::DegToRad();
    theta_deg = Theta;
    phi_deg = Phi;
    position.SetMagThetaPhi(Mag,theta_rad,phi_rad);
  }
  void AddCalibrationFactors(float cal){
    calibr_factors.push_back(cal);
  }
  void SetGoodEventThr(float thr_low, float thr_hig) {GoodEventThr = std::make_pair(thr_low,thr_hig);}

 public:
  double   GetThetaRad() const {return position.Theta();}
  double   GetThetaDeg() const {return position.Theta()*TMath::RadToDeg();}
  TVector3 GetPosition() const {return position;};
  void get_theta_phi_deg(double * t, double *f)
  {
      *t = theta_deg;
      *f = phi_deg;
  }
  int      GetRing()     const {return ring;}
  std::vector<float> GetCalibrFactors() const {return calibr_factors;};
  std::string GetName() {return name;}
  std::pair<float,float> GetGoodEvent() {return GoodEventThr;}

  double phi_deg;
  double theta_deg;
 private:
  int ring;
  std::string name;
  std::pair<float,float> GoodEventThr;
  TVector3 position;
  std::vector<float> calibr_factors;
};

#endif
