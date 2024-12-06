/***************************************************************************
                          tscaler_template.h  -  description
                             -------------------
    begin                : Tue May 20 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/


#ifndef TSCALER_template_H
#define TSCALER_template_H


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TGalileoEvent class
#ifndef  __CINT__

#include "TNeedle_element.h"
#include "spectrum.h"
#include <string>
#include "TGalileoEvent.h"

#include <time.h>
#include "Tincrementer_donnor.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**FRS scalers
  *@author Jurek Grebosz (IFJ Krakow, Poland)
  */
////////////////////////////////////////////////////////////////////////
/// \brief The Tscaler_template class
///
/// using namespace std;
///
enum class Tscaler_mode {  difference_value, real_value, value_one };
//--------------------------------------------------------------------------------
template <typename T>
class Tscaler_template : public TNeedle_element,  public Tincrementer_donnor
{
protected:
    T TGalileoEvent::*data_ptr;
    T TGalileoEvent::*time_of_the_measurement_in_seconds ;

    T previous_counts ;  // this we will need to exist outside this object, for users
    T last_current_counts ;  // here we store
    T current_counts ;
    T difference_value ;


    Tscaler_mode mode;
    // normally a scaller value is growing, but we want to show a value
    // between previous data and current data. But sometimes
    // this value is not growing, it is already send as absolute value
    bool flag_difference_scaler = true;


    int last_incremented_channel ;
    long maxchan = 3600;
    int suwak;
    T timestamps_FRS_at_beg_of_current_suwak_bin ;

    // seconds in channel zero
    int seconds_in_channel_zero ;

    spectrum_1D * spec_rel ;
    double right_edge;



    // sometime we want to konw how many we had in the last unit of time
    // (difference says only the difference since last event)
    T counts_diff_in_last_unit_of_time;

    T previous_time_of_the_measurement_in_seconds;
    T counts_at_begining_of_last_unit ; // second;

public:
    Tscaler_template(std::string _name,
                     T TGalileoEvent::*data,
                     T TGalileoEvent::*seconds_,
                     Tscaler_mode mode) ;
    ~Tscaler_template();

    // calibration i calculation
    void analyse_current_event() ;
    void make_preloop_action(std::ifstream &);   // read the calibration factors, gates

    /** dead time scaler may ask other scalers for their results
     */
    T give_last_value(){
        if(!calculations_already_done) analyse_current_event();
        return last_current_counts ;
    }
    int give_curr_diff_value()
    {
        return difference_value ;
    }

protected:
    void create_my_spectra();

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***************************************************************************
                              Member functions
********************************************** *****************************/

//#include "Tscaler_template.h"
#include "Tnamed_pointer.h"




//************************************************************************
template <typename T>
Tscaler_template<T>::Tscaler_template(std::string _name,
                                      T TGalileoEvent::*data,
                                      T TGalileoEvent::*seconds_,
                                      Tscaler_mode mode_
                                      ) : TNeedle_element(_name),
    data_ptr(data),
    time_of_the_measurement_in_seconds(seconds_),
    mode(mode_)
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

    //    named_pointer[name_of_this_element + "_previous_counts"]
    //            = Tnamed_pointer(&previous_counts, nullptr, this) ;

    //    named_pointer[name_of_this_element + "_current_counts"]
    //            = Tnamed_pointer(&current_counts, nullptr, this) ;

    //    named_pointer[name_of_this_element + "_difference_value"]
    //            = Tnamed_pointer(&difference_value, nullptr, this) ;


    //    named_pointer[name_of_this_element + "_counts_diff_in_last_unit_of_time"]
    //            = Tnamed_pointer(&counts_diff_in_last_unit_of_time, nullptr, this) ;

}
//************************************************************************
template <typename T>
Tscaler_template<T>::~Tscaler_template()
{}
//************************************************************************
template <typename T>
void  Tscaler_template<T>::create_my_spectra()
{

    //   string folder = "scalers/" + name_of_this_element ;
    std::string folder = "scalers/" ;
    std::string comment = "This graph moves left, just like a paper tape" ;
    // relative -----------------------------------
    std::string name = name_of_this_element   ;

    spec_rel = new spectrum_1D(name, name,
                               //                              400, 0, 400,
                               maxchan/5, 0, maxchan,
                               folder, comment);

    frs_spectra_ptr->push_back(spec_rel) ;
}
//**********************************************************************
template <typename T>
void Tscaler_template<T>::analyse_current_event()
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



        if(flag_first_time)
        {
            seconds_in_channel_zero = (unsigned)(event_unpacked->*time_of_the_measurement_in_seconds);
            flag_first_time = false ;
        }
        channel_to_increment =
                event_unpacked->*time_of_the_measurement_in_seconds -
                seconds_in_channel_zero  ;


        //        time_t now =  time(nullptr);
        //        if(flag_first_time)
        //        {
        //            seconds_in_channel_zero = now;
        //            flag_first_time = false ;
        //        }
        //        channel_to_increment = time(nullptr) - seconds_in_channel_zero;




        if(channel_to_increment > maxchan)  // there was some garbage
            //    if(channel_to_increment > 100000 )  // there was some garbage
        {
            channel_to_increment = 0 ;
            seconds_in_channel_zero = (unsigned)(event_unpacked->*time_of_the_measurement_in_seconds);
        }

        if(channel_to_increment > maxchan - 0.02*maxchan )
            //    if(channel_to_increment > 99000)
        {
            // scroll by one minute
            int scroll_nr = maxchan/10;   // was 60

            seconds_in_channel_zero += scroll_nr ;
            spec_rel->scroll_left_by_n_bins(scroll_nr);
            channel_to_increment -= scroll_nr ;
        }

        previous_counts = last_current_counts ;

        // the first incrementation will be a nonsese !, so...
        difference_value = (previous_counts != 0) ?
                    current_counts - previous_counts  :  0 ;


        //        static long fake = 0;
        //        fake++;
        //        fake = fake % maxchan;

        //        cout << "scaler= " <<name_of_this_element
        //             << ",  channel " <<(int)channel_to_increment
        //             << ", with difference_value " << difference_value
        //             << ", with current_counts " << current_counts
        //             << ", fake = " << fake
        //             << endl ;


        switch(mode)
        {
        case Tscaler_mode::difference_value:
            spec_rel ->inc_by((int)channel_to_increment, difference_value) ;
            break;
        case Tscaler_mode::real_value:
            spec_rel ->inc_by((int)channel_to_increment, current_counts) ;
            break;
        case Tscaler_mode::value_one:
            spec_rel ->inc_by((int)channel_to_increment, 1) ;
            break;

        }


        last_current_counts = current_counts;


    } // endif sensible y data
    calculations_already_done = true ;
}
//**************************************************************************
//**************************************************************************
template <typename T>
void  Tscaler_template<T>::make_preloop_action(std::ifstream & s)  // read the calibration factors, gates
{
    //cout << "preloop action for Reading the calibration for " << name_of_this_element << endl ;

    // remembering the start ?

    // ask what is ther right egde of the "paper"

    right_edge = spec_rel->give_max_chan_paper();
}

#endif  // _CINT__
#endif
