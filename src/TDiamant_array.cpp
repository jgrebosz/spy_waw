#include "TDiamant_array.h"
#include <dirent.h> // for scandir
//#include <malloc.h> // for scandir
#include <stdlib.h>

#include <sstream>
#include "TGalileoAnalysis.h"
#include "Tnamed_pointer.h"
#include "paths.h"
#include "Tlookup_table_doublets.h"
using namespace std;
TGalileoCalibratedEvent *TDiamant_array::tmp_event_calibrated;
TGalileoEvent *TDiamant_array::tmp_event_unpacked;

int TDiamant_array::multiplicity_raw;
int TDiamant_array::multiplicity_good;
//std::map<int,TEAGLEDetector*> TDiamant_array::lookup;

//static TDiamant_crystal::array_pointer ;


//*******************************************************
//*******************************************************
//         Diamant detectors array class
//*******************************************************
//*******************************************************
TDiamant_array::TDiamant_array(std::string name) : TNeedle_element(name),
    spectra_vector(*GalileoAnalysis_ptr->give_gal_ptr()->address_spectra_frs())
{
    //std::cout << "\n\n\n\nConstructor of the Rising Class =========================== " << std::endl ;
    //Tcrystal::rising_pointer = this ; // for crystals

    TDiamant_crystal::parent_pointer = this ; // for crystals

    //Tcluster::rising_pointer = this ; // for cluster

    //     dynamic_cast<TRisingAnalysis*> (TGo4Analysis::Instance());

    galileo_pointer = GalileoAnalysis_ptr->give_gal_ptr()  ;  // we will need if to ask traget for theta angles
    //spectra_vector = gal_pointer->address_spectra_frs();
    //gal_pointer->address_spectra_frs();

    //---------------------------------------------
    // constructing the current setup of Galileo
    //---------------------------------------------
    diamant_detector.clear();
    for(int n = 0 ; n < HOW_MANY_DIAMANT ; ++n)
    {
        string name = string("diamant_")     +
                ((n > 9)? "" : "0") + to_string(n) ;
        diamant_detector.push_back(
                    new TDiamant_crystal(name, n)
                    );
    }

    create_my_spectra() ;

    named_pointer["diamant_crystal_multiplicity_raw"]
            = Tnamed_pointer(&multiplicity_raw, nullptr, this) ;

    named_pointer["diamant_crystal_multiplicity_of_good"]
            = Tnamed_pointer(&multiplicity_good, nullptr, this) ;

    named_pointer["ger_synchro_data"]
            = Tnamed_pointer(&ger_synchro_data, nullptr, this) ;
}
//*************************************************************************
void TDiamant_array::create_my_spectra()
{

    std::string folder = "diamant/" ;
    std::string name;

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




    name = name_of_this_element + "_energy_vs_pid";
    spec_energy_vs_pid = new spectrum_2D(
                name,
                name,
                500, 0, 1,
                2000, 0, 8000,
                folder,
                "",  // note
                "INFO: ????"
                "\rY:\r " +  name_of_this_element + "_energy"
                );

    spectra_vector.push_back(spec_energy_vs_pid);







    //-------------------- MONITOR -------------------------
    int ile = 10;
    int margines = 2;
    int margines_gorny = 3;

    name = name_of_this_element + "_monitor";
    spec_geometry_monitor = new spectrum_2D(name,
                                            name,
                                            2*(ile+margines), -ile-margines, ile+margines,
                                            24, -12, 12,
                                            folder,
                                            "Monitoring",  // note
                                            "no incrementers available here"
                                            );

    spectra_vector.push_back(spec_geometry_monitor);

    // reading the geometry
    string fname_geometry = path.dir_experiment_setup() + "diamant_geometry.txt";

    ifstream file(fname_geometry);

    geom.resize(90);

    int id, x, y;
    if(file){
        for(int nr = 0 ; ; ++nr)
        {
            file >> id >> x >> y ;
            if(!file) break;
            geom[id] =para{ x, y};

        }
    }
    //--------------------------
    picture_width = 2* (ile+margines) ; //  + margines;
    // Description of a monitor map
    // opening a file "nazwa macierzy" + ".pinuptxt" as a new file  (trunc)

    string pinup_file = path.dir_my_binnings() + name  + ".mat.pinuptxt";
    ofstream f(pinup_file, ios::trunc);
    if(f)
    {
        // cout << "Opening  OK" << endl;
        f << "-9 11 The Spy resets this graph every " << (HOW_MANY_SECONDS_TO_RESET / 60.0 )<< " min "  << endl;

        for(int i = 0 ; i < geom.size() ;++i)
        {
            f << geom[i].x << "   " << geom[i].y+0.5 << " D" << i  << endl;                // x  y  etykieta_Ge_00

        }

    }
    else {

        cerr << " Warning: impossible to open file for writing. File : " << pinup_file << endl;
    }
}
//*************************************************************************
void TDiamant_array::make_preloop_action(std::ifstream & file)
{
    tmp_event_unpacked   = event_unpacked ;
    tmp_event_calibrated = TGalileoAnalysis::fxCaliEvent;
    std::cout << "TDiamant_array::make_preloop_action "  << std::endl;

    // first we inform where are the data coresponding to these devices
    //TDiamant_crystal::target_ptr = galileo_pointer->give_target_address() ;


    //simulate_event();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // very important:
    // calibration factors and gates are read everytime we start/stop the analysis

    read_good_crystal_specification();
    read_calibration();
    read_in_lookup_table();

    // zeroing monitoring statistics for the watchdog
}
//**********************************************************************
void TDiamant_array::read_calibration()
{


    for (auto d : diamant_detector) {
        d->make_preloop_action();
    }

    std::cout << "Diamant calibration loaded successfully" << std::endl;

}
//********************************************************
void TDiamant_array::analyse_current_event()
{
//    std::cout << "TDiamant_array::analyse_current_event()"  <<std::endl;

    //crystal_which_fired.clear() ; // new event,

    multiplicity_raw = 0 ;
    multiplicity_good = 0 ;

    // here, for testing purposes, we want pack artificially to the event
    //simulate_event();


    //        std::cout << "============ llNext event ================" << std::endl;
    //        std::cout << tmp_event_unpacked->eagle_fired.size() << std::endl;

    for(auto  wsk : diamant_detector)
    {
        // cout << "FFF" << wsk.first << endl;
        wsk->zeroing_good_flags();
    }

    //  std::map<int,bool>::iterator it_ = tmp_event_unpacked->diamant_fired.begin();

    for(auto d : diamant_detector)
    {

        d->analyse_current_event();

        //int x = it_->first % 15 ;
        if(d->flag_fired){

            int nr = d->give_id_number();



        //int nr = detector_eagle[it_->first]->give_id_number();  // 0-14
        int x = geom[nr].x;
        int y = geom[nr].y;


        spec_geometry_monitor->increment(x, y);
        }
        //                cout << " ------------------kONIEC ZDARZENIA " << endl;

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
            spec_geometry_monitor->increment(suwak-11, 11);
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
void  TDiamant_array::make_postloop_action()
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
void TDiamant_array::simulate_event()
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
bool TDiamant_array::save_selected_spectrum(std::string name)
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
void TDiamant_array::read_doppler_corr_options()
{}
//*************************************************************************
bool TDiamant_array::read_in_lookup_table()
{
    string s = path.dir_experiment_setup();
    string filename = s + "diamant_id_lookup";
    ifstream file (filename);
    if(!file){
        cout << "Cant open lookup table file: " << filename << endl;
        exit(88);
        return false;
    }

    //    if(look_ptr) delete look_ptr;
    //    look_ptr = new Tlookup_table_doublets(200, 17, "");
    //    // look_ptr->read_from_disk(filename);

    //    struct tryplet { int id;
    //           int board;
    //           int channel;
    //           };

    //    vector<tryplet> look;

    lookup_table.clear();

    string schowek;
    getline(file, schowek);  // first line is a comment with a description
    int id_ , board_, channel_;
    for( ; ; )
    {
        file >> id_ >> board_ >> channel_ ;
        if(file.eof() ) break;
        lookup_table[(board_ << 10) + channel_] = id_;
        //look.push_back(tryplet{ id_, board_, channel_});


//        cout << "id_ = " << id_
//             << ", board = " << board_
//             << ", channel = " << channel_ << endl;
    }

//cout<< " ===================" << endl;
//    for( auto x : lookup_table){
//        cout << "x.first = " << x.first
//             << ", x.second = " << x.second
//             << endl;
//    }
    return true;
}
//*******************************************************************
/** Rereading the definiton of the condiotinal (gated) spectra */
void TDiamant_array::read_def_conditional_spectra()   // called form preloop
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
void TDiamant_array::create_cond_spectrum(std::string name)
{
#ifdef NIGDY
#endif

}
//********************************************************************************
/** where the user spectra can be incremented */
void TDiamant_array::user_spectra_loop()
{
#ifdef NIGDY
#endif

}
//**************************************************************

//**************************************************************
void TDiamant_array::read_good_crystal_specification()
{
    // reading from the file
    std::string pname =  path.dir_options() + "good_galileo_germanium.txt" ;
    std::ifstream plik(pname.c_str());
    if (!plik) {
        std::cout << "Can't open file " << pname << std::endl;
        // so  we set the default values
        //------- En 4

        TDiamant_crystal::flag_good_energy_requires_threshold = true ;
        TDiamant_crystal::energy_threshold_lower = 20 ;
        TDiamant_crystal::energy_threshold_upper = 8192 ;
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



        TDiamant_crystal::flag_increment_energy_cal_with_zero   = false;
        //             (bool) Tfile_helper::find_in_file<bool>(plik, "increment_energy_cal_with_zero") ;

        TDiamant_crystal::flag_increment_time_with_zero = false;
        //             (bool) Tfile_helper::find_in_file<bool>(plik, "increment_time_with_zero") ;


        //------- En 4 ---------------------
        TDiamant_crystal::flag_good_energy_requires_threshold =
                (bool) Tfile_helper::find_in_file<bool>(plik, "good_energy_requires_threshold") ;

        TDiamant_crystal::energy_threshold_lower =
                Tfile_helper::find_in_file<double>(plik, "energy_threshold_lower") ;

        TDiamant_crystal::energy_threshold_upper =
                Tfile_helper::find_in_file<double>(plik, "energy_threshold_upper") ;
        // time -------
        TDiamant_crystal::flag_good_time_requires_threshold =
                (bool) Tfile_helper::find_in_file<bool>(plik, "good_time_requires_threshold") ;


        // The new version has time threshold for LOW and High, so actually this is not a threshold
        // but just a time gate
        try {  // the new style
            TDiamant_crystal::good_time_threshold_lower =
                    Tfile_helper::find_in_file<double>(plik, "good_time_threshold_lower") ;

            TDiamant_crystal::good_time_threshold_upper =
                    Tfile_helper::find_in_file<double>(plik, "good_time_threshold_upper") ;


        } catch (...) { // perhaps it is an old style
            // at first repair failed stream
            std::cout << "You are using the old style definiotion of 'good germanium'" << std::endl;
            // repair the stream
            plik.clear(plik.rdstate() & ~(std::ios::eofbit | std::ios::failbit));

            // now try to read
            TDiamant_crystal::good_time_threshold_lower =
                    Tfile_helper::find_in_file<double>(plik, "good_time_threshold") ;

            TDiamant_crystal::good_time_threshold_upper = 99999;
        }



    } catch (Tfile_helper_exception k) {
        std::cout << "Error while reading " << pname << "\n" << k.message << std::endl;
        exit(1);
    }
}

