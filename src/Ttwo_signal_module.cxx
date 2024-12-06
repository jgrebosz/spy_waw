#include "Ttwo_signal_module.h"
#include "Tnamed_pointer.h"

//***********************************************************************
//  constructor
Ttwo_signal_module::Ttwo_signal_module (std::string _name,
					   int TGalileoEvent::*first_ptr,
					   int TGalileoEvent::*second_ptr
					   ) : TNeedle_element(_name),
						  first_signal_ptr (first_ptr ),
						  second_signal_ptr(second_ptr )
{
  create_my_spectra();


  named_pointer[name_of_this_element + "_first"]
    = Tnamed_pointer(&first_signal_data, 0, this) ;
  named_pointer[name_of_this_element+ "_second"]
    = Tnamed_pointer(&second_signal_data, 0, this) ;

}
//************************************************************
void Ttwo_signal_module::create_my_spectra()
{

  //the spectra have to live all the time, so here we can create
  // them during  the constructor

  std::string folder = "tests/" + name_of_this_element ;


  //-----------
  std::string name = name_of_this_element + "_first"  ;
  spec_first = new spectrum_1D( name,
						 name,
						 8192, 0, 8192,
						 folder, "", name);
  frs_spectra_ptr->push_back(spec_first) ;

  //-----------
  name = name_of_this_element + "_second"  ;
  spec_second = new spectrum_1D( name,
						  name,
						  8192, 0, 8192,
						  folder, "", name);
  frs_spectra_ptr->push_back(spec_second) ;


}
//*************************************************************
void Ttwo_signal_module::analyse_current_event()
{


  // std::cout << "analyse_current_event()  for "
  //         <<  name_of_this_element
  //          << std::endl ;


  first_signal_data = event_unpacked->*first_signal_ptr;
  second_signal_data = event_unpacked->*second_signal_ptr;

  spec_first ->increment(first_signal_data) ;
  spec_second ->increment(second_signal_data ) ;



  calculations_already_done = true ;

}
//**************************************************************
// read the calibration factors, gates
void  Ttwo_signal_module::make_preloop_action(std::ifstream & s)  
{


}
//***************************************************************
