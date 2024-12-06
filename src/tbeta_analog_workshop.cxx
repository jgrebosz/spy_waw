#include "tbeta_analog_workshop.h"
#include "Tincrementer_donnor.h"
//*****************************************************************************************************
bool Tbeta_analog_workshop::check_selfgate(Tself_gate_abstract_descr* )
{
    return Tincrementer_donnor::check_selfgate();
}
//*****************************************************************************************************
  void  Tbeta_analog_workshop::during_analyse_current_event()
  {
  //==================
  // New, event - we are removing the old analog beta, because here we will make the new ones
  Tnamed_pointer::remove_from_map_ALL(
    "ALL_cluster_xia_cryst__beta_analog_(if_enabled)");
  Tnamed_pointer::remove_from_map_ALL(
    "ALL_cluster_xia_cryst__energy_dopplered_by_beta_analog_(if_enabled)");

  vector_beta_analog.clear();
//===============
 
  }
//*****************************************************************************************************
void  Tbeta_analog_workshop::during_preloop();    
{
  //==================

  // some dummy values
  beta_analog_enabled = false;
  begin_beta_analog   = 0.1;
  end_beta_analog  = 0.2;
  how_many_bins_of_beta_analog  = 1;


  std::string fname = path.dir_options() + "beta_analog.options" ;
  try
  {
    std::ifstream plik(fname.c_str()) ;
    if(!plik)
    {
      //      Tfile_helper_exception capsule;
      //      capsule.message =  "Can't open the file";
      //      throw capsule ;
      std::cout << "Can't open the file " << fname << " so I assume 'beta analog' disabled " << std::endl;
      beta_playfield_enabled = false ;
      return;
    }
    beta_analog_enabled =
      (bool) Tfile_helper::find_in_file(plik,"beta_analog_enabled");
    if(!beta_analog_enabled)
    {
      std::cout << "The 'beta analog system'    is set as:  disabled " << std::endl;
      return ;
    }
    // read the start beta
    begin_beta_analog   =
      Tfile_helper::find_in_file(plik,"begin_beta_analog");

    // read the stop beta
    end_beta_analog  =
      Tfile_helper::find_in_file(plik,"end_beta_analog");

    // read bins
    how_many_bins_of_beta_analog  =
      (int) Tfile_helper::find_in_file(plik,"how_many_bins_of_beta_analog");


    size_of_beta_analog_bin = (end_beta_analog - begin_beta_analog) / (how_many_bins_of_beta_analog -1) ;

    for(int i = 0 ; i < how_many_bins_of_beta_analog ; i++)
    {
      beta_analog_fixed_value.push_back( begin_beta_analog + (i *size_of_beta_analog_bin));

      double gamma_zero = 1 / sqrt(1- (beta_analog_fixed_value[i]*beta_analog_fixed_value[i]));
      double E_zero = (gamma_zero -1) * 931.5 ;
      double E_one = E_zero - energy_deposit_in_target ;
      double gamma_one = (E_one + 931.5)/ 931.5 ;
      beta_analog_after_target.push_back(sqrt(1 - (1/(gamma_one * gamma_one))));

      //             std::cout << "beta_analog_fixed_value[" << i <<"]=" << beta_analog_fixed_value[i];
      //             std::cout << ", beta_analog_after_target[" << i <<"]=" << beta_analog_after_target[i] << std::endl;
    }
    std::cout << "Success in reading the 'beta analog' data" << fname << std::endl;
  }
  catch(Tfile_helper_exception &m)
  {
    std::cout << "Ttarget:: Error while reading the file " << fname
    << m.message << std::endl;
    std::cout << "So the 'beta analog system'    is  disabled " << std::endl;
    //exit(88);
    //throw ;
  }
//========================

}
//*************************************************************************
//==================
void Tbeta_analog_workshop::remember_to_increment_matrix_beta_vs_energy_dopp(  int i,
    double energy,
    Tself_gate_abstract_descr *ptr_for_selfgate)
{
  // perhaps we want the incrementation under some condition.
  // But the condition can not be tested yet...

  //   points_ont_beta_vs_energy_dop_matrix.push_back
  //   (
  //     std::pair<double, double> (energy,
  //                           begin_beta_analog + (i * size_of_beta_analog_bin))
  //   );

  //----------
  vector_beta_analog.push_back(
    beta_analog_entry(
      begin_beta_analog + (i * size_of_beta_analog_bin),    // beta
      energy,                      // energy_doppler
      ptr_for_selfgate)
  );

//   int current = vector_beta_analog.size() -1;

  //Tincrementer_donnor * ptr = &vector_beta_analog[current];

/*    std::cout << "putting beta " <<  vector_beta_analog[current].beta_value
    << ", energy dop " << vector_beta_analog[current].energy_dopp
    
    << " size of vector_beta_analog= " << vector_beta_analog.size()
    << std::endl;*/
    
    
  Tnamed_pointer::add_as_mutative_ALL(
    "ALL_cluster_xia_cryst__beta_analog_(if_enabled)",
    &vector_beta_analog.back().beta_value,
    NULL, // no validator, so always valid
    &vector_beta_analog.back()
  );


  Tnamed_pointer::add_as_mutative_ALL(
    "ALL_cluster_xia_cryst__energy_dopplered_by_beta_analog_(if_enabled)",
    &vector_beta_analog.back().energy_dopp,
    NULL, // no validator, so always valid
    &vector_beta_analog.back()
  ); 

//   Tnamed_pointer::add_as_mutative_ALL(
//     "ALL_cluster_xia_cryst__beta_analog_(if_enabled)",
//     &vector_beta_analog[current].beta_value,
//     NULL, // no validator, so always valid
//     &vector_beta_analog[current]
//   );
// 
// 
//   Tnamed_pointer::add_as_mutative_ALL(
//     "ALL_cluster_xia_cryst__energy_dopplered_by_beta_analog_(if_enabled)",
//     &vector_beta_analog[current].energy_dopp,
//     NULL, // no validator, so always valid
//     &vector_beta_analog[current]
//   );

}
////==================
//************************************************************************
