/***************************************************************************
                          Tcondition_description.h  -  description
                             -------------------
    begin                : Mon May 19 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/


#ifndef TCONDITION_DESCRIPTION_H
#define TCONDITION_DESCRIPTION_H

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

/**This class represents the one piece of the chain of condition.
  *@author Jurek Grebosz (IFJ Krakow, Poland)
  */

class Tcondition_description
{
//   friend class manager_user_spectra ;
//    friend class condition_wizard ;
protected:
public:
  struct Titem_1D
  {
      bool must_be_valid ;
      std::string var_name ;
      double min_value, max_value ;
  };
  struct Titem_2D
  {
      bool must_be_valid ;
      std::string var_x_name ;
      std::string var_y_name ;
      std::string polygon_name;
  };


  std::string name ;
  std::vector<Titem_1D> item_1D_AND;
  std::vector<Titem_2D> item_2D_AND;
  std::vector<Titem_1D> item_1D_OR;
  std::vector<Titem_2D> item_2D_OR;
  std::vector<std::string> other_condition_AND;
  std::vector<std::string> other_condition_OR;
  std::vector<std::string> other_condition_NAND;
  std::vector<std::string> other_condition_NOR;

public:
	Tcondition_description();
	~Tcondition_description();
  /** No descriptions */
  void save_condition();
  /** No descriptions */
  void read_in_description();
  void set_name(std::string n) { name = n; }
  std::string give_name() { return name; }
 
  
};

#endif
