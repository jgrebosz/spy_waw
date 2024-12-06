#include "TDiamant_crystal.h"


#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include "TDiamant_array.h"
#include "Ttarget.h"
#include "TNeedle.h"
#include "Tnamed_pointer.h"
#include "Tself_gate_ge_galileo_descr.h"
#include "TGalileoAnalysis.h" // for verbose mode

using namespace std;
#define fif(x,y) Tfile_helper::find_in_file((x),(y));

//TDiamant_array    *TDiamant_crystal::array_nw_pointer;  // static pointer


bool TDiamant_crystal::flag_increment_energy_cal_with_zero;
bool TDiamant_crystal::flag_increment_time_with_zero;

bool   TDiamant_crystal::flag_good_energy_requires_threshold;
double TDiamant_crystal::energy_threshold_lower;
double TDiamant_crystal::energy_threshold_upper;

bool   TDiamant_crystal::flag_good_time_requires_threshold;
double TDiamant_crystal::good_time_threshold_lower; // this is the new style
double TDiamant_crystal::good_time_threshold_upper; // this is the new style

//static
TDiamant_array  *TDiamant_crystal::parent_pointer;
// static
spectrum_1D *TDiamant_crystal::spec_fan;
//bool TDiamant_crystal::flag_good_wAmpli_vs_eAmpli_polygon_needed;


//std::string TDiamant_crystal::good_polygon_name;

// static

//spectrum_2D *TDiamant_crystal::spec_total_energy_cal_vs_pid;
//spectrum_2D *TDiamant_crystal::spec_total_psd_caen_shifted_vs_light ;
//spectrum_2D *TDiamant_crystal::spec_total_psd_lf_shifted_vs_light ;

#define dcout   if(0)std::cout

//**************************************************************************
TDiamant_crystal::TDiamant_crystal(std::string nam, int id)

{
    //      std::cout << "Before constructor of crystal" << nam ;
    //            << ", addres of parent is "
    //            //<< parent_cluster
    //            << ", my address is " << this;
    //            << std::endl;

    my_selfgate_type = Tselfgate_type::galileo_ge;   // <---- important!


    name_of_this_element = nam;
    //    std::cout << name_of_this_element << std::endl;
    id_number = id;


    // to be able to increment total fan spectrum we must know which


    //   cluster_nr = parent_cluster->give_cluster_id();

    //  std::cout << "This crystal has numbers " << cluster_nr << ", "
    //      << id_number << std::endl;

    //  energy_data = 0;
    //  time_data = 0;
    //  threshold_to_multiplicity = 100; // 100 keV




    create_my_spectra();

    //if(flag_this_is_true_neda)
    {
        //        named_pointer[name_of_this_element + "_qlong_when_good"]
        //                = Tnamed_pointer(&qlong, &flag_good_data, this);


        //        named_pointer[name_of_this_element + "_qshort_when_good"]
        //                = Tnamed_pointer(&qshort, &flag_good_data, this);

        //        named_pointer[name_of_this_element + "_qfast_when_good"]
        //                = Tnamed_pointer(&qfast, &flag_good_data, this);

        //        named_pointer[name_of_this_element + "_qslow_when_good"]
        //                = Tnamed_pointer(&qslow, &flag_good_data, this);

        //        named_pointer[name_of_this_element + "_light_when_good"]
        //                = Tnamed_pointer(&light, &flag_good_data, this);

        //        named_pointer[name_of_this_element + "_psdcaen_shifted_when_good"]
        //                = Tnamed_pointer(&psdcaen_shifted , &flag_good_data, this);

        //        named_pointer[name_of_this_element + "_psdlf_shifted_when_good"]
        //                = Tnamed_pointer(&psdlf_shifted , &flag_good_data, this);

        //        named_pointer[name_of_this_element + "_psdcaen_when_good"]
        //                = Tnamed_pointer(&psdcaen, &flag_good_data, this);

        //        named_pointer[name_of_this_element + "_psdlf_when_good"]
        //                = Tnamed_pointer(&psdlf , &flag_good_data, this);

    }
    
    // ALL
    {
        // local scope
        std::string entry_name;
        Tmap_of_named_pointers::iterator pos;

        //        Tnamed_pointer::add_as_ALL("ALL_Neda_light_when_good", &light, &flag_good_data, this);

        //        Tnamed_pointer::add_as_ALL("ALL_Neda_qlong_when_good", &qlong, &flag_good_data, this);
        //        Tnamed_pointer::add_as_ALL("ALL_Neda_qfast_when_good", &qfast, &flag_good_data, this);
        //        Tnamed_pointer::add_as_ALL("ALL_Neda_qshort_when_good", &qshort, &flag_good_data, this);
        //        Tnamed_pointer::add_as_ALL("ALL_Neda_qslow_when_good", &qslow, &flag_good_data, this);
        //        Tnamed_pointer::add_as_ALL("ALL_Neda_psdlf_shifted_when_good", &psdlf_shifted, &flag_good_data, this);
        //        Tnamed_pointer::add_as_ALL("ALL_Neda_psdcaen_shifted_when_good", &psdcaen_shifted, &flag_good_data, this);


    } // end of local
    

    // cout << "After constructior "; // << endl;
}
//************************************************************************
void TDiamant_crystal::create_my_spectra()
{
    //    std::cout << "before creating the spectra " ; // << std::endl;

    // the spectra have to live all the time, so here we can create
    // them during  the constructor

    std::vector<spectrum_abstr *> &gal_spec_ref = parent_pointer->spectra_vector;

    std::string folder = "NEDA/" + name_of_this_element;

    // energy -------------------------------------
    std::string name = name_of_this_element;

    //-----------------------

    // ============== Spectrum of QShort measured by FPGA
    name = name_of_this_element;
    name+= "_energy";

    gal_spec_ref.push_back(spec_energy_raw = new spectrum_1D(name,
                                                             name,
                                                             2048,0,8192,
                                                             folder,
                                                             "Not ready ",
                                                             name_of_this_element + "_something..."));



    name = name_of_this_element;
    name+= "_top";
    gal_spec_ref.push_back(spec_top = new spectrum_1D(name,
                                                      name,
                                                      2048,0,8192,
                                                      folder,
                                                      "Top",
                                                      name_of_this_element + "_top")
            );


    name = name_of_this_element;
    name+= "_pid";
    gal_spec_ref.push_back(spec_pid = new spectrum_1D(name,
                                                      name,
                                                      500,0, 1,
                                                      folder,
                                                      "PID",
                                                      name_of_this_element + "_pid")
            );

    // ============== Spectrum of Energy vs. pid
    name = name_of_this_element + "_pid_vs_energy";
    spec_pid_vs_energy = new spectrum_2D(
                name,
                name,
                512, 0, 8192,
                500,0, 1,
                folder,
                "",  // note
                "X:\r " + name_of_this_element + "energy_when_good"
                + "\rY:\r " +  name_of_this_element + "_pid_when_good" );


    gal_spec_ref.push_back(spec_pid_vs_energy);



    //========================================
    // TOTALS

    if(spec_fan == nullptr)
    {


        // FAN spectrum of all crystals----------------------------------------
        name = "diamant_array_fan"  ;
        spec_fan = new spectrum_1D(
                    name,
                    name,
                    700, 0, 700,
                    folder,
                    "Detectors statistics displayed every 10 channels" ,

                    ""
                    );
        gal_spec_ref.push_back(spec_fan) ;


    }




    //    std::cout << "After creating the spectra " ; //<< std::endl;

}
//**********************************************************************
void TDiamant_crystal::analyse_current_event()
{
    //     std::cout << " analyse_current_event()  for "
    //          << name_of_this_element
    //          << std::endl;

    // checking if it is something (not zero) -- is made during unpacking

    zeroing_good_flags();


    if (! TDiamant_array::tmp_event_unpacked->diamant_fired[id_number])
        return;
    //----------------
    // so it fired !
    TDiamant_array::multiplicity_raw++;

    //     cout << "Nedda " << name_of_this_element << ", index = " << key  << " fired " << endl;

    //time_stamp = TDiamant_array::tmp_event_unpacked->diamant_data.timestamp;
    energy_raw     = TDiamant_array::tmp_event_unpacked->   diamant_data[id_number][0]   ;
    top = TDiamant_array::tmp_event_unpacked->   diamant_data[id_number][1]   ;

    flag_fired = true;
    if(flag_fired ) {
        TDiamant_array::multiplicity_good++;
        flag_good_data=true;

    }

    spec_energy_raw->increment(energy_raw);
    spec_top->increment(top);
    pid = 1. - (top * 1./ energy_raw);
    spec_pid->increment(pid);

    spec_pid_vs_energy->increment(energy_raw, pid);

    spec_fan->increment(10*id_number);

    if (GalileoAnalysis_ptr->is_verbose_on())
    {
        cout  << name_of_this_element
              << " id_number= "  << id_number
              <<" energy_raw  " << energy_raw
             << ", top = " << top
             << endl; // New line will be after the LP, SR printout
    }

    //    if(name_of_this_element.substr(0,3) == "LTR")
    //    {
    //        std::cout << "JESAT LTRIG ==========================================="
    //             << std::endl;
    //    }

    /* Note: incrementing the raw spectra is done only when multiplicity in this cluster is 1.
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

    // this value does not have to be zeroed,because anyway it is validated
    // by the flag good_data, so the value is always there, but the validator sometimes in
    // false







    //-------------------------------------------------
    increment_crystal_spectra_raw_and_fan();



    //  std::cout << " end of analyse_current_event()  for "
    //        << name_of_this_element
    //        << std::endl;
}

//**************************************************************************
void  TDiamant_crystal::make_preloop_action()
{

    //    energy_calibr_factors.clear();
    //    energy_calibr_factors=det->GetCalibrFactors();

    //    detector_position = det->GetPosition();
    //    det_name_nr = std::atoi(det->GetName().c_str());
    ////    cout << "Detector " << det->GetName().c_str()
    ////         << ", Has det_id =" << det_id
    ////         << endl;


    //    //read shift values
    //    if(flag_this_is_true_neda){
    //        if(!read_shift_values() )
    //        {
    //            exit(100);
    //        }
    //    }

    //    if(flag_this_is_true_neda)
    //    {
    //        // opening file with alignment for real_time
    //        // neda_time_positions
    //        parent_array->file_time_alignment.seekg(0); // rewind
    //        int nr = 0;
    //        double value = 0;
    //        double value_zero = 0;
    //        // read for neda 0
    //        parent_array->file_time_alignment >> nr >> value_zero;
    //        // read for me
    //        parent_array->file_time_alignment.seekg(0);
    //        for(int i = 0 ;  ; ++i)
    //        {
    //            parent_array->file_time_alignment >> nr >> value;
    ////            cout << "nr = " << nr
    ////                 << ", value = " << value << endl;

    //            if(det_name_nr == nr){
    //                alignment_correcting_value  = -value_zero +  value;
    ////                cout << "FOUND nr = " << nr
    ////                     << ", value = " << value
    ////                     << ", alignment_correcting_value = "
    ////                        << alignment_correcting_value
    ////                        << endl;
    //                break;
    //            }
    //            if(!parent_array->file_time_alignment) break;
    //        }
    //    }


}

//**********************************************************************

//**********************************************************************
void TDiamant_crystal::check_legality_of_data_pointers()
{}
////**********************************************************************
//bool TDiamant_crystal::read_shift_values()
//{
//    // files were open in TDiamant_array::read_calibration_from_lookuptable()
//    // and will be closed there

//    string filename = parent_array->filename_psd_caen;
//    try{
//        string keyword = "REQUESTED_PSDCAEN_POSITION";
//        REQUESTED_PSDCAEN_POSITION = fif(parent_array->file_psd_caen, keyword);
//        // read shift for particular detector
//        keyword = name_of_this_element + "_position_psdcaen";
//        position_psdcaen =  fif(parent_array->file_psd_caen, keyword);

//        filename = parent_array->filename_psd_lf;

//        keyword = "REQUESTED_PSDLF_POSITION";
//        REQUESTED_PSDLF_POSITION = fif(parent_array->file_psd_lf, keyword);
//        keyword = name_of_this_element + "_position_psdlf";
//        position_psdlf=  fif(parent_array->file_psd_lf, keyword);

//        return true;
//    }catch(Tno_keyword_exception c)
//    {
//        cerr << c.message << " "
//             << filename
//             << endl;
//        return false;
//    }
//    catch(Treading_value_exception c)
//    {
//        cerr << c.message
//             << " "
//             << filename
//             << endl;
//        return false;
//    }
//}
//*******************************************************************************
/** No descriptions */
//void TDiamant_crystal::align_neda_times_to_neda_nr_0()
//{
//    aligned_real_time =  real_time + alignment_correcting_value ;

//    // event.neda[i_list].time =   event.neda[i_list].time - card_shifts[id/16+2];
//}


//*******************************************************************************
/** No descriptions */
void TDiamant_crystal::zeroing_good_flags()
{
    flag_fired =
            flag_good_data =   false;

    energy_raw  = 0.;
    top = 0 ;
    pid = 0;

    pseudo_fan = -1; // zero would mean something!

}
/******************************************************************************/
bool TDiamant_crystal::check_selfgate(Tself_gate_abstract_descr *desc)
{



    return true; // when EVERYTHING was successful
}

//************************************************************************
/** No descriptions */
void TDiamant_crystal::increment_crystal_spectra_raw_and_fan()
{
    /*std::cout
   <<"Cluster " << cluster_nr <<" "<<id_number<<std::endl
   << energy_calibrated<<" "<<time
   <<" "<<energy20_calibrated<<std::endl;
   */

    //    if ( qshort >=1 ) spec_qshort->increment(qshort);
    //    if ( qlong  >=1 ) spec_qlong->increment(qlong);
    //    if ( qfast  >=1 ) spec_qfast->increment(qfast);
    //    if ( qslow  >=1 ) spec_qslow->increment(qslow);
    //    if ( light  >=1 ) spec_light->increment(light);


    // ----------

    //  if (Ge_time >= 1  || flag_increment_time_with_zero) {
    //    spec_time->increment(Ge_time);
    //    spec_total_time->increment(Ge_time);
    //  }
    //  if (energy_raw >= 1)spec_energy_raw->increment(energy_raw);
}
//**************************************************************************
/** No descriptions */
void TDiamant_crystal::increment_crystal_spectra_cal_and_doppler()
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
//void TDiamant_crystal::process_neda()   // to jest już zrobione w anayse_current event
//{
//#define REQ_PSDLF_POS    0.15
//#define REQ_PSDCAEN_POS  0.15



//    if (qshort>0) {
//      psdcaen =  (qlong - qshort)/qshort -
//                position_psdcaen + REQ_PSDCAEN_POS;
//    }

//    if (qslow >0) {
//      psdlf  =   qslow/qfast -
//                position_psdlf + REQ_PSDLF_POS;
//    }


//}
