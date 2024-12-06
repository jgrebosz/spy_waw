#include "TEAGLE_array.h"
#include <dirent.h> // for scandir
//#include <malloc.h> // for scandir
#include <stdlib.h>

#include <sstream>
#include "TGalileoAnalysis.h"
#include "Tnamed_pointer.h"

using namespace std;
TGalileoCalibratedEvent *TEAGLE_array::tmp_event_calibrated;
TGalileoEvent *TEAGLE_array::tmp_event_unpacked;

int TEAGLE_array::multiplicity_raw;
int TEAGLE_array::multiplicity_good;
std::map<int,TEAGLEDetector*> TEAGLE_array::lookup;

double GE_Eraw_max_range = 16384 ;
double GE_Ecal_max_range =  4096 ;
double GE_Time_max_range =  4096 ;


//double AC_Eraw_max_range = 4096 ;   ???????????????
//double AC_Ecal_max_range = 4096 ;
//double AC_Time_max_range = 4096 ;


//extern int local_mult_tmp ;
// const std::string TEAGLE_array::cluster_characters =
//     (HOW_MANY_GE_CRYSTALS == 8) ? ("ABCDGJKL")
//     : ("ABCDEFGJKLMNPQR") ;
//*******************************************************
//*******************************************************
//         Germanium detectors array class
//*******************************************************
//*******************************************************
TEAGLE_array::TEAGLE_array(std::string name) : TNeedle_element(name),
    spectra_vector(*GalileoAnalysis_ptr->give_gal_ptr()->address_spectra_frs())
{
    cout << "\n\n\n\nConstructor of the TEAGLE_array  =========================== " << std::endl ;
    //Tcrystal::rising_pointer = this ; // for crystals

    TEAGLE_crystal::array_nw_pointer = this ; // for crystals

    //Tcluster::rising_pointer = this ; // for cluster

    //     dynamic_cast<TRisingAnalysis*> (TGo4Analysis::Instance());

    galileo_pointer = GalileoAnalysis_ptr->give_gal_ptr()  ;  // we will need if to ask traget for theta angles
    //spectra_vector = gal_pointer->address_spectra_frs();
    //gal_pointer->address_spectra_frs();

    //---------------------------------------------
    // constructing the current setup of Galileo
    //---------------------------------------------
    detector_eagle.clear();
    TEAGLE_array::lookup = galileo_pointer->Get_EAGLE_LookUpTable();
    std::map<int,TEAGLEDetector*>::iterator it_ = TEAGLE_array::lookup.begin();

    for (; it_!=TEAGLE_array::lookup.end() ; ++it_) {
        if(it_->second){
             std::cout << "Making detector named: Ge_" << it_->second->GetName().c_str() << std::endl;
            detector_eagle[it_->first] = new TEAGLE_crystal(
                        //Form("Ge_%.3i_%s",it_->first,it_->second->GetName().c_str()),
                        Form("Ge_%s", it_->second->GetName().c_str()),
                        this, it_->first);

        }
    }

    create_my_spectra() ;

    named_pointer["Ge_crystal_multiplicity_raw"]
            = Tnamed_pointer(&multiplicity_raw, nullptr, this) ;

    named_pointer["Ge_crystal_multiplicity_of_good"]
            = Tnamed_pointer(&multiplicity_good, nullptr, this) ;

    named_pointer["ger_synchro_data"]
            = Tnamed_pointer(&ger_synchro_data, nullptr, this) ;
}
//*************************************************************************
void TEAGLE_array::create_my_spectra()
{

    std::string folder = "clusters/" ;

    //=============== calibrated spectra (for Gold lines) =====
    // energy cal ------------------------------------

    std::string name;

    vector<int> galileo_ring = {37, 143, 117, 101};

    for(int j = 0; j < int (galileo_ring.size())  ;j++){
#if NEED_DOPPLER_SPECTRA
#if 0
        name = name_of_this_element + Form("_ring_%i_energy_doppler",galileo_ring[j])  ;
        spec_gamma_ringDC[j] = new spectrum_1D(name,
                                               name,
                                               8192, 0, 8192,
                                               folder);
        spectra_vector.push_back(spec_gamma_ringDC[j]) ;
#endif
#endif


        name = name_of_this_element + Form("_ring_%i_energy",galileo_ring[j])  ;
        spec_gamma_ring[j] = new spectrum_1D(name,
                                             name,
                                             GE_Ecal_max_range, 0, GE_Ecal_max_range,
                                             folder);
        spectra_vector.push_back(spec_gamma_ring[j]) ;
    }

#if NEED_DOPPLER_SPECTRA

    name = name_of_this_element + "_total_energy_doppler"  ;
    spec_total_energy_doppler = new spectrum_1D(name,
                                                name,
                                                GE_Ecal_max_range, 0, GE_Ecal_max_range,
                                                folder);
    spectra_vector.push_back(spec_total_energy_doppler) ;
#endif

#ifdef NIGDY
    name = name_of_this_element + "_gamma_gamma_DC"  ;
    spec_gamma_gamma_doppler= new spectrum_2D(
                name,
                name,
                4096, 0, 4096,
                4096, 0, 4096,
                folder,
                "",  // note
                "X:\r " + name_of_this_element + "_DC_gam1"
                + "\rY:\r " +  name_of_this_element + "_DC_gam2");

    spectra_vector.push_back(spec_gamma_gamma_doppler);

    name = name_of_this_element + "_gamma_gamma"  ;
    spec_gamma_gamma= new spectrum_2D(
                name,
                name,
                4096, 0, 4096,
                4096, 0, 4096,
                folder,
                "",  // note
                "X:\r " + name_of_this_element + "_gam1"
                + "\rY:\r " +  name_of_this_element + "_gam2");

    spectra_vector.push_back(spec_gamma_gamma);


    name = name_of_this_element + "_gamma_with_at_least_1n";
    spec_gamma_al_1n = new spectrum_1D(name,
                                       name,
                                       4096,0,4096,
                                       folder,
                                       "X:\r " + name_of_this_element + "_gam1" );
    spectra_vector.push_back(spec_gamma_al_1n);


    name = name_of_this_element + "_gamma_with_at_least_1carbon";
    spec_gamma_al_1p = new spectrum_1D(name,
                                       name,
                                       4096,0,4096,
                                       folder,
                                       "X:\r " + name_of_this_element + "_gam1" );
    spectra_vector.push_back(spec_gamma_al_1p);

    name = name_of_this_element + "_gamma_with_at_least_1a";
    spec_gamma_al_1a = new spectrum_1D(name,
                                       name,
                                       4096,0,4096,
                                       folder,
                                       "X:\r " + name_of_this_element + "_gam1" );
    spectra_vector.push_back(spec_gamma_al_1a);

    //  name = name_of_this_element + "_gamma_with_at_least_1d";
    //  spec_gamma_al_1d = new spectrum_1D(name,
    //				     name,
    //				     4096,0,4096,
    //				     folder,
    //				     "X:\r " + name_of_this_element + "_gam1" );
    //  spectra_vector.push_back(spec_gamma_al_1d);

    name = name_of_this_element + "_gamma_gamma_with_at_least_1n"  ;
    spec_gamma_gamma_al_1n= new spectrum_2D(name,
                                            name,
                                            4096, 0, 4096,
                                            4096, 0, 4096,
                                            folder,
                                            "",  // note
                                            "X:\r " + name_of_this_element + "_DC_gam1"
                                            + "\rY:\r " +  name_of_this_element + "_DC_gam2");
    spectra_vector.push_back(spec_gamma_gamma_al_1n);

    name = name_of_this_element + "_gamma_gamma_with_at_least_1carbon"  ;
    spec_gamma_gamma_al_1p= new spectrum_2D(name,
                                            name,
                                            4096, 0, 4096,
                                            4096, 0, 4096,
                                            folder,
                                            "",  // note
                                            "X:\r " + name_of_this_element + "_DC_gam1"
                                            + "\rY:\r " +  name_of_this_element + "_DC_gam2");
    spectra_vector.push_back(spec_gamma_gamma_al_1p);

    name = name_of_this_element + "_gamma_gamma_with_at_least_1a"  ;
    spec_gamma_gamma_al_1a= new spectrum_2D(name,
                                            name,
                                            4096, 0, 4096,
                                            4096, 0, 4096,
                                            folder,
                                            "",  // note
                                            "X:\r " + name_of_this_element + "_DC_gam1"
                                            + "\rY:\r " +  name_of_this_element + "_DC_gam2");
    spectra_vector.push_back(spec_gamma_gamma_al_1a);

    name = name_of_this_element + "_gamma_gamma_with_at_least_1d"  ;
    //  spec_gamma_gamma_al_1d= new spectrum_2D(name,
    //					  name,
    //					  4096, 0, 4096,
    //					  4096, 0, 4096,
    //					  folder,
    //					  "",  // note
    //					  "X:\r " + name_of_this_element + "_DC_gam1"
    //					  + "\rY:\r " +  name_of_this_element + "_DC_gam2");
    //  spectra_vector.push_back(spec_gamma_gamma_al_1d);
#endif // NEVER

    // FAN spectrum of all crystals----------------------------------------
    name = name_of_this_element + "_fan"  ;
    spec_fan = new spectrum_1D(
                name,
                name,
                700, 0, 700,
                folder,
                "Detectors statistics displayed every 10 channels. Peaks mean a presence of: Energy_raw (0), Time_raw (1), E and T together (2) " ,
                "INFO:  Use 'ALL_Ge_crystal_pseudo_fan_when_good'\r"
                "\rExplanation: \rIf you want to create a fan spectrum gated by some condition\r"
                "- you need an incrementer similar to numbers which increment this fan specrum.\r"
                "\rThe difference between fan and pseudo-fan - is that\r"
                "if, for example, a Ge detector nr 6 fired: \r"
                "* The 'fan' spectrum is incremented in 1, 2 or 3 channels (for ex. 60, 61, 62)\r"
                "                      noting a presence of E, Time, E+T data \r\r"
                "* The pseudo_fan - incrementer of in this case will contain only one value (60 = 6 * 10)\r"
                "- informing that the detector nr 6 has fired"
                );
    spectra_vector.push_back(spec_fan) ;

    name = name_of_this_element + "_multiplicity_of_crystals_raw"  ;
    spec_crystals_multiplicity_raw = new spectrum_1D(
                name,
                name,
                25,0, 25,
                folder, "multiplicy of crystals which fired (ACS not counted)",
                "Ge_crystal_multiplicity_raw");
    spectra_vector.push_back(spec_crystals_multiplicity_raw) ;

    name = name_of_this_element + "_multiplicity_of_crystals_considered_as_good"  ;
    spec_crystals_multiplicity_good = new spectrum_1D(
                name,
                name,
                25,0, 25,
                folder,
                "multiplicy of crystals with energy and time data - marked as 'good' (ACS not counted)",
                "Ge_crystal_multiplicity_of_good");
    spectra_vector.push_back(spec_crystals_multiplicity_good) ;

    name = name_of_this_element + "_timestamp_difference_ge_ge"  ;
    spec_tsdiff_ge_ge = new spectrum_1D(name,
                                        name,
                                        1000,-500, 500,
                                        folder,
                                        "",
                                        "INFO: not available");
    spectra_vector.push_back(spec_tsdiff_ge_ge) ;

    //    name = name_of_this_element + "_time_difference_ge_ge"  ;
    //    spec_timediff_ge_ge = new spectrum_1D(name,
    //                                          name,
    //                                          4000,-1000,1000,
    //                                          folder,
    //                                          "",
    //                                          name);
    //    spectra_vector.push_back(spec_timediff_ge_ge) ;

    //    name = name_of_this_element + "_time_difference_ge_ge_corr"  ;
    //    spec_timediff_ge_ge_corr = new spectrum_1D(name,
    //                                               name,
    //                                               4000,-1000,1000,
    //                                               folder,
    //                                               "",
    //                                               name);
    //    spectra_vector.push_back(spec_timediff_ge_ge_corr) ;

//    name = name_of_this_element + "_energy_vs_crystal";
//    spec_energy_id = new spectrum_2D(
//                name,
//                name,
//                32, 0,32,
//                GE_Ecal_max_range,0, GE_Ecal_max_range,
//                folder,
//                "",  // note
//                "INFO: not available crystal ID "

//                );
//    spectra_vector.push_back(spec_energy_id);

    //    name = name_of_this_element + "_energy_vs_theta";
    //    spec_energy_theta_degree = new spectrum_2D(
    //                name,
    //                name,
    //                90,90,180,
    //                8192,0,4096,
    //                folder,
    //                "",  // note
    //                "X:\r " + name_of_this_element + "_theta_ge"
    //                + "\rY:\r " +  name_of_this_element + "_energy"
    //                );
    //    spectra_vector.push_back(spec_energy_theta_degree);

#if NEED_DOPPLER_SPECTRA
    name = name_of_this_element + "_energy_doppler_vs_theta";
    spec_energyDC_theta_degree = new spectrum_2D(
                name,
                name,
                90,90,180,
                GE_Ecal_max_range,0,GE_Ecal_max_range,
                folder,
                "",  // note
                "X:\r " + name_of_this_element + "_theta_ge"
                + "\rY:\r " +  name_of_this_element + "_energy_doppler_when_ok"
                );

    spectra_vector.push_back(spec_energyDC_theta_degree);
#endif

    //    name = name_of_this_element + "_in_coinc";
    //    spec_gamma_coinc = new spectrum_1D(name,
    //                                       name,
    //                                       GE_Ecal_max_range,0,GE_Ecal_max_range,
    //                                       folder,
    //                                       "",
    //                                       "");
    //    spectra_vector.push_back(spec_gamma_coinc);




#if NEED_DOPPLER_SPECTRA

#if 0
    // never used now
    name = name_of_this_element + "_in_coincDC";
    spec_gamma_coincDC = new spectrum_1D(name,
                                         name,
                                         4096,0,4096,
                                         folder,
                                         "","");
    spectra_vector.push_back(spec_gamma_coincDC);

    name = name_of_this_element + "_gamma_doppler_vs_partmult";
    spec_gammaDC_vs_partmult = new spectrum_2D(name,
                                               name,
                                               20,0,20,
                                               4096,0,4096,
                                               folder,
                                               "","");
    spectra_vector.push_back(spec_gammaDC_vs_partmult);
#endif
#endif
    name = name_of_this_element + "_timeDiff_Spider_Gal";
    spec_timediff_sg = new spectrum_1D(name,
                                       name,
                                       50,0,50,
                                       folder,
                                       "","");
    spectra_vector.push_back(spec_timediff_sg);

#if NEED_DOPPLER_SPECTRA
    name = name_of_this_element + "_energy_doppler_vs_theta_ge";
    spec_gammaDC_vs_angle = new spectrum_2D(name,
                                            name,
                                            180,0,180,
                                            2048,0,2048,
                                            folder,
                                            "",  // note
                                            "X:\r " + name_of_this_element + "_theta_ge"
                                            + "\rY:\r " +  name_of_this_element + "_energy_doppler_corrected"
                                            );

    spectra_vector.push_back(spec_gammaDC_vs_angle);
#endif

    //-------------------- MONITOR -------------------------
    //cout << lookup.size() << endl;

    int ile = highest_ge_nr;
    int margines = 2;
    int margines_gorny = 3;

    name = name_of_this_element + "_monitor";
    spec_geometry_monitor = new spectrum_2D(name,
                                            name,
                                            2*(ile+ 2*margines), -margines, ile+margines ,
                                            (2 + margines+ margines_gorny), -2 -margines, margines_gorny,
                                            folder,
                                            "Monitoring",  // note
                                            "no incrementers available here"
                                            );

    spectra_vector.push_back(spec_geometry_monitor);
    picture_width = ile+margines + margines;
    // Description of a monitor map
    // opening a file "nazwa macierzy" + ".pinuptxt" as a new file  (trunc)

    string pinup_file = path.dir_my_binnings() + name  + ".mat.pinuptxt";
    ofstream f(pinup_file, ios::trunc);
    if(f)
    {
        // cout << "Opening  OK" << endl;
        f << "3 2.5 The Spy resets this graph every " << (HOW_MANY_SECONDS_TO_RESET / 60.0 )<< " min "  << endl;
        f << "-1 0.5 Ge "  << endl;                // x  y  etykieta_Ge_00
        f << "-1 -2.5 ACS " << endl;
        for(int i = 0 ; i < ile ;++i)
        {
            int x = i + 0;

            f << x << " -0.2 " << i+1  << endl;                // x  y  etykieta_Ge_00
            f << x << " -3.2 " << i+1  << endl;                // x  y  etykieta_Ge_00
        }

    }
    else {

        cerr << " Warning: impossible to open file for writing. File : " << pinup_file << endl;
    }
}
//*************************************************************************
void TEAGLE_array::make_preloop_action(std::ifstream & file)
{
//    cout << __PRETTY_FUNCTION__ << endl;

    tmp_event_unpacked   = event_unpacked ;
    tmp_event_calibrated = TGalileoAnalysis::fxCaliEvent;
    //    std::cout << "TEAGLE_array::make_preloop_action "  << std::endl;

    // first we inform where are the data coresponding to these devices
    TEAGLE_crystal::target_ptr = galileo_pointer->give_target_address() ;


    //simulate_event();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



    std::string fname = path.dir_options() + "doppler.options" ;
    try
    {
        std::ifstream plik(fname.c_str()) ;
        if(!plik)
        {
            Tfile_helper_exception capsule;
            capsule.message =  "Can't open the file";
            throw capsule ;
        }


        TEAGLE_crystal::Eex = Tfile_helper::find_in_file<double>(plik, "Eex");
        TEAGLE_crystal::Ebeam = Tfile_helper::find_in_file<double>(plik, "Ebeam");
        TEAGLE_crystal::massP = Tfile_helper::find_in_file<double>(plik, "massP");
        TEAGLE_crystal::massT = Tfile_helper::find_in_file<double>(plik, "massT");
    }
    catch(Tfile_helper_exception &m)
    {
        std::cout << "\n\nTtarget_galileo:: Error while reading the file " << fname
                  << m.message << std::endl;
        std::cout << ">>>> Most probably you have too old version of parameters "
        "for Doppler correction\n"
        " Please open the dialog window in the Cracow: \n"
        "   spy_option->Doppler\n"
        "choose any combination of parameteres and press OK" << std::endl;
        exit(123);
        //throw ;
    }

    // very important:
    // calibration factors and gates are read everytime we start/stop the analysis

    read_good_crystal_specification();
    read_calibration_from_lookuptable();

    // zeroing monitoring statistics for the watchdog
}
//**********************************************************************
void TEAGLE_array::read_calibration_from_lookuptable()
{

    std::map<int,TEAGLEDetector*>::iterator it_ = TEAGLE_array::lookup.begin();
    for (; it_!=TEAGLE_array::lookup.end() ; ++it_) {
        if(it_->second){
//            std::cout << "Making preloop for id " << it_->first << std::endl;
            detector_eagle[it_->first]->make_preloop_action(it_->second);
        }
    }

    std::cout << "Germanium calibration loaded successfully" << std::endl;
}
//********************************************************
void TEAGLE_array::analyse_current_event()
{
    //    std::cout << "TEAGLE_array::analyse_current_event()"  <<std::endl;
    //crystal_which_fired.clear() ; // new event,

    multiplicity_raw = 0 ;
    multiplicity_good = 0 ;

    tsdiff=0.;
    timediff=0.;

    // here, for testing purposes, we want pack artificially to the event
    //simulate_event();


    //        std::cout << "============ llNext event ================" << std::endl;
    //        std::cout << tmp_event_unpacked->eagle_fired.size() << std::endl;

    for(auto  wsk : detector_eagle)
    {
        // cout << "FFF" << wsk.first << endl;
        wsk.second->zeroing_good_flags();
    }

    if(tmp_event_unpacked->eagle_fired.size()>0)
    {
        std::map<int,bool>::iterator it_ = tmp_event_unpacked->eagle_fired.begin();
        for(; it_ != tmp_event_unpacked->eagle_fired.end(); ++it_){
            //                  std::cout << "Eagle Ge (it_first, it_second) : ("
            //                        << it_->first << " , "  << it_->second
            //                        << ")"
            //                        << std::endl;
            if (it_->second) {
                detector_eagle[it_->first]->analyse_current_event();

                //int x = it_->first % 15 ;
                int nr = detector_eagle[it_->first]->give_nr_fan_germ() -1;
                //int nr = detector_eagle[it_->first]->give_id_number();  // 0-14
                int true_germ = detector_eagle[it_->first]->flag_true_germanium;
                int x = (nr) % highest_ge_nr;
                int y = 0;
                if(true_germ == false) {   // acs
                    y = -3;
                    x = (nr - 20) % highest_ge_nr;
                }
//                                for(int n = 0 ; n < highest_ge_nr ; ++n)
//                                    spec_geometry_monitor->increment(n, -8);
//                                cout << "inkrementacja monitor id = " << it_->first << " x = " << x << ", y = " << y << endl;

                spec_geometry_monitor->increment(x, y);
                //                cout << "Monitor it_->first = "
                //                                     << it_->first
                //                                     <<    ", id = "
                //                        << detector_eagle[it_->first]->give_id_number()
                //                        << " x = " << x
                //                        << ", y = " << y
                //                        << ", nr_fan_germ = "
                //                                     << detector_eagle[it_->first]->give_nr_fan_germ()
                //                                     << endl;
                //                cout << " ------------------kONIEC ZDARZENIA " << endl;
            }
        }
    }

    // Erasing monitor spectrum every 1 minute ===========================================
    int elapsed = time(nullptr)- last_time_of_erasing_monitor_spectra ;
    static double ostatni_dodany_kafelek_paska = 0;

    if(elapsed> HOW_MANY_SECONDS_TO_RESET )
    {
        // cout << "Reset of monitor spectrum" << endl;
        spec_geometry_monitor->give_root_spectrum_pointer()->Reset();
        last_time_of_erasing_monitor_spectra = time(nullptr);
        ostatni_dodany_kafelek_paska = 0 ;
    }else {

        double suwak = elapsed *picture_width / HOW_MANY_SECONDS_TO_RESET ;

        if(suwak > ostatni_dodany_kafelek_paska)
        {
            spec_geometry_monitor->increment(suwak-2, 2);
            //spec_geometry_monitor->increment(suwak+0.5, -2);

            ostatni_dodany_kafelek_paska += 0.5;
            //            cout << "\nElapsed EAGLE  = " << elapsed << ",  suwak = " << suwak
            //                 << ", ostatni dodany = " << ostatni_dodany_kafelek_paska << endl;
        }
    }








    //	      if(detector_eagle.find(it_->first)==detector_eagle.end())continue;
    //	      detector_eagle[it_->first]->analyse_current_event();
    //                detector_eagle[it_->first]->increment_crystal_spectra_raw_and_fan();
    //                // incrementing the total spectrum (always!)
    //                detector_eagle[it_->first]->increment_crystal_spectra_cal_and_doppler();
    //                double gammaE = detector_eagle[it_->first]->give_energy_calibrated();
    //		//                tmp_event_unpacked->eagle_data[it_->first].energy=gammaE;
    //                //		std::cout << it_g->first << "\t" << tmp_event_unpacked->eagle_data[it_g->first][0].eEnergy << std::endl;
    //                double gammaEDC = detector_eagle[it_->first]->GetgammaEDC();
    //                //tmp_event_unpacked->eagle_data[it_->first].wEnergy=gammaEDC;
    //                if (gammaE>=1.){
    //                    storeTS.push_back(detector_eagle[it_->first]->give_timestamp());
    //                    storePos.push_back(detector_eagle[it_->first]->give_position());
    //                    storeE.push_back(gammaE);
    //                    double theta = detector_eagle[it_->first]->give_theta()*TMath::RadToDeg();
    //                    spec_total_energy_doppler->increment(gammaEDC);
    //                    //		    std::cout << floor(theta+0.5) << std::endl;
    //                    if(spec_gamma_ring.find(floor(theta+0.5))!=spec_gamma_ring.end()){
    //                        spec_gamma_ring[floor(theta+0.5)]->increment(gammaE);
    //                        spec_gamma_ringDC[floor(theta+0.5)]->increment(gammaEDC);
    //                    }
    //
    //                    spec_energy_id->increment((double)it_->first,gammaE);
    //                    spec_energy_theta_degree->increment(theta,gammaE);
    //                    spec_energyDC_theta_degree->increment(theta,gammaEDC);
    //                }//
    //            } // endif crystal fired fired
    //            else {
    //                detector_eagle[it_->first]->zeroing_good_flags();
    //            }
    //        } // end for (cry)
    //
    //        // after working with the all crystal of this cluster
    //
    //        if (GalileoAnalysis_ptr->is_verbose_on()) {
    //            std::cout << "multiplicity_raw = " << multiplicity_raw << std::endl;
    //            std::cout << "multiplicity_of_good = " << multiplicity_good << std::endl;
    //        }

    /*
        std::map<int,bool>::iterator g1 = tmp_event_unpacked->eagle_fired.begin();
        std::map<int,bool>::iterator g2 = tmp_event_unpacked->eagle_fired.begin();
        for(;g1!=tmp_event_unpacked->eagle_fired.end();++g1){
            g2 = tmp_event_unpacked->eagle_fired.begin();
            for(;g2!=tmp_event_unpacked->eagle_fired.end();++g2){
                if(g1==g2)continue;

                if(detector_eagle[g1->first]->flag_good_data && detector_eagle[g2->first]->flag_good_data){

                    tsdiff = ((double)detector_eagle[g1->first]->give_timestamp()-
                            (double)detector_eagle[g2->first]->give_timestamp())*10;
                    timediff  = ((double)detector_eagle[g1->first]->give_ge_time()-
                            (double)detector_eagle[g2->first]->give_ge_time())*10;
                    spec_tsdiff_ge_ge->increment(tsdiff);
                    spec_timediff_ge_ge->increment(tsdiff+timediff);
                    if(TMath::Abs(timediff)<60){
              spec_gamma_gamma_doppler->increment(detector_eagle[g1->first]->GetgammaEDC(),
                              detector_eagle[g2->first]->GetgammaEDC());
              spec_gamma_gamma->increment(detector_eagle[g1->first]->give_energy_calibrated(),
                          detector_eagle[g2->first]->give_energy_calibrated());
                    }
                }
            }
        }
    */
    //    if(multiplicity_raw> 1)
    //        std::cout << "POdwojna multiplicity " << std::endl;

    spec_crystals_multiplicity_raw->increment(multiplicity_raw);
    spec_crystals_multiplicity_good->increment(multiplicity_good);
    //  }

    calculations_already_done = true ;

}

//***********************************************************************
void  TEAGLE_array::make_postloop_action()
{
    // std::cout << "F.Trising::make_postloop_action() " << std::endl ;

    // save all spectra made in frs objects and in frs elements
    // (mw, sci, etc.)
    //     std::cout <<  "Saving Rising " << spectra_rising->size() << " spectra... " << std::endl;
    //     for (unsigned int i = 0 ; i < spectra_rising->size() ; i++) {
    //         spectra_rising[i]->save_to_disk() ;
    //     }
    std::cout <<  "Saving Rising spectra - done " << std::endl;
}
//***********************************************************************
void TEAGLE_array::simulate_event()
{
    //    for(unsigned int i = 0 ; i < detector.size() ; i++)
    //    {
    //        //std::cout << "Loop i =  " << i << std::endl;
    //        for(int cri = 0 ; cri < 7 ; cri++)
    //        {
    //            tmp_event_unpacked->Cluster_data[i][cri][0] =
    //                (100*i) +(10*cri) +0 ; // energy
    //            tmp_event_unpacked->Cluster_data[i][cri][1] =
    //                (100*i) +(10*cri) +1;  // time
    //        }
    //        // remember the source of data
    //    }
}
//**********************************************************************
/** No descriptions */
bool TEAGLE_array::save_selected_spectrum(std::string name)
{
    for (unsigned int i = 0 ; i < spectra_vector.size() ; i++) {
        if (name == spectra_vector[i]->     give_name()) {
            spectra_vector[i]->save_to_disk() ;
            return true ;
        }
    }
    return false ;  // was not here, perhaps in frs or hector?
}
//***********************************************************************
/** Depending on the status of hardware we can use different algorithms */
void TEAGLE_array::read_doppler_corr_options()
{}
//*******************************************************************
/** Rereading the definiton of the condiotinal (gated) spectra */
void TEAGLE_array::read_def_conditional_spectra()   // called form preloop
{
    // here should be a function for retrieving the names.
    std::vector<std::string> names =
            Tfile_helper::find_files_in_directory(path.dir_user_def_spectra(),
                                                  ".user_definition");

    // loop which is looking into the directory for a specified definions *.
    for (unsigned nr = 0 ; nr < names.size() ; nr++) {
        std::cout << "User defined conditional file :" << names[nr] << std::endl;
        create_cond_spectrum(names[nr]);
    }
}
//*******************************************************************************
/** Create one conditional spectrum */
void TEAGLE_array::create_cond_spectrum(std::string name)
{
#ifdef NIGDY
#endif

}
//********************************************************************************
/** where the user spectra can be incremented */
void TEAGLE_array::user_spectra_loop()
{
#ifdef NIGDY
#endif

}
//**************************************************************

//**************************************************************
void TEAGLE_array::read_good_crystal_specification()
{
    // reading from the file
    std::string pname =  path.dir_options() + "good_galileo_germanium.txt" ;
    std::ifstream plik(pname.c_str());
    if (!plik) {
        std::cout << "Can't open file " << pname << std::endl;
        // so  we set the default values
        //------- En 4

        TEAGLE_crystal::flag_good_energy_requires_threshold = true ;
        TEAGLE_crystal::energy_threshold_lower = 20 ;
        TEAGLE_crystal::energy_threshold_upper = 8*8192 ;
        // everything else is zero (because  it is  static)
        return ;
    }
    try {
        // reading
        ///// increment_20MeV_cal_with_zero	0

        ///// good_20MeV_requires_threshold	0
        ///// en20MeV_threshold	0
        //

        // increment_energy_cal_with_zero	0
        // increment_time_with_zero	0

        // good_energy_requires_threshold	1
        // energy_threshold	500
        // energy_threshold_upper	8192
        //

        // good_time_requires_threshold	0
        // good_time_threshold_lower	0
        // good_time_threshold_upper	99999



        TEAGLE_crystal::flag_increment_energy_cal_with_zero   = false;
        //             (bool) Tfile_helper::find_in_file<bool>(plik, "increment_energy_cal_with_zero") ;

        TEAGLE_crystal::flag_increment_time_with_zero = false;
        //             (bool) Tfile_helper::find_in_file<bool>(plik, "increment_time_with_zero") ;


        //------- En 4 ---------------------
        TEAGLE_crystal::flag_good_energy_requires_threshold =
                (bool) Tfile_helper::find_in_file<bool>(plik, "good_energy_requires_threshold") ;

        TEAGLE_crystal::energy_threshold_lower =
                Tfile_helper::find_in_file<double>(plik, "energy_threshold_lower") ;

        TEAGLE_crystal::energy_threshold_upper =
                Tfile_helper::find_in_file<double>(plik, "energy_threshold_upper") ;
        // time -------
        TEAGLE_crystal::flag_good_time_requires_threshold =
                (bool) Tfile_helper::find_in_file<bool>(plik, "good_time_requires_threshold") ;


        // The new version has time threshold for LOW and High, so actually this is not a threshold
        // but just a time gate
        try {  // the new style
            TEAGLE_crystal::good_time_threshold_lower =
                    Tfile_helper::find_in_file<double>(plik, "good_time_threshold_lower") ;

            TEAGLE_crystal::good_time_threshold_upper =
                    Tfile_helper::find_in_file<double>(plik, "good_time_threshold_upper") ;


        } catch (...) { // perhaps it is an old style
            // at first repair failed stream
            std::cout << "You are using the old style definiotion of 'good germanium'" << std::endl;
            // repair the stream
            plik.clear(plik.rdstate() & ~(std::ios::eofbit | std::ios::failbit));

            // now try to read
            TEAGLE_crystal::good_time_threshold_lower =
                    Tfile_helper::find_in_file<bool>(plik, "good_time_threshold") ;

            TEAGLE_crystal::good_time_threshold_upper = 99999;
        }



    } catch (Tfile_helper_exception k) {
        std::cout << "Error while reading " << pname << "\n" << k.message << std::endl;
        exit(1);
    }
}

