#ifndef _GERMANIUM_ARRAY_CLASS_H_
#define _GERMANIUM_ARRAY_CLASS_H_


#include "experiment_def.h"
#include "spectrum.h"
#include "TEAGLEDetector.h"
#include "TEAGLE_crystal.h"
#include "TNeedle.h"
#include <vector>

#include "TGalileoEvent.h"
#include "TGalileoCalibratedEvent.h"

#include "Tincrementer_donnor.h"


///////////////////////////////////////////////////////////////////////////////
class TEAGLE_array :  public TNeedle_element, public Tincrementer_donnor
{
  friend class TEAGLE_crystal ;
  TNeedle    *galileo_pointer ;  // pointer to the frs object

  
  // all pointers to spectra for rising crystals and clusters
  // are registered also here - to make easier saving them
  // from time to time
  std::vector<spectrum_abstr *>   &spectra_vector; // pointer to vector of spectra in galileo class
  
  //-------------------------------
  // elements of the array
  std::map<int,TEAGLE_crystal*> detector_eagle ;
  static std::map<int,TEAGLEDetector*> lookup;
  //#ifdef NIGDY
  
  


  spectrum_1D *spec_tsdiff_ge_ge;
//  spectrum_1D *spec_timediff_ge_ge;
//  spectrum_1D *spec_timediff_ge_ge_corr;
  spectrum_1D *spec_crystals_multiplicity_raw ;
  spectrum_1D *spec_crystals_multiplicity_good ;
  //spectrum_2D *spec_energy_theta_degree;

//  spectrum_2D *spec_energy_id;
  spectrum_1D *spec_fan ; // fan spectrum for all crystals
  spectrum_2D *spec_geometry_monitor;
  static constexpr int HOW_MANY_SECONDS_TO_RESET =120;
  double picture_width;

  std::map<int,spectrum_1D*> spec_gamma_ring;

  spectrum_1D *spec_timediff_sg;

#if NEED_DOPPLER_SPECTRA
  spectrum_1D *spec_total_energy_doppler;
  //  spectrum_2D *spec_gamma_gamma_doppler;
//  std::map<int,spectrum_1D*> spec_gamma_ringDC;
  spectrum_2D *spec_energyDC_theta_degree;
  spectrum_2D *spec_gammaDC_vs_angle;
//  spectrum_1D *spec_gamma_coincDC;
#endif
//  spectrum_1D *spec_gamma_al_1n;
//  spectrum_1D *spec_gamma_al_1p;
//  spectrum_1D *spec_gamma_al_1a;
  //  spectrum_1D *spec_gamma_al_1d;
//  spectrum_1D *spec_gamma_coinc;

//  spectrum_2D *spec_gamma_gamma_al_1n;
//  spectrum_2D *spec_gamma_gamma_al_1p;
//  spectrum_2D *spec_gamma_gamma_al_1a;
  //  spectrum_2D *spec_gamma_gamma_al_1d;
//  spectrum_2D *spec_gammaDC_vs_partmult;
  std::vector<double> storeEDC;
  std::vector<double> storeE;
  std::vector<TVector3> storePos;
  std::vector<unsigned long long > storeTS;
  


public:
  static int multiplicity_raw;
  static int multiplicity_good;
  TEAGLE_array(std::string name);
  virtual ~TEAGLE_array() {
    // loop for destroying crystals?
    
  };
  
  static TGalileoCalibratedEvent *tmp_event_calibrated;
  static TGalileoEvent *tmp_event_unpacked;
  
  //   static const std::string cluster_characters; // Letters assigned to particular clusters
  
  
  //------------------------------
  virtual void make_preloop_action(std::ifstream &) ; // read the calibration factors, gates
  
  // taking the calibration factors - is called form preloop function
  void read_calibration_from_lookuptable();
  std::map<int,double> give_energyDC(){
    return gamma_energy_DC;
  }
  std::map<int,double> give_energy(){
    return gamma_energy;
  }
  
  void analyse_current_event();
  
  //void make_user_event_action(); // shop of events ?
  void make_postloop_action();  // saving the spectra ?
  
  //     void increm_spec_total_energy_calibrated(double ene_cal)
  //     {
  //       spec_total_energy_cal->increment(ene_cal)  ;
  //     }
  
  //     void increm_spec_total_time_calibrated(double tim_cal)
  //     {
  //       spec_total_time_cal->increment(tim_cal ) ;
  //     }
  
  
  
  //     TNeedle *frs_ptr() {
  //         return galileo_pointer ;   // for doppler corr. we need contact with target
  //     }
  
  double  give_target_x() {
    return target_x ;
  }
  double  give_target_y() {
    return target_y ;
  }
  /** No descriptions */
  bool save_selected_spectrum(std::string name);
  /** Depending on the status of hardware we can use different algorithms */

void read_doppler_corr_options();

  /** Before analyzing anything in Hector and Rising */
  //  bool first_check_cluster_multiplicity();
  /** No descriptions */
  //     int give_synchro_data() ;;
  /** No descriptions */
  void read_good_crystal_specification();
  /** No descriptions */
  void monitoring_statistics();
  int highest_ge_nr =0;
  void set_highest_ge_nr (int nr)
  {
      if(nr > highest_ge_nr) highest_ge_nr = nr;
  }
  //------------------------------
protected:
  double target_x ;   // we keep it here for doppler correction
  /**  */
  std::map<int,double> gamma_energy_DC;
  std::map<int,double> gamma_energy;

  double tsdiff;
  double timediff;
  double timediff_corr;
  
  double target_y ;
  /**  */
  int ger_synchro_data;  // time calibrator from the vxi crate nr 1
  int ger2_synchro_data; // time calibrator from the vxi crate nr 2
  
  //for testing time
  
  void simulate_event();
  void create_my_spectra() ;
  /** Rerading the definiton of the conditinal (gated) spectra */
  void read_def_conditional_spectra();
  /** Create one conditional spectrum */
  void create_cond_spectrum(std::string name);
  /** where the user spectra can be incremented */
  void user_spectra_loop();
  /** This function reads the options defined in the dialog window of the
   grebware viewer. The result is given to the static members of
   the Tcluster */

  time_t last_time_of_erasing_monitor_spectra = 0;

  // std::vector <unsigned long long> wektor_timestampow;
};
/////////////////////////////////////////////////////////////////////


#endif // _GERMANIUM_ARRAY_CLASS_H_
