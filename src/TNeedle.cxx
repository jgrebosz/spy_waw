#include "TNeedle.h"

#include <dirent.h> // for scandir
//#include <malloc.h> // for scandir
#include <stdlib.h>
#include <fstream>

#include <sstream>

#include <ctime>

#include "TEAGLE_array.h"
#include "TNEDA_array.h"
#include "TDSSD_ring.h"
#include "TDiamant_array.h"
#include "Ttarget_galileo.h"

#include "Tgrebware_exception.h"
#include "Tone_signal_module.h"
#include "T32_signal_module.h"
#include "Tone_signal_pattern_module.h"

#include "Tnamed_pointer.h"
#include "TGalileoAnalysis.h"                   // for verbose mode flag
#include <algorithm>
#include "Tscaler.h"
#include "Tscaler_template.h"
#include "Ttimestamp_handler.h"

using namespace std;

//*******************************************************
//*******************************************************
//         Galileo detectors configuration class
//*******************************************************
//*******************************************************
TNeedle::~TNeedle()               // DEStructor
{
    for (unsigned long i = 0 ; i < spectra_gal.size() ; i++) {
        delete  spectra_gal[i];
    }

    for (unsigned long i = 0 ; i <  module.size() ; i++) {
        delete  module[i];
    }
    LookUpTable.clear();


}
//*********************************************************************************
TNeedle::TNeedle(TGalileoAnalysis *ptr)                // constructor
{

    // global
    GalileoAnalysis_ptr = ptr;
    //First step is to read the lookup table to know the number of detectors
    //we also set here the Experimental conditions (Beam, Target, Beam energy, ...)
    LookUpTable.clear();
    SetupExperiment();


    // read_interesting_Eraw_Ecal_Traw_Tcal_range();

    // filling the vector with the proper configuration
    // it must be here in the constructor, because they have
    // spectra, which have to be alive always

    // common to all of TNeedle_elements (to know where is
    //their register of the spectra)
    TNeedle_element::owner = this ;

    //---------------------------------------------
    // constructing the current setup of Galileo
    //---------------------------------------------

    if(NEDA_LUT.size()>0){
        module.push_back(neda_pointer = new TNEDA_array("NEDA_array"));
    }
    if(EAGLE_LUT.size()>0){
        module.push_back(eagle_pointer = new TEAGLE_array("EAGLE_array"));
    }


#if DIAMANT_PRESENT

    module.push_back(diamant_pointer =
            new TDiamant_array("diamant_array"));

#endif
    //================================================
    //  Galileo  Target
    //================================================

    module.push_back(target_ptr =  new Ttarget_galileo("target",
                                                       "none",
                                                       "none"
                                                       )) ;
    // ---------- DSSD -------------------------------------------
    if(DSSD_LUT.size()>0){
        module.push_back(dssd_pointer = new TDSSD_ring("DSSD", 1));
    }



    //    // Scalers: Trigger   and RF
    //    module.push_back(new
    //                     Tscaler("scaler_trigger",
    //                             &TGalileoEvent::LTRIG_data,  // skad brac dane w evencie
    //                             &TGalileoEvent::timestamp
    //                             ));
    module.push_back(new
                     Tscaler_template<unsigned long long >
                     ("scaler_trigger",
                      &TGalileoEvent::LTRIG_data,  // skad brac dane w evencie
                      &TGalileoEvent::seconds,
                      //                      &time_now,
                      Tscaler_mode::value_one   //

                      ));

    // module.push_back(new Ttimestamp_handler("Timestamp_helper") );

    create_your_spectra() ;
}
//*************************************************************************
void TNeedle::create_your_spectra()
{

}
//**********************************************************************
void TNeedle::make_preloop_action(
        TGalileoEvent *ev_unp ,
        TGalileoCalibratedEvent *event_calibrated_ptr)
{
    // now this adres if fixed
    TNeedle_element::event_unpacked = ev_unp ;
    event_calibrated = event_calibrated_ptr ;
    memset(statistics_frs, 0 , sizeof(statistics_frs));

    // very important:
    // calibrations and gates are read everytime we start the analysis

    read_calibration_and_gates_from_disk();
}
//**********************************************************************
void TNeedle::read_calibration_and_gates_from_disk()
{


    //--------------------------------------------
    // reading the calibration of all frs stuff
    // here we can open the calibration file
    //-------------------------------------------

    string cal_filename = path.dir_experiment_setup()  + "neda_lookup_table_and_calibration.txt";
    try {
        ifstream plik(cal_filename.c_str()) ;    // ascii file
        if (!plik) {
            string mess = "I can't open NEDA calibration file: " +
                    cal_filename  ;
            throw mess ;
        }


        // here we assume that pointers to all the devices (multiwires,
        //   scintillators, are
        // in the module vector of pointers

        for (unsigned int i = 0 ; i < module.size() ; i++) {
            module[i]->make_preloop_action(plik) ;
        }
        plik.close();

    }  // end of try
    catch (Tfile_helper_exception &k) {
        std::cerr << "Error while reading calibration file "
                  << cal_filename
                  << ":\n"
                  << k.message << std::endl  ;

        exit(-1) ;    // is it healthy ?

    } catch (std::string sss)
    {
        std::cerr << "Error while reading calibration file "
                  << cal_filename
                  << ":\n"
                  << sss << std::endl  ;

        exit(-1) ;    // is it healthy ?

    }
    std::cout << "Galileo calibration successfully read" << std::endl;

}
//*********************************************************
//*********************************************************
void TNeedle::analyse_current_event()
{

    // static int nr_ev ;
    //
    //    std::cout << "TNeedle class  works on a new event === nr "
    //              << (nr_ev++)
    //              << " =====================" << std::endl ;

    {
        if(TNeedle_element::event_unpacked->neda_fired.empty() &&
                TNeedle_element::event_unpacked->eagle_fired.empty()
                //                &&  TNeedle_element::event_unpacked->dssd_fired.empty()
                )
        {
            // nothing fired, so return
            //  NO! IT MUST BE DONE [Jurek Grebosz] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // because some flags must be zeroed even if the detector didnt fired
            // otherwise the spikes will be
            //return;  // <-----------!
        }


        static unsigned long long first_TS= 0;
        static bool flag_first_event = true;
        unsigned long long TS = TNeedle_element::event_unpacked->timestamp;

        if(flag_first_event){
            first_TS  = TS;
            flag_first_event = false;
        }
        TS = TS - first_TS;

        if(TNeedle_element::event_unpacked->neda_fired.size() > 0)

            for(auto f : TNeedle_element::event_unpacked->neda_fired)
            {
                if(f.first < 85)
                    if(f.second)   // if this neda fired... we can read its timestamp
                    {
                        //                cout << "AAA Neda which fired key = " << f.first
                        //                << "   its timestamp = " <<
                        //                        TNeedle_element::event_unpacked->neda_data[f.first].timestamp
                        //                << endl;
                    }

            }

        if(TNeedle_element::event_unpacked->eagle_fired.size() > 0)
            for(auto f : TNeedle_element::event_unpacked->eagle_fired)
            {
                if(f.second)   // if this eagle fired... we can read its timestamp
                {
                    //                cout << "BBB Eagle which fired = " << f.first
                    //                << "   its timestamp = " <<
                    //                        TNeedle_element::event_unpacked->eagle_data[f.first].timestamp
                    //                << endl;
                }

            }

#if 0
        if(TNeedle_element::event_unpacked->dssd_fired.size() > 0)
            for(auto f : TNeedle_element::event_unpacked->dssd_fired)
            {
                if(f.second)   // if this dssd fired... we can read its timestamp
                {
                    //                cout << "BBB Eagle which fired = " << f.first
                    //                << "   its timestamp = " <<
                    //                        TNeedle_element::event_unpacked->dssd_data[f.first].timestamp
                    //                << endl;
                }

            }
#endif

        //    cout << "=====================TS raw = "
        //            << TNeedle_element::event_unpacked->timestamp
        //            <<   ", calculated TS = " << TS
        //              << " neda fired = " << TNeedle_element::event_unpacked->neda_fired.size()
        //              << " eagle fired = " << TNeedle_element::event_unpacked->eagle_fired.size()
        //         << " dssd fired = " << TNeedle_element::event_unpacked->dssd_fired.size()
        //              << endl;
        //    for( auto n : TNeedle_element::event_unpacked->dssd_data)
        //    {
        //        if(n.timestamp )
        //            cout << "timestamp z nedy = " << n.timestamp
        //                 << " calculated timest = " << n.timestamp - first_TS
        //                 << endl;
        //    }
        //    cout << "-------------------------------------- " << endl ;
    }


    TNeedle_element::event_unpacked->seconds =  time(nullptr);  // for some scalers

    if (GalileoAnalysis_ptr->is_verbose_on())
    {
        std::cout  << "Needle   subevent arrived " << std::endl;
    }
    for (unsigned int i = 0 ; i < module.size() ; i++) {
        module[i]->clear_already_done_flag() ;
    }

    // here we can check what was forgotten (not calculated
    // in a chain to get Zet), we can make it now.

    for (unsigned int i = 0 ; i < module.size() ; i++) {
        if (!module[i]->was_it_calculated()) {
            //      std::cout << "The "
            //		<<  module[i]->give_name()
            //		<< " was 'forgotten', so doing this now  !!! "
            //		<< std::endl ;
            module[i]->analyse_current_event();
        }
    }

    // Need to implement this also
    //    if(rfd_present && labr_present)
    //    ((Tlabr_array*)module[1])->analyse_coincidences_rfd();

    frs_watch_dog_report();
}
//********************************************************************
void  TNeedle::make_postloop_action()
{
    // save all spectra made in frs objecti and in frs elements
    // (mw, sci, etc.)
    std::cout <<  "Galileo saving " << spectra_gal.size() << " spectra... " << std::endl;
    for (unsigned int i = 0 ; i < spectra_gal.size() ; i++) {
        // std::cout << "Saving: " <<spectra_gal[i]->give_name() << "  .... ";
        spectra_gal[i]->save_to_disk() ;
        // std::cout << "done" << std::endl;
    }

    std::cout <<  "Galileo saved  spectra - finished " << std::endl;
}
//*******************************************************************
//*******************************************************************
//*******************************************************************
TNeedle_element *TNeedle::address_of_device(std::string name)
{
    //  transform(name.begin(), name.end(), name.begin(),  toupper);
    for (unsigned i = 0 ; i < name.size() ; i++)
        name[i] = toupper(name[i]);

    // we keep traces of all scintillators, mutliwires etc. in one vector
    //std::cout << "Searching the address of the element " << name << std::endl;

    for (unsigned int i = 0 ; i < module.size() ; i++) {
        //std::cout << module[i]->give_name() << std::endl;
        std::string found = module[i]->give_name() ;
        //transform(found.begin(), found.end(), found.begin(),  toupper);
        for (unsigned k = 0 ; k < found.size() ; k++)
            found[k] = toupper(found[k]);

        if (name == found) {
            return module[i] ; //returning pointer to this device
        }
    }

    // here we are if such was not found

    // perhaps somebody put 0 device

    if (name == "" || name == "0" || name == "NONE"
            )
        return nullptr ; //returning pointer to this device


    // really nothing

    std::cout << "Error during the constructor of the TNeedle:\n"
              << "You asked for an address of the device called " << name
              << " but it is not (yet) defined,\n"
              << "so its address can not be found. "
              << "Note: Till this place in the program, so far "
                 " the following devices were defined: "
              << std::endl ;

    for (unsigned int i = 0 ; i < module.size() ; i++) {
        std::cout << module[i]->give_name() << ", " ;
    }
    std::cout << std::endl;

    exit(-2);
}
//*****************************************************************
/**   For doppler correction algorithm */
double TNeedle::give_beta_tof()
{
    return 0.5 ;  // NOTE:: FAKE  zet_calc->give_beta();
}
//*****************************************************************
/** this function decides how often make autosave of spectra */
void TNeedle::perhaps_autosave()

{
#ifdef NIGDY   // this was moved to run_implicit_loop
#endif

}
//*****************************************************************
/** To refresh more often spectra which are currently on the screen
    of the grebware viewer */
//*****************************************************************
bool TNeedle::save_selected_spectrum(std::string name)
{

    for (unsigned int i = 0 ; i < spectra_gal.size() ; i++) {
        //        cout << "spectrim nr " << i
        //             << " name = "
        //             << spectra_gal[i]->give_name()
        //             << endl;

        if (name == spectra_gal[i]->give_name()) {
            spectra_gal[i]->save_to_disk() ;
            return true ;
        }
    }
    return false ; // was not here, perhaps in rising or hector?
}
//*********************************************************************
/**************************************************** No descriptions */
//int TNeedle::give_synchro_data()
//{
//    return  synchro_object->give_value() ;
//}
/***********************************************************************/
void TNeedle::frs_watch_dog_report()
{
#ifdef NIGDY
    static int events_in_statistics;

    // preparing statistics for frs -------------------------
    events_in_statistics++;

    //int i = 0 ;

#define inputev  TNeedle_element::event_unpacked

    put_to_statistics(inputev->sc21_de_l, 0) ;
    put_to_statistics(inputev->sc21_de_r, 1);

    //    std::cout << "left = " << (inputev->sc21_de_l)
    //    << ", right " << (inputev->sc21_de_r) << std::endl;
    put_to_statistics(inputev->sc21_l_sc21_r_dt, 2);

    put_to_statistics(inputev->sc21_r_sc41_r_dt, 3);
    put_to_statistics(inputev->sc21_l_sc41_l_dt, 4);

    put_to_statistics(inputev->sc41_de_l, 5);
    put_to_statistics(inputev->sc41_de_r, 6);
    put_to_statistics(inputev->sc41_l_sc41_r_dt, 7);

    for (int m = 0 ; m < 8 ; m++) {
        put_to_statistics(inputev->music_8_energy[m], 8 + m);
    }

    for (int m = 0 ; m < 8 ; m++) {
        put_to_statistics(inputev-> music_8_time[m], 16 + m);
    }


    if (mw41_ptr->was_x_ok() && mw41_ptr->was_y_ok()) {
        statistics_frs[24]++;
    }

    // checking the time of storing report

    //===========  WATCH DOG =========================================
    // checking the current time
    static time_t previous = time(0) ;
    time_t now = time(&now) ;

    const int how_often_seconds = 60  ;


    if (now - previous < how_often_seconds)
        return ;

    // now something for frs --------------------

    // opening the file
    std::string frs_name = "watch_dog_frs_statistics.log" ;
    std::ofstream plik_frs(frs_name.c_str());

    if (!plik_frs) {
        std::cout << "Can't open file " << frs_name
                  << " for current watch dog FRS report !!! " << std::endl ;
    }

    // plik << "Current report " << std::endl ;
    struct tm *timeinfo;

    timeinfo = localtime(&now);

    std::ostringstream iii ;
    iii << "FRS report was made at " <<  asctime(timeinfo)
           ; //<< ends ;  <------------ error for strings

    std::string information = iii.str() ;

    struct {
        int  moment;
        int real_seconds_of_measurement;
        int predicted_seconds;
        int nr_events;
    }
    opis ;

    opis.moment  = now ;
    opis.real_seconds_of_measurement = now - previous ;
    opis.predicted_seconds = how_often_seconds;
    opis.nr_events = events_in_statistics ;


    // how many seconds was really measured, how many it was predicted
    plik_frs.write((char *)  &opis, sizeof(opis));

    //frs  -----------------

    plik_frs.write((char *) statistics_frs, sizeof(statistics_frs));
    plik_frs.write((char *) information.c_str(), information.size());

    plik_frs.close() ;

    // zeroing
    memset(statistics_frs, 0 , sizeof(statistics_frs));
    events_in_statistics = 0 ;

    previous = now ;

#endif

}
/**********************************************************************/
inline void TNeedle::put_to_statistics(int x, int i)
{
    // const int
    //    down = 10,
    //    up = 4000 ;

    if ((x) > 10 && (x) < 4000) {
        statistics_frs[i]++ ;
        //    std::cout << "i = "<< i << " packed " << statistics_frs[i]
        //          << " because x= " << x << std::endl;
    }
    //i++;
}
/**********************************************************************/
void TNeedle::put_data_to_ntuple_tree()
{}

//**********************************************************************
void TNeedle::prepare_scalers_for_active_stopper()
{

}
//**********************************************************************
void TNeedle::SetupExperiment()
{
    //max_subdata = HOW_MANY_SUBCHANNELS; /* Dino says so*/
    SetExperiment();
    SetLookUpTable();

}
//**********************************************************************
void TNeedle::SetLookUpTable()
{
    /* std::string look_file(path.dir_experiment_setup() + "galileo_lookup_table.txt");
     std::ifstream LUT(look_file);
     if (!LUT) {
     std::cerr << "Error while opening file " << look_file << std::endl;
     std::exit(10);
     }
     number_of_germanium  = 0 ;
     number_of_euclides   = 0 ;
     //number_of_neutronwall= 0 ;
     euclides_present.clear();
     germanium_present.clear();
     while(LUT.good()){
     std::string aline;
     std::getline(LUT,aline);
     if(aline.empty()) continue;
     if(aline[0]=='#' || aline[0]=='/') continue;
     std::istringstream oneline(aline);
     TString DetName;int domain=-1;
     oneline >> DetName >> domain;

     if(domain<0 || DetName.Length()==0) continue;
     TObjArray* toks=DetName.Tokenize("_");
     TString detector_type = ((TObjString*)toks->At(0))->GetString();
     int detector_array_nb  =  (((TObjString*)toks->At(1))->GetString()).Atoi();
     if(detector_type.CompareTo("germanium")==0){
     number_of_germanium++;
     germanium_present[detector_array_nb]=true;
     }
     else if (detector_type.CompareTo("euclides")==0){
     number_of_euclides++;
     euclides_present[detector_array_nb]=true;
     }

     LookUpTable[domain] = std::make_pair(detector_array_nb,DetName);
     delete toks;
     }

     std::map<int,std::pair<int,std::string> >::iterator it_ = LookUpTable.begin();
     for(;it_ != LookUpTable.end();++it_){
     std::cout << it_->second.second << " "
     << it_->second.first  << " "
     << it_->first << std::endl;
     }

     std::cout << "LookUp Table read correctly: " << std::endl
     << "  " << number_of_germanium   << " Germanium detector"     <<  std::endl
     << "  " << number_of_euclides    << " Euclides Telescopes"    <<  std::endl
     ;*/
}
//***************************************************************************************************
void TNeedle::create_groups_of_spectra()
{
    std::map<std::pair<TString,int>, std::vector<TString> > spec_container;
    for(unsigned int j=0 ; j<spectra_gal.size() ;j++){
        TString spec_name =  spectra_gal[j]->give_name();
        TObjArray* toks=spec_name.Tokenize("_");
        TString detector_type = ((TObjString*)toks->At(0))->GetString();
        if(detector_type.CompareTo("Ge")==0 || detector_type.CompareTo("Eu") == 0){
            int detector_array_nb  =  (((TObjString*)toks->At(1))->GetString()).Atoi();
            std::pair<TString,int> index = std::make_pair(detector_type,detector_array_nb);
            spec_container[index].push_back(spec_name);
        }
        delete toks;
    }
    std::map<int,std::pair<int,std::string> >::iterator it_ = LookUpTable.begin();
    for(;it_ != LookUpTable.end();++it_){
        std::string filename= path.dir_groups() + it_->second.second + ".group";
        std::ofstream det_group; det_group.open(filename.c_str());
        std::ostringstream name_prefix;
        if(it_->second.second[0]=='g'){
            std::pair<TString,int> index = std::make_pair("Ge",it_->second.first);
            for(unsigned int j=0;j<spec_container[index].size();++j){
                if(spec_container[index][j].Contains("_vs_")){
                    det_group << spec_container[index][j] << ".mat" << std::endl;
                }
                else {
                    det_group << spec_container[index][j] << ".spc" << std::endl;
                }
            }
        }
        else if(it_->second.second[0]=='e'){
            std::pair<TString,int> index = std::make_pair("Eu",it_->second.first);
            for(unsigned int j=0;j<spec_container[index].size();++j){
                if(spec_container[index][j].Contains("_vs_")){
                    det_group << spec_container[index][j] << ".mat" << std::endl;
                }
                else {
                    det_group << spec_container[index][j] << ".spc" << std::endl;
                }
            }

        }
        else if(it_->second.second[0]=='n'){
            det_group << it_->second.second << ".spc";
        }
    }
}
//**********************************************************************
void TNeedle::SetExperiment()
{
    std::string exp_description(path.dir_experiment_setup() + "experiment_description.txt");
    std::ifstream EXP(exp_description);
    if (!EXP) {
        std::cerr << "Error while opening file " << exp_description << std::endl;
        std::exit(10);
    }
    while(EXP.good()){
        std::string aline;
        std::getline(EXP,aline);
        if(aline.empty()) continue;
        if(aline[0]=='#' || aline[0]=='/') continue;
        std::istringstream oneline(aline);
        TString keyword;
        oneline >> keyword;
        if (keyword.CompareTo("beam")==0){
            unsigned int z=0; unsigned int a=0;
            oneline >> z >> a;
            SetBeam(z,a);
        }
        else if (keyword.CompareTo("target")==0){
            unsigned int z=0; unsigned int a=0;
            oneline >> z >> a;
            SetTarget(z,a);
        }
        else if (keyword.CompareTo("beta")==0){
            double beta;
            oneline >> beta;
            SetRecoilVelocity(beta);
        }
        else if (keyword.CompareTo("beam_energy")==0){
            double energy;
            oneline >> energy;
            SetBeamEnergy(energy);
        }
        else if(keyword.CompareTo("NEDA")==0){
            oneline >> neda_present;
            if(!SetupNEDA(neda_present)){
                std::cerr << "Error with the Setup of NEDA detectors" << std::endl;
                std::exit(1);
            }
        }
        else if(keyword.CompareTo("EAGLE")==0){
            oneline >> eagle_present;
            if(!SetupEAGLE(eagle_present)){
                std::cerr << "Error with the Setup of EAGLE detectors" << std::endl;
                std::exit(1);
            }
        }
        else if(keyword.CompareTo("DSSD")==0){
            oneline >> dssd_present;
            if(!SetupDSSD(dssd_present)){
                std::cerr << "Error with the Setup of DSSD detectors" << std::endl;
                std::exit(1);
            }
        }
        else {
            std::cerr << "Keyword not recognize, Please check TNeedle.cxx" << std::endl;
            \
            continue;
        }
    }

}
//**********************************************************************

bool TNeedle::SetupNEDA(bool present)
{
    if(!present){
        std::cout << "NEDA is not present in the setup so we don't need to do anything for it!!" << std::endl;
        return true;
    }
    //    std::cout << "NEDA is in the experimental setup so let's configure it!" << std::endl;
    if(Read_NEDA_LookUpTable()) return true;
    else return false;
}
//**********************************************************************

bool TNeedle::SetupEAGLE(bool present)
{
    if(!present){
        std::cout << "EAGLE is not present in the setup so we don't need to do anything for it!!" << std::endl;
        return true;
    }
    //    std::cout << "EAGLE is in the experimental setup so let's configure it!" << std::endl;
    if(Read_EAGLE_LookUpTable()) {
        return true;
    }
    else return false;
}

//**********************************************************************

bool TNeedle::SetupDSSD(bool present)
{
    if(!present){
        std::cout << "DSSD is not present in the setup so we don't need to do anything for it!!" << std::endl;
        return true;
    }
    //    std::cout << "DSSD is in the experimental setup so let's configure it!" << std::endl;
    if(Read_DSSD_LookUpTable()) {
        return true;
    }
    else return false;
}

//**********************************************************************

bool TNeedle::Read_NEDA_LookUpTable()
{
    //Here we use the same function than in the offline analysis so that the
    //lookup table for online/offline are finally the same except for the
    //first column
    std::stringstream LUTFile;
    //    int detector_array_nb = -1;
    TString signal_type;
    LUTFile << path.dir_experiment_setup() << "neda_lookup_table_and_calibration.txt";
    std::ifstream lookuptable(LUTFile.str().c_str());

    if(!lookuptable.good()){
        std::cerr << "Could not open " << LUTFile.str().c_str()
                  << " and I need it" << std::endl;
        std::abort();
        //        return false;
    } else {
        while(lookuptable.good()){
            std::string oneline;
            std::getline(lookuptable,oneline);
            if(!lookuptable.good()) continue;
            if(oneline[0]=='#') continue; // ignore lines stating with #
            if(oneline[0]=='/') continue; // ignore lines start with /
            if(oneline.empty()) continue; // ignore empty lines
            std::istringstream aline(oneline);
            int board, channel;
            bool enable = false;
            aline >> board >> channel >> enable;
            if(enable==1){
                channel = board *16 + channel;
                NEDA_LUT[channel]=new TNEDADetector();
                //                std::cout << " NEDA AT CHANNEL "  << channel << std::endl;
                TString DetName;
                float theta(0.),phi(0.),thr_CS(0.);
                int pol_order(0);
                aline >> DetName >> thr_CS >> theta >> phi >> pol_order;
                NEDA_LUT[channel]->SetPosition(theta,phi);
                NEDA_LUT[channel]->SetName(DetName.Data());
                for(int j = 0 ; j<pol_order;++j){
                    float par = -FLT_MAX;
                    aline >> par;
                    if (par>-FLT_MAX) {
                        NEDA_LUT[channel]->AddCalibrationFactors(par);
                        //                        cout << "par = " << par << endl;
                    }
                }
            }
        }
    }
    return true;
}

//**********************************************************************

bool TNeedle::Read_EAGLE_LookUpTable()
{
    //Here we use the same fonction than in the offline analysis so that the
    //lookup table for online/offline are finally the same except for the
    //first column
    std::stringstream LUTFile;
    //    int detector_array_nb = -1;
    TString signal_type;
    LUTFile << path.dir_experiment_setup() << "eagle_lookup_table_and_calibration.txt";
    std::ifstream lookuptable(LUTFile.str().c_str());
    if(!lookuptable.good()){
        std::cerr << "Could not open " << LUTFile.str().c_str()
                  << " and I need it" << std::endl;
        std::abort();
        //return false;
    } else {
        while(lookuptable.good()){
            //            cout << "TOP -------------" << endl;
            std::string oneline;
            std::getline(lookuptable,oneline);
            if(!lookuptable.good()) continue;

            if(oneline[0]=='#') continue; // ignore lines stating with #
            if(oneline[0]=='/') continue; // ignore lines start with /
            if(oneline.empty()) continue; // ignore empty lines
            std::istringstream aline(oneline);
            int board, channel;
            int enable = 0;  // NOTE: now enabe is int!
            aline >> board >> channel >> enable;

            if(enable==1)
            {
                channel = board *16 + channel;
                EAGLE_LUT[channel]=new TEAGLEDetector();
                //                std::cout << " enable 1 so EAGLE AT CHANNEL "  << channel << std::endl;
                TString DetName;
                float theta(0.),phi(0.),thr_CS(0.);
                int pol_order(0);
                aline >> DetName >> thr_CS >> theta >> phi >> pol_order;
                EAGLE_LUT[channel]->SetPosition(theta,phi);
                EAGLE_LUT[channel]->SetName(DetName.Data());
                //                cout << "Detector nr (channel): " << channel << " DetName " << DetName ;
                for(int j = 0 ; j < pol_order; ++j)
                {
                    float par = -FLT_MAX;
                    aline >> par;
                    if (par > -FLT_MAX) {
                        //                        cout << " coeff = " << par ;
                        EAGLE_LUT[channel]->AddCalibrationFactors(par);
                    }
                }
                //                                cout << endl;
            }
            else if(enable==2)
            {

                channel = board *16 + channel;
                EAGLE_LUT[channel]=new TEAGLEDetector();
                //                std::cout << " \t\tenable 2 so Compton EAGLE AT CHANNEL "  << channel << std::endl;
                TString DetName;
                float thr_CS(0.);
                aline >> DetName >> thr_CS ;
                EAGLE_LUT[channel]->SetName(DetName.Data());
                //                std::cout << " \t\tCompton EAGLE NAME= ["  << DetName.Data() << "]" << std::endl;

            }
        }

    }
    cout << "Eagle Lookup table size = " << EAGLE_LUT.size() << endl;
    if(EAGLE_LUT.size() >= HOW_MANY_EAGLE)
    {
        cout << "Error, too many Eagle detectors, "
                << "\nplease change HOW_MANY_EAGLE definition "
                << "in the file experiment_def.h to at least "
                << EAGLE_LUT.size() +1
                << " and recompile the SPY" << endl;
        abort();
    }

return true;
}
//***************************************************************************
//**********************************************************************

bool TNeedle::Read_DSSD_LookUpTable()
{
    //    cout << __PRETTY_FUNCTION__ << endl;
    //Here we use the same fonction than in the offline analysis so that the
    //lookup table for online/offline are finally the same except for the
    //first column
    std::stringstream LUTFile;
    //    int detector_array_nb = -1;
    TString signal_type;
    LUTFile << path.dir_experiment_setup() << "dssd_lookup_table_and_calibration.txt";
    std::ifstream lookuptable(LUTFile.str().c_str());
    if(!lookuptable.good()){
        std::cerr << "Could not open " << LUTFile.str().c_str()
                  << " and I need it" << std::endl;
        std::abort();
        //return false;
    } else {
        while(lookuptable.good()){
            std::string oneline;
            std::getline(lookuptable,oneline);
            if(!lookuptable.good()) continue;

            if(oneline[0]=='#') continue; // ignore lines stating with #
            if(oneline[0]=='/') continue; // ignore lines start with /
            if(oneline.empty()) continue; // ignore empty lines
            std::istringstream aline(oneline);
            int board, channel;
            int enable = 0;  // NOTE: now enabe is int!
            aline >> board >> channel >> enable;

            if(enable==1)
            {
                channel = board *16 + channel;
                DSSD_LUT[channel]=new TDSSDDetector();
                //                std::cout << " DSSD AT CHANNEL "  << channel << std::endl;
                TString DetName;
                float theta(0.),phi(0.),thr_CS(0.);
                int pol_order(0);
                aline >> DetName >> thr_CS >> theta >> phi >> pol_order;
                //                cout << "DetName = " << DetName
                //                     << ", THETA = " << theta << ", pHI = " << phi << endl;

                DSSD_LUT[channel]->SetPosition(theta,phi);
                DSSD_LUT[channel]->SetName(DetName.Data());
                //                cout << "Detector nr (channel): " << channel ;
                for(int j = 0 ; j < pol_order; ++j)
                {
                    float par = -FLT_MAX;
                    aline >> par;
                    if (par > -FLT_MAX) {
                        //                        cout << " coeff = " << par ;
                        DSSD_LUT[channel]->AddCalibrationFactors(par);
                    }
                }
                //                cout << endl;
            }
            else if(enable==2)
            {

                channel = board *16 + channel;
                DSSD_LUT[channel]=new TDSSDDetector();
                //                std::cout << " Compton DSSD AT CHANNEL "  << channel << std::endl;
                TString DetName;
                float thr_CS(0.);
                aline >> DetName >> thr_CS ;
                DSSD_LUT[channel]->SetName(DetName.Data());
                //                std::cout << " Compton DSSD NAME= ["  << DetName.Data() << "]" << std::endl;

            }
        }

    }

    return true;
}




