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
// This information isused by the cracow
//////////////////////////////////////////////////////////////////
class Cspectra_descriptor
{
  std::string name ;
  std::string notice;
public:
  
  Cspectra_descriptor(const std::string fname = "spectra/descriptions.txt")
  {
    name = fname ;
    //
    std::ofstream plik(name.c_str());     // deleting the previous contents
    plik << "version_with_incrementers\n";  // to distinguish from the old style
    plik.close();
  }
  //--------------------------- for 1D spectra
#define WHERE   std::cout << __FILE__<< ", line " << __LINE__ << std::endl;;
  void add_entry(const std::string spec_name, double bin_x, double first_x, double last_x, const std::string notice, const std::string list_of_incrementers)
  {
    //  std::cout << "linia " << __LINE__ << std::endl;;
    std::ofstream plik(name.c_str(), std::ios::app);
    
    plik << spec_name << "\n" << bin_x
    << "\t" << first_x << "\t" << last_x
    << "\n{\n" << notice << "\n}"
    << "\n{\n"
    << list_of_incrementers
    << "\n}" << std::endl;
    plik.close();
  }
  //---------------------------  for 2D spectra
  void add_entry(const std::string spec_name,
                 int bin_x, double first_x, double last_x,
                 int bin_y, double first_y, double last_y, const std::string notice, const std::string list_of_incrementers)
  {
    std::ofstream plik(name.c_str(), std::ios::app);
    plik << spec_name << "\n"
    << bin_x << "\t" << first_x << "\t" << last_x
    << "\n"
    << bin_y << "\t" << first_y << "\t" << last_y ;
    
    plik << "\n{\n" << notice << "\n}"
    << "\n{\n" << list_of_incrementers << "\n}"
    << std::endl;
    
    plik.close();
  }
  void add_notice(const std::string txt)
  {
    notice += txt ;
  }
  
};
//***************************************

// including my version of Cern Root histograms - without using Root library
#include "Thistograms_jurek.h"


//////////////////////////////////////////////////////////////////
class spectrum_abstr
{
  
protected:
  TH1 * ptr_root_spectrum;
  // char spectrum_name[100] ;
  int statistics_of_increments ;
  
  bool * ptr_condition_flag_is_true ;
  
  static  Cspectra_descriptor *spectra_descriptor;
  
public:
  
  spectrum_abstr()
  {
    ptr_condition_flag_is_true = 0 ;
    
    if(spectra_descriptor == 0)
      spectra_descriptor = new Cspectra_descriptor;
  }
  //------------------------
  
  virtual void int_incrementer_X(int * candidate) = 0 ;
  virtual void int_incrementer_Y(int * candidate) = 0 ;
  
  virtual void double_incrementer_X(double * candidate) = 0 ;
  virtual void double_incrementer_Y(double * candidate) = 0 ;
  
  void set_adres_of_condition_flag(bool * adres)
  {
    ptr_condition_flag_is_true = adres;
  }
  
  
  bool can_we_increment()
  {
    if(ptr_condition_flag_is_true == 0)
    {
      //std::cout << " There was 0 in spectrum condiotion flag" << std::endl ;
      return true;  // there was no condition
    }
    else
    {
      //std::cout << " There was reall adres in spectrum condiotion flag (and *adres is " <<*(ptr_condition_flag_is_true)  << std::endl ;
      return * (ptr_condition_flag_is_true) ;   //there was, and it is true
    }
  }
  
  
  virtual void reset_incrementers() = 0 ;
  
  virtual void increment_yourself()
  {}
  ;  // = 0;
  TH1* give_root_spectrum_pointer()
  {
    return  ptr_root_spectrum;
  }
  
  //------------------------------
  const std::string give_name()
  {
    return ptr_root_spectrum->GetName();
  }
  
  
  void statistics(int nr = 1)
  {
    statistics_of_increments += nr ;
  }
  virtual int give_statistic()
  {
    return statistics_of_increments;
  }
  
  virtual ~spectrum_abstr()
  {
    delete spectra_descriptor ;
    spectra_descriptor = 0 ; // null
  } ;
  
  /** No descriptions */
  void add_notice(const std::string txt);    // why it is here  ?
  
  virtual void save_to_disk() = 0 ;
  virtual void increment(int value) = 0 ;
  virtual void increment(double value) = 0 ;
};

///////////////////////////////////////////////////////////////////////////////
class spectrum_1D : public spectrum_abstr
{
  
protected:
  //  int * *int_incrementers_list ; // pointer to table of pointers
  std::vector<int*> int_incrementers_vector ;
  
  // int how_many_int_incrementers;
  
  //double * *double_incrementers_list ; // pointer to table of pointers
  //int how_many_double_incrementers;
  std::vector<double*> double_incrementers_vector ;
  
  int spectrum_length ; // so many "bins"
  double min_bin ;
  double max_bin ;
  
public:
  
  //-------------------------------------------
  spectrum_1D(const std::string name_root, const std::string title, int h_one, double h_two, double h_three,
              const std::string folder = "", const std::string note = "", const std::string list_incrementers = "no incrementers known");
  
  spectrum_1D(const std::string name_root, int h_one, double h_two, double h_three,
              const std::string folder = "", const std::string note = "", const std::string list_incrementers = "no incrementers known");

  spectrum_1D() ;  // default for contitional spectra
  
  // D'TOR-------------------------------------
  ~spectrum_1D();
  
  //-------------------------------------------
  void increment_yourself();
  
  //------------------------------------------
  void int_incrementer_X(int * candidate);
  void int_incrementer_Y(int * candidate);
  
  void double_incrementer_X(double * candidate);
  void double_incrementer_Y(double * candidate);
  
  
  void reset_incrementers()
  {
    int_incrementers_vector.resize(0) ;
    // same for doubles
    double_incrementers_vector.resize(0) ;
  }
  //------------------------------------------
  void save_to_disk() ;
  
  //    void increment(int value)  ;
  //    void increment(double value)  ;
  //----------------------
  void increment(int value)
  {
    //              std::cout << "just before incrementation " << ptr_root_spectrum->GetName()
    //                    << " with " << value << std::endl ;
    ptr_root_spectrum->Fill(value);     //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //statistics();
  }
  // -------------------------------------------
  void increment(double value)
  {
    // std::cout << "just before incrementation " << value << std::endl ;
    ptr_root_spectrum->Fill(value);      //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //statistics();
  }
  
  void zeroing()
  {
    // std::cout << "just before zeroing " << value << std::endl ;
    ptr_root_spectrum->Reset();
  }
  
  void increment(int, int)  ;
  void inc_by(long int chan, int value);    // increment channel nr chan, by value
  /** this function is meant only for scalers "spectra". They are scrolled like
   pen writing on the paper band */
  void scroll_left_by_n_bins(int bins);
  /** No descriptions */
  void create(const std::string name_root, const std::string  title, int nr_of_bins,
              double first_channel,
              double last_channel, const std::string folder, const std::string  note, const std::string list_incrementers);
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
  
  std::vector<int*> x_int_incrementers_vector ;
  std::vector<double*> x_double_incrementers_vector ;
  
  
  int x_spectrum_length ; // so many "bins"
  double x_min_bin ;
  
  std::vector<int*> y_int_incrementers_vector ;
  std::vector<double*> y_double_incrementers_vector ;
  
  int y_spectrum_length ; // so many "bins"
  double y_min_bin ;
  
public:
  
  //-------------------------------------------
  spectrum_2D(const std::string  name_root, const std::string  title,
              int nr_of_bins_x, double first_channel_x, double last_channel_x,
              int nr_of_bins_y, double first_channel_y, double last_channel_y,
              const std::string  folder = "", const std::string  note = "", const std::string list_incrementers = "2D incrementers");
  spectrum_2D(const std::string  name_root,
              int nr_of_bins_x, double first_channel_x, double last_channel_x,
              int nr_of_bins_y, double first_channel_y, double last_channel_y,
              const std::string  folder = "", const std::string  note = "", const std::string list_incrementers = "2D incrementers");

  // D'TOR-------------------------------------
  ~spectrum_2D();
  
  
  //-------------------------------------------
  void increment_yourself();
  
  //------------------------------------------
  void int_incrementer_X(int * candidate);
  void int_incrementer_Y(int * candidate);
  
  void double_incrementer_X(double * candidate);
  void double_incrementer_Y(double * candidate);
  
  void reset_incrementers()
  {
    x_int_incrementers_vector.resize(0) ;
    x_double_incrementers_vector.resize(0) ;
    
    y_int_incrementers_vector.resize(0) ;
    y_double_incrementers_vector.resize(0) ;
  }
  //------------------------------------------
  void save_to_disk() ;
  
  virtual void increment(double value)
  {
    increment((int) value) ;
  }
  
  //------------------------------------------
  void increment(int value)
  {
    std::cout << "Fuction: increment for matrix must have 2 arguments...  press any key" << std::endl ;
    int liczba ;
    std::cin >> liczba ;
  }
  //------------------------------------------
  void increment(int x_value, int y_value)
  {
    // std::cout << "spectrum_2D:: increment" << std::endl;
    ptr_root_spectrum->Fill(x_value, y_value);      //@@@@@@@@@@@@@@@@@@
    //statistics();
  }
  
  void increment(double x_value, double y_value)
  {
    ptr_root_spectrum->Fill(x_value, y_value);    // @@@@@@@@@@@@@@@@@@@
    //statistics();
  }
  void increment(int x_value, double y_value)
  {
    ptr_root_spectrum->Fill(x_value, y_value);      //@@@@@@@@@@@@@@@@@@
  }
  void increment(double x_value,  int y_value)
  {
    ptr_root_spectrum->Fill(x_value, y_value);      //@@@@@@@@@@@@@@@@@@
  }
  void read_from_disk();
  /** No descriptions */
  void increment_by(int x, int y, int value);
};
///////////////////////////////////////////////////////////////////////////////
#endif // SPECTRUM_H

