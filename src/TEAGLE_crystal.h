#ifndef _GERM_crystal_
#define _GERM_crystal_
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

class TEAGLE_array;
class Ttarget_galileo;


/////////////////////////////////////////////////////////////////////////////
class TEAGLE_crystal : public Tincrementer_donnor
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
    
    static TEAGLE_array  *array_nw_pointer;

    static Ttarget_galileo *target_ptr;   // for faster access of the doppler corr.



    bool flag_fired;
    bool flag_good_data;
    bool flag_true_germanium = true; // not an anticompton shield
#ifdef ADDBACK_NEEDED
    bool flag_good_nonaddbacked_crystal; // fired good, and additionally was not used for addback
#endif


    double doppler_factor;

    /** If it was possible to make good doppler correction (Cate & MW was ok) */
    bool flag_doppler_successful;

protected:
    TEAGLE_array *parent_array;
    std::string name_of_this_element;
    int id_number; //
    int cluster_nr;  // we need for fan spectra incrementation
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
    double energy_cal_acs_approved ;
    double time_raw   ;
    double time_cal  ;
    double cfd_raw ;

    double compton_energy_raw ;
    double compton_time_raw;
    bool flag_acs_approved;

    unsigned long long time_stamp;

    TF1 *time_extrapol;

    double energy_doppler_corrected;

    double DopplerCorrectNoAncillary(double,double,double);


    double xv1, yv1, zv1, length_v1; // vector of product (scattered_particle)
    double xv2, yv2, zv2, length_v2; // vector of gamma

    double theta_radians_between_gamma_vs_scattered_particle;
    double phi_radians_between_gamma_vs_scattered_particle;
//    double doppler_corrected_energy;


    spectrum_1D *spec_energy_raw;
    spectrum_1D *spec_energy_cal;


    spectrum_1D *spec_time;
    spectrum_1D *spec_cfd;

    spectrum_1D *spec_energy_cal_acs_approved;
    spectrum_1D *spec_energy_cal_acs_rejected;




    spectrum_2D *spec_energy_cal_vs_time;
#if NEED_DOPPLER_SPECTRA
    spectrum_1D *spec_energy_doppler;
#endif
    spectrum_1D *spec_energy_CS;

    // Total
    static  spectrum_2D *spec_total_energy_cal_vs_time ;
    static  spectrum_1D *spec_total_time;
    static  spectrum_1D *spec_total_energy_cal;
    static  spectrum_1D *spec_total_energy_cal_acs_approved;

    //------------------




    // Total spectra are common to this class
#if NEED_DOPPLER_SPECTRA
    static spectrum_1D    *spec_total_energy_doppler;
#endif

    spectrum_1D *spec_fan;

    int pseudo_fan; // Piotr wants to create fan spectrum gated by some condition, so he needs
    // an inrementor similar to this what increments Fan specrum. The difference between fan
    // spectrum is, that fan spectrum for current crystal increments three channels np.
    // for ex. 60, 62, 64  - corresponding to presence of  E4, E20, FT
    // This incrementer will contain only one "channel" (60)


public:
    TEAGLE_crystal(std::string nam, TEAGLE_array *parent_ptr, int nr);
    virtual ~TEAGLE_crystal() {};
    void set_event_ptr_for_crystal_data(int ptr[3]);
    void analyse_current_event();
    double Get_time_extrapol (double);
    //void doppler_corrector_basic();
    int give_nr_fan_germ () { return nr_fan_germ; }

    int give_id_number() { return id_number; }
#ifdef DOPPLER_CORRECTION_NEEDED
    void calculate_chosen_doppler_algorithm();
    double give_energy_doppler_corrected() const {
        return energy_doppler_corrected;
    }

    /** For perfect condition  (Tracing OK, Cate OK) */
    /** Based on the simplest algorithm (from Hans-Juergen) */

    void doppler_corrector_basic();
    /** Options are read from the disk by TRising class (during preloop)
   and then are stored in TEAGLE_crystal static values */
    static void set_doppler_corr_options
    (int typ_track, int typ_cate_pos, int typ_doppl_algor);

    // for beta playfield
    std::vector<double> betaN_doppler_factor;

#endif
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

    double give_ge_time() const {
        return time_cal;
    }

    double give_gp_phi() const {  // gamma vs particle  angle (phi)
        return phi_radians_between_gamma_vs_scattered_particle;
    }

    double give_gp_theta() const { // gamma vs particle  angle (theta)
        return theta_radians_between_gamma_vs_scattered_particle;
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
    void doppler_corrector_TVector3();;
    double doppler_needle(double energy_ge, double theta_ge, double phi_ge, double theta_dssd, double phi_dssd);



#endif // def DOPPLER_CORRECTION_NEEDED

#ifdef ADDBACK_NEEDED
    void set_flag_participant_of_addback() {
        flag_participant_of_addback = true;
        flag_good_nonaddbacked_crystal = false;
    }
#endif // #ifdef ADDBACK_NEEDED

    /** No descriptions */
protected:
    void create_my_spectra();
    void check_legality_of_data_pointers();

    // option for doppler correction
    static type_of_tracking_requirements    tracking_position_on_target;
    static type_of_cate_requirements        tracking_position_on_cate;
    static type_of_doppler_algorithm        which_doppler_algorithm;
    static bool flag_beta_from_frs;     // which beta to use, frs or fixed
    //  static double beta_fixed_value;     // if fixed, here it is
    static double energy_deposit_in_target;// bec. beta on the other side
    // of the target will be slower
#ifdef ADDBACK_NEEDED
    bool flag_participant_of_addback;  // if this crystal was used for making addback
#endif

public:
    double GetgammaEDC() {return energy_doppler_corrected;}

    static double Eex ;   // [MeV] excitation energy of target nucleus;
    static double Ebeam; //[MeV] beam E;
    static double massP;  // MeV/c2   Mass of projectile in u*931.5 [MeV/c2];
    static double massT;  // MeV/c2  Mass of target  in u*931.5 [MeV/c2];

};




extern double GE_Eraw_max_range  ;
extern double GE_Ecal_max_range  ;
extern double GE_Time_max_range  ;



#endif
