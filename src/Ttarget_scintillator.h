#ifndef _TARGET_SCINTILLATOR_H_
#define _TARGET_SCINTILLATOR_H_


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
class Ttarget_scintillator : public TNeedle_element, public Tincrementer_donnor
{

  // friend class focus_point ; // will read this private data, but using
  // constant pointer

 //double distance ;
  

  // pointer to members of event class
  // it must be done like that, because the event does not
  // exist yet. It will be produced by the factory 
  // in the moment of running the analysis.

  int TGalileoEvent::*x; 
  int TGalileoEvent::*y;

  int x_value, y_value ; // values
  bool
    x_fired,
    y_fired;
  
  spectrum_1D * spec_xy_fan ;
  

 public:
  //  constructor
  Ttarget_scintillator (std::string _name,
                          int TGalileoEvent::*x_ptr,
                          int TGalileoEvent::*y_ptr) ;


  void analyse_current_event() ;
  void make_preloop_action(std::ifstream & );  // read the calibration factors, gates
  
  
 // double give_distance() { return distance ; }
 protected:
  void create_my_spectra();
  //void check_legality_of_data_pointers();   
  
};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _TARGET_SCINTILLATOR_H_
