
/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   jerzy.grebosz@ifj.edu.pl   *
 *                                                                         *
 ***************************************************************************/

#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>

#include "data_psa_65000_1.h"
#include "tbuffer_of_data_block.h"
#include "TGalileoEvent.h"
#include "TGalileoAnalysis.h"

extern TGalileoAnalysis *GalileoAnalysis_ptr;
std::map<int,TNEDADetector* >     data_psa_65000_1::lookup_neda;
std::map<int,TEAGLEDetector* >    data_psa_65000_1::lookup_eagle;
std::map<int,TDSSDDetector* >    data_psa_65000_1::lookup_dssd;

bool data_psa_65000_1::flag_lookup_loaded = false;

bool flag_talk2 = false; // true ; //
//******************************************************************
data_psa_65000_1::data_psa_65000_1()
{
    galileo_pointer = GalileoAnalysis_ptr->give_gal_ptr();
    neda_pres    = galileo_pointer->IsNEDAPresent();
    eagle_pres   = galileo_pointer->IsEAGLEPresent();

    dssd_pres   = galileo_pointer->IsDSSDPresent();

    if(neda_pres) {
        event_ptr->neda_fired.clear();
    }
    if(eagle_pres){
        event_ptr->eagle_fired.clear();
    }
    if(dssd_pres){
//        event_ptr->dssd_fired.clear();
    }

    if(!flag_lookup_loaded){
        lookup_neda.clear();
        lookup_eagle.clear();
        lookup_dssd.clear();

        //std::cout << "First time so we set the lookup table" << std::endl;
        //TGalileoAnalysis *ptr_anal = GalileoAnalysis_ptr;
        if(neda_pres){
            lookup_neda  = galileo_pointer->Get_NEDA_LookUpTable();
            //std::cout << "We have the look up table of NEDA" << std::endl;
        }
        if(eagle_pres){
            lookup_eagle = galileo_pointer->Get_EAGLE_LookUpTable();
            // std::cout << "We have the look up table of EAGLE" << std::endl;
        }

        if(dssd_pres){
            lookup_dssd = galileo_pointer->Get_DSSD_LookUpTable();
            // std::cout << "We have the look up table of DSSD" << std::endl;
        }

        flag_lookup_loaded = true;
    }
}
//*****************************************************************
using namespace std;
//******************************************************************
int data_psa_65000_1::fill_neda_structure(Tbuffer_of_data_block *buf,
                                          Tkey *key)
{

    if(key->give_length()!=0x34)std::cout << "Strange neda event size: " << key->give_length() << std::endl;
    int this_frame_length = 0;

    frameKey        = key->give_message_type()   ;
    detector_domain = key->give_id_of_detector() ;
    tstamp          = key->give_timestamp()      ;
    //if(flag_talk2)
        //printf("TS = %08X %08X\n",key->give_ts_high(),key->give_ts_low());
//    std::cout << __PRETTY_FUNCTION__ << " tstamp = " << tstamp
//              << "  detector_domain = " << detector_domain
//              << endl;

#define get_item(x)     memcpy(&x, buf->gg(sizeof(x)), sizeof(x));	\
    this_frame_length+= sizeof(x) ;

    subDataNEDA_t d;

    get_item(d.qshort);
    get_item(d.qlong);
    get_item(d.qfast);
    get_item(d.qslow);
    get_item(d.light);
    get_item(d.cfd);
    get_item(d.intrise);
    get_item(d.nbzero);
    get_item(d.flags);

    d.timestamp = tstamp;

    if(!key->IsIdle()) {
        sub_data_neda = d;

//        std::cout << __func__
//                  << " tstamp = " << tstamp
//                  << "  detector_domain = " << detector_domain << endl;
    }
    else {
        detector_domain = -1;
    }



    if(flag_talk2)std::cout << "This frame length = " << this_frame_length << std::endl;
    return this_frame_length;
}
//***************************************************************************
int data_psa_65000_1::fill_eagle_structure(Tbuffer_of_data_block *buf,
                                           Tkey *key)
{

    int this_frame_length = 0;
    if(key->give_length()!=24)std::cout << "Strange eagle event size: " << key->give_length() << std::endl;
    frameKey        = key->give_message_type()   ;
    detector_domain = key->give_id_of_detector() ;
    tstamp          = key->give_timestamp()      ;
    if(flag_talk2)
        printf("TS = %08X %08X\n",key->give_ts_high(),key->give_ts_low());
    subDataEAGLE_t d;
    get_item(d.energy);
    get_item(d.cfd);
    d.timestamp = tstamp;

    if(!key->IsIdle()){
        sub_data_eagle = d;
//        std::cout << __func__
//                  << " tstamp = " << tstamp
//                  << "  detector_domain = " << detector_domain << endl;
    }
    else {
        detector_domain = -1;
    }

    if(flag_talk2)std::cout << "This frame length = " << this_frame_length << std::endl;
    return this_frame_length;
}
//***************************************************************************
int data_psa_65000_1::fill_dssd_structure(Tbuffer_of_data_block *buf,
                                           Tkey *key)
{

    int this_frame_length = 0;
    if(key->give_length()!=24)std::cout << "Strange dssd event size: " << key->give_length() << std::endl;
    frameKey        = key->give_message_type()   ;
    detector_domain = key->give_id_of_detector() ;
    tstamp          = key->give_timestamp()      ;
    if(flag_talk2)
        printf("TS = %08X %08X\n",key->give_ts_high(),key->give_ts_low());
    subDataDSSD_t d;
    get_item(d.energy);
    get_item(d.cfd);
    d.timestamp = tstamp;

    if(!key->IsIdle()){
        sub_data_dssd = d;
//        std::cout << __func__
//                  << " tstamp = " << tstamp
//                  << "  detector_domain = " << detector_domain << endl;
    }
    else {
        detector_domain = -1;
    }

    if(flag_talk2)std::cout << "This frame length = " << this_frame_length << std::endl;
    return this_frame_length;
}
//***************************************************************************
/*!
  \fn data_psa_65000_1::distribute_data()
*/
void data_psa_65000_1::distribute_data()
{
    if(flag_talk2)
        cout << __func__ << " frameKey " << hex << frameKey << dec << endl;

    //    if(frameKey != keyDataGGP)
    //        cout << "Note: this is not keyDataGGP data" << endl;

    //    cout << "sub_data_vector size = " << sub_data_vector.size() << endl;

    if (detector_domain < 0) {
        // cout << "domain ujemny" << endl;
        return;
}
    try {
        //        std::cout
        //                  << "Timestamp = " << tstamp
        //                  << std::endl;
        switch(frameKey){

        case keyDataEAGLE:
            if(flag_talk2)
                std::cout << "--------- keyDataEAGLE for detector_domain nr "
                          << detector_domain
                             //<< ", timestamp = " << tstamp
                          << std::endl;


            if(lookup_eagle.find(detector_domain) == lookup_eagle.end()){
                std::cerr << "Error: Detector domain " << detector_domain << " is not in EAGLE lookup table!" << std::endl;
            }
            else{
                event_ptr->eagle_fired[detector_domain]=true;
                event_ptr->eagle_data[detector_domain].energy    = sub_data_eagle.energy   ;
                event_ptr->eagle_data[detector_domain].cfd       = sub_data_eagle.cfd      ;
                event_ptr->eagle_data[detector_domain].timestamp       = sub_data_eagle.timestamp;
                event_ptr->timestamp = tstamp;
                //                 std::cout << "--------- keyDataEAGLE for detector_domain nr "
                //                           << detector_domain

                //                 << ",  energy = " << sub_data_eagle.energy
                //                           << "2 Timestamp = " << tstamp
                //                           << " sub_data_eagle.timestamp = "
                //                            << sub_data_eagle.timestamp
                //                           << std::endl;
            }
            break;

        case keyDataNEDA:
             if(flag_talk2)
                std::cout << "BBB keyDataNEDA, detector_domain= " << detector_domain << std::endl;

            if(lookup_neda.find(detector_domain)==lookup_neda.end())
            {
                std::cerr << "Error: Detector domain " << detector_domain << " is not in NEDA lookup table!" << std::endl;
                int m = detector_domain ;
                cout << "detector_domain = " << m << endl;
                 m++;


            }else{
                event_ptr->neda_fired[detector_domain]=true;
                event_ptr->neda_data[detector_domain].timestamp = tstamp;
                event_ptr->neda_data[detector_domain].qshort    = sub_data_neda.qshort   ;
                event_ptr->neda_data[detector_domain].qlong     = sub_data_neda.qlong    ;
                event_ptr->neda_data[detector_domain].qfast     = sub_data_neda.qfast    ;
                event_ptr->neda_data[detector_domain].qslow     = sub_data_neda.qslow    ;
                event_ptr->neda_data[detector_domain].light     = sub_data_neda.light    ;
                event_ptr->neda_data[detector_domain].cfd       = sub_data_neda.cfd      ;
                event_ptr->neda_data[detector_domain].intrise   = sub_data_neda.intrise  ;
                event_ptr->neda_data[detector_domain].nbzero    = sub_data_neda.nbzero   ;
                event_ptr->neda_data[detector_domain].flags     = sub_data_neda.flags    ;

                event_ptr->timestamp = tstamp;

//                                std::cout
//                                          << "2 Timestamp = " << tstamp
//                                          << " sub_data_neda.timestamp = "
//                                          << sub_data_neda.timestamp
//                                          << std::endl;

//                                std::cerr << "Detector domain "
//                                          << detector_domain

//                                          << " tstamp " << tstamp
//                //                          << " has qlong = " <<
//                //                          event_ptr->neda_data[detector_domain].qlong
//                                          << std::endl;
                // Trigger for nedda comes in id = 92

            }

            break;
        case keyDataDSSD:
            if(flag_talk2)
                std::cout << "--------- keyDataDSSD for detector_domain nr "
                          << detector_domain
                             //<< ", timestamp = " << tstamp
                          << std::endl;





            if(lookup_dssd.find(detector_domain) == lookup_dssd.end()){
                std::cerr << "Error: Detector domain " << detector_domain << " is not in DSSD lookup table!"
                    << std::endl;

            }
            else{

                string name = lookup_dssd.find(detector_domain)->second->GetName();

                int det_nr = stoi(name) ;
                event_ptr->dssd_fired[det_nr]=true;
                event_ptr->dssd_data[det_nr].energy    = sub_data_dssd.energy   ;
                event_ptr->dssd_data[det_nr].cfd       = sub_data_dssd.cfd      ;
                event_ptr->dssd_data[det_nr].timestamp       = sub_data_dssd.timestamp;
                event_ptr->timestamp = tstamp;
//                std::cout << "--------- keyDatadssd for det_nr "
//                                           << det_nr

//                                           << ",  energy = " << sub_data_dssd.energy
//                                           << "2 Timestamp = " << tstamp
//                                           << " sub_data_dssd.timestamp = "
//                                            << sub_data_dssd.timestamp
//                                            << std::endl;
            }
            break;


            /*
        case keyDataGGP:
    if(flag_talk2) cout << "keyDataGGP" << endl;
    if(galileo_pres){
    int siz = sub_data_vector.size();

    for (int i = 0 ; i < fmin(siz , max_subdata) ; i++) {
    event_ptr->eagle_fired[detector_domain]=true;
    event_ptr->germanium_data[detector_domain][i].timestamp = tstamp;
    event_ptr->germanium_data[detector_domain][i].eAmpli    = sub_data_vector[i].eAmpli;
    event_ptr->germanium_data[detector_domain][i].eEnergy   = sub_data_vector[i].eEnergy ;
    event_ptr->germanium_data[detector_domain][i].wBase     = sub_data_vector[i].wBase;
    event_ptr->germanium_data[detector_domain][i].wAmpli    = sub_data_vector[i].wAmpli ;
    event_ptr->germanium_data[detector_domain][i].wEnergy   = sub_data_vector[i].wEnergy;
    event_ptr->germanium_data[detector_domain][i].trTime    = sub_data_vector[i].trTime ;
    event_ptr->germanium_data[detector_domain][i].xAmpli    = sub_data_vector[i].xAmpli;
    event_ptr->germanium_data[detector_domain][i].xEnergy   = sub_data_vector[i].xEnergy;
    } // end for
    }
    break;

      */

        default:
                  printf("Warning: 0x%08X detector key not implemented\n",frameKey);
            break;
        }
    }// end try
    catch (std::out_of_range) {
        std::cout << "data_psa_65000_1::distribute_data()      Alarm, detector_domain = " << detector_domain << std::endl;
    }
}
//***************************************************************************

