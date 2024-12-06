/***************************************************************************
                          tfile_helper.h  -  description
                             -------------------
    begin                : Sat May 17 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl


    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!!
    There is a similar file in the cracow program, however it does not
    have a function:
   -  read_banana
    bool read_default_gate(std::string gate_name, double *low, double *high);
 ***************************************************************************/



#ifndef TFILE_HELPER_H
#define TFILE_HELPER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
// FOR GLOBAL FUNCITONS ( read gate, read banana)

//#include "TIFJAnalysis.h"  // for registering conditions
#include "TjurekPolyCond.h"


/**this class helps to search inside the configuration files, or callibration files
for some keywords. Such keywords are usually followed by the values of
the associated parameter
  *@author Jurek Grebosz (IFJ Krakow, Poland)
  */
// two classes to carry the exception
class Tfile_helper_exception
{
public:
    std::string message ;

};
class Tno_keyword_exception : public Tfile_helper_exception { } ;
class Treading_value_exception : public Tfile_helper_exception { } ;

#define typ_strumienia std::istream
//**********************************************************************
namespace File_helper
{

/** This function searches (in the given file) a keyword and then reads the
value which follows it. Then returns this value as the result.  */
template <typename result>
result find_in_file(typ_strumienia& s, std::string slowo) ;

/** find a keyword, - just for positioning cursor  */
void spot_in_file(typ_strumienia& s, std::string slowo);
/** find in the given directory the files which names contain a filter substd::string */

std::vector<std::string> find_files_in_directory(std::string dir, std::string filter);

void repair_the_stream(typ_strumienia& s)  ;

};
/////////////////////////////////////////////////////////////////////////
std::istream & zjedz(std::istream & plik);

bool read_default_gate(std::string gate_name, double *low, double *high);
bool read_banana(std::string gate_name, TjurekPolyCond *  *polygon);



//****************************************************************************
/** This function searches (in the given file) a keyword and then reads the
 value which follows it. Then returns this value as the result.  */

template <typename result_type = double>  // here we could use a default (= double)
result_type File_helper::find_in_file(typ_strumienia& s, std::string slowo)
{

  s.seekg(0) ;    // rewind
  std::string arch_s = slowo ; // to have oryginal case for error message

  if(s.eof())
    std::cout << "Something wrong with the file caused EOF state. Put a new line on the end of it "
    << std::endl;

  // to be independent of lower/upper case of keywords...
  //transform(slowo.begin(), slowo.end(), slowo.begin(),  tolower);
  for(unsigned i = 0 ; i < slowo.size() ; i++) slowo[i] = tolower(slowo[i]);

   //   std::cout << "Keyword to find is " << slowo << std::endl ;
  std::string word ;
  //-----------------
  while(s)
  {
    s >> zjedz >> word ;
    //transform(word.begin(), word.end(), word.begin(), tolower);
    for(unsigned i = 0 ; i < word.size() ; i++) word[i] = tolower(word[i]);

    if(word == slowo) break ;
  }
  //----------------

  if(!s)
  {

    Tno_keyword_exception capsule ;
    capsule.message =  "I can't find keyword \""
    + arch_s
    + "\" in the file.(The search was case UNsensitive)" ;
    throw capsule ;
  }
      //std::cout << "found word " << word << std::endl ;
  // otherwise we read the data
  result_type value ;
  s >> value ;
  //std::cout << "przeczytana value = " << value << std::endl;

  if(!s)
  {


    Treading_value_exception capsule;

    capsule.message = "I found the keyword \""
    + slowo
    + "\", but an error occured during in reading its value." ;

    throw capsule ;

  }
  return value ;

}


// useful alias ////////////////////////////////////////////////////////////
namespace FH = File_helper;
namespace Tfile_helper = File_helper;
namespace Nfile_helper = File_helper;

#endif
