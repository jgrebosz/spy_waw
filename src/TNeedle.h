#ifndef _Needle_CLASS_H_
#define _Needle_CLASS_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TGalileoEvent class
#ifndef  __CINT__

//#include <boost/tokenizer.hpp>
//#include <tokenizer.hpp>

#include <vector>
#include <map>
#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TFile.h"

#include "TNeedle_element.h"
#include "TNEDADetector.h"
#include "TEAGLEDetector.h"
#include "TDSSDDetector.h"
#include "Ttarget_galileo.h"

#include "TGalileoEvent.h"
#include "TGalileoCalibratedEvent.h"
//#include "Go4ConditionsBase/TGo4PolyCond.h"
//#include "Go4Analysis/TGo4Analysis.h"  // for registering conditions
#include "Tnamed_pointer.h"

class TEAGLE_array;
class TNEDA_array;
class TDiamant_array;
class TDSSD_ring;


class TGalileoAnalysis;
///////////////////////////////////////////////////////////////////////////////
class TNeedle : public Tincrementer_donnor
{
  
  // all spectra for frs calculations are here
  std::vector<spectrum_abstr*> spectra_gal ;
  std::vector<TNeedle_element *> module;  // only pointers to devices defined below
  
  //-------------------------------
  int dead_time ;  // will be calculated by the scalers
  
  TGalileoCalibratedEvent* event_calibrated ;
  
public:
  TNeedle(TGalileoAnalysis *ptr);
  ~TNeedle();
  
  
  // filling the vector with the proper configuration
  // or perhaps we can make it in preloop - when we know all the options?
  
  std::vector<spectrum_abstr*> *  address_spectra_frs() { return &spectra_gal ; }
  

  //------------------------------
  void make_preloop_action(TGalileoEvent* event_unpacked ,
                           TGalileoCalibratedEvent* event_calibrated_ptr);
  
  // taking the calibration factors
  void read_calibration_and_gates_from_disk();
  // above function is called form preloop function
  //std::map<int,bool> list_of_si_telescope() {return euclides_fired;}
  //std::map<int,bool> list_of_germanium() {return eagle_fired;}
  //std::map<int,bool> list_of_neutronwall() {return neutronwall_fired;}
  //std::map<int,bool> list_of_tac() {return tac_fired;}
  void analyse_current_event();
  void make_user_event_action(); // shop of events ?
  void make_postloop_action();  // saving the spectra ?


  
  // Setting up the array
  virtual bool SetupNEDA(bool);
  virtual bool SetupEAGLE(bool);
  virtual bool SetupDSSD(bool);
  //Look Up Table

  void SetLookUpTable(); // Reading of the lookup table

  
  bool IsNEDAPresent()  const {return neda_present;}
  bool IsEAGLEPresent() const {return eagle_present;}
  bool IsDSSDPresent() const {return dssd_present;}
  
  std::map<int,std::pair<int,std::string> > const& GetLookUpTable() {
    return LookUpTable;
  }
  
  std::map<int,TNEDADetector* > const& Get_NEDA_LookUpTable() {
    return NEDA_LUT;
  }
  
  std::map<int,TEAGLEDetector* > const& Get_EAGLE_LookUpTable() {
    return EAGLE_LUT;
  }
  std::map<int,TDSSDDetector* > const& Get_DSSD_LookUpTable() {
    return DSSD_LUT;
  }
  
  // Experiment Parameters (Beam, Target, Beam energy)
  void SetExperiment();
  virtual void SetBeam(int z,int a)    {beam = std::make_pair(z,a);}
  virtual void SetTarget(int z, int a) {target = std::make_pair(z,a);}
  virtual void SetBeamEnergy(double e) {beam_energy = e;}
  virtual void SetRecoilVelocity(double b) {average_recoil_velocity = b;}
  std::pair<int,int> GetBeam()       const {return beam;}
  double             GetBeamEnergy() const {return beam_energy;}
  std::pair<int,int> GetTarget()     const {return target;}
  double             GetBetaRecoil() const {return average_recoil_velocity;}
  
  TNeedle_element* address_of_device(std::string name);
  /**   For doppler correction algorithm */
  double give_beta_tof();
  
  /** doppler correcting objects (crystals) will use its data */
  Ttarget * give_target_rising_address() { return target_rising_ptr ; }
  Ttarget_galileo * give_target_address() { return target_ptr ; }

  




  TEAGLE_array * give_eagle_pointer()  { return eagle_pointer; }
  TDSSD_ring * give_dssd_pointer()  { return dssd_pointer; }

  TNEDA_array  * give_neda_pointer()   { return neda_pointer;      }
  TDiamant_array  * give_diamant_pointer()   { return diamant_pointer;      }
  
  /** this function decides how often make autosave of spectra */
  /** No descriptions */
  // bool in_gate_pos_xy_on_mw42();
  void perhaps_autosave();
  /** To refresh more often spectra which are currently on the screen of the grebware viewer */
  bool save_selected_spectrum(std::string name);
  /** CATE will make everything only when some rising multiplicity */
  void remember_rising_cluster_multiplicity(bool ok);
  /** No descriptions */
  int give_synchro_data();
  /** No descriptions */
  void frs_watch_dog_report();
  /** No descriptions */
  void put_data_to_ntuple_tree();
  /** To have the proper ranges of the spectra with zet or aoq  */
  void read_interesting_Eraw_Ecal_Traw_Tcal_range();
  //  bool is_clusters_multiplicity_acceptable() { return  clusters_multiplicity_acceptable; }
  
  //   spectrum_1D *spec_sum_e_41LRUD;
  //------------------------------
protected:
  
  void create_your_spectra();
  void create_groups_of_spectra();
  /** No descriptions */
  void put_to_statistics(int x, int i);
  int statistics_frs[30];   // selected signals of frs
  
  void prepare_scalers_for_active_stopper();
  
protected: // Protected attributes
  /** doppler correcting devices will ask for some data */
  Ttarget * target_rising_ptr;
  
  Ttarget_galileo * target_ptr;
  
private:
  
  // Experimental setting configuration
  // Call when the spy start to set the experiment parameters
  void SetupExperiment();
  virtual bool Read_NEDA_LookUpTable();
  virtual bool Read_EAGLE_LookUpTable();
  virtual bool Read_DSSD_LookUpTable();
  // Now we create for each array a LookUpTable so that we
  // have more flexibility for the user to setup their
  // own experiment
  std::map<int,TNEDADetector*>  NEDA_LUT;
  std::map<int,TEAGLEDetector*> EAGLE_LUT;
  std::map<int,TDSSDDetector*> DSSD_LUT;
  
  bool eagle_present;
  bool neda_present;
  bool diamant_present;
  bool dssd_present;


  TEAGLE_array * eagle_pointer = nullptr;
  TNEDA_array *  neda_pointer = nullptr;
 
  TDiamant_array *  diamant_pointer = nullptr;
  TDSSD_ring * dssd_pointer = nullptr;

  std::map<int,std::pair<int,std::string> > LookUpTable ;

  double average_recoil_velocity;
  std::pair<unsigned int, unsigned int> target;
  std::pair<unsigned int, unsigned int> beam  ;
  double beam_energy;
  int number_of_germanium;
  int number_of_euclides;
  //int number_of_neutronwall;
protected:
  long long timestamp_SPILL_beg;
  long long timestamp_FRS ;
  int timestamp_FRS_subevent_in_miliseconds;
  
  long long timestamp_of_previous_FRS_subevent ;
  int frs_time_since_beginning_of_spill ;
  int frs_time_since_previous_subevent;
  
  
};

#endif // __CINT__

#endif // _Needle_CLASS_H_
