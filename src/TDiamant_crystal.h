#ifndef _Diamant_crystal_
#define _Diamant_crystal_
#include "randomizer.h"
#include "spectrum.h"
#include <string>
#include "Tfile_helper.h"
#include "Tincrementer_donnor.h"

#include <TVector3.h>
#include <TMath.h>
#include "TF1.h"
#include <TRandom.h>
#include "doppler_enums.h"
#include "DataFrames.h"

#include "TEAGLEDetector.h"

class TDiamant_array;
class Ttarget_galileo;


/////////////////////////////////////////////////////////////////////////////
class TDiamant_crystal : public Tincrementer_donnor
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
    
    static TDiamant_array  *parent_pointer;

    static Ttarget_galileo *target_ptr;   // for faster access of the doppler corr.



    bool flag_fired;
    bool flag_good_data;




protected:
    // TDiamant_array *parent_array;
    std::string name_of_this_element;
    int id_number; //
    // int cluster_nr;  // we need for fan TDiamant_arraytra incrementation
    int nr_fan_germ;
    std::vector<float> energy_calibr_factors;



    //===== position of the crystal ===============
    double phi_geom, theta_geom;
    double  phi_geom_radians,  theta_geom_radians;
    double distance_from_target;          // from the target
    double crystal_position_x, crystal_position_y, crystal_position_z;



    //=============================================

    // now we do this with set of data
    subData_t d;

    double energy_raw ;
    double energy_cal ;
    double top   ;
    double pid;

    unsigned long long time_stamp;



    spectrum_1D *spec_energy_raw;
    //spectrum_1D *spec_energy_cal;

    spectrum_1D *spec_pid;
    spectrum_1D *spec_top;





    spectrum_2D *spec_pid_vs_energy;



//    // Total
//    static  spectrum_2D *spec_total_energy_cal_vs_pid ;
//    static  spectrum_1D *spec_total_top;
//    static  spectrum_1D *spec_total_energy_cal;
//    static  spectrum_1D *spec_total_energy_cal_acs_approved;

    //------------------




    // Total spectra are common to this class


    static spectrum_1D *spec_fan;

    int pseudo_fan; // Piotr wants to create fan spectrum gated by some condition, so he needs
    // an inrementor similar to this what increments Fan specrum. The difference between fan
    // spectrum is, that fan spectrum for current crystal increments three channels np.
    // for ex. 60, 62, 64  - corresponding to presence of  E4, E20, FT
    // This incrementer will contain only one "channel" (60)


public:
    TDiamant_crystal(std::string nam, int id);
    virtual ~TDiamant_crystal() {};
    void set_event_ptr_for_crystal_data(int ptr[3]);
    void analyse_current_event();
    void  make_preloop_action();
    double Get_time_extrapol (double);
    void doppler_corrector_basic();
    int give_nr_fan_germ () { return nr_fan_germ; }

    int give_id_number() { return id_number; }

    TVector3 detector_position;
    void make_preloop_action(TEAGLEDetector*);     // read the calibr. factors, gates
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

    // for coincidence matrices
    double give_energy_raw() const {
        return energy_raw;
    }
    double give_energy_calibrated() const {
        return energy_cal;
    }
    unsigned long long give_timestamp() {
        return time_stamp;
    }






    /** this function contains the algorithm of the doppler correction
   mentioned on the web page of Rising */


    /** No descriptions */
    void zeroing_good_flags();
    /** No descriptions */
    bool check_selfgate(Tself_gate_abstract_descr *desc);
    /** No descriptions */
    void increment_crystal_spectra_cal_and_doppler();
    /** No descriptions */
    void increment_crystal_spectra_raw_and_fan();
    /** No descriptions */
    void increment_crystal_spectra_cal_and_doppler_when_mult_1();
    void increment_spect_total_addbacked_cal_and_doppler();

#ifdef DOPPLER_CORRECTION_NEEDED

    /** this function contains the algorithm of the doppler correction
   using the class TVector3 from ROOT library */
    void doppler_corrector_TVector3();


#endif // def DOPPLER_CORRECTION_NEEDED


    /** No descriptions */
protected:
    void create_my_spectra();
    void check_legality_of_data_pointers();



public:

};





#endif
