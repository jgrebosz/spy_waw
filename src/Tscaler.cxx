/***************************************************************************
                          tscaler.cpp  -  description
                             -------------------
    begin                : Tue May 20 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
 ********************************************** *****************************/

#include "Tscaler.h"
#include "Tnamed_pointer.h"

using namespace std;


//************************************************************************
Tscaler::Tscaler(string _name,
                 int TGalileoEvent::*data,
                 unsigned long long TGalileoEvent::*seconds_
                 ) : TNeedle_element(_name),
    data_ptr(data),
    time_of_the_measurement_in_seconds(seconds_)
{
    seconds_in_channel_zero = 0 ;
    previous_counts = 0 ;
    last_current_counts = 0 ;
    current_counts = 0 ;
    difference_value = 0 ;
    suwak = 0 ;

    counts_diff_in_last_unit_of_time = 0;
    previous_time_of_the_measurement_in_seconds = 0;


    create_my_spectra();

    named_pointer[name_of_this_element + "_previous_counts"]
            = Tnamed_pointer(&previous_counts, nullptr, this) ;

    named_pointer[name_of_this_element + "_current_counts"]
            = Tnamed_pointer(&current_counts, nullptr, this) ;

    named_pointer[name_of_this_element + "_difference_value"]
            = Tnamed_pointer(&difference_value, nullptr, this) ;


    named_pointer[name_of_this_element + "_counts_diff_in_last_unit_of_time"]
            = Tnamed_pointer(&counts_diff_in_last_unit_of_time, nullptr, this) ;

}
//************************************************************************
Tscaler::~Tscaler()
{}
//************************************************************************
void  Tscaler::create_my_spectra()
{

    //   string folder = "scalers/" + name_of_this_element ;
    string folder = "scalers/" ;
    string comment = string("Unit is 1s. This graph will slowly move left, just like the paper tape") ;
    // relative -----------------------------------
    string name = name_of_this_element   ;
    spec_rel = new spectrum_1D(name, name,
                               //                              400, 0, 400,
                               1000, 0, 1000,
                               folder, comment);

    frs_spectra_ptr->push_back(spec_rel) ;
}
//**********************************************************************
void Tscaler::analyse_current_event()
{
    //   cout << "analyse_current_event()  for "
    //            <<  name_of_this_element
    //             << endl ;

    static bool flag_first_time = true ;
    // -------------------------------------------------------------
    current_counts =  event_unpacked->*data_ptr ;

    if(current_counts > 0)
    {

        //   cout << name_of_this_element << " = "  << current_counts << ", while time "
        //   << event_unpacked->*time_of_the_measurement_in_seconds << endl;
        //
        //    if(name_of_this_element == "scaler_frs_free_trigger")
        //    {
        //    current_counts = current_counts ;  // fake for breakpoiint
        //    }
        //

        unsigned int channel_to_increment ;

        channel_to_increment =
                event_unpacked->*time_of_the_measurement_in_seconds -
                seconds_in_channel_zero  ;

        if(flag_first_time)
        {
            seconds_in_channel_zero = (unsigned)(event_unpacked->*time_of_the_measurement_in_seconds);
            flag_first_time = false ;
        }

        if(channel_to_increment > 1000)  // there was some garbage
            //    if(channel_to_increment > 100000 )  // there was some garbage
        {
            channel_to_increment = 0 ;
            seconds_in_channel_zero = (unsigned)(event_unpacked->*time_of_the_measurement_in_seconds);
        }

        if(channel_to_increment > 960)
            //    if(channel_to_increment > 99000)
        {
            // scroll by one minute
            int scroll_nr = 60 ;

            seconds_in_channel_zero += scroll_nr ;
            spec_rel->scroll_left_by_n_bins(scroll_nr);
            channel_to_increment -= scroll_nr ;
        }

        previous_counts = last_current_counts ;

        // the first incrementation will be a nonsese !, so...
        difference_value = (previous_counts != 0) ?
                    current_counts - previous_counts  :  0 ;

//            cout << "scaler= " <<name_of_this_element
//            << ",  channel " <<(int)channel_to_increment
//            << ", with difference_value " << difference_value
//             << ", with current_counts " << current_counts
//            << endl ;


        //spec_rel ->inc_by((int)channel_to_increment, difference_value) ;
        spec_rel ->inc_by((int)channel_to_increment, current_counts) ;
        //  spec_rel ->manually_inc_by((int)suwak++, difference_value ) ;

        last_current_counts = current_counts;


    } // endif sensible y data
    calculations_already_done = true ;
}
//**************************************************************************


//**************************************************************************
void  Tscaler::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{
    //cout << "preloop action for Reading the calibration for " << name_of_this_element << endl ;

    // remembering the start ?

    // ask what is ther right egde of the "paper"

    right_edge = spec_rel->give_max_chan_paper();

}
//***************************************************************
/** dead time scaler may ask other scalers for their results
 */
int Tscaler::give_last_value()
{
    if(!calculations_already_done) analyse_current_event();
    return last_current_counts ;
}
