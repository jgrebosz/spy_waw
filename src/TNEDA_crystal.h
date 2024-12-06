#ifndef _NEDA_CRYSTAL_
#define _NEDA_CRYSTAL_
#include "randomizer.h"
#include "spectrum.h"
#include <string>
#include "Tfile_helper.h"
#include "Tincrementer_donnor.h"

#include <TVector3.h>
#include <TMath.h>
#include <TRandom.h>
#include "doppler_enums.h"
#include "DataFrames.h"

#include "TNEDADetector.h"

class TNEDA_array;

/////////////////////////////////////////////////////////////////////////////
class TNEDA_crystal : public Tincrementer_donnor
{
public:
  
  // options set by parrent array
  static bool flag_increment_energy_cal_with_zero;
  static bool flag_increment_time_with_zero;
  
  static bool flag_good_energy_requires_threshold;
  static double energy_threshold_lower;
  static double energy_threshold_upper;
  
  static bool flag_good_time_requires_threshold;
  static double good_time_threshold_lower; // this is the new style
  static double good_time_threshold_upper; // this is the new style
  
  static bool flag_good_wAmpli_vs_eAmpli_polygon_needed;
  bool flag_polygon_loaded;
  static std::string good_polygon_name;
  
  TjurekPolyCond * good_polygon ;
  
  static TNEDA_array  *array_nw_pointer;
  
  
  
  bool flag_fired;
  bool flag_good_data;
  
  void set_xyz(TVector3 v){
      v_xyz = v;
      crystal_position_x = v.x();
      crystal_position_y = v.y();
      crystal_position_z = v.z();
  }
  
protected:
  TNEDA_array *parent_array;
  std::string name_of_this_element;
  int key; // one of 7 numbers
  int cluster_nr;  // we need for fan spectra incrementation
  
  std::vector<float> energy_calibr_factors;
  
  
  
  //===== position of the crystal ===============
  double phi_geom, theta_geom;
  double  phi_geom_radians,  theta_geom_radians;
  double distance_from_target;          // from the target
  double crystal_position_x, crystal_position_y, crystal_position_z;
  


  TVector3 v_xyz;   // geometric position
  //=============================================
  
  // now we do this with set of data
  double   qshort  ;
  double   qlong   ;
  double   qfast   ;
  double   qslow   ;
  double   light_raw   ;
  double   cfd     ;
  double   intrise ;
  uint16_t nbzero  ;
  uint16_t flags   ;
  uint16_t det_name_nr  ;
  double   psdcaen ;
  double   psdlf   ;

  double   psdcaen_shifted , position_psdcaen;  // after user defined shift
  double   psdlf_shifted  , position_psdlf ;
  double light_cal;


  double  REQUESTED_PSDCAEN_POSITION;
  double  REQUESTED_PSDLF_POSITION;

  
  unsigned long long time_stamp;
  double  rtime ; // real time
  double  aligned_real_time ; // after alignment to Neda nr 0
  double  alignment_correcting_value;

  double xv1, yv1, zv1, length_v1; // vector of product (scattered_particle)
  double xv2, yv2, zv2, length_v2; // vector of gamma
  
  double theta_radians_between_gamma_vs_scattered_particle;
  double phi_radians_between_gamma_vs_scattered_particle;
  
  spectrum_1D *spec_qshort;
  spectrum_1D *spec_qlong;
  spectrum_1D *spec_qfast;
  spectrum_1D *spec_qslow;
  spectrum_1D *spec_light_raw;
  spectrum_1D *spec_light_cal;

  spectrum_1D *spec_psd_caen;
  spectrum_1D *spec_psd_lf;

  spectrum_1D *spec_psd_caen_shifted;
  spectrum_1D *spec_psd_lf_shifted;
  
  
  spectrum_2D *spec_psd_caen_vs_qlong;
//  spectrum_2D *spec_psd_lf_vs_light_cal  ;

  // spectrum_2D *spec_psd_caen_shifted_vs_light_cal;
//  spectrum_2D *spec_psd_lf_shifted_vs_light_cal;

  static spectrum_2D *spec_total_psd_caen_shifted_vs_light_raw ;
  static spectrum_2D *spec_total_psd_lf_shifted_vs_light_raw ;
//  static spectrum_2D *spec_total_psd_caen_shifted_vs_light_cal ;
//  static spectrum_2D *spec_total_psd_lf_shifted_vs_light_cal ;
  
  // Total
  static  spectrum_2D *spec_total_energy_cal_vs_time ;
  static  spectrum_1D *spec_total_time;
  static  spectrum_1D *spec_total_energy_cal;
  
  //------------------  
  spectrum_1D *spec_fan;
  
  int pseudo_fan; // Piotr wants to create fan spectrum gated by some condition, so he needs
  // an inrementor similar to this what increments Fan specrum. The difference between fan
  // spectrum is, that fan spectrum for current crystal increments three channels np.
  // for ex. 60, 62, 64  - corresponding to presence of  E4, E20, FT
  // This incrementer will contain only one "channel" (60)
  
  
public:
  TNEDA_crystal(std::string nam, TNEDA_array *parent_ptr, int key);
  virtual ~TNEDA_crystal() {};
  void set_event_ptr_for_crystal_data(int /*ptr[3]*/);
  void analyse_current_event();
  
  TVector3 detector_position;
  void make_preloop_action(TNEDADetector*);     // read the calibr. factors, gates
  TVector3 give_position() const{
    return detector_position;
  }
  double give_phi()   const {
    return detector_position.Phi();
  }
  double give_theta() const {
    return detector_position.Theta();
  }
  double give_x() const {
    return detector_position.X();
  }
  double give_y() const {
    return detector_position.Y();
  }
  double give_z() const {
    return detector_position.Z();
  }
  bool give_flag_this_is_true_neda() { return flag_this_is_true_neda; }
  
  // for coincidence matrices
  double give_qshort ()  const {return qshort  ; }
  double give_qlong  ()  const {return qlong   ; }
  double give_qfast  ()  const {return qfast   ; }
  double give_qslow  ()  const {return qslow   ; }
  double give_light_raw  ()  const {return light_raw   ; }
  double give_light_cal  ()  const {return light_cal   ; }
  double give_cfd    ()  const {return cfd     ; }
  double give_intrise()  const {return intrise ; }
  double give_psdcaen()  const {return psdcaen ; }
  double give_psdlf  ()  const {return psdlf   ; }
  
  int    give_nbzero ()  const {return nbzero  ; }
  int    give_flags  ()  const {return flags   ; }
  uint16_t give_det_name_nr   ()  const {return det_name_nr  ; }
  auto give_det_key   ()  const {return key  ; }

  double give_real_time  ()  const {return rtime  ; }

  bool give_flag_fired()  const {return flag_fired   ; }
  std::string give_name() { return  name_of_this_element;}

  void step2_make_rtime(double time_ref);

  /** this function contains the algorithm of the doppler correction
   mentioned on the web page of Rising */
  
  
  /** No descriptions */
  void zeroing_good_flags();
  /** No descriptions */
  bool check_selfgate(Tself_gate_abstract_descr *);
  /** No descriptions */
  void increment_crystal_spectra_cal_and_doppler();
  /** No descriptions */
  void increment_crystal_spectra_raw_and_fan();
  
  
  /** No descriptions */
protected:
  void create_my_spectra();
  void check_legality_of_data_pointers();
  
  // option for doppler correction
  static type_of_tracking_requirements    tracking_position_on_target;
  static type_of_cate_requirements        tracking_position_on_cate;

  static bool flag_beta_from_frs;     // which beta to use, frs or fixed
  //  static double beta_fixed_value;     // if fixed, here it is
  static double energy_deposit_in_target;// bec. beta on the other side
  // of the target will be slower
  bool read_shift_values();
  bool flag_this_is_true_neda = true; // sometimes we send Triggers in the channels of Nedda

  void align_neda_times_to_neda_nr_0();
  void process_neda();


public:
 };


extern double GE_Eraw_max_range  ;
extern double GE_Ecal_max_range  ;
extern double GE_Time_max_range  ;

#endif
