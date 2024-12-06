#include "paths_spy.h"
#include <fstream>
#include <iostream>
//#include <qdir.h>


Tpaths path ;
//***********************************************************
bool Tpaths::read_from_disk()
{
  /*
   std::ifstream plik("paths.cnf");
   if(plik)
   {
   plik >> spy_dir ;
   }
   else{
   std::cout << "Error while reading the file: paths.cnf" << std::endl;
   }
   check_for_slashes() ;
   
   if(is_it_legal())
   return true;
   else return false;*/
   return false;
}
//***********************************************************
void Tpaths::save_to_disk()
{
  check_for_slashes() ;
  
  std::ofstream plik("paths.cnf");
  if(plik)
  {
    plik << spy_dir << std::endl  ;
  }
  else {
    std::cout << "Error while saving the file: paths.cnf" << std::endl;
  }
}

//************************************************************
#if 0
bool paths::is_it_legal()
{
  
  
  boost::filesystem::path file(Qspectra());
  if (exists(file))
  {
    if (is_directory(file)){
      save_to_disk();
      return true; // std::cout << name << " is a directory.\n";
      
    }
    else
      return false; // std::cout << name << " is a non-directory file.\n";
  }
  else
    return false ; // std::cout << name << " does not exist.\n";
  
#############################
  // we test here if spectra directory exist here
  QDir dir;
  
  if( dir.exists(Qspectra()) )
  {
    save_to_disk() ;
    return true;
  }
  else return false;
}
#endif

//***********************************************************
void Tpaths::check_for_slashes()
{
  int last_character =  spy_dir.length() -1;
  if(spy_dir[last_character] != '/' )
  {
    //std::cout << "No slash on the end : "<< spectra << std::endl ;
    spy_dir += "/" ;
    //std::cout << "after correction: "<< spectra << std::endl ;
  }
  //-----------------------------
  
  
  //-- a new structure of directories introduced, many of them
  // are nested now in a subdirectory called 'system'
  spectra      = spy_dir + "spectra/";   // OK2
  gates_1D      = spy_dir + "system/gates/";     // ok2
  polygons      = spy_dir + "system/polygons/";    // OK2
  commands      = spy_dir + "system/commands/";   // OK2
  groups        = spy_dir + "groups/";   // OK2
  mbs_settings  = spy_dir + "system/mbs_settings/"; // ok2
  options       = spy_dir + "options/";   // ok2
  user_def_spectra  = spy_dir + "system/definitions_user_spectra/";  //ok2
  autom         = spy_dir + "system/automatic/"; // OK2
  calibration   = spy_dir + "calibration/"; // ok2
  conditions    = spy_dir + "system/conditions/";   // ok
  my_binnings    = spy_dir + "my_binnings/";
  incrementers = spy_dir + "system/incrementers_user_def/";
  experiment_setup = spy_dir + "experiment_setup/";
}
