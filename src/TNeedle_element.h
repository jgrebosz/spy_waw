#ifndef _FRS_ELEMENT_H_
#define _FRS_ELEMENT_H_

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TGalileoEvent class
#ifndef  __CINT__

// abstract class for all elements of the frs, such as multiwire chambers, 
// music, scinitillator etc

#include <vector>
#include <fstream>
#include <string>

#include "Tfile_helper.h"
#include "randomizer.h"


class TNeedle ;
class spectrum_abstr ;
class TGalileoEvent ;


std::istream & zjedz(std::istream & plik);
// for spectra, they can contain a standard texts

extern std::string noraw ;
extern std::string noinc;
/////////////////////////////////////////////////////////////////////////////
class TNeedle_element   
{

 public:
  static TNeedle * owner ;
  /** Tells if this paricular object should load its data into the ntuple.
(For example some multiwires are interesting, other multiwires are not) */

  static TGalileoEvent * event_unpacked ; 
 protected:
  //TNeedle * parent ;


  // To avoid a waterfall scheme and to give more freedom to objects 
  // we allow them to ask other object for prepared data. 
  // If they were not calculated yet, they will be calculted.
  //
  bool calculations_already_done ;
  


  // this is not a vector. This is only a pointer to a vector
  // which is in the parent class frs. There, we "register" all
  // spectra created in various frs_elements. By this - it is
  // easier to make automatic save

  // but of course the real owners of the spectra are particullar 
  // frs_elements
 
  std::vector<spectrum_abstr*> *frs_spectra_ptr ;

  //------------------------------

  std::string name_of_this_element ; // for example SCI21_LR
  // geometrical position on the z axis (along beam line)


  //const 
  //double distance ;  // not necessery const


  // member functions -------------------

  // constructor------------------
 TNeedle_element( std::string _name);

  //frs_ele ment(std::string _name, double dist );
 
 public:
  // destructor-----------------------
  virtual ~TNeedle_element(){}  ; 

  virtual void analyse_current_event()= 0 ; //  pure virtual  ;
  virtual void make_preloop_action(std::ifstream &) = 0 ; //{} ; // read the calibration factors, gates

 protected:
  virtual void increment_your_spectra(){} ;
  virtual void save_your_spectra(){};

 public:
  void clear_already_done_flag() {calculations_already_done = false ; }
  bool was_it_calculated()       { return calculations_already_done ; } 
  //  double give_distance() const   { return distance ; }  
  std::string give_name()             { return name_of_this_element ; } 

};

#endif // __CINT__

#endif // _FRS_ELEMENT_H_
