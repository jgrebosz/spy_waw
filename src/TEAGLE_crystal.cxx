#include "TEAGLE_crystal.h"
//#include "Tcluster_xia.h"

#include <sstream>
#include <iomanip>

#include <algorithm>
#include "TEAGLE_array.h"
#include "Ttarget.h"
#include "TNeedle.h"
#include "Tnamed_pointer.h"
#include "Tself_gate_ge_galileo_descr.h"
#include "TGalileoAnalysis.h" // for verbose mode
#include "TDSSD_ring.h"

using namespace std;

TEAGLE_array    *TEAGLE_crystal::array_nw_pointer;  // static pointer
Ttarget_galileo *TEAGLE_crystal::target_ptr;       // for faster acces of the doppler correction



// enums declared inside the TEAGLE_crystal class
type_of_tracking_requirements   TEAGLE_crystal::tracking_position_on_target;
type_of_cate_requirements       TEAGLE_crystal::tracking_position_on_cate;
type_of_doppler_algorithm       TEAGLE_crystal::which_doppler_algorithm;

bool   TEAGLE_crystal::flag_beta_from_frs; // which beta to use, frs or fixed
double TEAGLE_crystal::energy_deposit_in_target; // bec. beta on the other side
// of the target will be slower

bool TEAGLE_crystal::flag_increment_energy_cal_with_zero;
bool TEAGLE_crystal::flag_increment_time_with_zero;

bool   TEAGLE_crystal::flag_good_energy_requires_threshold;
double TEAGLE_crystal::energy_threshold_lower;
double TEAGLE_crystal::energy_threshold_upper;

bool   TEAGLE_crystal::flag_good_time_requires_threshold;
double TEAGLE_crystal::good_time_threshold_lower; // this is the new style
double TEAGLE_crystal::good_time_threshold_upper; // this is the new style

#if NEED_DOPPLER_SPECTRA
spectrum_1D *TEAGLE_crystal::spec_total_energy_doppler;
#endif

spectrum_1D *TEAGLE_crystal::spec_total_time;
spectrum_1D *TEAGLE_crystal::spec_total_energy_cal;
spectrum_1D *TEAGLE_crystal::spec_total_energy_cal_acs_approved;

spectrum_2D *TEAGLE_crystal::spec_total_energy_cal_vs_time;

// static
double TEAGLE_crystal::Eex ;   // [MeV] excitation energy of target nucleus;
double TEAGLE_crystal::Ebeam;  // [MeV] beam E;
double TEAGLE_crystal::massP;  // MeV/c2   Mass of projectile in u*931.5 [MeV/c2];
double TEAGLE_crystal::massT;

#define dcout   if(1) std::cout

//**************************************************************************
TEAGLE_crystal::TEAGLE_crystal(std::string nam, TEAGLE_array *parent_ptr, int nr): parent_array(parent_ptr)
{
    //  std::cout << "constructor of crystal"
    //        << ", addres of parent is "
    //        << parent_cluster
    //        << ", my address is " << this
    //        << std::endl;

    my_selfgate_type = Tselfgate_type::galileo_ge;   // <---- important!

    id_number = nr;  // note: this starts form 1,and goes: 2,3,4,5,6,7
    cluster_nr = 0 ; // used only for fan spec and for TGalileo Calibrated event

    name_of_this_element = nam;

    if(nam.find("acs") == string::npos){
        // ZWYKLY GERMAN
        //        cout << "To nie jest ACS" << endl;
        flag_true_germanium = true;
        nr_fan_germ = stoi(nam.substr(3,2));
        parent_array->set_highest_ge_nr (nr_fan_germ);

    }else
    {
        // Anti Compton Shield ASC
        nr_fan_germ  = stoi(nam.substr(6,2)) + 20 ;
        flag_true_germanium = false;

    }

    // to be able to increment total fan spectrum we must know which
    // cluster it is

    //   cluster_nr = parent_cluster->give_cluster_id();

        std::cout << "This crystal has numbers "
                  << cluster_nr << ", "
                  << id_number
                  << " nr_germ = " << nr_fan_germ
                  << std::endl;

    //  energy_data = 0;
    //  time_data = 0;
    //  threshold_to_multiplicity = 100; // 100 keV



#ifdef DOPPLER_CORRECTION_NEEDED
    doppler_factor = 1;
#endif // def DOPPLER_CORRECTION_NEEDED

    create_my_spectra();

    named_pointer[name_of_this_element + "_energy_raw_when_fired"]
            = Tnamed_pointer(&energy_raw, &flag_fired, this);


    named_pointer[name_of_this_element + "_energy_cal_when_fired"]
            = Tnamed_pointer(&energy_cal, &flag_fired, this);

    named_pointer[name_of_this_element + "_energy_cal_acs_approved"]
            = Tnamed_pointer(&energy_cal_acs_approved, &flag_acs_approved, this);

    //    named_pointer[name_of_this_element + "_energy_cal_acs_approved"]
    //            = Tnamed_pointer(&energy_cal_acs_rej, &flag_acs_approved, this);

    named_pointer[name_of_this_element + "_time_when_fired"]
            = Tnamed_pointer(&cfd_raw, &flag_fired, this);

    named_pointer[name_of_this_element + "_energy_cal_when_good"]
            = Tnamed_pointer(&energy_cal, &flag_good_data, this);

    named_pointer[name_of_this_element + "_time_when_good"]
            = Tnamed_pointer(&time_raw, &flag_good_data, this);

    named_pointer[name_of_this_element + "_cfd_when_goody"]
            = Tnamed_pointer(&cfd_raw, &flag_good_data, this);

    named_pointer[name_of_this_element + "_theta_radians_between_gamma_vs_scattered_particle"]
            = Tnamed_pointer(&theta_radians_between_gamma_vs_scattered_particle, &flag_good_data, this);

#if (CURRENT_EXPERIMENT_TYPE == GALILEO_PROTOTYPE)

#ifdef TIME_DIFF_USED
    named_pointer[name_of_this_element + "_time_diff_calibrated_when_good"]
            = Tnamed_pointer(&time_diff_calibrated, &flag_good_data, this);
#endif



    // ALL ===========================================
    {
        // local scope
        std::string entry_name;
        Tmap_of_named_pointers::iterator pos;


        Tnamed_pointer::add_as_ALL("ALL_Ge_energy_cal_acs_approved",
                                   &energy_cal_acs_approved,
                                   &flag_acs_approved, this);
        //        named_pointer[name_of_this_element + "_energy_cal_acs_approved"]
        //                = Tnamed_pointer(&energy_cal_acs_approved, &flag_acs_approved, this);

#ifdef TIME_DIFF_USED
        entry_name =  "ALL_cluster_xia_crys_time_diff_calibrated_when_good";
        Tnamed_pointer::add_as_ALL(entry_name, &time_diff_calibrated,
                                   &flag_good_data, this);
#endif

    } // end of local
#endif



#ifdef DOPPLER_CORRECTION_NEEDED
    named_pointer[name_of_this_element +
            "_energy_doppler_when_OK"]
            = Tnamed_pointer(&energy_doppler_corrected,
                             &flag_doppler_successful, this);


    // beta playfield
    //    for (int i = 0; i < 9; i++) {
    //        std::string bet("beta");
    //        bet += char('1' + i);
    //        named_pointer[name_of_this_element +
    //                "_energy_doppler_" + bet + "_when_successful"]
    //                = Tnamed_pointer(&energy_doppler_corrected_betaN[i],
    //                                 &flag_doppler_successful, this);
    //    }
#endif

    // TOTALS===================================


    // find in the map the entry with such a name -
    // if there is not - create it
    // if there is already - open it and add to it

    std::string entry_name;

    if(flag_true_germanium) {
        entry_name =  "ALL_Ge_crystal_energy_cal_when_good";
        Tnamed_pointer::add_as_ALL(entry_name, &energy_cal, &flag_good_data, this);

        entry_name = "ALL_Ge_crystal_energy_doppler_when_OK";
        Tnamed_pointer::add_as_ALL(entry_name, &energy_doppler_corrected, &flag_doppler_successful,this);

        //----------------------------
        entry_name =  "ALL_Ge_crystal_time_when_good";
        Tnamed_pointer::add_as_ALL(entry_name, &cfd_raw, &flag_good_data, this);


        entry_name =  "ALL_Ge_crystal_energy_doppler_acs_approved_when_good";
        Tnamed_pointer::add_as_ALL(entry_name, &  energy_cal_acs_approved,
                                   &flag_acs_approved, this);

        entry_name =  "ALL_Ge_crystal_energy_acs_approved_when_good";
        Tnamed_pointer::add_as_ALL(entry_name, &  energy_cal_acs_approved,
                                   &flag_acs_approved, this);

        //        entry_name =  "ALL_Ge_crystal_energy_acs_rejected_when_good";
        //        Tnamed_pointer::add_as_ALL(entry_name, &  energy_cal_acs_approved,
        //                                   &flag_acs_approved, this);
        //----------------------------
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
        entry_name =  "ALL_Ge_crystal_time_minus_GeOR_cal_when_good";
        Tnamed_pointer::add_as_ALL(entry_name, &time_minus_GeOR_cal,
                                   &flag_good_data, this);
#endif

        //------------------------------

        entry_name =  "ALL_Ge_crystal_energy_doppler_when_OK";
        Tnamed_pointer::add_as_ALL(entry_name, & energy_doppler_corrected,
                                   &flag_doppler_successful, this);

#ifdef DOPPLER_CORRECTION_NEEDED
        //    // Beta playfield
        //    for (int i = 0; i < 9; i++) {

        //        std::string bet("beta");
        //        bet += char('1' + i);
        //        entry_name =  "ALL_Ge_crystal_energy_doppler_" + bet +
        //                "_when_successful";

        //        Tmap_of_named_pointers::iterator pos;
        //        pos = named_pointer.find(entry_name);
        //        if (pos != named_pointer.end())
        //            pos->second.add_item(&energy_doppler_corrected_betaN[i],
        //                                 &flag_doppler_successful, this);
        //        else
        //            named_pointer[entry_name] =
        //                    Tnamed_pointer(&energy_doppler_corrected_betaN[i],
        //                                   &flag_doppler_successful, this);

        //    }

#endif // Doppler

        //  pseudo FAN for Piotrek
        entry_name =  "ALL_Ge_crystal_pseudo_fan_when_good";
        Tnamed_pointer::add_as_ALL(entry_name, &pseudo_fan, &flag_good_data, this);

        entry_name = "ALL_theta_radians_between_gamma_vs_scattered_particle_when_good";
        Tnamed_pointer::add_as_ALL(entry_name, &theta_radians_between_gamma_vs_scattered_particle, &flag_good_data, this);
    }

}
//************************************************************************
void TEAGLE_crystal::create_my_spectra()
{
    //std::cout << "before creating the spectra " << std::endl;

    // the spectra have to live all the time, so here we can create
    // them during  the constructor

    std::vector<spectrum_abstr *> &gal_spec_ref = parent_array->spectra_vector;

    std::string folder = "crystals/" + name_of_this_element;

    // energy -------------------------------------
    std::string name = name_of_this_element;
    name += "_energy_raw";
    spec_energy_raw = new spectrum_1D(
                name,
                name,
                8192*4, 0, 8192*4,
                folder,
                "",  // note
                name_of_this_element + "_energy_raw_when_fired");
    gal_spec_ref.push_back(spec_energy_raw);

    //-----------------------



    //=============== doppler corrected spectra  =====
#if NEED_DOPPLER_SPECTRA
    name = name_of_this_element;

    name += "_doppler_corrected";
    spec_energy_doppler = new spectrum_1D(
                name,
                name,
                8192, 0, 8192,
                folder,
                "WQorks for DSSD Ring",  // note
                name_of_this_element + "_energy_doppler_when_OK");
    gal_spec_ref.push_back(spec_energy_doppler);
#endif





    //=============== calibrated spectra  =====
    // energy cal ------------------------------------
    name = name_of_this_element +"_energy_cal";

    gal_spec_ref.push_back(spec_energy_cal = new spectrum_1D(
                name,
                name,
                GE_Ecal_max_range, 0, GE_Ecal_max_range,
                folder,
                "",  // note
                name_of_this_element + "_energy_cal_when_fired")
            );


    // energy cal ACS approved ------------------------------------


    name = name_of_this_element +"_energy_cal_acs_approved";
    gal_spec_ref.push_back(spec_energy_cal_acs_approved =
            new spectrum_1D(
                name,
                name,
                GE_Ecal_max_range, 0, GE_Ecal_max_range,
                folder,
                "",  // note
                name_of_this_element + "_energy_cal_acs_approved")
            );

    if (!spec_total_energy_cal_acs_approved) {
        name = "Ge_total_energy_cal_acs_approved";
        gal_spec_ref.push_back(spec_total_energy_cal_acs_approved =
                new spectrum_1D(
                    name,
                    name,
                    GE_Ecal_max_range, 0, GE_Ecal_max_range,
                    folder,
                    "",  // note
                    "ALL_Ge_energy_cal_acs_approved")
                );
    }
    // energy cal ACS rejected------------------------------------

    name = name_of_this_element +"_energy_cal_acs_rejected";

    gal_spec_ref.push_back(spec_energy_cal_acs_rejected =
            new spectrum_1D(
                name,
                name,
                GE_Ecal_max_range, 0, GE_Ecal_max_range,
                folder,
                "",  // note
                "INFO: use a self gate ")
            );

    //---------------
    name = name_of_this_element + "_time";
    spec_time = new spectrum_1D(name,
                                name,
                                1024, 0, 100,
                                folder, "This is not a true time. It is just copy of cfd signal", // note
                                name_of_this_element + "_time_when_good");
    gal_spec_ref.push_back(spec_time);



    name = name_of_this_element + "_cfd";
    spec_cfd = new spectrum_1D(name,
                               name,
                               1024*16, 0, 1024*16,
                               folder, "", // note
                               name_of_this_element + "_cfd_when_good");
    gal_spec_ref.push_back(spec_cfd);



    name = name_of_this_element + "_energy_cal_vs_time";
    spec_energy_cal_vs_time = new spectrum_2D(
                name,
                name,
                512, 0, 128,
                GE_Ecal_max_range / 4, 0, GE_Ecal_max_range,
                folder,
                "",  // note
                "X:\r " + name_of_this_element + "_time_when_fired"
                + "\rY:\r " +  name_of_this_element + "_energy_cal_when_fired");


    gal_spec_ref.push_back(spec_energy_cal_vs_time);

    // static total spectra------------------------------------------------------
    if (!spec_total_energy_cal) {
        name = "Ge_total_energy_cal_good";
        spec_total_energy_cal = new spectrum_1D(
                    name,
                    name,
                    GE_Ecal_max_range, 0, GE_Ecal_max_range,
                    folder, "",
                    "ALL_Ge_crystal_energy_cal_when_good"
                    );
        gal_spec_ref.push_back(spec_total_energy_cal);
    }

    //------------------
    if (!spec_total_time) {

        name = "Ge_total_time";
        spec_total_time = new spectrum_1D(name,
                                          name,
                                          512,0,256,
                                          folder, "",
                                          "ALL_Ge_crystal_time_when_good"
                                          );
        gal_spec_ref.push_back(spec_total_time);
    }



    if (!spec_total_energy_cal_vs_time) {
        name = "Ge_total_energy_cal_vs_time";
        spec_total_energy_cal_vs_time = new spectrum_2D(
                    name,
                    name,
                    512, 0, GE_Time_max_range,
                    GE_Ecal_max_range / 4, 0, GE_Ecal_max_range,
                    folder, "",
                    "X:\r ALL_Ge_crystal_time_when_good\r" // no coma!
                    "Y:\r ALL_Ge_crystal_energy_cal_when_good\r"
                    " (when x and y detectors are the SAME)"
                    );
        gal_spec_ref.push_back(spec_total_energy_cal_vs_time);
    }



    //========================================

}
//**********************************************************************
void TEAGLE_crystal::analyse_current_event()
{
    //    std::cout << " analyse_current_event()  for "
    //                << name_of_this_element << ", id_number= "
    //                << id_number
    //                << std::endl;


    // checking if it is something (not zero) -- is made during unpacking

    zeroing_good_flags();


    if (TEAGLE_array::tmp_event_unpacked->eagle_fired.find(id_number) ==
            TEAGLE_array::tmp_event_unpacked->eagle_fired.end())
        return;
    //----------------
    // so it fired !
    if(flag_true_germanium) TEAGLE_array::multiplicity_raw++;


    energy_raw = TEAGLE_array::tmp_event_unpacked->eagle_data[id_number].energy;
    cfd_raw = TEAGLE_array::tmp_event_unpacked->eagle_data[id_number].cfd   ;

    time_raw = cfd_raw ; // temporary solution

    flag_fired = (energy_raw >= 0) || (cfd_raw >= 0);
    auto timestamp = TEAGLE_array::tmp_event_unpacked->eagle_data[id_number].timestamp  ;

    if(flag_true_germanium){
        compton_energy_raw = TEAGLE_array::tmp_event_unpacked->eagle_data[id_number+1].energy;
        compton_time_raw = TEAGLE_array::tmp_event_unpacked->eagle_data[id_number+1].cfd;
        if(compton_energy_raw > 1) flag_acs_approved = false;
        else flag_acs_approved = true;
    }

    if(flag_true_germanium)TEAGLE_array::multiplicity_good++;

    flag_good_data=true;
    if (GalileoAnalysis_ptr->is_verbose_on())
    {

        std::cout  << name_of_this_element
                   << ", id_number = " << id_number
                      //<< ", timestamp = " << timestamp
                   << ", Eraw =  " //<< std::setw(9)
                   << energy_raw
                      //<< ", Time (cfd) = " //<< std::setw(9)
                      //<< cfd_raw
                   << " Compton energy = " << compton_energy_raw
                   << std::endl; // New line will be after the LP, SR printout
    }

    //    if(parent_array->wektor_timestampow.)
    //    parent_array->wektor_timestampow.push_back(timestamp);

    /* Note: incrementing the raw spectra is done only when multiplicity in this cluster is 1.
   all this is done from Trising class, by calling
   Tcluster_xia::calculate_single_spectrum()
   The "missing" instructions which were here - are moved just there
   */

    //===================================================
    // energy calibration  - second time (sometimes the hardware calibarion is not good enough )
    //if (energy_cal == energy_raw)
    // in the even it can come already calibarted

    energy_cal = 0;
    double tmp_val = energy_raw+gRandom->Uniform(-.5,.5);
    for(UInt_t i=0; i < energy_calibr_factors.size();++i)
        energy_cal += energy_calibr_factors[i]*pow(tmp_val,(int)i);


    // ---------------- RINGS of Theta ------------------------------
    double theta_deg = (give_theta() *180 /3.14159);

    //  cout << "detector " << name_of_this_element << " ma theta = "
    //       << theta_deg
    //       << endl;

    if(theta_deg > 100 && theta_deg < 103)
    {
        //cout << "To jest Ring 101 " << endl;
        parent_array->spec_gamma_ring[2]->increment(energy_cal);
    }

    else if(theta_deg > 116 && theta_deg < 118)
    {
        //cout << "To jest Ring 117" << endl;
        parent_array->spec_gamma_ring[1]->increment(energy_cal);
    }

    else if(theta_deg > 142 && theta_deg < 144)
    {
        //cout << "To jest Ring 143 " << endl;
        parent_array->spec_gamma_ring[0]->increment(energy_cal);
    }

    //=================================================
    // time calibration not needed, it is made in hardware

    //###########################################################

    if (GalileoAnalysis_ptr->is_verbose_on()) {
        std::cout << std::endl;
    }


    //================================
    // GOOD
    //================================

    //    if (
    //            flag_good_wAmpli_vs_eAmpli_polygon_needed &&
    //            good_polygon->Test(energy_cal, Ampli_energy_cal) == true)
    //    {

    flag_good_data = true;
    //    if(
    if(flag_good_energy_requires_threshold)
        if( (energy_cal <  energy_threshold_lower) || (energy_cal >  energy_threshold_upper))
            flag_good_data = false;

    if(flag_good_time_requires_threshold)
        if(( (time_raw < good_time_threshold_lower) || (time_raw > good_time_threshold_upper)))
            flag_good_data = false;




#ifdef ADDBACK_NEEDED
    flag_good_nonaddbacked_crystal = true; // later can made false by the multiaddback routine
#endif



    //}
    // this value does not have to be zeroed,because anyway it is validated
    // by the flag good_data, so the value is always there, but the validator sometimes in
    // false



    increment_crystal_spectra_raw_and_fan();
    increment_crystal_spectra_cal_and_doppler();

    //  std::cout << " end of analyse_current_event()  for "
    //        << name_of_this_element
    //        << std::endl;

    // calculations_already_done = true ;

}
#ifdef DOPPLER_CORRECTION_NEEDED
//**************************************************************************
// This name of the fuction is not good. Should be:
//  chose_and_calculate_doppler()
//**************************************************************************

void  TEAGLE_crystal::calculate_chosen_doppler_algorithm()
{
    cout << "NEVER USED???" << endl;

    switch (which_doppler_algorithm) {
    case tracked_algorithm:
        if (target_ptr->is_doppler_possible()) doppler_corrector_TVector3();
        break;
    case basic_algorithm:
        doppler_corrector_basic();
        break;
    case needle_algorithm:

        break;
    default:
        std::cout
                << "Error, the type of doppler correction algorithm was not selected "
                   "in the file: option/doppler.options\n (the value is now = "
                << which_doppler_algorithm << ")" << std::endl;
        break;
    }
}
#endif
//**************************************************************************
void  TEAGLE_crystal::make_preloop_action(TEAGLEDetector *det)
{
    //    cout << __PRETTY_FUNCTION__ << endl;

    spec_fan = array_nw_pointer->spec_fan;

    energy_calibr_factors.clear();
    energy_calibr_factors=det->GetCalibrFactors();


    if(flag_true_germanium)
    {
//                cout << "==========================="
//                    << __PRETTY_FUNCTION__ << endl;
        // we read cal factors only for channels with true detector (not Anticompton Shield)

        // open calibration file eagle_current_calibr_factors.txt

        string pathed_file = path.dir_experiment_setup() +  "eagle_current_calibr_factors.txt";
        string linijka;

        ifstream cal_file(pathed_file);
        if(cal_file)
        {
            energy_calibr_factors.clear();

            while (true)
            {
                // search line starting with id number
                getline(cal_file, linijka);
                if(!cal_file) break;

                //            cout << "read line  linijka = '" << linijka << "'" << endl;
                istringstream s(linijka);
                int nr;

                //                 cout << " Przed czytaniem z tej linijki " << endl;

                s >> nr ;
                if(!s)
                {
                    cout << "Error while reading " << pathed_file  << " bduring searching det nr " << nr <<   endl;
                    continue;
                }

                //                cout << "Przeczytany numer nr = " << nr
                //                     << " a szukamy " << nr_fan_germ
                //                     << endl;

                if(nr == nr_fan_germ)
                {
                    //                    cout << "Znaleziona linijka z danymi tego detektora  " << endl;
                    // read 2 or 3 factors
                    double offset;
                    double gain;
                    double quadratic;

                    s >> offset  >> gain;
                    energy_calibr_factors.push_back(offset);
                    energy_calibr_factors.push_back(gain);
                    s >> quadratic ;
                    if(quadratic > 0)
                    {
                        energy_calibr_factors.push_back(quadratic);
                    }

                    //                    cout << "New calibration factors for det eagle "
                    //                    << name_of_this_element  << " fan  " << nr_fan_germ
                    //                    <<  "  id = " << id_number  << "  "
                    //                         << " are: ";
                    //                    for( auto x : energy_calibr_factors)
                    //                    {
                    //                        cout << " " << x << "  ";
                    //                    }
                    //                    cout  << endl;
                    goto cont;

                }
                else continue;
            }
        }
        else {
            cout << "Error while opening file " << pathed_file
                 << " with data for det nr "  << id_number

                 << " nr_fan_germ " << nr_fan_germ

                 << endl;
        }
    }
cont:
    //--------------------------------------------
    detector_position = det->GetPosition();

    //    if(id_number == 0)
    //    {
    //        cout << name_of_this_element << " , offset = " << energy_calibr_factors[0] << endl;
    //    }
    theta_geom =  det->GetThetaDeg();
    phi_geom = det->GetPosition().Phi()*TMath::RadToDeg();


    double par_time_ge[]={  4.86149e+01,
                            2.38914e+00,
                            -5.06104e-01,
                            -1.14306e-02,
                            2.03889e-02};

    time_extrapol = new TF1(Form("time_extra_%s",det->GetName().c_str()),
                            "[0]+[1]*TMath::Log(x/1000)+[2]*TMath::Power(TMath::Log(x/1000),2)+[3]*TMath::Power(TMath::Log(x/1000),3)+[4]*TMath::Power(TMath::Log(x/1000),4)",0.5,9000);
    for(UInt_t j=0;j<5;++j){
        time_extrapol->FixParameter(j,par_time_ge[j]);
    }

    // good data polygon

    // doppler correction options are read by target and stored
    // in the static members of this clas (TEAGLE_crystal)

#ifdef DOPPLER_CORRECTION_NEEDED
    betaN_doppler_factor.resize(9);

    // get data from doppler.option file

    //    cout << "Ebeam = " << Ebeam << endl;

#endif  // def DOPPLER_CORRECTION_NEEDED

}
//**********************************************************************
void TEAGLE_crystal::set_event_ptr_for_crystal_data(int ptr[3])
{}
//**********************************************************************
void TEAGLE_crystal::check_legality_of_data_pointers()
{}


#ifdef DOPPLER_CORRECTION_NEEDED
//*******************************************************************
void TEAGLE_crystal::set_doppler_corr_options
(int typ_track, int typ_cate_pos, int typ_doppl_algor)
{
    tracking_position_on_target = (type_of_tracking_requirements) typ_track;
    tracking_position_on_cate   = (type_of_cate_requirements) typ_cate_pos;
    which_doppler_algorithm = (type_of_doppler_algorithm)  typ_doppl_algor;
}
#endif   // DOPPLER_CORRECTION_NEEDED


using namespace std;


//***************************************************************
/** Based on the simplest algorithm (from Hans-Juergen) */
void TEAGLE_crystal::doppler_corrector_basic()
{
    // this algorithm below is on the web page ===================
    double beta = target_ptr->give_beta_after_target();

    //energy_doppler_corrected = energy_cal
    doppler_factor = (1.0 - beta * cos(theta_geom_radians))
            / (sqrt(1.0 - beta * beta));


    //    if (target_ptr->is_beta_playfield_enabled()) {
    //        for (int n = 0; n < 9; n++) {
    //            beta =  target_ptr->give_betaN_after_target(n);

    //            betaN_doppler_factor[n] = (1.0 - beta * cos(theta_geom_radians))
    //                    / (sqrt(1.0 - beta * beta));
    //        }
    //    }


    flag_doppler_successful = true;
}

//*******************************************************************************
/** No descriptions */
void TEAGLE_crystal::zeroing_good_flags()
{
    //   cout << "zeroing_good_flags() for" << name_of_this_element << endl;
    flag_fired =
            flag_good_data =   false;

    energy_raw = 0.;
    energy_cal = 0.;
    cfd_raw   = 0.;
    time_cal   = 0.;
    time_raw   = 0.;
    cfd_raw = 0. ;
    compton_energy_raw = 0. ;
    compton_time_raw = 0.0;
    flag_acs_approved = false;
    energy_cal_acs_approved = 0.;

    time_stamp = 0;
    energy_doppler_corrected = 0.;

    flag_doppler_successful =   false;
    pseudo_fan = -1; // zero would mean something!

}
/******************************************************************************/
bool TEAGLE_crystal::check_selfgate(Tself_gate_abstract_descr *desc)
{
    //        cout << __PRETTY_FUNCTION__ << endl;
    // here we assume that this is really germ
    Tself_gate_ge_galileo_descr *d = (Tself_gate_ge_galileo_descr *)desc;
    if (d->enable_en4gate) {
        if (energy_cal < d->en4_gate[0]
                ||
                energy_cal > d->en4_gate[1])
            return false;
    }


    //---------------------------------------
    //   Two versions of time selfate. One is the classical one. The other
    //   is about the time difference between two
    //       gammas - o X axis and Y axis

    if (d->enable_time_gate) {
        // CLASSICAL
        if (d->enable_xy_gamma_time_difference == false) {
            if (cfd_raw < d->time_gate[0] ||
                    cfd_raw > d->time_gate[1])
                return false;
        } else { // DIFFERENCE time gate
            //       here we check if this is for X axis - we just store
            // the time value

            if (d->flag_this_is_for_X_incrementer) {
                d->safe_for_x_time = cfd_raw;
            } else {   // if it is an Y axis - we make the difference
                double diff = fabs(cfd_raw -  d->safe_for_x_time);
                if (diff < d->time_gate[0] || diff > d->time_gate[1])
                    return false;  // out of the time range
            }
        }
    }

    // banana gate according to Marek Pfutzner idea
    if (d->enable_energy_time_polygon_gate) {

        if (d->polygon && d->polygon->Test(cfd_raw, energy_raw) == false) {
            //std::cout << " is outside " << std::endl;;
            return false;
        }
    }



    // THETA- PHI--------------------------------------------
    if (d->enable_geom_theta_gate) {
        //            cout << "Checking theta gate..."
        //                 << ", theta geom = " << theta_geom
        //                 << " d->geom_theta_gate[0] = "
        //                 << d->geom_theta_gate[0]
        //                 << " d->geom_theta_gate[1]= "
        //                 << d->geom_theta_gate[1]
        //                 << endl;
        if (theta_geom < d->geom_theta_gate[0] ||
                theta_geom > d->geom_theta_gate[1])
            return false;
    }
    if (d->enable_geom_phi_gate) {
        //            cout << "Checking phi gate..."
        //                           << ", PHI geom = " << phi_geom
        //                           << " d->geom_phi_gate[0] = "
        //                           << d->geom_phi_gate[0]
        //                           << " d->geom_phi_gate[1]= "
        //                           << d->geom_phi_gate[1]
        //                           << endl;
        if (phi_geom < d->geom_phi_gate[0] ||
                phi_geom > d->geom_phi_gate[1])
            return false;
    }

    // SCATERED ----------------------------------------------
    if (d->enable_gp_theta_gate) {
        if (theta_radians_between_gamma_vs_scattered_particle <  d->gp_theta_gate[0]
                ||
                theta_radians_between_gamma_vs_scattered_particle >
                d->gp_theta_gate[1])
            return false;
    }
    if (d->enable_gp_phi_gate) {
        if (phi_radians_between_gamma_vs_scattered_particle  < d->gp_phi_gate[0]
                ||
                phi_radians_between_gamma_vs_scattered_particle > d->gp_phi_gate[1])
            return false;
    }
    // -----ANTICOMPTON----------------------------------------------------
    //    if (d->flag_ACS_antycoincydence == false)
    //    {
    if(d->enable_AC_energy_gate)
    {
        if(d->flag_ACS_antycoincydence == false )
        {   // normal coincidence
            if (   compton_energy_raw  < d->AC_energy_gate[0]    ||
                   compton_energy_raw > d->AC_energy_gate[1])
                return false ;
        }else{ // anticoincidence
            if (   compton_energy_raw  >= d->AC_energy_gate[0]    &&
                   compton_energy_raw <= d->AC_energy_gate[1])
                return false ;
        }
    }
    // --- time
    if(d->enable_AC_time_gate)
    {
        if(d->flag_ACS_antycoincydence == false )
        {   // normal coincidence
            if (   compton_time_raw  < d->AC_time_gate[0]    ||
                   compton_time_raw > d->AC_time_gate[1])
                return false ;
        }else{ // anticoincidence
            if (   compton_time_raw  >= d->AC_time_gate[0]    &&
                   compton_time_raw <= d->AC_time_gate[1])
                return false ;
        }
    }

    //        cout << "Selfgate true!" << endl;
    return true; // when EVERTHING was successful
}

//************************************************************************
/** No descriptions */
void TEAGLE_crystal::increment_crystal_spectra_raw_and_fan()
{
    //      std::cout << __PRETTY_FUNCTION__
    //       <<"   Ge_" << nr_fan_germ  <<" id = " << id_number
    ////       << ", energu cal = " << energy_cal
    //       << " energy raw " << energy_raw
    //       << std::endl;


    // fan spectrum
    //int fan_offset = pseudo_fan = (10 * id_number) + 0;
    int fan_offset = (10 * nr_fan_germ) + 0;

    if (energy_raw >= 1) spec_fan->increment(fan_offset + 0);
    if (cfd_raw >= 1) spec_fan->increment(fan_offset + 1);
    if (energy_raw >= 1 && cfd_raw >= 1) spec_fan->increment(fan_offset + 2);
    //    cout
    //            << "Fan  nr_fan_germ = " << nr_fan_germ
    //            << ", czyli fan_offset = "
    //            << fan_offset

    //    << ",  id_number = " << id_number
    //    << " -------------------- ggggggg  name_of_this_element = " << name_of_this_element << endl;

    // ----------

    if (cfd_raw >= 1  || flag_increment_time_with_zero) {
        spec_time->increment(cfd_raw);
        spec_cfd->increment(cfd_raw);
        if(flag_true_germanium) spec_total_time->increment(cfd_raw);
    }
    if (energy_raw >= 1)spec_energy_raw->increment(energy_raw);

}


//**************************************************************************
/** No descriptions */
void TEAGLE_crystal::increment_crystal_spectra_cal_and_doppler()
{
    if(! flag_true_germanium)return;
    /*std::cout
   <<"Cluster " << cluster_nr <<" "<<id_number<<std::endl
   << energy_cal<<" "<<time
   <<std::endl;
   */

    /*  if (energy_cal >= 1) {
   sum_energy_cal += energy_cal;
   }
   */
    // incrementing calibrated versions of the spectrum


    if (energy_cal >= 1  || flag_increment_energy_cal_with_zero) {
        spec_energy_cal->increment(energy_cal);

        if(flag_good_data){
            spec_total_energy_cal->increment(energy_cal);
        }
        //        if(energy_cal < 10)
        //        std::cout << "incrementing " << name_of_this_element
        //                  << " energy_raw ="
        //                  << energy_raw
        //                  << "  energy cal =" << energy_cal << std::endl;

        if(flag_acs_approved &&flag_good_data ){
            /*if (energy_cal >= 1)*/
            {
                energy_cal_acs_approved = energy_cal;
                spec_energy_cal_acs_approved->increment(energy_cal_acs_approved);
                spec_total_energy_cal_acs_approved->increment(energy_cal_acs_approved);


                double theta_dssd = 0 ;
                double phi_dssd = 0 ;

                auto ptr_dssd = parent_array->owner->give_dssd_pointer();

                // cout << "========================================" << endl;
                if(ptr_dssd->give_current_theta_phi(& theta_dssd, &phi_dssd) == true)
                {
                    cout << "current germanium nr " << name_of_this_element
                            // << " dssd  theta = "
                            // << "theta = " << theta_dssd
                            // << ", phi = " << phi_dssd
                         << endl;

                    energy_doppler_corrected = doppler_needle(energy_cal_acs_approved, theta_geom, phi_geom,
                                                              theta_dssd, phi_dssd );

                    if(energy_doppler_corrected >= 1.)
                    {
                        spec_energy_doppler->increment(energy_doppler_corrected);
                        parent_array->spec_total_energy_doppler->increment(energy_doppler_corrected);
                        flag_doppler_successful = true;
                    }
                    else {
                        flag_doppler_successful = false;
                    }
                }

            }

        }else
        {
            spec_energy_cal_acs_rejected->increment(energy_cal);
        }
    }




    if ( (cfd_raw >= 1  || flag_increment_time_with_zero)
         &&
         (energy_cal >= 1  || flag_increment_energy_cal_with_zero)) {
        spec_energy_cal_vs_time->increment(cfd_raw, energy_cal);
        spec_total_energy_cal_vs_time->increment(cfd_raw, energy_cal);
    }






    //  // incrementing calibrated versions of the spectrum
    //  double recoil_velocity = target_ptr->give_beta_after_target();
    //  // std::cout << "Velocity = " << recoil_velocity << std::endl;
    //
    //  double costheta = TMath::Cos(detector_position.Theta());
    //  energy_doppler_corrected = DopplerCorrectNoAnc(energy_cal,recoil_velocity,costheta);
    //  if(energy_doppler_corrected>=1.){
    //    spec_energy_doppler->increment(energy_doppler_corrected);
    //    flag_doppler_successful = true;
    //  }
    //  else {
    //    flag_doppler_successful = false;
    //  }

    // incrementing calibrated versions of the spectrum
    // double recoil_velocity = target_ptr->give_beta_after_target();
    // std::cout << "Velocity = " << recoil_velocity << std::endl;

    //      double costheta = TMath::Cos(detector_position.Theta());
    //      energy_doppler_corrected = DopplerCorrectNoAnc(energy_cal,recoil_velocity,costheta);



}
//**************************************************************************
double TEAGLE_crystal::DopplerCorrectNoAncillary(double gammaE,
                                                 double recoil_beta,
                                                 double cosTheta )
{
    double recGamma = sqrt(1. - recoil_beta*recoil_beta);
    double cDoppler = (1. - recoil_beta * cosTheta)/recGamma;
    return cDoppler*gammaE;
}
//****************************************************************************
void TEAGLE_crystal::doppler_corrector_TVector3() {
    cout << "Empty f." << __PRETTY_FUNCTION__ << endl;;
}

using namespace TMath;
//****************************************************************************
double TEAGLE_crystal::doppler_needle(double energy_ge, double theta_ge, double phi_ge, double theta_dssd, double phi_dssd)
{

    dcout << __PRETTY_FUNCTION__ << endl;

    //    double Eex=0.5;   // [MeV] excitation energy of target nucleus
    //    double Ebeam=90.0; //[MeV] beam E
    //    const double massP = 13041.0;  // MeV/c2   Mass of projectile in u*931.5 [MeV/c2]
    //    const double massT = 102465.0;  // MeV/c2  Mass of target  in u*931.5 [MeV/c2]

    //    phi_pin_array[this_pinid] -- phi_sector
    //    theta_dssd_array[this_pinid] -- theta ring

    //    double phi_dssd_array[SECTORS_NR];  // phi_sector
    //    double theta_dssd_array[RINGS_NR];  // theta ring


    dcout << "energy_ge = " << energy_ge
          << ", theta_ge = " << theta_ge
          << ", phi_ge = " << phi_ge
          << ", theta_dssd = " << theta_dssd
          << ", phi_dssd = " << phi_dssd
          << endl;

    double taubar_kin = Sqrt(   Ebeam / (Ebeam - Eex * ( 1 + massP / massT)  )   );

    double tau_kin = taubar_kin * (massP / massT);

    dcout << "taubar_kin =" << taubar_kin
          << ", tau_kin = " << tau_kin << endl;

    //  theta_cm in radians
    double theta_cm = 2 * atan(
                (
                    Cos(theta_dssd * DegToRad())
                    -
                    Sqrt(1-Sin(theta_dssd * DegToRad()) * Sin(theta_dssd * DegToRad()) * tau_kin * tau_kin)
                    )
                /
                (Sin(theta_dssd * DegToRad()) * (tau_kin-1))
                );

    dcout << "theta_cm = " << theta_cm
          << " because:\n Cos(theta_dssd * DegToRad()) = "
        << (Cos(theta_dssd * DegToRad()))
          << "\n  Sqrt(1-Sin(theta_dssd * DegToRad()) * Sin(theta_dssd * DegToRad()) * tau_kin * tau_kin) = "
         << Sqrt(1-Sin(theta_dssd * DegToRad()) * Sin(theta_dssd * DegToRad()) * tau_kin * tau_kin)
         << "\n  divided by  (Sin(theta_dssd * DegToRad()) * (tau_kin-1)) = "
            << (Sin(theta_dssd * DegToRad()) * (tau_kin-1))
            << endl;


    double theta_target = std::atan( (Sin(theta_cm))/(taubar_kin-Cos(theta_cm))); // theta_target in radians

    dcout << "theta_target = " << theta_target << endl;


    double phi_target = (phi_dssd * DegToRad())+Pi();		// phi_target in radians

    dcout << "phi_target = " << phi_target << endl;



    double EnergyT = ((massP * massT)/((massP+massT) * (massP+massT))) * (1+taubar_kin * taubar_kin-2 * taubar_kin * cos(theta_cm)) * (Ebeam-Eex * (1+massP/massT));
    // double EnergyT = ((4 * massP * massT)/((massP+massT) * (massP+massT))) * ecal * (Cos(theta_dssd * DegToRad())) * (Cos(theta_dssd * DegToRad()));

    dcout << "EnergyT = " << EnergyT << endl;


    double costheta_target = (Sin(theta_ge * DegToRad())) * (Sin(theta_target)) * (Cos(phi_target - phi_ge * DegToRad()) )
            +(Cos(theta_ge * DegToRad())) * (Cos(theta_target));
    dcout << "costheta_target = " << costheta_target << endl;


    double betaResidual_target = Sqrt(1-Power(massT/(massT+EnergyT),2));
    dcout << "betaResidual_target = " << betaResidual_target << endl;

    double energy_doppler_corrected = energy_ge * (1. / sqrt(1 - betaResidual_target * betaResidual_target) * (1 - betaResidual_target * costheta_target));
    dcout << "energy_doppler_corrected = " << energy_doppler_corrected << endl;
    return energy_doppler_corrected ;
}
