#ifndef _NEDA_ARRAY_CLASS_H_
#define _NEDA_ARRAY_CLASS_H_


#include "experiment_def.h"
#include "spectrum.h"
#include "TNEDADetector.h"
#include "TNEDA_crystal.h"
#include "TNeedle.h"
#include <vector>

#include "TGalileoEvent.h"
#include "TGalileoCalibratedEvent.h"

#include "Tincrementer_donnor.h"
struct dublet
{
    double timest;
    ulong  key;
    dublet(double e, int i) : timest(e), key(i) {}
};

///////////////////////////////////////////////////////////////////////////////
class TNEDA_array :  public TNeedle_element, public Tincrementer_donnor
{
    friend class TNEDA_crystal ;
    TNeedle    *galileo_pointer ;  // pointer to the frs object


    // all pointers to spectra for rising crystals and clusters
    // are registered also here - to make easier saving them
    // from time to time
    std::vector<spectrum_abstr *>   &spectra_vector; // pointer to vector of spectra in galileo class

    //-------------------------------
    // elements of the array
    std::map<int,TNEDA_crystal*> detector_neda ;
    static std::map<int,TNEDADetector*> lookup;

    //-------------
    // std::map<double, int>,TNEDA_crystal*> sorted_neda;


    spectrum_1D *spec_multiplicity_raw;
    spectrum_1D *spec_multiplicity_of_good;
    spectrum_2D *spec_light_cal_id;
    spectrum_2D *spec_qshort_id;
    spectrum_2D *spec_qlong_id;
    spectrum_2D *spec_qfast_id;
    spectrum_2D *spec_qslow_id;
    spectrum_2D *spec_psdcaen_vs_id;
    spectrum_2D *spec_psdlf_vs_id;
    spectrum_1D *spec_hitpattern ; // fan spectrum for all crystals
    spectrum_1D *spec_total_psd_lf_shifted;
    spectrum_1D *spec_total_psd_caen_shifted;

    spectrum_1D *spec_rtime  ;
    spectrum_1D *spec_psdlf  ;

    spectrum_2D *spec_hNe_dt_vs_dr;

    spectrum_2D *spec_PSDcaen_vs_id;
    spectrum_2D *spec_PSDlf_vs_light_raw  ;
    spectrum_2D *spec_PSDlf_vs_light_cal  ;

    spectrum_2D *spec_rtime_vs_psdlf  ;
    spectrum_2D *spec_geometry_monitor;
    static constexpr int HOW_MANY_SECONDS_TO_RESET =120;
    double picture_width;

    // files for shift of psd value
    std::ifstream file_psd_lf;
    std::ifstream file_psd_caen;
    std::ifstream file_time_alignment;

    std::string filename_psd_lf;
    std::string filename_psd_caen;
    std::string filename_neda_time_alignment;


public:
    static int multiplicity_raw;
    static int multiplicity_good;
    TNEDA_array(std::string name);
    virtual ~TNEDA_array() {
        // loop for destroying crystals?

    };

    static TGalileoCalibratedEvent *tmp_event_calibrated;
    static TGalileoEvent *tmp_event_unpacked;

    TjurekPolyCond * neutron_cclf_cut_polygon_ptr = nullptr;
    TjurekPolyCond * neutron_second_t2_cut_polygon_ptr = nullptr;
    TjurekPolyCond * triangel_polygon_ptr = nullptr;

    bool reading_neutron_polygon(std::string name_a, TjurekPolyCond **polygon, std::vector<double> vec);

    int nr_of_clean_neutrons;


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
    void read_neda_xyz();

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

    double dr, dt ;
    bool dr_valid, dt_valid;
    /**  */
    int ger_synchro_data;  // time calibrator from the vxi crate nr 1
    int ger2_synchro_data; // time calibrator from the vxi crate nr 2
    std::vector<TVector3> neda_xyz;

    //for testing time

    void simulate_event();
    void create_my_spectra() ;
    /** Rerading the definiton of the conditinal (gated) spectra */
    void read_def_conditional_spectra();
    /** Create one conditional spectrum */
    void create_cond_spectrum(std::string name);
    /** where the user spectra can be incremented */
    void user_spectra_loop();
    time_t last_time_of_erasing_monitor_spectra = 0;

    void process_neda();

    // std::vector<std::pair<double, TNEDA_crystal*> > times_of_crystals;
    std::vector<dublet> times_of_crystals;
};
/////////////////////////////////////////////////////////////////////
struct NEUTRON_DATA
{
    uint id;
    uint i_list;   // position on the NEDA data list
    uint ban_result;
    bool         pur;
    bool         pll;
    double        rtime;    // time vs time ref of the event;
    double        qshort;
    double        qlong;
    double        qfast;
    double        qslow;
    double        light;
    double        cfd;
    double        intrise;
    unsigned short nbzero;
    double        cclf;
    double        ccfpga;
};

#endif // _NEDA_ARRAY_CLASS_H_
