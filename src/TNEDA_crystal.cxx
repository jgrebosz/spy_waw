#include "TNEDA_crystal.h"
//#include "Tcluster_xia.h"

#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include "TNEDA_array.h"
#include "Ttarget.h"
#include "TNeedle.h"
#include "Tnamed_pointer.h"
#include "Tself_gate_ge_galileo_descr.h"
#include "TGalileoAnalysis.h" // for verbose mode

using namespace std;
#define fif(x,y) Tfile_helper::find_in_file<double>((x),(y));

TNEDA_array    *TNEDA_crystal::array_nw_pointer;  // static pointer

// enums declared inside the TNEDA_crystal class
type_of_tracking_requirements   TNEDA_crystal::tracking_position_on_target;
type_of_cate_requirements       TNEDA_crystal::tracking_position_on_cate;

bool   TNEDA_crystal::flag_beta_from_frs; // which beta to use, frs or fixed
double TNEDA_crystal::energy_deposit_in_target; // bec. beta on the other side
// of the target will be slower

bool TNEDA_crystal::flag_increment_energy_cal_with_zero;
bool TNEDA_crystal::flag_increment_time_with_zero;

bool   TNEDA_crystal::flag_good_energy_requires_threshold;
double TNEDA_crystal::energy_threshold_lower;
double TNEDA_crystal::energy_threshold_upper;

bool   TNEDA_crystal::flag_good_time_requires_threshold;
double TNEDA_crystal::good_time_threshold_lower; // this is the new style
double TNEDA_crystal::good_time_threshold_upper; // this is the new style


bool TNEDA_crystal::flag_good_wAmpli_vs_eAmpli_polygon_needed;


std::string TNEDA_crystal::good_polygon_name;

// static
spectrum_1D *TNEDA_crystal::spec_total_time;
spectrum_1D *TNEDA_crystal::spec_total_energy_cal;
spectrum_2D *TNEDA_crystal::spec_total_energy_cal_vs_time;
//spectrum_2D *TNEDA_crystal::spec_total_psd_caen_shifted_vs_light_cal ;
//spectrum_2D *TNEDA_crystal::spec_total_psd_lf_shifted_vs_light_cal ;
spectrum_2D *TNEDA_crystal::spec_total_psd_caen_shifted_vs_light_raw ;
spectrum_2D *TNEDA_crystal::spec_total_psd_lf_shifted_vs_light_raw ;
#define dcout   if(0)std::cout

//**************************************************************************
TNEDA_crystal::TNEDA_crystal(std::string nam, TNEDA_array *parent_ptr, int key_): parent_array(parent_ptr)
{
    //      std::cout << "Before constructor of crystal" << nam ;
    //            << ", addres of parent is "
    //            //<< parent_cluster
    //            << ", my address is " << this;
    //            << std::endl;

    my_selfgate_type = Tselfgate_type::galileo_ge;   // <---- important!

    key = key_;  //
    cluster_nr = 0 ; // used only for fan spec and for TGalileo Calibrated event

    name_of_this_element = nam;
    //    std::cout << name_of_this_element << std::endl;

    if(name_of_this_element.substr(0, 4) == "NEDA")
        flag_this_is_true_neda = true;
    else{
        flag_this_is_true_neda = false;
        // cerr << name_of_this_element << " id = " << id_number << " This is not a true neda" << endl;
    }

    // to be able to increment total fan spectrum we must know which
    // cluster it is

    //   cluster_nr = parent_cluster->give_cluster_id();

    //  std::cout << "This crystal has numbers " << cluster_nr << ", "
    //      << id_number << std::endl;

    //  energy_data = 0;
    //  time_data = 0;
    //  threshold_to_multiplicity = 100; // 100 keV


    good_polygon = nullptr;
    good_polygon_name = "no_polygon";


    create_my_spectra();

    //if(flag_this_is_true_neda)
    {
        named_pointer[name_of_this_element + "_qlong_when_good"]
                = Tnamed_pointer(&qlong, &flag_good_data, this);


        named_pointer[name_of_this_element + "_qshort_when_good"]
                = Tnamed_pointer(&qshort, &flag_good_data, this);

        named_pointer[name_of_this_element + "_qfast_when_good"]
                = Tnamed_pointer(&qfast, &flag_good_data, this);

        named_pointer[name_of_this_element + "_qslow_when_good"]
                = Tnamed_pointer(&qslow, &flag_good_data, this);

        named_pointer[name_of_this_element + "_light_raw_when_good"]
                = Tnamed_pointer(&light_raw, &flag_good_data, this);
        named_pointer[name_of_this_element + "_light_cal_when_good"]
                = Tnamed_pointer(&light_cal, &flag_good_data, this);

        named_pointer[name_of_this_element + "_psdcaen_shifted_when_good"]
                = Tnamed_pointer(&psdcaen_shifted , &flag_good_data, this);

        named_pointer[name_of_this_element + "_psdlf_shifted_when_good"]
                = Tnamed_pointer(&psdlf_shifted , &flag_good_data, this);

        named_pointer[name_of_this_element + "_psdcaen_when_good"]
                = Tnamed_pointer(&psdcaen, &flag_good_data, this);

        named_pointer[name_of_this_element + "_psdlf_when_good"]
                = Tnamed_pointer(&psdlf , &flag_good_data, this);

        named_pointer[name_of_this_element + "_rtime_when_good"]
                = Tnamed_pointer(&rtime , &flag_good_data, this);

    }
    
    // ALL
    {
        // local scope
        std::string entry_name;
        Tmap_of_named_pointers::iterator pos;

        Tnamed_pointer::add_as_ALL("ALL_Neda_light_cal_when_good", &light_cal, &flag_good_data, this);
        Tnamed_pointer::add_as_ALL("ALL_Neda_light_raw_when_good", &light_raw, &flag_good_data, this);

        Tnamed_pointer::add_as_ALL("ALL_Neda_qlong_when_good", &qlong, &flag_good_data, this);
        Tnamed_pointer::add_as_ALL("ALL_Neda_qfast_when_good", &qfast, &flag_good_data, this);
        Tnamed_pointer::add_as_ALL("ALL_Neda_qshort_when_good", &qshort, &flag_good_data, this);
        Tnamed_pointer::add_as_ALL("ALL_Neda_qslow_when_good", &qslow, &flag_good_data, this);
        Tnamed_pointer::add_as_ALL("ALL_Neda_psdlf_shifted_when_good", &psdlf_shifted, &flag_good_data, this);
        Tnamed_pointer::add_as_ALL("ALL_Neda_psdcaen_shifted_when_good", &psdcaen_shifted, &flag_good_data, this);
        Tnamed_pointer::add_as_ALL("ALL_Neda_rtime_when_good", &rtime, &flag_good_data, this);

    } // end of local
    

    // cout << "After constructior "; // << endl;
}
//************************************************************************
void TNEDA_crystal::create_my_spectra()
{
    //    std::cout << "before creating the spectra " ; // << std::endl;

    // the spectra have to live all the time, so here we can create
    // them during  the constructor

    std::vector<spectrum_abstr *> &gal_spec_ref = parent_array->spectra_vector;

    std::string folder = "NEDA/" + name_of_this_element;

    // energy -------------------------------------
    std::string name = name_of_this_element;
    /*  name += "_energy_raw";
   spec_energy_raw = new spectrum_1D(
   name,
   name,
   16382, -.5, 16381.5,
   folder,
   "",  // note
   name_of_this_element + "_energy_raw_when_good");
   gal_spec_ref.push_back(spec_energy_raw);
   */
    //-----------------------

    // ============== Spectrum of QShort measured by FPGA
    name = name_of_this_element;
    name+= "_qshort";
    spec_qshort = new spectrum_1D(name,
                                  name,
                                  2048,0,65535,
                                  folder,
                                  flag_this_is_true_neda ? "QShort integrated by FPGA" : "Always a constant value (one peak)",
                                  name_of_this_element + "_qshort_when_good");
    gal_spec_ref.push_back(spec_qshort);

    // ============== Spectrum of QLong measured by FPGA
    name = name_of_this_element;
    name+= "_qlong";
    spec_qlong = new spectrum_1D(name,
                                 name,
                                 2048,0,65535,
                                 folder,
                                 flag_this_is_true_neda ? "QLong integrated by FPGA" : "",
                                 name_of_this_element + "_qlong_when_good");
    gal_spec_ref.push_back(spec_qlong);

    // ============== Spectrum of QFast calculated by the LF
    name = name_of_this_element;
    name+= "_qfast";
    spec_qfast = new spectrum_1D(name,
                                 name,
                                 2048,0,65535,
                                 folder,
                                 flag_this_is_true_neda ? "QFast integrated by FPGA" : "",

                                 name_of_this_element + "_qfast_when_good");
    gal_spec_ref.push_back(spec_qfast);

    // ============== Spectrum of QFast calculated by the LF
    name = name_of_this_element;
    name+= "_qslow";
    spec_qslow = new spectrum_1D(name,
                                 name,
                                 2048,0,65535,
                                 folder,
                                 flag_this_is_true_neda ? "QSlow integrated by FPGA" : "",
                                 name_of_this_element + "_qslow_when_good");
    gal_spec_ref.push_back(spec_qslow);


    // ============== Spectrum of Light calculated by the LF
    name = name_of_this_element;
    name+= "_light_raw";
    spec_light_raw = new spectrum_1D(name,
                                     name,
                                     2048,0,65535*2,
                                     folder,
                                     flag_this_is_true_neda ? "Light_raw integrated by FPGA" : "",
                                     name_of_this_element + "_light_raw_when_good");
    gal_spec_ref.push_back(spec_light_raw);

    name = name_of_this_element;
    name+= "_light_cal";
    spec_light_cal = new spectrum_1D(name,
                                     name,
                                     2048,0,65535,
                                     folder,
                                     flag_this_is_true_neda ? "light_cal integrated by FPGA" : "",
                                     name_of_this_element + "_light_cal_when_good");
    gal_spec_ref.push_back(spec_light_cal);


    // ============== Spectrum of PSD vs QLong integrated by FPGA
    name = name_of_this_element + "_psd_vs_qlong_caen";
    spec_psd_caen_vs_qlong = new spectrum_2D(
                name,
                name,
                2048, 0, 32000,
                512,0,1,
                folder,
                "",  // note
                "X:\r " + name_of_this_element + "_qlong_when_good"
                + "\rY:\r " +  name_of_this_element + "_psdcaen_when_good");


    gal_spec_ref.push_back(spec_psd_caen_vs_qlong);


    if(flag_this_is_true_neda){

        //        // ============== Spectrum of psd vs Light integrated by LF
        //        name = name_of_this_element + "_psdlf_vs_light_cal";
        //        spec_psd_lf_vs_light_cal = new spectrum_2D(
        //                    name,
        //                    name,
        //                    2048, 0, 65535,
        //                    512,0,1,
        //                    folder,
        //                    "",  // note
        //                    "X:\r " + name_of_this_element + "_light_cal_when_good"
        //                    + "\rY:\r " +  name_of_this_element + "_psdlf_when_good");


        //        gal_spec_ref.push_back(spec_psd_lf_vs_light_cal);



        // ============== Spectrum of PSD calculated in  FPGA ======================
        name = name_of_this_element + "_psd_caen";
        spec_psd_caen = new spectrum_1D(
                    name,name,
                    1500, 0, 1.5,
                    folder,
                    "PSD calculated in FPGA",
                    name_of_this_element + "_psdcaen_when_good");


        gal_spec_ref.push_back(spec_psd_caen);

        name = name_of_this_element + "_psd_caen_shifted";
        spec_psd_caen_shifted = new spectrum_1D(
                    name,name,
                    1500, 0, 1.5,
                    folder,
                    "PSD calculated in FPGA (shifted)",
                    name_of_this_element + "_psdcaen_shifted_when_good");

        gal_spec_ref.push_back(spec_psd_caen_shifted);

        //        name = name_of_this_element + "_psd_caen_shifted_vs_light_cal";
        //        spec_psd_caen_shifted_vs_light_cal = new spectrum_2D(
        //                    name,
        //                    name,
        //                    1024, 0, 65535/2,
        //                    512,0,1,
        //                    folder,
        //                    "",  // note
        //                    "X:\r " + name_of_this_element + "_light_cal_when_good"
        //                    + "\rY:\r " +  name_of_this_element + "_psdlf_shifted_when_good");
        //        gal_spec_ref.push_back(spec_psd_caen_shifted_vs_light_cal);

        //        // ============== Spectrum of PSD calculated in  LF ======================
        name = name_of_this_element + "_psd_lf";
        spec_psd_lf = new spectrum_1D(
                    name, name,
                    1500, 0, 1.5,
                    folder,
                    "PSD calculated in LF",
                    name_of_this_element + "_psdlf_when_good");

        //    auto xmin = spec_psd_lf->give_root_spectrum_pointer()->GetXaxis()->GetXmin();
        //    auto xmax = spec_psd_lf->give_root_spectrum_pointer()->GetXaxis()->GetXmax();
        //     auto xnbins = spec_psd_lf->give_root_spectrum_pointer()->GetXaxis()->GetNbins();

        //cout << " xnbins  =  " << xnbins
        //     << " xminn  =  " << xmin
        //        << " xmaxn  =  " << xmax
        //     << endl;


        gal_spec_ref.push_back(spec_psd_lf);

        name = name_of_this_element + "_psd_lf_shifted";
        spec_psd_lf_shifted = new spectrum_1D(
                    name, name,
                    1500, 0, 1.5,
                    folder,
                    "PSD calculated in LF (shifted)",
                    name_of_this_element + "_psdlf_shifted_when_good");

        gal_spec_ref.push_back(spec_psd_lf_shifted);

        //        name = name_of_this_element + "_psd_lf_shifted_vs_light_cal";
        //        gal_spec_ref.push_back(
        //                    spec_psd_lf_shifted_vs_light_cal = new spectrum_2D(
        //                    name,
        //                    name,
        //                    1024, 0, 65535/2,
        //                    512,0,1,
        //                    folder,
        //                    "",  // note
        //                    "X:\r " + name_of_this_element + "_light_cal_when_good"
        //                    + "\rY:\r " +  name_of_this_element + "_psdlf_shifted_when_good")
        //                );
        //========================================
        // TOTALS

        //        if(spec_total_psd_caen_shifted_vs_light_cal == nullptr){


        //            name = "NEDA_total_psd_caen_shifted_vs_light_cal";
        //            cout << "creating spec " << name<< " for " <<  name_of_this_element<< endl;
        //            gal_spec_ref.push_back(
        //                        spec_total_psd_caen_shifted_vs_light_cal = new spectrum_2D(
        //                        name,
        //                        name,
        //                        1024, 0, 65535/2,
        //                        512,0,1,
        //                        folder,
        //                        "",  // note
        //                        "X:\r ALL_Neda_light_cal_when_good"
        //                        "\rY:\r ALL_Neda_psdcaen_shifted_when_good")
        //                    );
        //        }


        //        if(!spec_total_psd_lf_shifted_vs_light_cal)
        //        {
        //            name = "NEDA_total_psd_lf_shifted_vs_light_cal";
        //            gal_spec_ref.push_back(
        //                        spec_total_psd_lf_shifted_vs_light_cal = new spectrum_2D(
        //                        name,
        //                        name,
        //                        1024, 0, 65535/2,
        //                        512,0,1,
        //                        folder,
        //                        "",  // note
        //                        "X:\r ALL_Neda_light_when_good"
        //                        "\rY:\r ALL_Neda_psdlf_shifted_when_good")
        //                    );
        //        }

        if(spec_total_psd_caen_shifted_vs_light_raw == nullptr){


            name = "NEDA_total_psd_caen_shifted_vs_light_raw";
            //             cout << "creating spec " << name<< " for " <<  name_of_this_element<< endl;
            gal_spec_ref.push_back(
                        spec_total_psd_caen_shifted_vs_light_raw = new spectrum_2D(
                        name,
                        name,
                        1024, 0, 65535*2,
                        512,0,1,
                        folder,
                        "",  // note
                        "X:\r ALL_Neda_light_raw_when_good"
                        "\rY:\r ALL_Neda_psdcaen_shifted_when_good")
                    );
        }


        if(!spec_total_psd_lf_shifted_vs_light_raw)
        {
            name = "NEDA_total_psd_lf_shifted_vs_light_raw";
            gal_spec_ref.push_back(
                        spec_total_psd_lf_shifted_vs_light_raw = new spectrum_2D(
                        name,
                        name,
                        1024, 0, 65535*2,
                        512,0,1,
                        folder,
                        "",  // note
                        "X:\r ALL_Neda_light_raw_when_good"
                        "\rY:\r ALL_Neda_psdlf_shifted_when_good")
                    );
        }

    } // endif flag_true_neda


    //    std::cout << "After creating the spectra " ; //<< std::endl;

}
//**********************************************************************
void TNEDA_crystal::analyse_current_event()
{
    //     std::cout << " analyse_current_event()  for "
    //          << name_of_this_element
    //          << std::endl;

    // checking if it is something (not zero) -- is made during unpacking

    zeroing_good_flags();


    if (TNEDA_array::tmp_event_unpacked->neda_fired.find(key)
            ==
            TNEDA_array::tmp_event_unpacked->neda_fired.end()) return;
    //----------------
    // so it fired !
    if(flag_this_is_true_neda)TNEDA_array::multiplicity_raw++;

    //     cout << "Nedda " << name_of_this_element << ", index = " << key  << " fired " << endl;

    time_stamp = TNEDA_array::tmp_event_unpacked->neda_data[key].timestamp;
    qshort     = TNEDA_array::tmp_event_unpacked->neda_data[key].qshort   ;
    qlong      = TNEDA_array::tmp_event_unpacked->neda_data[key].qlong    ;
    qfast      = TNEDA_array::tmp_event_unpacked->neda_data[key].qfast    ;
    qslow      = TNEDA_array::tmp_event_unpacked->neda_data[key].qslow    ;
    light_raw      = TNEDA_array::tmp_event_unpacked->neda_data[key].light    ;
    cfd        = TNEDA_array::tmp_event_unpacked->neda_data[key].cfd      ;
    intrise    = TNEDA_array::tmp_event_unpacked->neda_data[key].intrise  ;
    nbzero     = TNEDA_array::tmp_event_unpacked->neda_data[key].nbzero   ;
    flags      = TNEDA_array::tmp_event_unpacked->neda_data[key].flags    ;

    flag_fired = (qlong > 0) || (qfast > 0);

    if(flag_fired and flag_this_is_true_neda) {
        TNEDA_array::multiplicity_good++;
        flag_good_data=true;

        double time_raw = (time_stamp * 10) +
                (cfd +
                 gRandom->Uniform(-5, 5)
                 ) /1000 ;// Marcin decided this formula

        //        cout << "Random -5, 5 = " << gRandom->Uniform(-5, 5)
        //             << endl;

        //        cout << "Wg formuly marcina = real_time_raw tego detektora = "
        //         << setprecision(25) << time_raw << endl;


        aligned_real_time =  time_raw + alignment_correcting_value ;
        rtime  =aligned_real_time;

        //        cout << "Po align  real_time tego detektora = "
        //             << aligned_real_time << " det=" << name_of_this_element << endl;
    }

    if(key == 92 ) // if this is trigger data comming in this channel 92
    {
        [[maybe_unused]]  static long licznik_sekund ;
        TNEDA_array::tmp_event_unpacked->LTRIG_data =
                TNEDA_array::tmp_event_unpacked->neda_data[key].qlong ;
        //        TNEDA_array::tmp_event_unpacked->timestamp =
        //                ++licznik_sekund;
        //                time(nullptr);

        //        cout << "Wartość triggera " <<  TNEDA_array::tmp_event_unpacked->LTRIG_data
        //             << " - seconds " <<  TNEDA_array::tmp_event_unpacked->seconds << endl;
    }

    if (GalileoAnalysis_ptr->is_verbose_on())
        if(flag_this_is_true_neda )
        {
            std::cout  << name_of_this_element
                       << " id_number= "  << key
                       <<" timestamp = " << time_stamp
                         //                  << ", QFast =  " //<< std::setw(9)
                         //                  << qfast
                         //                  << ", QSlow= " //<< std::setw(9)
                         //                  << qslow
                         //                 << ", Light= " //<< std::setw(9)
                         //                  << light
                         //                  << ", qlong= " //<< std::setw(9)
                         //                  << qlong
                      << ", cfd= " << std::setw(9)
                      << cfd
                      << ", real_time= " << std::setw(9)
                      << rtime
                      << ", aligned_real_time= " << std::setw(9)
                      << aligned_real_time
                      << std::endl; // New line will be after the LP, SR printout
        }

    //    if(name_of_this_element.substr(0,3) == "LTR")
    //    {
    //        std::cout << "JESAT LTRIG ==========================================="
    //             << std::endl;
    //    }

    /* Note: incrementing the raw spectra is done only when multiplicity in this cluster is 1.yy
          all this is done from Trising class, by calling
          Tcluster_xia::calculate_single_spectrum()
          The "missing" instructions which were here - are moved just there
          */

    //===================================================
    // energy calibration  - second time (sometimes the hardware calibarion is not good enough )
    //if (energy_calibrated == energy_raw)
    // in the even it can come already calibarted



    //=================================================
    // time calibration not needed, it is made in hardware

    //###########################################################

    if (GalileoAnalysis_ptr->is_verbose_on()) {
        std::cout << std::endl;
    }


    //================================
    // GOOD
    //================================
    /*  if (flag_good_wAmpli_vs_eAmpli_polygon_needed &&
          good_polygon->Test(energy_calibrated, Ampli_energy_calibrated) == true){
          (flag_good_energy_requires_threshold  ?
          (energy_calibrated >=  energy_threshold_lower && energy_calibrated <=  energy_threshold_upper)
          :
          true)
          &&
          (flag_good_time_requires_threshold  ?   // acutally now this is not
          // a threshold, but the gate
          (time >=  good_time_threshold_lower)
          :
          true)
          &&
          (flag_good_time_requires_threshold  ?
          (time <=  good_time_threshold_upper)
          :
          true)


          )
          {
          flag_good_data = true;
          #ifdef ADDBACK_NEEDED
          flag_good_nonaddbacked_crystal = true; // later can made false by the multiaddback routine
          #endif

          }


          // this value does not have to be zeroed,because anyway it is validated
          // by the flag good_data, so the value is always there, but the validator sometimes in
          // false

          std::cout
          <<"Cluster " << cluster_nr <<" "<<id_number<<std::endl
          << energy_calibrated<<" "<<time
          << std::endl;
          */


    if(flag_this_is_true_neda){

        // Light calibration

        //        cout << name_of_this_element <<  ", light raw = " << light << endl;
        //        cout << "calibr factors = " ;
        light_cal = 0;
        double tmp_val = light_raw + gRandom->Uniform(-.5,.5);
        for(UInt_t i=0; i < energy_calibr_factors.size();++i){
            //            cout << energy_calibr_factors[i] << ",  " ;
            light_cal += energy_calibr_factors[i] * pow(tmp_val,(int)i);

        }

        //        cout << name_of_this_element <<  ", light cal = " << light_cal << endl;



        psdcaen = -1;
        if(qshort>0) {
            psdcaen = (qlong-qshort)/qshort;
            //psdcaen = 0.6;   //FAKE
            spec_psd_caen->increment((psdcaen));
            spec_psd_caen_vs_qlong->increment(qlong, psdcaen);
        }

        psdlf =-1.;
        if(qfast>0) {
            psdlf = qslow/qfast;

            //std::cout << "psd_lf = " << psd_lf << std::endl;

            spec_psd_lf->increment(psdlf);
            //            spec_psd_lf_vs_light_cal->increment(light_cal, psdlf);
        }


        // shifting both psd ----------------------------------------

        //#define REQUESTED_PSDLF_POSITION  0.1

        //  psdlf_shifted = psdlf - psdlf_position[det_id] +
        //           REQUESTED_PSDLF_POSITION;
        //                 //  det_id = {0...51}

        psdlf_shifted    = psdlf - position_psdlf   +  REQUESTED_PSDLF_POSITION;
        psdcaen_shifted = psdcaen - position_psdcaen + REQUESTED_PSDCAEN_POSITION;

        //    if(name_of_this_element == "NEDA_01")
        //    {
        //        cout << "psdlf = "<< psdlf
        //             << " position_psdlf = "  << position_psdlf
        //             << " psdlf - position_psdlf  = " << psdlf - position_psdlf
        //             << " REQUESTED_PSDLF_POSITION = " << REQUESTED_PSDLF_POSITION
        //             << " psdlf_shifted = " << psdlf_shifted
        //             << endl;
        //    }



        spec_psd_lf_shifted->increment(psdlf_shifted);
        spec_psd_caen_shifted->increment(psdcaen_shifted);

        parent_array->spec_total_psd_lf_shifted->increment(psdlf_shifted);
        parent_array
                ->spec_total_psd_caen_shifted->increment(psdcaen_shifted);

        //        spec_psd_caen_shifted_vs_light_cal->increment(light_cal, psdcaen_shifted);
        //        spec_psd_lf_shifted_vs_light_cal->increment(light_cal, psdlf_shifted);

        //        spec_psd_caen_shifted_vs_light_cal->increment(light_cal, psdcaen_shifted);
        //        spec_psd_lf_shifted_vs_light_cal->increment(light_cal, psdlf_shifted);

        // TOTAL
        spec_total_psd_caen_shifted_vs_light_raw->increment(light_raw, psdcaen_shifted);
        spec_total_psd_lf_shifted_vs_light_raw->increment(light_raw, psdlf_shifted);
        //        spec_total_psd_caen_shifted_vs_light_cal->increment(light_cal, psdcaen_shifted);
        //        spec_total_psd_lf_shifted_vs_light_cal->increment(light_cal, psdlf_shifted);
    }
    //-------------------------------------------------
    increment_crystal_spectra_raw_and_fan();



    //  std::cout << " end of analyse_current_event()  for "
    //        << name_of_this_element
    //        << std::endl;
}

//**************************************************************************
void  TNEDA_crystal::make_preloop_action(TNEDADetector *det)
{

    energy_calibr_factors.clear();
    energy_calibr_factors=det->GetCalibrFactors();

    detector_position = det->GetPosition();
    det_name_nr = std::atoi(det->GetName().c_str());
    //    cout << "Neda Detector " << det->GetName().c_str()
    //         << "  energy_calibr_factors = " <<
    //            energy_calibr_factors[0]
    //         << ", " << energy_calibr_factors[1]
    //         << endl;


    //read shift values
    if(flag_this_is_true_neda){
        if(!read_shift_values() )
        {
            exit(100);
        }
    }

    if(flag_this_is_true_neda){
        // opening file with alignment for real_time
        // neda_time_positions
        parent_array->file_time_alignment.seekg(0); // rewind
        int nr = 0;
        double value = 0;
        double value_zero = 0;
        // read for neda 0
        parent_array->file_time_alignment >> nr >> value_zero;
        // read for me
        parent_array->file_time_alignment.seekg(0);
        for(int i = 0 ;  ; ++i)
        {
            parent_array->file_time_alignment >> nr >> value;
            //            cout << "nr = " << nr
            //                 << ", value = " << value << endl;

            if(det_name_nr == nr){
                alignment_correcting_value  = -value_zero +  value;
                //                cout << "FOUND nr = " << nr
                //                     << ", value = " << value
                //                     << ", alignment_correcting_value = "
                //                        << alignment_correcting_value
                //                        << endl;
                break;
            }
            if(!parent_array->file_time_alignment) break;
        }
    }


}
//**********************************************************************
void TNEDA_crystal::step2_make_rtime(double time_ref)
{
    rtime -= time_ref;

    //    cout << "Obliczony real time w ramach eventu = " << real_time
    //         << " d = "  << name_of_this_element << endl;
    // Perhaps some spectra incrementaion here

}
//**********************************************************************
void TNEDA_crystal::set_event_ptr_for_crystal_data(int /* ptr[3]*/ )
{}
//**********************************************************************
void TNEDA_crystal::check_legality_of_data_pointers()
{}
//**********************************************************************
bool TNEDA_crystal::read_shift_values()
{
    // files were open in TNEDA_array::read_calibration_from_lookuptable()
    // and will be closed there

    string filename = parent_array->filename_psd_caen;
    try{
        string keyword = "REQUESTED_PSDCAEN_POSITION";
        REQUESTED_PSDCAEN_POSITION = fif(parent_array->file_psd_caen, keyword);
        // read shift for particular detector
        keyword = name_of_this_element + "_position_psdcaen";
        position_psdcaen =  fif(parent_array->file_psd_caen, keyword);

        filename = parent_array->filename_psd_lf;

        keyword = "REQUESTED_PSDLF_POSITION";
        REQUESTED_PSDLF_POSITION = fif(parent_array->file_psd_lf, keyword);
        keyword = name_of_this_element + "_position_psdlf";
        position_psdlf=  fif(parent_array->file_psd_lf, keyword);

        return true;
    }catch(Tno_keyword_exception c)
    {
        cerr << c.message << " "
             << filename
             << endl;
        return false;
    }
    catch(Treading_value_exception c)
    {
        cerr << c.message
             << " "
             << filename
             << endl;
        return false;
    }
}
//*******************************************************************************
/** No descriptions */
//void TNEDA_crystal::align_neda_times_to_neda_nr_0()
//{
//    aligned_real_time =  real_time + alignment_correcting_value ;

//    // event.neda[i_list].time =   event.neda[i_list].time - card_shifts[id/16+2];
//}


//*******************************************************************************
/** No descriptions */
void TNEDA_crystal::zeroing_good_flags()
{
    flag_fired =
            flag_good_data =   false;

    qshort     = 0.;
    qlong      = 0.;
    qfast      = 0.;
    qslow      = 0.;
    light_raw      = 0.;
    cfd        = 0.;
    intrise    = 0.;
    nbzero     = 0 ;
    flags      = 0 ;
    time_stamp = 0 ;
    rtime = 0;

    light_cal = 0;

    pseudo_fan = -1; // zero would mean something!

}
/******************************************************************************/
bool TNEDA_crystal::check_selfgate(Tself_gate_abstract_descr  *   /* desc */ )
{



    return true; // when EVERTHING was successful
}

//************************************************************************
/** No descriptions */
void TNEDA_crystal::increment_crystal_spectra_raw_and_fan()
{
    /*std::cout
   <<"Cluster " << cluster_nr <<" "<<id_number<<std::endl
   << energy_calibrated<<" "<<time
   <<" "<<energy20_calibrated<<std::endl;
   */

    if ( qshort >=1 ) spec_qshort->increment(qshort);
    if ( qlong  >=1 ) spec_qlong->increment(qlong);
    if ( qfast  >=1 ) spec_qfast->increment(qfast);
    if ( qslow  >=1 ) spec_qslow->increment(qslow);
    if ( light_raw  >=1 ) spec_light_raw->increment(light_raw);
    if ( light_cal  >=1 ) spec_light_cal->increment(light_cal);


    // ----------

    //  if (Ge_time >= 1  || flag_increment_time_with_zero) {
    //    spec_time->increment(Ge_time);
    //    spec_total_time->increment(Ge_time);
    //  }
    //  if (energy_raw >= 1)spec_energy_raw->increment(energy_raw);
}
//**************************************************************************
/** No descriptions */
void TNEDA_crystal::increment_crystal_spectra_cal_and_doppler()
{
    /*std::cout
   <<"Cluster " << cluster_nr <<" "<<id_number<<std::endl
   << energy_calibrated<<" "<<time
   <<std::endl;
   */

    /*  if (energy_calibrated >= 1) {
   sum_energy_cal += energy_calibrated;
   }
   */
    // incrementing calibrated versions of the spectrum
    //std::cout << energy_raw << "  " << energy_calibrated << std::endl;
    // if (energy_raw >= 1  || flag_increment_energy_cal_with_zero) {
    //   spec_energy_cal->increment(energy_calibrated);
    //   spec_total_energy_cal->increment(energy_calibrated);
    // }
    //
    //
    //
    // if ( (Ge_time >= 1  || flag_increment_time_with_zero)
    //     &&
    //     (energy_calibrated >= 1  || flag_increment_energy_cal_with_zero)) {
    //   spec_energy_cal_vs_time->increment(Ge_time, energy_calibrated);
    //   spec_total_energy_cal_vs_time->increment(Ge_time, energy_calibrated);
    // }

}
//**************************************************************************
/** No descriptions */
void TNEDA_crystal::process_neda()   // to jest już zrobione w anayse_current event
{
#define REQ_PSDLF_POS    0.15
#define REQ_PSDCAEN_POS  0.15



    if (qshort>0) {
        psdcaen =  (qlong - qshort)/qshort -
                position_psdcaen + REQ_PSDCAEN_POS;
    }

    if (qslow >0) {
        psdlf  =   qslow/qfast -
                position_psdlf + REQ_PSDLF_POS;
    }


}
