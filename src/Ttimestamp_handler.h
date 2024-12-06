#ifndef _timestamp_handler_H_
#define _timestamp_handler_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TGalileoEvent class
#ifndef  __CINT__



#include "TNeedle_element.h"
#include "spectrum.h"
#include <string>

#include "TGalileoEvent.h"
#include "Tincrementer_donnor.h"

//////////////////////////////////////////////////////////////////////////////
class Ttimestamp_handler : public TNeedle_element, public Tincrementer_donnor
{

    // friend class focus_point ; // will read this private data, but using
    // constant pointer
    double distance ;
    double left_raw, right_raw, up_raw, down_raw, anode_raw ;

    unsigned long long first_timestamp;

    bool flag_first_time = true;

    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int TGalileoEvent::*x_left;
    int TGalileoEvent::*x_right;
    int TGalileoEvent::*y_up;
    int TGalileoEvent::*y_down;
    int TGalileoEvent::*anode;


    spectrum_1D * spec_all_ts;
//    spectrum_1D * spec_right ;
//    spectrum_1D * spec_up;
//    spectrum_1D * spec_down ;
//    spectrum_1D * spec_anode ;

//    spectrum_1D * spec_x ;
//    spectrum_1D * spec_x_sum ;
//    spectrum_1D * spec_y ;
//    spectrum_1D * spec_y_sum ;

//    spectrum_2D * spec_xy ;


public:
    //  constructor
    Ttimestamp_handler(std::string _name);
//    ,
//               int TGalileoEvent::*timestamp) ;


    void analyse_current_event() ;
    void make_preloop_action(std::ifstream &);   // read thye calibration factors, gates


protected:
    void create_my_spectra();
    //void check_legality_of_data_pointers();

};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _timestamp_handlerMożna_H_
