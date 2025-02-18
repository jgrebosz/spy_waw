#include "Tcristal.h"
#include "Tcluster.h"



#include <sstream>
#include <iomanip>


#include <algorithm>
#include "TEAGLE_array.h"
#include "Ttarget.h"
#include "TNeedle.h"
#include "Thector.h"
#include "Tnamed_pointer.h"
#include "Tself_gate_ger_descr.h"
#include "TGalileoAnalysis.h" // for verbose mode
TEAGLE_array * Tcristal::array_ge_pointer ; // static pointer
Ttarget * Tcristal::target_rising_ptr ;      // for faster acces of the doppler correction

int randomizer::randomizer_word  ;  // static

// enums declared inside the Tcristal class
type_of_tracking_requirements   Tcristal::tracking_position_on_target ;
type_of_cate_requirements       Tcristal::tracking_position_on_cate ;
type_of_doppler_algorithm       Tcristal::which_doppler_algorithm ;
bool   Tcristal::flag_beta_from_frs ; // which beta to use, frs or fixed
//double Tcristal::beta_fixed_value ;  // if fixed, here it is
double Tcristal::energy_deposit_in_target ; // bec. beta on the other side
// of the target will be slower

// for beta playfield
//std::vector<double> betaN_after_target ;  // is static, for all cristals


int Tcristal::multiplicity ;
//int Tcristal::threshold_to_multiplicity ; // from this energy we will count it as "fired" !
double Tcristal::sum_energy_cal ;
double Tcristal::sum_energy_dop ;

bool Tcristal::flag_increment_20MeV_cal_with_zero;
bool Tcristal::flag_increment_4MeV_cal_with_zero  ;
bool Tcristal::flag_increment_time_cal_with_zero  ;

bool Tcristal::flag_good_20MeV_requires_threshold ;
double Tcristal::en20MeV_threshold;

bool Tcristal::flag_good_4MeV_requires_threshold  ;
double Tcristal::en4MeV_threshold  ;
double Tcristal::en4MeV_threshold_upper  ;

bool Tcristal::flag_good_time_requires_threshold  ;
// double Tcristal::good_time_threshold ;
double Tcristal::good_time_threshold_lower ; // this is the new style
double Tcristal::good_time_threshold_upper ; // this is the new style



//extern int local_mult_tmp ;

// static
spectrum_1D * Tcristal::spec_total_time_cal_minus_GeOR_cal;




#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
Tcristal *  Tcristal::pointer_to_the_cristal_which_first_fired;
double Tcristal::time_of_the_cristal_which_first_fired;      // needs zeroing

#endif

#define dcout   if(0)std::cout

//**************************************************************************
Tcristal::Tcristal (Tcluster *parent_ptr, int nr):parent_cluster(parent_ptr)
{
    //  std::cout << "constructor of cristal"
    //        << ", addres of parent is "
    //        << parent_cluster
    //        << ", my address is " << this
    //        << std::endl;

    id_number = nr ;  // note: this starts form 1,and goes: 2,3,4,5,6,7
    std::ostringstream sss ;
    sss << parent_cluster->give_detector_name()
    << "_"  << id_number ;

    name_of_this_element = sss.str() ;

    // to be able to increment total fan spectrum we must know which
    // cluster it is

    cluster_nr = parent_cluster->give_cluster_id();

    //  std::cout << "This cristal has numbers " << cluster_nr << ", "
    //      << id_number << std::endl;

    //  threshold_to_multiplicity = 100 ; // 100 keV

    doppler_factor =0;
    create_my_spectra();

    named_pointer[name_of_this_element + "_energy4MeV_raw_when_fired"]
    = Tnamed_pointer(&energy_raw, &flag_fired, this) ;
    named_pointer[name_of_this_element + "_energy20MeV_raw_when_fired"]
    = Tnamed_pointer(&energy20_raw, &flag_fired, this) ;
    named_pointer[name_of_this_element+ "_time_raw_when_fired"]
    = Tnamed_pointer(&time_raw, &flag_fired, this) ;

    named_pointer[name_of_this_element + "_energy4MeV_cal_when_fired"]
    = Tnamed_pointer(&energy_calibrated, &flag_fired, this) ;
    named_pointer[name_of_this_element + "_energy20MeV_cal_when_fired"]
    = Tnamed_pointer(&energy20_calibrated, &flag_fired, this) ;
    named_pointer[name_of_this_element + "_time_cal_when_fired"]
    = Tnamed_pointer(&time_cal, &flag_fired, this) ;

    named_pointer[name_of_this_element + "_energy4MeV_cal_when_good"]
    = Tnamed_pointer(&energy_calibrated, &flag_good_data, this) ;
    named_pointer[name_of_this_element + "_energy20MeV_cal_when_good"]
    = Tnamed_pointer(&energy20_calibrated, &flag_good_data, this) ;
    named_pointer[name_of_this_element + "_time_cal_when_good"]
    = Tnamed_pointer(&time_cal, &flag_good_data, this) ;



    named_pointer[name_of_this_element + "_energy4MeV_doppler_when_successful"]
    = Tnamed_pointer(&energy_doppler_corrected, &flag_doppler_successful, this) ;
    named_pointer[name_of_this_element + "_energy20MeV_doppler_when_successful"]
    = Tnamed_pointer(&energy20_doppler_corrected, &flag_doppler_successful, this) ;



    for (int i = 0 ; i < 9 ; i++)
    {
        std::string bet("beta") ;
        bet += char('1' + i) ;
        named_pointer[name_of_this_element + "_energy4MeV_doppler_" + bet + "_when_successful"]
        = Tnamed_pointer(&energy_doppler_corrected_betaN[i], &flag_doppler_successful, this) ;

        named_pointer[name_of_this_element + "_energy20MeV_doppler_" + bet + "_when_successful"]
        = Tnamed_pointer(&energy20_doppler_corrected_betaN[i], &flag_doppler_successful, this) ;
    }
    // TOTALS


    // find in the map the entry with such a name -
    // if there is not - create it
    // if there is already - open it and add to it

    std::string entry_name =  "ALL_cluster_crys_energy4MeV_cal_when_good" ;
    Tmap_of_named_pointers::iterator pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&energy_calibrated, &flag_good_data, this ) ;
    else
        named_pointer[entry_name] = Tnamed_pointer(&energy_calibrated, &flag_good_data,
                                    this ) ;

    //-----------------
    entry_name =  "ALL_cluster_crys_energy4MeV_cal_when_fired" ;
    pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&energy_calibrated, &flag_fired, this ) ;
    else
        named_pointer[entry_name] = Tnamed_pointer(&energy_calibrated, &flag_fired,
                                    this ) ;

    //---------------------------
    entry_name =  "ALL_cluster_crys_energy20MeV_cal_when_good" ;
    pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&energy20_calibrated, &flag_good_data, this ) ;
    else
        named_pointer[entry_name] = Tnamed_pointer(&energy20_calibrated, &flag_good_data, this ) ;

    //----------------------------
    entry_name =  "ALL_cluster_crys_time_cal_when_good" ;
    pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&time_cal, &flag_good_data, this ) ;   // flag_good_data
    else
        named_pointer[entry_name] = Tnamed_pointer(&time_cal, &flag_good_data, this ) ;  // tu tez
    //----------------------------
    entry_name =  "ALL_cluster_crys_time_cal_when_fired" ;
    pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&time_cal, &flag_fired, this ) ;
    else
        named_pointer[entry_name] = Tnamed_pointer(&time_cal, &flag_fired, this ) ;

    //------------------------------
    entry_name =  "ALL_cluster_crys_energy4MeV_doppler_when_successful" ;
    pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&energy_doppler_corrected, &flag_doppler_successful, this ) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&energy_doppler_corrected, &flag_doppler_successful, this ) ;


    // Beta playfield
    for (int i = 0 ; i < 9 ; i++)
    {

        std::string bet("beta") ;
        bet += char('1' + i) ;
        entry_name =  "ALL_cluster_crys_energy4MeV_doppler_" + bet + "_when_successful" ;

        pos = named_pointer.find(entry_name);
        if (pos != named_pointer.end ())
            pos->second.add_item(&energy_doppler_corrected_betaN[i], &flag_doppler_successful, this ) ;
        else
            named_pointer[entry_name] =
                Tnamed_pointer(&energy_doppler_corrected_betaN[i], &flag_doppler_successful, this ) ;

    }



    //------------------------------
    entry_name =  "ALL_cluster_crys_energy20MeV_doppler_when_successful" ;
    pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&energy20_doppler_corrected, &flag_doppler_successful, this ) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&energy20_doppler_corrected, &flag_doppler_successful, this ) ;



    //  pseudo FAN for Piotrek
    entry_name =  "ALL_cluster_crys_pseudo_fan_when_good" ;
    pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&pseudo_fan, &flag_good_data, this ) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&pseudo_fan, &flag_good_data, this ) ;

    //----------------------------

    // Beta playfield
    for (int i = 0 ; i < 9 ; i++)
    {

        std::string bet("beta") ;
        bet += char('1' + i) ;
        entry_name =  "ALL_cluster_crys_energy20MeV_doppler_" + bet + "_when_successful" ;

        pos = named_pointer.find(entry_name);
        if (pos != named_pointer.end ())
            pos->second.add_item(&energy20_doppler_corrected_betaN[i], &flag_doppler_successful, this ) ;
        else
            named_pointer[entry_name] =
                Tnamed_pointer(&energy20_doppler_corrected_betaN[i], &flag_doppler_successful, this ) ;
    }

    //----------------------------
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005


    named_pointer[name_of_this_element + "_LRtime_calibrated_when_first"]
    = Tnamed_pointer(&LRtime_calibrated, &flag_this_cristal_triggered_electronics, this) ;

    named_pointer[name_of_this_element + "_time_cal_of_the_first_cristal_minus_time_cal_when_calculated"]
    = Tnamed_pointer(&time_cal_of_the_first_cristal_minus_time_cal, &flag_dt_calculated, this) ;

    named_pointer[name_of_this_element + "_LR_plus_dt_time_cal_when_calculated"]
    = Tnamed_pointer(&LR_plus_dt_time_cal, &flag_dt_calculated, this) ;


    // one flag
    named_pointer[name_of_this_element + "_flag_this_cristal_triggered_electronics"]
    = Tnamed_pointer(&flag_this_cristal_triggered_electronics, 0, this) ;


    //  pseudo FAN for Piotrek, but validaed by triggering ellectronics flag
    entry_name =  "ALL_cluster_crys_pseudo_fan_when_triggered" ;
    pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&pseudo_fan, &flag_this_cristal_triggered_electronics, this ) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&pseudo_fan, &flag_this_cristal_triggered_electronics, this ) ;

    //--------

    named_pointer[name_of_this_element + "_time_LR_plus_SR_plus_offsets_difference"] =
        Tnamed_pointer(&time_LR_plus_SR_plus_offsets_difference,
                       &flag_dt_calculated, this) ;

    //-----------
    named_pointer[name_of_this_element + "_time_cal_minus_GeOR_cal"]
    = Tnamed_pointer(&time_cal_minus_GeOR_cal, NULL, this) ;

// ---------------- ALL



    entry_name =  "ALL_cluster_crys_time_LR_plus_SR_plus_offsets_difference_when_valid" ;
    pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&time_LR_plus_SR_plus_offsets_difference, &flag_dt_calculated, this ) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&time_LR_plus_SR_plus_offsets_difference, &flag_dt_calculated, this ) ;

//----------
    entry_name =  "ALL_cluster_crys_LR_plus_dt_time_cal_when_valid" ;
    pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&LR_plus_dt_time_cal, &flag_dt_calculated, this ) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&LR_plus_dt_time_cal, &flag_dt_calculated, this ) ;


    entry_name =  "ALL_cluster_crys_time_cal_minus_GeOR_cal_when_valid" ;
    pos = named_pointer.find(entry_name);
    if (pos != named_pointer.end ())
        pos->second.add_item(&time_cal_minus_GeOR_cal, &flag_fired, this ) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&time_cal_minus_GeOR_cal, &flag_fired, this ) ;





#endif

}
//************************************************************************
void Tcristal::create_my_spectra()
{

    //std::cout << "before creating the spectra " << std::endl ;

    // the spectra have to live all the time, so here we can create
    // them during  the constructor

    std::vector<spectrum_abstr*> *rising_spectra_ptr =
        array_ge_pointer -> address_spectra_rising();

    std::string folder = "clusters/" + parent_cluster->give_detector_name() ;

    // energy -------------------------------------
    std::string name = name_of_this_element ;
    name += "_energy4MeV_raw"  ;
    spec_energy_raw = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,  // here the binning must start from 0, because Carl Wheldon automatic routine expect this
        folder );
    rising_spectra_ptr->push_back(spec_energy_raw) ;


    //-----------------------
    name = name_of_this_element ;
    name += "_energy20MeV_raw"  ;
    spec_energy20_raw = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,   // here the binning must start from 0, because Carl Wheldon automatic routine expect this
        folder );
    rising_spectra_ptr->push_back(spec_energy20_raw) ;
    name = name_of_this_element ;

    // time ------------------------------------------

    name += "_time_raw"  ;
    spec_time_raw = new spectrum_1D( name,
                                     name,
                                     8192, 0, 8192,
                                     folder );
    rising_spectra_ptr->push_back(spec_time_raw) ;


    name = name_of_this_element + "_time_cal_minus_GeOR_cal"  ;
    spec_time_cal_minus_GeOR_cal = new spectrum_1D( name,
            name,
            8192, -8192, 8192,
            folder,
            "after aritmetics:   time_cal   -  cluster_vxi_P_time_minus_Ge_time_OR_cal" );

    rising_spectra_ptr->push_back(spec_time_cal_minus_GeOR_cal) ;

    //=============== calibrated spectra  =====
    // energy cal ------------------------------------
    name = name_of_this_element ;

    name += "_energy4MeV_cal"  ;
    spec_energy_cal = new spectrum_1D(
        name,
        name,
        4096, 0, 4096,
        folder );
    rising_spectra_ptr->push_back(spec_energy_cal) ;
    name = name_of_this_element ;
    name += "_energy20MeV_cal"  ;
    spec_energy20_cal = new spectrum_1D(
        name,
        name,
        5000, 0, 25000,
        folder );
    rising_spectra_ptr->push_back(spec_energy20_cal) ;
    name = name_of_this_element ;
    name += "_time_cal"  ;
    spec_time_cal = new spectrum_1D( name,
                                     name,
                                     8192, 0, 8192,
                                     folder );
    rising_spectra_ptr->push_back(spec_time_cal) ;


    //========================================

    name = name_of_this_element  + "_theta_mrad"  ;
    spec_theta_miliradians = new spectrum_1D( name,
            name,
            1000, 0, 100,
            folder );
    rising_spectra_ptr->push_back(spec_theta_miliradians) ;

    name = name_of_this_element ;
    name += "_energy4MeV_dop"  ;
    spec_energy_doppler = new spectrum_1D(
        name,
        name,
        4096, 0, 4096,
        folder );
    rising_spectra_ptr->push_back(spec_energy_doppler) ;

    name = name_of_this_element ;
    name += "_energy20MeV_dop"  ;
    spec_energy20_doppler = new spectrum_1D(
        name,
        name,
        5000, 0, 25000,
        folder );
    rising_spectra_ptr->push_back(spec_energy20_doppler) ;


    if (!spec_total_time_cal_minus_GeOR_cal)
    {
        name = "cluster_total_time_cal_minus_GeOR_cal";
        spec_total_time_cal_minus_GeOR_cal = new spectrum_1D( name,
                name,
                8192, -8192, 8192,
                folder );
        rising_spectra_ptr->push_back(spec_total_time_cal_minus_GeOR_cal);
    }

    //####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################
    // spectra for new way of clibraning th G-factor experiment


    name = name_of_this_element + "_LRtime_cal"  ;
    spec_LRtime_cal = new spectrum_1D( name,
                                       name,
                                       8192, 0, 8192,
                                       folder );
    rising_spectra_ptr->push_back(spec_LRtime_cal) ;

    // delta t  time_cal_of_the_first_cristal_minus_time_cal
    name = name_of_this_element +  "_time_cal_of_the_first_cristal_minus_time_cal"  ;
    spec_time_cal_of_the_first_cristal_minus_time_cal
    = new spectrum_1D( name,
                       name,
                       3000, 0, 3000,
                       folder );
    rising_spectra_ptr->push_back(spec_time_cal_of_the_first_cristal_minus_time_cal) ;

    //------------ LR_plus_dt_time_cal
    name = name_of_this_element +  "_LR_plus_dt_time_cal"  ;
    spec_LR_plus_dt_time_cal
    = new spectrum_1D( name,
                       name,
                       3000, 0, 12000,
                       folder );
    rising_spectra_ptr->push_back(spec_LR_plus_dt_time_cal) ;





    //------------ LR_plus_dt_time_cal
    name = name_of_this_element +  "_time_LR_plus_SR_plus_offsets_difference"  ;
    spec_time_LR_plus_SR_plus_offsets_difference
    = new spectrum_1D( name,
                       name,
                       3000, 0, 12000,
                       folder );
    rising_spectra_ptr->push_back(spec_time_LR_plus_SR_plus_offsets_difference) ;


#endif

}
//**********************************************************************
void Tcristal::analyse_current_event()
{
    //  std::cout << " analyse_current_event()  for "
    //        << name_of_this_element
    //        << std::endl ;

    // checking if it is something (not zero) -- is made during unpacking

    zeroing_good_flags();

    energy20_doppler_corrected =  energy_doppler_corrected = 0 ;
    doppler_factor = 0 ;
    for (int i = 0 ; i < 9 ;i++)
    {
        energy_doppler_corrected_betaN[i] =
            energy20_doppler_corrected_betaN[i] = 0 ;
        betaN_doppler_factor[i] = 0 ;
    }

    //----------------
    // so it fired !

    time_raw = TEAGLE_array::tmp_event_unpacked->Cluster_data[cluster_nr][id_number-1][2];  // time
    energy20_raw = TEAGLE_array::tmp_event_unpacked->Cluster_data[cluster_nr][id_number-1][0];
    energy_raw = TEAGLE_array::tmp_event_unpacked->Cluster_data[cluster_nr][id_number-1][1];   // e4

    flag_fired =  energy_raw || energy20_raw || time_raw;

    // the time information delivered by XIA electronics
    //  time_xia_minus_GeOr_raw =  Trising::tmp_event_unpacked->ger_xia_dgf[cluster_nr][id_number-1][0] ;
    //  energy_xia_raw =  Trising::tmp_event_unpacked->ger_xia_dgf[cluster_nr][id_number-1][1];
    //
    //  flag_xia_fired =  energy_xia_raw || time_xia_minus_GeOr_raw ;



    if (GalileoAnalysis_ptr->is_verbose_on() )
    {
        std::cout  << name_of_this_element
              << ", E4raw =  " << std::setw(4) << energy_raw
              << ", E20raw = " << std::setw(5) << energy20_raw
              << ", Traw = " << std::setw(4) << time_raw
      << std::flush;  // no std::endl, bec calibrated values will be in the same line
    }

    /* Note: incrementing the raw spectra is done only when multiplicity in this cluster is 1.
    	 all this is done from Trising class, by calling Tcluster::calculate_single_spectrum()
    	 The "missing" instructions which were here - are moved just there
    */


    // energy calibration -------------------------------
    energy_calibrated = 0 ;
    // it was nonsense to calibrate the raw zero
    if (energy_raw)
    {

        double energy_tmp = (energy_raw) + randomizer::randomek() ;

        if (energy_calibr_factors[2]== 0)
        {
            energy_calibrated = energy_calibr_factors[0]
                                 + (energy_tmp * energy_calibr_factors[1]) ;
        }
        else for (unsigned int i = 0 ; i < energy_calibr_factors.size() ; i++)
            {
                energy_calibrated += energy_calibr_factors[i] * pow(energy_tmp, (int)i) ;
            }
    }


    // 20 MeV  -------------------------------------------
    energy20_calibrated = 0 ;
    double energy20_tmp = (energy20_raw) + randomizer::randomek() ;

    if (energy20_raw) // it is nonsense to calibrate the raw zero
    {

        if (energy20_calibr_factors[2]== 0) // simple linear calibration
        {
            energy20_calibrated = energy20_calibr_factors[0]
                                  + (energy20_tmp * energy20_calibr_factors[1]) ;
        }
        else for (unsigned int i = 0 ; i < energy20_calibr_factors.size() ; i++)
            {
                energy20_calibrated +=
                    energy20_calibr_factors[i] * pow(energy20_tmp, (int)i) ;
            }
    }

    // time calibration --------------------------------
    time_cal = 0 ;
    if (time_raw)  // it is nonsense to calibrate the raw zero
    {
        double time_tmp = time_raw + randomizer::randomek() ;
        if (time_calibr_factors[2]== 0) // simple linear calibration
        {
            time_cal = time_calibr_factors[0]
                       + (time_tmp * time_calibr_factors[1]) ;
        }
        else for (unsigned int i = 0 ; i < time_calibr_factors.size() ; i++)
            {
                time_cal += time_calibr_factors[i] * pow(time_tmp, (int)i) ;
            }
    }

    if (GalileoAnalysis_ptr->is_verbose_on() )
    {
        std::cout
            // << name_of_this_element
            << ", E4cal =  " << std::setw(4) << energy_calibrated
            << ", E20cal = " << std::setw(5) << energy20_calibrated
            << ", Tcal = " << std::setw(4) << time_cal
            <<std::endl;
    }


    //=======================
    // ---incr spectra
    // ---> fragment moved into the function Tcluster::calculate_single_spectrum

    //================================
    // GOOD
    //================================
    if (
        flag_fired   // sometimes all 3 kinds of data are 0. Flag_fired is of course 0,
        // but here, would be flag_good_data == 1
        // if we do not require any threshould. To avoid this, we as the flag_fired to be true
        &&


        (flag_good_20MeV_requires_threshold  ?
         (energy20_calibrated >  en20MeV_threshold)
         :
         true
        )
        &&
        (flag_good_4MeV_requires_threshold  ?
         (energy_calibrated >=  en4MeV_threshold && energy_calibrated <=  en4MeV_threshold_upper )
         :
         true)
        &&
        (flag_good_time_requires_threshold  ?   // acutally now this is not a threshold, but the gate
         (time_cal >=  good_time_threshold_lower)
         :
         true)

        &&
        (flag_good_time_requires_threshold  ?
         (time_cal <=  good_time_threshold_upper)
         :
         true)
    )
    {
        flag_good_data = true;
        Tcristal::multiplicity++ ;
    }

    //####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################

    LRtime_calibrated = 0 ; //because not all will fire and calibrate
    // this above in case of the g-factor experiments was the calibration of the Short Range time
    // If we want to check which of them was the first one, we need the maximum of this
    // calibrated times (but still in the range)

    //   std::cout << name_of_this_element << "Value of time " <<  time_cal << std::endl;
    if (time_of_the_cristal_which_first_fired < time_cal
            &&  flag_good_data )  // this means that the time lies inside the time gate
    {
        //         std::cout << "remembered as maximal " << std::endl;
        pointer_to_the_cristal_which_first_fired = this;
        time_of_the_cristal_which_first_fired = time_cal;      // needs zeroing
    }

    // for g-factor this is a aritmetic formula to know the time difference between registering the
    // particle (scintillator) and registering the gamma.
    time_cal_minus_GeOR_cal =  Trising::vxi_P_time_minus_Ge_time_OR_cal - time_cal;
    spec_total_time_cal_minus_GeOR_cal->increment(time_cal_minus_GeOR_cal);


#endif


    // Piotr wants to have the incrementer similar to this what increments fan spectrum
    pseudo_fan = (100*cluster_nr) +(10*id_number) ;
    // this value does not have to be zeroed,because anyway it is validated by the flag
    // good_data, so the value is always there, but the validator sometimes in false


    /*std::cout
      <<"Cluster " << cluster_nr <<" "<<id_number<<std::endl
      << energy_calibrated<<" "<<time_cal
      <<" "<<energy20_calibrated<<std::endl;
    */

#ifdef VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT
    // NOTE id_number starts from 1,   so: 1,2,3,4,5,6,7
    Trising::tmp_event_calibrated->en4_cal[cluster_nr][id_number-1] = energy_calibrated;
    Trising::tmp_event_calibrated->en20_cal[cluster_nr][id_number-1] = energy20_calibrated;
    Trising::tmp_event_calibrated->tim_cal[cluster_nr][id_number-1] =time_cal;
#endif

    //  std::cout << " end of analyse_current_event()  for "
    //        << name_of_this_element
    //        << std::endl ;

}

#ifdef DOPPLER_CORRECTION_NEEDED

//**************************************************************************
// This name of the fuction is not good. Should be:
//  chose_and_calculate_doppler()
//**************************************************************************
void  Tcristal::calculate_chosen_doppler_algorithm()
{
    switch (which_doppler_algorithm)
    {
    case tracked_algorithm:
        if (target_ptr->is_doppler_possible()) doppler_corrector_TVector3();
        break;
    case basic_algorithm:
        doppler_corrector_basic();
        break;
    default:
        std::cout
            << "Error, the type of doppler correction algorithm was not selected "
            "in the file: option/doppler.options\n (the value is now = "
            << which_doppler_algorithm << ")" << std::endl ;
        break ;
    }
}
#endif // def DOPPLER_CORRECTION_NEEDED

//**************************************************************************
void  Tcristal::make_preloop_action(std::istream & s)
{
    // check_legality_of_data_pointers();

    // total spectra are in the rising object
    spec_total_theta_miliradians = array_ge_pointer->spec_total_theta_miliradians ;

#ifdef DOPPLER_CORRECTION_NEEDED
    spec_total_energy_doppler= array_ge_pointer->spec_total_energy_doppler ;
    spec_total_energy20_doppler= array_ge_pointer->spec_total_energy20_doppler ;
#endif

    spec_fan = array_ge_pointer->spec_fan;

    // std::cout << "Reading the calibration for the cristal "
    //        << name_of_this_element
    //        << std::endl ;

    // in this file we look for a string
    energy_calibr_factors.clear() ;
    int how_many_factors = 3 ;
    std::string slowo = name_of_this_element + "_energy4MeV_factors" ;

    // first reading the energy calibration
    Tfile_helper::spot_in_file(s, slowo );
    for (int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;
        if (!s)
        {
            Treading_value_exception capsule ;
            capsule.message =
                "I found keyword '" + slowo
                + "' but error was in reading its value." ;
            throw capsule ;
        }
        // if ok
        energy_calibr_factors.push_back(value ) ;
    }

    energy20_calibr_factors.clear() ;
    slowo = name_of_this_element + "_energy20MeV_factors" ;

    // first reading the energy calibration
    Tfile_helper::spot_in_file(s, slowo );
    for (int i = 0 ; i < how_many_factors ; i++)
    {

        // otherwise we read the data
        double value ;
        s >> value ;
        if (!s)
        {
            Treading_value_exception capsule ;
            capsule.message =
                "I found keyword '" + slowo
                + "' but error was in reading its value." ;
            throw capsule ;
        }
        // if ok
        energy20_calibr_factors.push_back(value ) ;
    }

    // reading the time calibration factors --------------
    time_calibr_factors.clear() ;
    slowo = name_of_this_element + "_time_factors" ;
    Tfile_helper::spot_in_file(s, slowo );
    for (int i = 0 ; i < how_many_factors ; i++)
    {
        double value ;
        s >> value ;
        if (!s)
        {
            Treading_value_exception capsule ;
            capsule.message =
                "I found keyword '" + slowo
                + "' but error was in reading its value." ;
            throw capsule ;
        }
        // if ok
        time_calibr_factors.push_back(value ) ;
    }

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    // calibration factors for common time singal TAC (Long range)
    // (each of cristals has private factors, and only this one, which is considered
    // as first, which fired - will make the calibration

    LRtime_calibr_factors.clear() ;
    slowo = name_of_this_element + "_LRtime_factors" ;
    Tfile_helper::spot_in_file(s, slowo );
    for (int i = 0 ; i < how_many_factors ; i++)
    {
        double value ;
        s >> value ;
        if (!s)
        {
            Treading_value_exception capsule ;
            capsule.message =
                "I found keyword '" + slowo
                + "' but error was in reading its value." ;
            throw capsule ;
        }
        // if ok
        LRtime_calibr_factors.push_back(value ) ;
    }
#endif

    // ---------------------------------------------------
    // Geometry: phi, theta
    //----------------------------------------------------

    // but this is in a different file....

    // std::cout << " reading the geometry angles  for cristal "
    //        << name_of_this_element
    //        << std::endl ;


    //-----------------

    // here we read the Geometry
    //


    std::string geometry_file = path.dir_calibration() + "rising_geometry.txt" ;
    try
    {
        std::ifstream plik_geometry(geometry_file.c_str() );
        if (! plik_geometry)
        {
            std::cout << "I can't open  file: " << geometry_file  ;
            exit(-1) ;
        }
        std::string keyword =name_of_this_element + "_phi_theta_distance" ;
        Tfile_helper::spot_in_file(plik_geometry, keyword);
        plik_geometry >> zjedz >> phi_geom
        >> zjedz >> theta_geom
        >> zjedz >> distance_from_target ;
        if (!plik_geometry)
        {
            std::cout << "Error while reading (inside geometry file "
                 << geometry_file
                 << ") phi and theta and distance of"
                 << name_of_this_element
                 << "\nI found keyword '"
                 << keyword
                 << "' but error was while reading its (three) values"
                 << " of phi and theta."
                 << std::endl  ;
            exit(-1);
        }
    }
    catch (Tfile_helper_exception &k)
    {
        std::cerr << "Error while reading geometry file "
             << geometry_file
             << ":\n"
             << k.message << std::endl  ;
        exit(-1) ;
    }

    distance_from_target *= 10 ; // it was in [cm] and we need in milimeters !
    phi_geom_radians   =  M_PI * phi_geom / 180.0 ;
    theta_geom_radians =  M_PI * theta_geom / 180.0;

    // ============== recalculate phi and theta into x,y,z of the cristal
    cristal_position_x = distance_from_target * sin(theta_geom_radians) * cos(phi_geom_radians) ;
    cristal_position_y = distance_from_target * sin(theta_geom_radians) * sin(phi_geom_radians) ;
    cristal_position_z = distance_from_target * cos(theta_geom_radians);

    // doppler correction options are read by target and stored
    // in the static members of this clas (Tcristal)

    betaN_doppler_factor.resize(9);
}
//**********************************************************************
//void Tcristal::set_event_ptr_for_cristal_data(int ptr[3])
//{
//	// now we can define local pointers
//  std::cout << "Do not call this function " << std::endl;
//}
////**********************************************************************
//void Tcristal::check_legality_of_data_pointers()
//{
//  std::cout << "Do not call this function " << std::endl;
//}


//*******************************************************************
void Tcristal::set_doppler_corr_options
(int typ_track, int typ_cate_pos, int typ_doppl_algor)
{
    tracking_position_on_target = (type_of_tracking_requirements) typ_track;
    tracking_position_on_cate   = (type_of_cate_requirements) typ_cate_pos;
    which_doppler_algorithm = (type_of_doppler_algorithm)  typ_doppl_algor ;
}
//***************************************************************
/** Based on the simplest algorithm (from Hans-Juergen) */
void Tcristal::doppler_corrector_basic()
{

    std::cout << "Never used" << std::endll;
    // this algorithm below is on the web page ===================
    double beta = target_rising_ptr->give_beta_after_target();

    //energy_doppler_corrected = energy_calibrated
    doppler_factor=  ( 1.0 - beta * cos(theta_geom_radians))
                     / (sqrt(1.0 - beta * beta));


    if (target_rising_ptr->is_beta_playfield_enabled() )
    {
        for (int n = 0 ; n < 9 ; n++)
        {
            beta = 	target_rising_ptr->give_betaN_after_target(n);

            betaN_doppler_factor[n] =  ( 1.0 - beta * cos(theta_geom_radians))
                                       / (sqrt(1.0 - beta * beta));
        }
    }


    flag_doppler_successful = true ;
}
//*******************************************************************************
/** No descriptions */
void Tcristal::zeroing_good_flags()
{
    flag_fired =
        flag_good_data =
            flag_doppler_successful = false ;

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    flag_dt_calculated = false; // validator
    flag_this_cristal_triggered_electronics = false;
    time_LR_plus_SR_plus_offsets_difference = 0;
    time_cal_minus_GeOR_cal = 0 ;
#endif
    energy_calibrated = 0;
}
/******************************************************************************/
bool Tcristal::check_selfgate(Tself_gate_abstract_descr *desc)
{
    // here we assume that this is really germ
    Tself_gate_ger_descr *d = (Tself_gate_ger_descr*)desc ;
    if (d->enable_en4gate)
    {
        if (energy_calibrated < d->en4_gate[0]
                ||
                energy_calibrated > d->en4_gate[1])
            return false ;
    }
    if (d->enable_en20gate)
    {
        if (energy20_calibrated < d->en20_gate[0]
                ||
                energy20_calibrated > d->en20_gate[1])
            return false ;
    }
    if (d->enable_time_gate)
    {
        if (time_cal < d->time_gate[0] || time_cal > d->time_gate[1])
            return false ;
    }
    if (d->enable_geom_theta_gate)
    {
        if (theta_geom < d->geom_theta_gate[0] || theta_geom > d->geom_theta_gate[1])
            return false;
    }
    if (d->enable_geom_phi_gate)
    {
        if (phi_geom < d->geom_phi_gate[0] || phi_geom > d->geom_phi_gate[1])
            return false;
    }
    if (d->enable_gp_theta_gate)
    {
        if (theta_radians_between_gamma_vs_scattered_particle <  d->gp_theta_gate[0]
                ||
                theta_radians_between_gamma_vs_scattered_particle > d->gp_theta_gate[1])
            return false ;
    }
    if (d->enable_gp_phi_gate)
    {
        if (phi_radians_between_gamma_vs_scattered_particle  < d->gp_phi_gate[0]
                ||
                phi_radians_between_gamma_vs_scattered_particle > d->gp_phi_gate[1])
            return false;
    }
    if (d->enable_mult_in_cluster_gate)
    {
        int mult =  parent_cluster->get_this_cluster_multiplicity();
        if ( mult < d->mult_in_cluster_gate[0]
                ||
                mult > d->mult_in_cluster_gate[1]
           )
            return false ;
    }
#ifdef AC_PRESENT
    if (d->enable_AC_in_cluster_gate)
    {
        int AC_energy =  parent_cluster->get_this_cluster_AC_energy();
        if ( AC_energy < d->AC_in_cluster_gate[0]
                ||
                AC_energy > d->AC_in_cluster_gate[1]
           )
            return false ;
    }
#endif // def AC_PRESENT

    return true ; // when EVETHING was successful
}

//**************************************************************************************************
/** No descriptions */
void Tcristal::increment_cristal_spectra_raw_and_fan()
{
    /*std::cout
      <<"Cluster " << cluster_nr <<" "<<id_number<<std::endl
      << energy_calibrated<<" "<<time_cal
      <<" "<<energy20_calibrated<<std::endl;
    */
    int fan_offset = (100*cluster_nr) +(10*id_number) ;
    if (energy_raw) spec_fan->increment(fan_offset + 0);
    if (energy20_raw) spec_fan->increment(fan_offset + 1);
    if (time_raw) spec_fan->increment(fan_offset + 2);
    if (energy_raw && energy20_raw && time_raw)
        spec_fan->increment(fan_offset + 3);


    // ----------
    spec_time_raw->increment(time_raw )  ;
    spec_energy_raw->increment(energy_raw);
    spec_energy20_raw->increment(energy20_raw);

    spec_time_cal_minus_GeOR_cal->increment(time_cal_minus_GeOR_cal);


}
//*******************************************************************************************
/** No descriptions */
void Tcristal::increment_cristal_spectra_cal_and_doppler()
{
    /*std::cout
      <<"Cluster " << cluster_nr <<" "<<id_number<<std::endl
      << energy_calibrated<<" "<<time_cal
      <<" "<<energy20_calibrated<<std::endl;
    */

    sum_energy_cal += energy_calibrated ;


    // incrementing calibrated versions of the spectrum

    if (energy_raw !=0  || flag_increment_4MeV_cal_with_zero)
    {
        spec_energy_cal->increment(energy_calibrated ) ;
        array_ge_pointer->increm_spec_total_energy_calibrated(energy_calibrated) ;


    }
    if (energy20_raw !=0  || flag_increment_20MeV_cal_with_zero)
    {
        spec_energy20_cal->increment(energy20_calibrated ) ;
        array_ge_pointer->increm_spec_total_energy20_calibrated(energy20_calibrated) ;
    }
    if (time_raw !=0  || flag_increment_time_cal_with_zero)
    {
        spec_time_cal->increment(time_cal )  ;
        array_ge_pointer->increm_spec_total_time_calibrated(time_cal) ;
    }

    //####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################
    rising_pointer->increm_spec_cluster_total_time_cal_minus_GeOR_cal(time_cal_minus_GeOR_cal) ;
#endif

#ifdef DOPPLER_CORRECTION_NEEDED

    // Doppler corrected spectra we do only when flag  GOOD is set

    if ( flag_good_data == true )
    {

        calculate_chosen_doppler_algorithm();

        if (flag_doppler_successful)
        {
            energy_doppler_corrected = energy_calibrated * doppler_factor;
            energy20_doppler_corrected = energy20_calibrated * doppler_factor;

            // for beta playfield

            if (target_ptr->is_beta_playfield_enabled() )
            {
                for (int n = 0 ; n < 9 ; n++)
                {
                    energy_doppler_corrected_betaN[n] =
                        energy_calibrated * betaN_doppler_factor[n] ;
                    energy20_doppler_corrected_betaN[n] =
                        energy20_calibrated * betaN_doppler_factor[n] ;
                }
            }

            spec_energy_doppler->increment(energy_doppler_corrected);
            spec_total_energy_doppler->
            increment(energy_doppler_corrected);

            sum_energy_dop += energy_doppler_corrected  ;

            spec_energy20_doppler->increment(energy20_doppler_corrected);
            spec_total_energy20_doppler->
            increment(energy20_doppler_corrected);

            spec_theta_miliradians->
            increment(theta_radians_between_gamma_vs_scattered_particle);
            spec_total_theta_miliradians->
            increment(theta_radians_between_gamma_vs_scattered_particle);
        }
    }

#endif //def DOPPLER_CORRECTION_NEEDED

}
//*******************************************************************************************
/** No descriptions */
void Tcristal::increment_spect_total_addbacked_cal_and_doppler()
{
    // incrementing calibrated versions of the spectrum

    if (energy_raw !=0  || flag_increment_4MeV_cal_with_zero)
    {
        //spec_energy_cal->increment(energy_calibrated ) ;
        array_ge_pointer->increm_spec_total_energy_calibrated_addbacked(energy_calibrated) ;

    }
    if (energy20_raw !=0  || flag_increment_20MeV_cal_with_zero)
    {
        //		spec_energy20_cal->increment(energy20_calibrated ) ;
        array_ge_pointer->increm_spec_total_energy20_calibrated_addbacked(energy20_calibrated) ;
    }


#ifdef DOPPLER_CORRECTION_NEEDED

    // Doppler corrected spectra we do only when flag  GOOD is set

    if ( flag_good_data == true )
    {

        calculate_chosen_doppler_algorithm();
        if (flag_doppler_successful)
        {
            energy_doppler_corrected=energy_calibrated * doppler_factor;
            energy20_doppler_corrected=energy20_calibrated * doppler_factor;

            array_ge_pointer->increm_spec_total_energy_doppler_addbacked(energy_doppler_corrected);
            array_ge_pointer->increm_spec_total_energy20_doppler_addbacked(energy20_doppler_corrected);
        }
    }
#endif // DOPPLER_CORRECTION_NEEDED

}
//****************************************************************************************
/** this function contains the algorithm of the doppler correction
using the class TVector3 from ROOT library */
void Tcristal::doppler_corrector_TVector3()
{

}
//******************************************************************************************************************
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

/** After analysing all cristals, we know which of them has a prompt time (smallest SR) so now we can
  return to continue the calibration of the LR time */
void Tcristal::analyse_LR_common_time()
{

    // what about zeroing this pointer ?  necessary?

    time_cal_of_the_first_cristal_minus_time_cal =
        pointer_to_the_cristal_which_first_fired->give_time_calibrated()  // this is SR time
        - time_cal;
    spec_time_cal_of_the_first_cristal_minus_time_cal->increment(
        time_cal_of_the_first_cristal_minus_time_cal);


    LR_plus_dt_time_cal =
        Trising::vxi_LR_time_calibrated_by_the_first_who_fired +  time_cal_of_the_first_cristal_minus_time_cal ;
    spec_LR_plus_dt_time_cal->increment(LR_plus_dt_time_cal);

    flag_dt_calculated = true; // validator


    // For Lili ===================================
    double offset_of_first = pointer_to_the_cristal_which_first_fired->give_calibrating_offset();

    time_LR_plus_SR_plus_offsets_difference =
        Trising::vxi_LR_time_calibrated_by_the_first_who_fired    // LR
        +  time_cal_of_the_first_cristal_minus_time_cal    // SR
        +( give_calibrating_offset() - offset_of_first);




    spec_time_LR_plus_SR_plus_offsets_difference->
    increment(time_LR_plus_SR_plus_offsets_difference);

    if (RisingAnalysis_ptr->is_verbose_on() )
    {
        std::cout << name_of_this_element
             << ", LR_plus_dt_time_cal = "
             << std::setw(6) << LR_plus_dt_time_cal
             <<   ", time_LR_plus_SR_plus_offsets_difference= "
             << std::setw(6) <<   time_LR_plus_SR_plus_offsets_difference
             << std::endl;

    }


}
#endif // CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

//******************************************************************************************************************
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

/** when it is known, which cristal was triggering the sytem (create the prompt) -
he can calibrate the common TAC Long range timeNo descriptions */

void Tcristal::you_are_chosen_to_calibrate_LRtime()
{

    // time calibration --------------------------------
    LRtime_calibrated = 0 ;
    double time_tmp = Trising::vxi_P_time_minus_Ge_time_OR_raw + randomizer::randomek() ;
    if (LRtime_calibr_factors[2]== 0) // simple linear calibration
    {
        LRtime_calibrated = LRtime_calibr_factors[0]
                            + (time_tmp * LRtime_calibr_factors[1]) ;
    }
    else for (unsigned int i = 0 ; i < LRtime_calibr_factors.size() ; i++)
        {
            LRtime_calibrated += LRtime_calibr_factors[i] * pow(time_tmp, (int)i) ;
        }
    Trising::vxi_LR_time_calibrated_by_the_first_who_fired = LRtime_calibrated;  // to static
    spec_LRtime_cal->increment( LRtime_calibrated);


    flag_this_cristal_triggered_electronics = true;

    if (RisingAnalysis_ptr->is_verbose_on() )
    {
        std::cout << "----- " << name_of_this_element << " was chosen to calibrate LR  raw = "
             << std::setw(
                          
                          6) << Trising::vxi_LR_time_calibrated_by_the_first_who_fired
             << std::endl;
    }



}
#endif // CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

//***************************************************************************************

