/***************************************************************************
                          Tself_gate_ge_cristal_galileo_descr.cpp  -  description
                             -------------------
    begin                : Mon Sep 8 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz, IFJ Krakow, Poland
    email                : jerzy.grebosz@ifj.edu.pl
   
    ======================================================
    This file may be copied from the cracow directrly 
     to the spy directory, but remember
    to change the extension from *.cpp into *.cxx
    ======================================================
    
 ***************************************************************************/

#include "Tself_gate_neutronWall_descr.h"
#include "Tfile_helper.h"
#include <iostream>

#define fif(x,y) Nfile_helper::find_in_file((x),(y))

//********************************************************************
Tself_gate_neutronWall_descr::Tself_gate_neutronWall_descr()
{


  sg_type = Tselfgate_type::galileo_ge; //  germanium_cristal ; // this is germanium
  enable_zcogate = false ;
  zco_gate[0] = 0 ;
  zco_gate[1] = 4096 ;

//  enable_en20gate = false;
//  en20_gate[0] = 0 ;
//  en20_gate[1] = 20000 ;

  enable_tof_gate = false ;
  tof_gate[0] = 0;
  tof_gate[1] = 8192;

  enable_xy_gamma_tof_difference = false; // Marek Pfutzner

  enable_energy_tof_polygon_gate = false;
  energy_tof_polygon_gate = "no_polygon";
	polygon = nullptr;
  //-------------------



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




}
//**********************************************************************

//*********************************************************************
void Tself_gate_neutronWall_descr::read_definition_from(std::string pathed_name)
{

  std::string file_name = pathed_name ;
  // path.user_def_spectra + name_of_spectrum + ".self_gate_ger_cristal" ;
  std::ifstream plik(file_name.c_str());
  if(!plik)
  {
        throw std::runtime_error( " Can't open the file "  + file_name +  " for reading selfgate");
  }

  try
  {
    //type =
      // I decided not to read this. It is already fixed
      // any errors will be automaticly fixed here
//      Nfile_helper::spot_in_file(plik, "type_of_self_gate") ;
//      plik >> sg_type;

    FH::spot_in_file(plik, "name");
    plik >> name;   // will be without the path and extension

    //SPY NEEDS the extension to recognize the title
    //    unsigned int i = name.find( ".self_gate_ger_cristal"); // != std::string::npos)
    //    if(i == std::string::npos)
    //    {
    //      name += ".self_gate_ger_cristal" ;
    //    }

    enable_zcogate = (bool) Nfile_helper::find_in_file(plik, "enable_zcogate");

    zco_gate[0] = fif(plik, "zco_gate_low");
    zco_gate[1] = fif(plik, "zco_gate_high");


    enable_tof_gate = (bool) fif(plik, "enable_tof_gate");

    tof_gate[0] = fif(plik, "tof_gate_low");
    tof_gate[1] = fif(plik, "tof_gate_high");

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


      enable_xy_gamma_tof_difference = false ;
      enable_xy_gamma_tof_difference = (bool) fif(plik, "enable_xy_gamma_tof_difference" );

      enable_energy_tof_polygon_gate  = false;
      enable_energy_tof_polygon_gate  = (bool) fif(plik, "enable_energy_tof_polygon_gate" );


      energy_tof_polygon_gate = "no_polygon";
		std::cout << "Before reading the polygon" << std::endl;
        if(enable_energy_tof_polygon_gate)
		{
            Nfile_helper::spot_in_file(plik, "energy_tof_polygon_gate" );
            plik >> energy_tof_polygon_gate;
		}



  }
  catch(Tfile_helper_exception &m)
  {
    std::cout << "Error during reading the selfgate file "
    << file_name << " : "
    << m.message << std::endl;
	 exit(12);
  }
  //   std::cout << "Success" << std::endl;
}
//*************************************************************************
void Tself_gate_neutronWall_descr::write_definitions(std::string path_only)
{
  std::string file_name = path_only + name;

  //  if(file_name.find(".self_gate_ger_cristal") == std::string::npos)
  //  {
  //    file_name += ".self_gate_ger_cristal" ;
  //  }

  std::ofstream plik(file_name.c_str());
  if(!plik)
  {
    std::cout << "Can't open the file " <<file_name
    << " for writing " << std::endl;
    return ;
  }

  plik
  << "// This is a definiton of the 'self_gate' for Galileo germanium cristal\n"
  << "// comments are marked using two slashes: // comment \n\n"
  << "\n//-----------------------------------------------------\n\n"

  << "type_of_self_gate\t\t" << sg_type << "\t\t// 1= Ger cristal, 2= Hec BaF\n"
  << "name\t\t " << name
  //<< ".self_gate_ger_cristal"
  << "\n\n"


  << "enable_zcogate\t\t" << enable_zcogate
  << "\tzco_gate_low\t"  << zco_gate[0]
  << "\tzco_gate_high\t" << zco_gate[1] << "\t// eneryg 4MeV gate\n"


  << "enable_tof_gate\t\t" << enable_tof_gate
  << "\ttof_gate_low\t"  << tof_gate[0]
  << "\ttof_gate_high\t" << tof_gate[1]
  << "\t// tof gate\n"

  << "\tenable_xy_gamma_tof_difference\t\t" << enable_xy_gamma_tof_difference
  //----------
  << "\nenable_energy_tof_polygon_gate\t\t"
  <<       enable_energy_tof_polygon_gate

  << "\nenergy_tof_polygon_gate\t\t"      << energy_tof_polygon_gate




  //--------------
  << "\nenable_geom_theta_gate\t\t" << enable_geom_theta_gate
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

  //-----------------------------



  << "\n//-----------------------------------------------------\n\n"

  << std::endl;


}
//*********************************************************************


