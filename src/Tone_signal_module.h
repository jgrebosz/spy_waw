#ifndef _one_signal_module_H_
#define _one_signal_module_H_


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
class Tone_signal_module : public TNeedle_element , public Tincrementer_donnor
{

  unsigned int unsigned_signal_data;
  int signed_signal_data;

  // pointer to members of event class
  // it must be done like that, because the event does not
  // exist yet. It will be produced by the factory
  // in the moment of running the analysis.

  int TGalileoEvent::*signal_ptr;
  

  spectrum_1D * spec_first ;

 public:
  //  constructor
  Tone_signal_module (std::string _name, int TGalileoEvent::*first_ptr) ;


  void analyse_current_event() ;
  void make_preloop_action(std::ifstream & );  // read the calibration factors, gates
  int give_value() { return signed_signal_data; }

 protected:
  void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _one_signal_module_H_
