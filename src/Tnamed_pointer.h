/***************************************************************************
                          Tnamed_pointer.h  -  description
                             -------------------
    begin                : Wed Aug 13 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/
#ifndef TNAMED_POINTER_H
#define TNAMED_POINTER_H

#include <string>
#include <map>
#include <vector>

class Tincrementer_donnor;
/**This is the class for wizard. It will read the names of the variables
which we will need on spectrum
  *@author dr. Jerzy Grebosz (IFJ Krakow, Poland)
  */

///////////////////////////////////////////////////////////////////////////
class Tnamed_pointer
{
public:
  bool flag_multi_entry ;

bool mutative;  /**this ALL incrementer can change its components any event
// if was invented for the 'analog beta' system. For every event, for every gamma energy
// we make many (for example 200) versions of doppler correction - with different beta
// the results of such correction will be available in a ALL... incrementere. But this
// incremetner can have sometimes 200 * n items, and sometimes 0 items - depending
// on event.
// This flag tells that this incrementer is one of such.
// Before incrementing the user spectrum - the incremetning routine will reread the 
// current 'components' of this incrementer */


  enum Twhat_type {
    is_double =20000, is_int, is_bool,
    //is_vector_double, is_vector_int, is_vector_bool
  };

  Twhat_type what_type ;
  union {
    double *double_ptr ;
    int    *int_ptr;
    bool   *bool_ptr ;
  };
  bool *data_usable  ;  // ptr to the value telling if the data is good of bad (element not fired)
  Tincrementer_donnor *ptr_detector ; // to know when X Y are comming from the same detector

  // for TOTAL entries ----------
  struct vek_entry
  {
    Twhat_type v_what_type ;
    union {
      double *v_double_ptr ;
      int    *v_int_ptr;
      bool   *v_bool_ptr ;
    };
    bool *v_data_usable ;// ptr to the value telling if the data is good of bad (element not fired)
    Tincrementer_donnor *v_ptr_detector;
  };

  std::vector<vek_entry> vek ;

public:
  Tnamed_pointer();
  //~Tnamed_pointer();

  // null below means that no indicator, so the data is always usable
  // Otherwise there is a pointer to bool value telling:
  //    - this data is true (fired, good)
  //    - this data is false (not fired, not good)
  Tnamed_pointer(int *ip, bool *data_usable, Tincrementer_donnor *det);
  Tnamed_pointer(bool *b_ptr, bool *data_usable, Tincrementer_donnor *det);
  Tnamed_pointer(double *p, bool *data_usable, Tincrementer_donnor *det, bool flag_mutative = false);

  void add_item(int *ip, bool *data_usable, Tincrementer_donnor *det);
  void add_item(bool *b_ptr, bool *data_usable, Tincrementer_donnor *det);
  void add_item(double *p, bool *data_usable, Tincrementer_donnor *det);
  
  static
  void add_as_ALL(std::string entry_name, double *p, bool *data_usable, Tincrementer_donnor *det);
  static
  void add_as_ALL(std::string entry_name, int *p, bool *data_usable, Tincrementer_donnor *det);
  static
  void add_as_ALL(std::string entry_name, bool *p, bool *data_usable, Tincrementer_donnor *det);

  static
  void add_as_mutative_ALL(std::string entry_name, double *p, bool *data_usable, Tincrementer_donnor *det);

// template <typename typ>
//   static
//   void add_as_mutative_ALL(std::string entry_name, typ  p, bool *data_usable, Tincrementer_donnor *det);


static
  void remove_from_map_ALL(std::string entry_name);

  /** No descriptions */

  int is_in_AND_gate(double low, double high, bool obligatory);
  int is_in_OR_gate(double low, double high, bool obligatory);

protected: // Protected methods
  /** The first entry always is placed in single.
  In case of adding the next - we move single to the [0] elem of vector */
  void copy_single_to_vector();
};
/////////////////////////////////////////////////////////////////////
typedef std::map<std::string, Tnamed_pointer> Tmap_of_named_pointers;

extern std::map<std::string, Tnamed_pointer> named_pointer ;

#endif
