/***************************************************************************
                          Tnamed_pointer.cpp  -  description
                             -------------------
    begin                : Wed Aug 13 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/

#include "Tnamed_pointer.h"
#include <string>
#include <map>

#include <iostream>

std::map<std::string, Tnamed_pointer> named_pointer ;
//**************************************************************************
Tnamed_pointer::Tnamed_pointer()
{
  flag_multi_entry = false;
mutative = false;

  //what_type = 0 ;
  double_ptr = 0 ;
  int_ptr    = 0 ;
  bool_ptr = 0 ;
  data_usable = 0 ;
}
//***********************************************************************
#ifdef NIGDY
Tnamed_pointer::~Tnamed_pointer()
{}
#endif
/****************************************************** No descriptions */
Tnamed_pointer::Tnamed_pointer(double *p, bool *enable, Tincrementer_donnor *det,
bool flag_mutative)
{
  flag_multi_entry = false;
mutative = flag_mutative;

  double_ptr = p ;
  what_type = is_double ;
  data_usable = enable ;
  ptr_detector = det ;
}
/***************************************************** No descriptions */
Tnamed_pointer::Tnamed_pointer(int *ip, bool *enable, Tincrementer_donnor *det)
{
  flag_multi_entry = false;
mutative = false;

  int_ptr = ip ;
  what_type = is_int ;
  data_usable = enable ;
  ptr_detector = det ;
}
/****************************************************** No descriptions */
Tnamed_pointer::Tnamed_pointer(bool *b_ptr, bool *enable, Tincrementer_donnor *det)
{
  flag_multi_entry = false;
mutative = false;

  bool_ptr = b_ptr ;
  what_type = is_bool ;
  data_usable = enable ;
  ptr_detector = det ;
}
//**********************************************************************
/****************************************************** No descriptions */
void Tnamed_pointer::add_item(int *ip, bool *enable, Tincrementer_donnor *det)
{
  if(vek.size() == 0)
  {
    copy_single_to_vector();
  }

  // now we can add the new entry
  vek_entry tmp ;
  tmp.v_data_usable  = enable ; // was by mistake: data_usable ;
  tmp.v_int_ptr = ip ;
  tmp.v_what_type = is_int ;
  tmp.v_ptr_detector = det ;
  vek.push_back(tmp);
}
//**********************************************************************
void Tnamed_pointer::add_item(bool *b_ptr, bool *enable, Tincrementer_donnor *det)
{
  if(vek.size() == 0)
  {
    copy_single_to_vector();
  }

  // now we can add the new entry
  vek_entry tmp ;
  tmp.v_data_usable  = enable ;
  tmp.v_bool_ptr = b_ptr ;
  tmp.v_what_type = is_bool ;
  tmp.v_ptr_detector = det ;
  vek.push_back(tmp);
}
//**********************************************************************
void Tnamed_pointer::add_item(double *p, bool *enable, Tincrementer_donnor *det)
{
  if(vek.size() == 0)
  {
    copy_single_to_vector();
  }
  // now we can add the new entry
  vek_entry tmp ;
  tmp.v_data_usable  = enable ;
  tmp.v_double_ptr = p ;
  tmp.v_what_type = is_double ;
  tmp.v_ptr_detector = det ;
  vek.push_back(tmp);
}
//*********************************************************************
/** The first entry always is placed in single.
In case of adding the next - we move single to the [0] elem of vector */
void Tnamed_pointer::copy_single_to_vector()
{
  // copy the single entry into the element of vector
  vek_entry tmp ;
  tmp.v_data_usable  = data_usable ;
  tmp.v_ptr_detector = ptr_detector;
  tmp.v_what_type = what_type;

  switch(what_type)
  {
  case is_double :
    tmp.v_double_ptr = double_ptr ;
    //tmp.v_what_type = is_double ;
    break;

  case is_int :
    tmp.v_int_ptr = int_ptr ;
    //tmp.v_what_type = is_int ;
    break;

  case is_bool :
    tmp.v_bool_ptr = bool_ptr ;
    //tmp.v_what_type = is_bool ;
    break;

  }
  vek.push_back(tmp);
  flag_multi_entry = true ;

}
/**************************************************************/
// result 0 = false, 1 = true, -1 = not fired, try another
int Tnamed_pointer::is_in_AND_gate(double min_value, double max_value, bool obligatory)
{
  double value =0;

  if(!flag_multi_entry)
  {
    if(data_usable != 0)   // adres of flag was given
    {
      if(*data_usable == false)  // non legal now ?
      {
        if(obligatory)
        {
          return 0 ; //false;     // non legal, but was obligatory - >SO: false
        }
        else  // we wish only 'if present', only 'if legal'
        {
			 std::cout << "I have doubts how to decide in this conditional case " << std::endl;
          return -1 ;    // non legal, but only WHEN legal -> SO: return -1, and the calling function have to reacty properly for this -1
        }
      }
      //else - if is legal value , make test
    }
    // else if 0 legality pointer- always make a test

    switch(what_type )
    {
    case is_int:
      value = *(int_ptr) ;
      break;

    case is_double:
      value = *(double_ptr) ;
      break;

    case is_bool:
      value = *(bool_ptr) ;
      break;
    }

    if(value >= min_value  &&  value <= max_value)
    {
      return true ;
    }
    else return false ;

  }
  //===================== MULTI entry ================
  else
  {
    bool at_least_one_was_legal = false ;

    for(unsigned m = 0 ; m < vek.size() ; m++)
    {
      if(vek[m].v_data_usable != 0)   // adres of flag was given
      {
        if(*vek[m].v_data_usable == false)  // non legal now ?
        {
          if(obligatory)
          {
            return false;
          }
          else  // we wish only 'if present', only 'if legal'
          {
            continue ; //return true ;
          }
        }
        //else - if is legal, make test
      }

      // -------- lets test

      double value = 0 ;
      switch(vek[m].v_what_type)
      {
      case    Tnamed_pointer::is_double:
        value = (*(vek[m].v_double_ptr));
        break;
      case    Tnamed_pointer::is_int:
        value = (*(vek[m].v_int_ptr));
        break;
      case    Tnamed_pointer::is_bool:
        value = (*(vek[m].v_bool_ptr));
        break;
      }

      // std::cout << "gamma = " << value << std::endl;

      if(value < min_value
          ||
          value > max_value)
      {
        // std::cout << "      ^---------- False " << std::endl;
        return false;
      }
      // std::cout << "  true " << std::endl;
      at_least_one_was_legal = true;

    } // for mulit items (

    // std::cout <<"All legal multi were true " << std::endl;

    // all multi were true and at least one was legal
    return at_least_one_was_legal ;

  } // endif multi
}
//******************************************************************
/**************************************************************/
int Tnamed_pointer::is_in_OR_gate(double min_value, double max_value, bool obligatory)
{
  double value = 0;

  if(!flag_multi_entry)
  {
    if(data_usable != 0)   // adres of legality flag (validator) was given
    {
      if(*data_usable == false)  // validator says: "non legal now"
      {
        if(obligatory)
        {
          return false;     // non legal, but obligatory - >false
        }
        else  // we wish only 'if present', only 'if legal'
        {
          return -1 ;    // non legal, but was not obligatory SO: return -1, and the calling function have to reacty properly for this -1
        }
      }
      //else - if is legal value , make test
    }
    // else if 0 legality pointer- always make a test

    switch(what_type )
    {
    case is_int:
      value = *(int_ptr) ;
      break;

    case is_double:
      value = *(double_ptr) ;
      break;

    case is_bool:
      value = *(bool_ptr) ;
      break;
    }

    if(value >= min_value  &&  value <= max_value)
    {
      return true ;
    }
    else return false ;

  }
  //===================== MULTI entry ================
  else
  {
    for(unsigned m = 0 ; m < vek.size() ; m++)
    {
      if(vek[m].v_data_usable != 0)   // adres of flag was given
      {
        if(*vek[m].v_data_usable == false)  // non legal now ?
        {
          if(obligatory)
          {
            continue ;
          }
          else  // we wish only 'if present', only 'if legal'
          {
            continue ; 
          }
        }
        //else - if is legal, make test
      }

      // -------- lets test

      double value = 0 ;
      switch(vek[m].v_what_type)
      {
      case    Tnamed_pointer::is_double:
        value = (*(vek[m].v_double_ptr));
        break;
      case    Tnamed_pointer::is_int:
        value = (*(vek[m].v_int_ptr));
        break;
      case    Tnamed_pointer::is_bool:
        value = (*(vek[m].v_bool_ptr));
        break;
      }

      //std::cout << "value = " << value  << std::endl;

      if(value >= min_value
          &&
          value <= max_value)
      {
        // std::cout << "      <----------true " << std::endl;
        return true;
      }
      //     std::cout << "  true " << std::endl;

    } // for mulit items (

    //std::cout << "No multi was true " << std::endl;

    return false ; // no multi was true

  } // endif multi
}
//**********************************************************************
void Tnamed_pointer::
add_as_ALL(std::string entry_name, double *p, bool *data_usable, Tincrementer_donnor *det)
{
  Tmap_of_named_pointers::iterator pos;
  pos = named_pointer.find(entry_name);
  if (pos != named_pointer.end ())
  {
    pos->second.add_item(p, data_usable, det);
//     std::cout << "Adding " << entry_name << " flag_multi_entry= " <<
//     pos->second.flag_multi_entry
//     << ", size of vec = " << pos->second.vek.size()
//     << std::endl;

  }
  else
    named_pointer[entry_name] = Tnamed_pointer(p, data_usable, det );
}

//**********************************************************************
// template <typename typ>
// void Tnamed_pointer::
// add_as_mutative_ALL(std::string entry_name, typ p, bool *data_usable, 
// 						Tincrementer_donnor *det)

void Tnamed_pointer::
add_as_mutative_ALL(std::string entry_name, double *p, bool *data_usable, 
						Tincrementer_donnor *det)
{
  Tmap_of_named_pointers::iterator pos;
  pos = named_pointer.find(entry_name);
  if (pos != named_pointer.end ())
  {
    pos->second.add_item(p, data_usable, det);
//     std::cout << "Adding " << entry_name << " flag_multi_entry= " <<
//     pos->second.flag_multi_entry
//     << ", size of vec = " << pos->second.vek.size()
//     << std::endl;

  }
  else
    named_pointer[entry_name] = Tnamed_pointer(p, data_usable, det, true );
}

//**********************************************************************
void Tnamed_pointer::
add_as_ALL(std::string entry_name, int *p, bool *data_usable, Tincrementer_donnor *det)
{
  Tmap_of_named_pointers::iterator pos;
  pos = named_pointer.find(entry_name);
  if (pos != named_pointer.end ())
    pos->second.add_item(p, data_usable, det);
  else
    named_pointer[entry_name] = Tnamed_pointer(p, data_usable, det );
}
//***********************************************************************/
void Tnamed_pointer::
add_as_ALL(std::string entry_name, bool *p, bool *data_usable, Tincrementer_donnor *det)
{
  Tmap_of_named_pointers::iterator pos;
  pos = named_pointer.find(entry_name);
  if (pos != named_pointer.end ())
    pos->second.add_item(p, data_usable, det);
  else
    named_pointer[entry_name] = Tnamed_pointer(p, data_usable, det );
}
//***********************************************************************/
/// This function we need for dynamically changing 'beta analog' incrementer
// we will remove it every begining of event analysis and build it once more
// depending how many items have to be placed. Sometimes only 1, sometimes 200
void Tnamed_pointer::remove_from_map_ALL(std::string entry_name)
{
  Tmap_of_named_pointers::iterator pos;
  pos = named_pointer.find(entry_name);
  if (pos != named_pointer.end ())  // if it really exist
  {
    named_pointer.erase(pos);
  }

}
