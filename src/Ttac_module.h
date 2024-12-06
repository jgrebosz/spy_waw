#ifndef _TAC_MODULE_
#define _TAC_MODULE_
#include "randomizer.h"
#include "spectrum.h"
#include <string>
#include "Tfile_helper.h"
#include "Tincrementer_donnor.h"

#include <TVector3.h>
#include <TMath.h>

#include "DataFrames.h"
#include "TTACDetector.h"

class TneutronWall_array;
class Ttarget;

/////////////////////////////////////////////////////////////////////////////
class Ttac_module : public Tincrementer_donnor
{
public:

    // options set by parrent array
    static bool flag_increment_zco_cal_with_zero;
    static bool flag_increment_qvc_cal_with_zero;
    static bool flag_increment_tof_cal_with_zero;

    static bool flag_good_zco_requires_threshold;
    static double zco_threshold;
    static double zco_threshold_upper;

    static bool flag_good_qvc_requires_threshold;
    static double qvc_threshold;
    static double qvc_threshold_upper;

    static bool flag_good_tof_requires_threshold;
    static double good_tof_threshold_lower; // this is the new style
    static double good_tof_threshold_upper; // this is the new style

    static TneutronWall_array  *array_nw_pointer;

    static int multiplicity_raw;
    static int multiplicity_good;
    static Ttarget *target_ptr;   // for faster access of the doppler corr.

    bool flag_fired;
    bool flag_good_data;


   //double doppler_factor;

    /** If it was possible to make good doppler correction (Cate & MW was ok) */
   // bool flag_doppler_successful;

protected:
    TneutronWall_array *parent_array;
    std::string name_of_this_element;
    int id_number; // one of 7 numbers
    int cluster_nr;  // we need for fan spectra incrementation

    int tac_raw;
    double tac_cal;
    std::vector<float> calibr_factors;

    //TAC SPECTRA
    spectrum_1D *spec_tac_raw ;
    spectrum_1D *spec_tac_cal  ;

    int pseudo_fan;

public:
    Ttac_module(std::string nam, TneutronWall_array *parent_ptr);
    virtual ~Ttac_module() {};
    void set_event_ptr_for_crystal_data(int ptr[3]);
    void analyse_current_event(int nr);


    TVector3 detector_position;
    void make_preloop_action(TTACDetector*);     // read the calibr. factors, gates
    std::string give_name() const {
        return name_of_this_element;
    }


    /** No descriptions */
    void zeroing_good_flags();
    /** No descriptions */
    bool check_selfgate(Tself_gate_abstract_descr *desc);
    /** No descriptions */
    void increment_crystal_spectra_cal();
    /** No descriptions */
    void increment_crystal_spectra_raw();
    /** No descriptions */
    double give_tac_cal()
    {return tac_cal;}

    /** No descriptions */
protected:
    void create_my_spectra();
    void check_legality_of_data_pointers();

    // option for doppler correction

//    static bool flag_beta_from_frs;     // which beta to use, frs or fixed
//    //  static double beta_fixed_value;     // if fixed, here it is
//    static double energy_deposit_in_target;// bec. beta on the other side
//    // of the target will be slower

public:
};

#endif
