/***************************************************************************
                          Tself_gate_aga_descr.cpp  -  description
                             -------------------
    begin                : Mon Sep 8 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz, IFJ Krakow, Poland
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/

#include "Tself_gate_aga_descr.h"
#include "Tfile_helper.h"
#include <iostream>

#define fif(x,y) Tfile_helper::find_in_file((x),(y))

//********************************************************************
Tself_gate_aga_descr::Tself_gate_aga_descr()
{
  type =  agata_cristal ;    // this is germanium
  enable_core_energy_gate = false ;
  core_energy_gate[0] = 0 ;
  core_energy_gate[1] = 4096 ;

  
  enable_time_gate = false ;
  time_gate[0] = 0;
  time_gate[1] = 8192;

  //---------------    ????????????? degrees 
  enable_geom_theta_gate = false ;
  geom_theta_gate[0] = 0;
  geom_theta_gate[1] = 360;

  enable_geom_phi_gate = false ;
  geom_phi_gate[0] = 0;
  geom_phi_gate[1] = 360;

  // gamma - particle angle ---> Radians ???????????
  enable_gp_theta_gate = false ;
  gp_theta_gate[0] = 0;
  gp_theta_gate[1] = 6.28;
  
  enable_gp_phi_gate = false ;
  gp_phi_gate[0] = 0;
  gp_phi_gate[1] = 6.28;

  // this is the multiplicity inside the cryostat where this cristal belongs
  enable_mult_in_cryostat_gate = false;
  mult_in_cryostat_gate[0] = 0;
  mult_in_cryostat_gate[1] = 7;


}
//**********************************************************************

//*********************************************************************
void Tself_gate_aga_descr::read_definition_from(std::string pathed_name)
{


  std::string file_name = pathed_name ;
    // path.user_def_spectra + name_of_spectrum + ".self_gate_aga_cristal" ;
  std::ifstream plik(file_name.c_str());
  if(!plik)
  {
      std::cout << "Can't open the file "  << file_name
            << " for reading " << std::endl;
      return ;
  }

  try
  {
    type = unknown;
    if((int) fif(plik, "type_of_self_gate") == agata_cristal) type = agata_cristal;

    Tfile_helper::spot_in_file(plik, "name");
    plik >> name;   // will be without the path and extension

    //SPY NEEDS the extension to recognize the title
//    unsigned int i = name.find( ".self_gate_aga_cristal"); // != std::string::npos)
//    if(i == std::string::npos)
//    {
//      name += ".self_gate_aga_cristal" ;
//    }

    enable_core_energy_gate = (bool) Tfile_helper::find_in_file(plik, "enable_core_energy_gate");

    core_energy_gate[0] = fif(plik, "core_energy_gate_low");
    core_energy_gate[1] = fif(plik, "core_energy_gate_high");




      enable_time_gate = (bool) fif(plik, "enable_time_gate");

      time_gate[0] = fif(plik, "time_gate_low");
      time_gate[1] = fif(plik, "time_gate_high");

      //---------------
      enable_geom_theta_gate = (bool) fif(plik, "enable_geom_theta_gate");

      geom_theta_gate[0] = fif(plik, "geom_theta_gate_low");
      geom_theta_gate[1] = fif(plik, "geom_theta_gate_high");


      enable_geom_phi_gate = (bool) fif(plik, "enable_geom_phi_gate");

      geom_phi_gate[0] = fif(plik, "geom_phi_gate_low");
      geom_phi_gate[1] = fif(plik, "geom_phi_gate_high");

   //---------------
      enable_gp_theta_gate = (bool) fif(plik, "enable_gp_theta_gate");

      gp_theta_gate[0] = fif(plik, "gp_theta_gate_low");
      gp_theta_gate[1] = fif(plik, "gp_theta_gate_high");


      enable_gp_phi_gate = (bool) fif(plik, "enable_gp_phi_gate");

      gp_phi_gate[0] = fif(plik, "gp_phi_gate_low");
      gp_phi_gate[1] = fif(plik, "gp_phi_gate_high");

     //------------
      enable_mult_in_cryostat_gate = (bool) fif(plik, "enable_mult_in_cryostat_gate");

      mult_in_cryostat_gate[0] = fif(plik, "mult_in_cryostat_gate_low");
      mult_in_cryostat_gate[1] = fif(plik, "mult_in_cryostat_gate_high");

  }
  catch(Tfile_helper_exception &m)
  {
    std::cout << "Error during reading the file "
         << file_name << " : "
         << m.message << std::endl;
  }
}
//*************************************************************************
void Tself_gate_aga_descr::write_definitions(std::string path_only)
{
  std::string file_name = path_only + name;

//  if(file_name.find(".self_gate_aga_cristal") == std::string::npos)
//  {
//    file_name += ".self_gate_aga_cristal" ;
//  }

  std::ofstream plik(file_name.c_str());
  if(!plik)
    {
      std::cout << "Can't open the file " <<file_name
          << " for writing " << std::endl;
      return ;
    }

  plik
    << "// This is a definiton of the 'self_gate' for germanium agata cristal\n"
    << "// comments are marked using two slashes: // comment \n\n"
    << "\n//-----------------------------------------------------\n\n"

    << "type_of_self_gate\t\t" << type << "\t\t// 1= Ger cristal, 2= Hec BaF\n"
    << "name\t\t " << name
    //<< ".self_gate_aga_cristal"
    << "\n\n"


<< "enable_core_energy_gate\t\t" << enable_core_energy_gate
<< "\tcore_energy_gate_low\t"  << core_energy_gate[0]
<< "\tcore_energy_gate_high\t" << core_energy_gate[1] << "\t// eneryg 4MeV gate\n"


<< "enable_time_gate\t\t" << enable_time_gate
<< "\ttime_gate_low\t"  << time_gate[0]
<< "\ttime_gate_high\t" << time_gate[1] << "\t// time gate\n"

//--------------
<< "enable_geom_theta_gate\t\t" << enable_geom_theta_gate
<< "\tgeom_theta_gate_low\t"  << geom_theta_gate[0]
<< "\tgeom_theta_gate_high\t" << geom_theta_gate[1] << "\t// geom_theta gate\n"

<< "enable_geom_phi_gate\t\t" << enable_geom_phi_gate
<< "\tgeom_phi_gate_low\t"  << geom_phi_gate[0]
<< "\tgeom_phi_gate_high\t" << geom_phi_gate[1] << "\t// geom_phi gate\n"

//--------------
<< "enable_gp_theta_gate\t\t" << enable_gp_theta_gate
<< "\tgp_theta_gate_low\t"  << gp_theta_gate[0]
<< "\tgp_theta_gate_high\t" << gp_theta_gate[1] << "\t// gamma - scattered particle\n"

<< "enable_gp_phi_gate\t\t" << enable_gp_phi_gate
<< "\tgp_phi_gate_low\t"  << gp_phi_gate[0]
<< "\tgp_phi_gate_high\t" << gp_phi_gate[1] << "\t// gamma - scattered particle\n"


<< "enable_mult_in_cryostat_gate\t\t" << enable_mult_in_cryostat_gate
<< "\tmult_in_cryostat_gate_low\t"  << mult_in_cryostat_gate[0]
<< "\tmult_in_cryostat_gate_high\t" << mult_in_cryostat_gate[1] << "\t// multiplicity\n"



    << "\n//-----------------------------------------------------\n\n"

    << std::endl;


}
//*********************************************************************


