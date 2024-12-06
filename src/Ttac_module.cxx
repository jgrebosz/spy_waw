#include "Ttac_module.h"
#include "TneutronWall_crystal.h"
#include <sstream>
#include <iomanip>


#include <algorithm>
#include "TneutronWall_array.h"
#include "Ttarget.h"
#include "TRandom.h"
#include "TNeedle.h"
#include "Tnamed_pointer.h"
#include "Tself_gate_neutronWall_descr.h"
#include "TGalileoAnalysis.h" // for verbose mode

TneutronWall_array*  Ttac_module::array_nw_pointer;  // <---static pointer
Ttarget*  Ttac_module::target_ptr;       // for faster acces of the doppler correction

#define dcout   if(0)std::cout



//**************************************************************************
Ttac_module::Ttac_module(std::string nam, TneutronWall_array *parent_ptr): parent_array(parent_ptr)
{
  
  my_selfgate_type = Tselfgate_type::galileo_ge;   // <---- important!
  
  name_of_this_element = nam;
  
  create_my_spectra();
  
}
//************************************************************************
void Ttac_module::create_my_spectra()
{
  //std::cout << "before creating the spectra " << std::endl;
  
  // the spectra have to live all the time, so here we can create
  // them during  the constructor
  
  std::vector<spectrum_abstr *> &gal_spec_ref = parent_array->spectra_vector;
  std::string name;
  std::string folder = "tac/" ;
  
  //---------------
  name = name_of_this_element + "_raw";
  spec_tac_raw = new spectrum_1D(
                                 name,
                                 name,
                                 2048,-.5,4095.5,
                                 folder,"",
                                 "X:\r TAC"
                                 );
  gal_spec_ref.push_back(spec_tac_raw);
  
  name = name_of_this_element +  "_cal";
  spec_tac_cal = new spectrum_1D(
                                 name,
                                 name,
                                 2048,-.5,1023.5,
                                 folder,"",
                                 "X:\r TAC"
                                 );
  gal_spec_ref.push_back(spec_tac_cal);
}
//**********************************************************************
void Ttac_module::analyse_current_event(int nr)
{
  
  //        std::cout << " analyse_current_event()  for "
  //           << name_of_this_element << " " << id_number
  //            << std::endl;
  
  // checking if it is something (not zero) -- is made during unpacking
  
  zeroing_good_flags();
  
  //if(nr>3) std::cout << "Kasia " << nr << std::endl;
  
  //    if (TneutronWall_array::tmp_event_unpacked->neutronWall_fired[id_number] == false) return;
  //----------------
  // so it fired !
  
  neutronWall_TAC_t d = TneutronWall_array::tmp_event_unpacked->neutron_wall_tac[nr];
  //memcpy(d, (void*) &TneutronWall_array::tmp_event_unpacked->neutron_wall_data[id_number], sizeof(d));
  if(d.tac<1) return;
  tac_raw = d.tac;
  flag_fired = (tac_raw >= 1);
  
  if (GalileoAnalysis_ptr->is_verbose_on()) {
    std::cout  << name_of_this_element
    << ", Eraw =  " //<< std::setw(9)
    << tac_raw
    << std::endl; // New line will be after the LP, SR printout
  }
  
  double tac_rand = d.tac+gRandom->Uniform(-0.5,0.5);
  tac_cal=0.;
  for(Int_t j=0;j<calibr_factors.size();++j){
    // std::cout << "calibration " << calibr_factors[j] << std::endl;
    tac_cal+=calibr_factors[j]*TMath::Power(tac_rand,j);
    //  std::cout << tac_cal << std::endl;
  }
  d.tac_calibrated = tac_cal;
  // std::cout << tac_rand << "  " << tac_cal << std::endl;
  //###########################################################
  
  if (GalileoAnalysis_ptr->is_verbose_on()) {
    std::cout << std::endl;
  }
}

//**************************************************************************

void  Ttac_module::make_preloop_action(TTACDetector* det)
{
   
  calibr_factors = det->GetCalibrFactors();
  
}
//**********************************************************************
void Ttac_module::set_event_ptr_for_crystal_data(int ptr[3])
{}
//**********************************************************************
void Ttac_module::check_legality_of_data_pointers()
{}


//*******************************************************************************
/** No descriptions */
void Ttac_module::zeroing_good_flags()
{
  flag_fired =
  flag_good_data =   false;
  
  tac_cal = 0;
  tac_raw = 0 ;
  
  pseudo_fan = -1; // zero would mean something!
  
}
/******************************************************************************/
bool Ttac_module::check_selfgate(Tself_gate_abstract_descr *desc)
{
  
}

//************************************************************************
/** No descriptions */
void Ttac_module::increment_crystal_spectra_raw()
{
  // std::cout << "Increment " << id << "  " << tac_raw << std::endl;
  if(tac_raw>1){
    spec_tac_raw->increment(tac_raw);
  }
  
}
//**************************************************************************
/** No descriptions */
void Ttac_module::increment_crystal_spectra_cal()
{
  
  //   std::cout << tac_cal << std::endl;
  if(tac_cal>1){
    spec_tac_cal->increment(tac_cal);
  }
}

