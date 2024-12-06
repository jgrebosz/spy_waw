#include "Tone_signal_pattern_module.h"
#include "Tnamed_pointer.h"
#include <sstream>
#include <iomanip>


//***********************************************************************
//  constructor
Tone_signal_pattern_module::Tone_signal_pattern_module (std::string _name,
    int TGalileoEvent::*first_ptr
                                                       ) : TNeedle_element(_name),
    signal_ptr (first_ptr )
{
  create_my_spectra();


  named_pointer[name_of_this_element + "_full_pattern"]
  = Tnamed_pointer(&signal_data, 0, this) ;

  for(int i = 0 ; i < 32 ; i++)
  {
    std::ostringstream s;
    s << name_of_this_element << "_bit_" << std::setw(2) << std::setfill('0') << i ;

    named_pointer[s.str() ] = Tnamed_pointer(&our_incrementer[i], 0, this) ;
  }
}
//************************************************************
void Tone_signal_pattern_module::create_my_spectra()
{

  //the spectra have to live all the time, so here we can create
  // them during  the constructor

  std::string folder = "tests/" + name_of_this_element ;


  //-----------
  std::string name = name_of_this_element + "_signal"  ;
  spec_first = new spectrum_1D( name,
                                name,
                                8192, 0, 8192,
                                folder, "",
										  name_of_this_element + "_full_pattern"
										);
  frs_spectra_ptr->push_back(spec_first) ;

  //-----------

}
//*************************************************************
void Tone_signal_pattern_module::analyse_current_event()
{
  // std::cout << "analyse_current_event()  for "
  //         <<  name_of_this_element  << std::endl ;

  signal_data = event_unpacked->*signal_ptr;
  spec_first ->increment(signal_data) ;

//std::cout << "signal for pattern = 0x "<< std::hex << signal_data << std::dec << std::endl;
  
  // now unpacking for incrementers
  for(int i = 0 ; i < 32 ; i++)
  {
    our_incrementer[i] = (signal_data & (1 << i)) ? true : false;

//     if(i == 15 && our_incrementer[i])
//     {
//       std::cout << "jedynka" << std::endl;
//     }
  }

  calculations_already_done = true ;

}
//**************************************************************
// read the calibration factors, gates
void  Tone_signal_pattern_module::make_preloop_action(std::ifstream & s)
{

}
//***************************************************************
