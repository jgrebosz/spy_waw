//
// C++ Interface: Tuser_incrementer
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TUSER_INCREMENTER_H
#define TUSER_INCREMENTER_H


#include "experiment_def.h"
/**
This is a class which helps the user to define his new incremeters
which are kind of arithmetic on the existing ones.
@author dr. Jerzy Grebosz
*/

#ifdef USER_INCREMENTERS_ENABLED

#include <iostream>
#include <string>
#include <sstream>
#include "Tnamed_pointer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tuser_incrementer
{
protected:
  std::string name_result;
  std::string name_a, name_b;

  double result;
  bool result_validator;
  Tnamed_pointer *a_ptr;
  Tnamed_pointer *b_ptr;
public:
  enum operation_type { plus = 1, minus=2, multiply=3, divide=4 };
protected:
  operation_type operation;

public:
  Tuser_incrementer(): name_result("NONE"), a_ptr(0), b_ptr(0), operation(plus) {}
  
  Tuser_incrementer(std::string name_of_result,
                    std::string name_of_component_a,
                    operation_type   oper,
                    std::string name_of_component_b);

  ~Tuser_incrementer();
  void evaluate_incrementer_for_current_event();
  void read_in_parameters(std::string name_without_extension);
  
 void  set_parameters(std::string name_of_result,
                    std::string name_of_component_a,
                    operation_type   oper,
                    std::string name_of_component_b);


};
#endif // #ifdef USER_INCREMENTERS_ENABLED

#endif
