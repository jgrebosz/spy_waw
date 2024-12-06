#ifndef _two_signal_module_H_
#define _two_signal_module_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TGalileoEvent class
#ifndef  __CINT__



#include "TNeedle_element.h"
#include "spectrum.h"
#include <string>

#include "TGalileoEvent.h"
#include "Tincrementer_donnor.h"

//class focus_point ;

//////////////////////////////////////////////////////////////////////////////
class Ttwo_signal_module : public TNeedle_element , public Tincrementer_donnor
{

  // friend class focus_point ; // will read this private data, but using
  // constant pointer

  int first_signal_data;
  int second_signal_data;

  // pointer to members of event class
  // it must be done like that, because the event does not
  // exist yet. It will be produced by the factory
  // in the moment of running the analysis.

  int TGalileoEvent::*first_signal_ptr;
  int TGalileoEvent::*second_signal_ptr;

  spectrum_1D * spec_first ;
  spectrum_1D * spec_second ;


 public:
  //  constructor
  Ttwo_signal_module (std::string _name,
			 int TGalileoEvent::*first_ptr,
			 int TGalileoEvent::*second_ptr
			 ) ;


  void analyse_current_event() ;
  void make_preloop_action(std::ifstream & );  // read the calibration factors, gates

 protected:
  void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _two_signal_module_H_
