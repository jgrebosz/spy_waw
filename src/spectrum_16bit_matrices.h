//////////////////////////////////////////////////////////////////
// Jurek Grebosz, 14 June 2002
//
// spectra hierarchy
/////////////////////////////////////////////////////////////////

#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "experiment_def.h"
//#include "TH1.h"

#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include "paths_spy.h"
extern Tpaths path;
////////////////////////////////////////////////////////////////////
// Descriptor stores on the disk the list of all currently collected spectra
// This information isused by the grebware
//////////////////////////////////////////////////////////////////
class Cspectra_descriptor
{
  std::string name ;
  std::string notice;
public:
  
  Cspectra_descriptor ( const std::string fname = path.dir_spectra() + "descriptions.txt" )
  {
    name = fname ;
    //
    std::ofstream plik ( name.c_str() );  // deleting the previous contents
    plik << "version_with_incrementers\n";  // to distinguish from the old style
    plik.close();
  }
  //--------------------------- for 1D spectra
  void add_entry ( const std::string spec_name, double bin_x, double first_x, double last_x, const std::string notice, const std::string list_of_incrementers )
  {
    std::ofstream plik ( name.c_str(), std::ios::app );
    plik << spec_name << "\n" << bin_x << "\t" << first_x << "\t" << last_x  ;
    plik << "\n{\n" << notice << "\n}"
    << "\n{\n" << list_of_incrementers << "\n}" << std::endl;
    plik.close();
  }
  //---------------------------  for 2D spectra
  void add_entry ( const std::string spec_name,
                  int bin_x, double first_x, double last_x,
                  int bin_y, double first_y, double last_y, const std::string notice, const std::string list_of_incrementers )
  {
    std::ofstream plik ( name.c_str(), std::ios::app );
    plik << spec_name << "\n"
    << bin_x << "\t" << first_x << "\t" << last_x
    << "\n"
    << bin_y << "\t" << first_y << "\t" << last_y ;
    
    plik << "\n{\n" << notice << "\n}"
    << "\n{\n" << list_of_incrementers << "\n}"
    << std::endl;
    
    plik.close();
  }
  void add_notice ( const std::string txt )
  {
    notice += txt ;
  }
  
};
//***************************************

// including my version of Cern Root histograms - without using Root library
#include "Thistograms_jurek.h"
//#include "TH1.h"
//#include "TH2.h"
//#include "TAxis.h"

//////////////////////////////////////////////////////////////////
class spectrum_abstr
{
  
protected:
  TH1 * ptr_root_spectrum;
  static  Cspectra_descriptor *spectra_descriptor;
  
public:
  
  spectrum_abstr()
  {
    if ( spectra_descriptor == 0 )   // <-- static
      spectra_descriptor = new Cspectra_descriptor;  // only one for all
  }
  //------------------------
  
  TH1* give_root_spectrum_pointer()
  {
    return  ptr_root_spectrum;
  }
  
  //------------------------------
  const std::string give_name()
  {
    return ptr_root_spectrum->GetName();
  }
  
  virtual ~spectrum_abstr()
  {
    delete spectra_descriptor ;
    spectra_descriptor = 0 ; // null
  } ;
  
  /** No descriptions */
  void add_notice ( const std::string txt ); // why it is here  ?
  
  virtual void save_to_disk() = 0 ;
  virtual void increment ( int value ) = 0 ;
  virtual void increment ( double value ) = 0 ;
};

///////////////////////////////////////////////////////////////////////////////
class spectrum_1D : public spectrum_abstr
{
protected:
  int spectrum_length ; // so many "bins"
  double min_bin ;
  double max_bin ;
  
public:
  spectrum_1D ( const std::string name_root, const std::string title, int h_one, double h_two, double h_three,
               const std::string folder, // = "",
               const std::string note, //  = "",
               const std::string list_incrementers //= "no incrementers known"
  );
  
  spectrum_1D() ;  // default for contitional spectra
  
  // D'TOR-------------------------------------
  ~spectrum_1D();
  void save_to_disk() ;
  //----------------------
  void increment ( int value )
  {
    //              std::cout << "just before incrementation " << ptr_root_spectrum->GetName()
    //                    << " with " << value << std::endl ;
    ptr_root_spectrum->Fill ( value );  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //statistics();
  }
  // -------------------------------------------
  void increment ( double value )
  {
    // std::cout << "just before incrementation " << value << std::endl ;
    ptr_root_spectrum->Fill ( value );   //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //statistics();
  }
  
  void zeroing(  )
  {
    // std::cout << "just before zeroing " << value << std::endl ;
    ptr_root_spectrum->Reset ( );
  }
  
  void increment ( int, int )  ;
  void increment_by ( long int chan, int value ); // increment channel nr chan, by value
  /** this function is meant only for scalers "spectra". They are scrolled like
   pen writing on the paper band */
  void scroll_left_by_n_bins ( int bins );
  /** No descriptions */
  void create ( const std::string name_root, const std::string  title, int nr_of_bins,
               double first_channel,
               double last_channel, const std::string folder, const std::string  note, const std::string list_incrementers );
  /** for continuation option */
  void read_from_disk();
  /** No descriptions */
  double give_max_chan_paper();
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class spectrum_2D : public spectrum_abstr
{
protected:
  int x_spectrum_length ; // so many "bins"
  double x_min_bin ;
  
  int y_spectrum_length ; // so many "bins"
  double y_min_bin ;
public:
  spectrum_2D ( const std::string  name_root, const std::string  title,
               int nr_of_bins_x, double first_channel_x, double last_channel_x,
               int nr_of_bins_y, double first_channel_y, double last_channel_y,
               const std::string  folder , // = "",
               const std::string  note , // = "",
               const std::string list_incrementers  // = "2D incrementers"
  );
  
  // D'TOR-------------------------------------
  ~spectrum_2D();
  
  void increment_yourself();
  void save_to_disk() ;
  
  virtual void increment ( double value )
  {
    increment ( ( int ) value ) ;
  }
  //------------------------------------------
  void increment ( int value )
  {
    std::cout << "Fuction: increment for matrix must have 2 arguments...  press any key" << std::endl ;
    int liczba ;
    std::cin >> liczba ;
  }
  //------------------------------------------
  void increment ( int x_value, int y_value )
  {
    // std::cout << "spectrum_2D:: increment" << std::endl;
    ptr_root_spectrum->Fill ( x_value, y_value );   //@@@@@@@@@@@@@@@@@@
    //statistics();
  }
  
  void increment ( double x_value, double y_value )
  {
    ptr_root_spectrum->Fill ( x_value, y_value ); // @@@@@@@@@@@@@@@@@@@
    //statistics();
  }
  
  void increment ( int x_value, double y_value )
  {
    ptr_root_spectrum->Fill ( x_value, y_value ); // @@@@@@@@@@@@@@@@@@@
    //statistics();
  }
  void increment ( double x_value, int y_value )
  {
    ptr_root_spectrum->Fill ( x_value, y_value ); // @@@@@@@@@@@@@@@@@@@
    //statistics();
  }
  
  void read_from_disk();
  /** No descriptions */
  void increment_by ( int x, int y, int value );
};
///////////////////////////////////////////////////////////////////////////////
#endif // SPECTRUM_H

