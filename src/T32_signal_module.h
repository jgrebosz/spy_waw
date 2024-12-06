#ifndef _32_signal_module_H_
#define _32_signal_module_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TGalileoEvent class
#ifndef  __CINT__

#include <cstdlib>
#include <cstring>  // for memset

#include "TNeedle_element.h"
#include "spectrum.h"
#include <string>
#include <sstream>

#include <iomanip>

#include "TGalileoEvent.h"
#include "Tincrementer_donnor.h"

//class focus_point ;
//typedef unsigned int calkow;
typedef int calkow;
//////////////////////////////////////////////////////////////////////////////
template <calkow nr_of_channels>
class T32_signal_module : public TNeedle_element , public Tincrementer_donnor
{
  int signal_data[nr_of_channels];
  double signal_data_cal[nr_of_channels];

  std::vector<double>  cal_factors [nr_of_channels];


  // pointer to members of event class
  // it must be done like that, because the event does not
  // exist yet. It will be produced by the factory
  // in the moment of running the analysis.

  int (TGalileoEvent::*signal_ptr)[nr_of_channels];

public:
  //  constructor
  T32_signal_module (std::string _name, int (TGalileoEvent::*first_ptr)[nr_of_channels],
                     bool flag_produce_fan_spectrum_arg , //  false, // generally we do not want. Let the user make them as user defined
                     bool flag_produce_signal_spectra_arg,  // = false // generally we do not want. Let the user make them as user defined
                     bool flag_we_want_calibrate_signals_arg
                    );

  void analyse_current_event()  ;
  void make_preloop_action(std::ifstream & ) ;  // read the calibration factors, gates

protected:
  void create_my_spectra();
  bool flag_produce_fan_spectrum; // generally we do not want. Let the user make them as user defined
  bool flag_produce_signal_spectra; // generally we do not want. Let the user make them as user defined
  bool flag_we_want_calibrate_signals;

  spectrum_1D * spec_chan[nr_of_channels];
  spectrum_1D * spec_chan_cal[nr_of_channels];

  spectrum_1D * spec_fan;
  bool flag_atmospheric_pressure_block;
};

#include "Tnamed_pointer.h"
#include <sstream>

//***********************************************************************
//  constructor

template <calkow nr_of_channels>
T32_signal_module<nr_of_channels>::T32_signal_module (std::string _name,
    int (TGalileoEvent::*first_ptr)[nr_of_channels],
    bool flag_produce_fan_spectrum_arg, // generally we do not want. Let the user make them as user defined
    bool flag_produce_signal_spectra_arg, // generally we do not want. Let the user make them as user defined
    bool flag_we_want_calibrate_signals_arg
                                                     )
    : TNeedle_element(_name),  signal_ptr (first_ptr ),
    flag_produce_fan_spectrum(flag_produce_fan_spectrum_arg),
    flag_produce_signal_spectra(flag_produce_signal_spectra_arg),
    flag_we_want_calibrate_signals(flag_we_want_calibrate_signals_arg )
{
//   std::cout << "For "
//   << name_of_this_element
//   << " flag_we_want_calibrate_signals is "
//   << flag_we_want_calibrate_signals << std::endl;
flag_atmospheric_pressure_block = false;

#if (defined ZET_CORRECTED_BY_PRESSURE) && \
(CURRENT_EXPERIMENT_TYPE==RISING_ACTIVE_STOPPER_100TIN)
if(name_of_this_element == "munich_multiplicity") flag_atmospheric_pressure_block = true;
#endif

  create_my_spectra();

  //    for(int i = 0 ; i < 32 ; i++)
  //      {
  //        named_pointer[name_of_this_element + "_first"]
  //          = Tnamed_pointer(&first_signal_data, 0, this) ;
  //      }
}

//************************************************************
template <calkow nr_of_channels>
void T32_signal_module<nr_of_channels>::create_my_spectra()
{
  //the spectra have to live all the time, so here we can create
  // them during  the constructor
  std::string folder = "munich/" + name_of_this_element ;
  //-------------- for testing,  spectra of all ADC and TDCchannels
  for(int i = 0 ; i < nr_of_channels ; i++)
  {
    // incrementers
    std::ostringstream plik ;
    plik << name_of_this_element
    << "["
    << (i< 10 ? "0" : "")
    << i << "]" ;

    std::string name = plik.str();
    named_pointer[name] = Tnamed_pointer(&signal_data[i], 0, this) ;

    // spectra
    if(flag_produce_signal_spectra)
    {
      spec_chan[i] = new spectrum_1D( name, name,
                                      4095, 1, 4096,
                                      folder, "", noraw);
      frs_spectra_ptr->push_back(spec_chan[i]) ;

      if(flag_we_want_calibrate_signals)
      {
        std::string name_cal = name+"_cal";
        named_pointer[name_cal ] = Tnamed_pointer(&signal_data_cal[i], 0, this) ;

        spec_chan_cal[i] = new spectrum_1D( name_cal, name_cal,
                                            4095, 1, 4096,
                                            folder, "", noraw);
        frs_spectra_ptr->push_back(spec_chan_cal[i]) ;
      }// end if we want cal
    }
  }
  // fan spectrum
  if(flag_produce_fan_spectrum)
  {
    spec_fan = new spectrum_1D(
                 name_of_this_element+ "_fan",
                 name_of_this_element+ "_fan",
                 nr_of_channels, 0, nr_of_channels,
                 folder, "", noraw);
    frs_spectra_ptr->push_back(spec_fan) ;
  }
}
//*************************************************************
template <calkow nr_of_channels>
void T32_signal_module<nr_of_channels>::analyse_current_event()
{
  // std::cout << "analyse_current_event()  for "
  //         <<  name_of_this_element
  //          << std::endl ;

  for(int i = 0 ; i < nr_of_channels ; i++)
  {
    signal_data[i] = (event_unpacked->*signal_ptr)[i];
    signal_data_cal[i]= 0;

    if(flag_produce_signal_spectra)
    {
      spec_chan[i]->increment(signal_data[i]);
//       std::cout << "before calibration" << name_of_this_element
//       << i << "   signal data = " <<  signal_data[i]
//       << ", flag_we_want_calibrate_signals= " << flag_we_want_calibrate_signals
//       << std::endl;
    }

    if(flag_produce_fan_spectrum && signal_data[i])
    {
      spec_fan->increment(i);
    }

    if(signal_data[i] && flag_we_want_calibrate_signals)
    {
      double tmp = signal_data[i] + randomizer::randomek() ;
      signal_data_cal[i] =
        (cal_factors [i][2] * tmp * tmp) +
        (cal_factors [i][1] * tmp) +
        (cal_factors [i][0]);

//        std::cout << "after calibratnio " << name_of_this_element << " is ["
//        << i<< "]"
//        << signal_data_cal[i] << std::endl;

// if(i == 0 && flag_atmospheric_pressure_block)
// {
// // std::cout << "Saving the pressure raw = " << signal_data[0]
// // << ", calibrated = " << signal_data_cal[0] << std::endl;
//  event_unpacked->calibrated_current_pressure = signal_data_cal[i];
//  }

      if(flag_produce_signal_spectra)
      {
        spec_chan_cal[i]->increment(signal_data_cal[i]);
      }
    }

   
  }

  calculations_already_done = true ;
}
//*************************************************************
template <calkow nr_of_channels>
void T32_signal_module<nr_of_channels>::make_preloop_action(std::ifstream & )
{
  // as almost nobody needs the calibration(only munich people for Sn100 experiment
  // so the calibration will be in the file related to this module

  if(!flag_we_want_calibrate_signals) return;

  else
    flag_we_want_calibrate_signals = true;

  std::cout << "Reading the calibration for " << name_of_this_element << std::endl ;

  for(int i = 0 ; i < nr_of_channels ; i++)
    cal_factors [i].clear();


  std::string cal_filename = path.dir_calibration() + name_of_this_element +  "_calibration.txt";
  try
  {
    std::ifstream plik(cal_filename.c_str() ) ; // ascii file
    if(!plik)
    {
      std::string mess = "I can't open calibration file: "
                    + cal_filename  ;
      throw mess ;
    }

    for(int i = 0 ; i < nr_of_channels ; i++)
    {
      std::ostringstream slowo;
      slowo <<  "channel_" << std::setw(3) << std::setfill('0') << i <<  "_calibration_factors" ;

      double tmp =
        Tfile_helper::find_in_file<double> ( plik,  slowo.str());
      cal_factors [i].push_back(tmp);  // ofset

      plik >> zjedz >> tmp;
      cal_factors [i].push_back(tmp);  // slope

      plik >> zjedz >> tmp;
      cal_factors [i].push_back(tmp);  // quadratic
    }// end for

  }  // end of try
  catch(Tno_keyword_exception &k)
  {
    std::cerr << "Error while reading the calibration file "
    << cal_filename
    << ":\n"
    << k.message
    << std::endl  ;
    exit(-1)
    ; // is it healthy ?
  }
  catch(Treading_value_exception &k)
  {
    std::cerr << "Error while reading calibration file "
    << cal_filename
    << ":\n"
    << k.message
    << std::endl  ;
    exit(-1)
    ; // is it healthy ?
  }
  catch(std::string sss)
  {
    std::cerr << "Error while reading calibration file "
    << cal_filename
    << ":\n"
    << sss << std::endl  ;
    exit(-1)
    ; // is it healthy ?
  }

  flag_we_want_calibrate_signals = true;

  std::cout << "End of reaadin calibration" << name_of_this_element
  << ", flag_we_want_calibrate_signals= " << flag_we_want_calibrate_signals
  << std::endl;
}
/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _32_signal_module_H_
