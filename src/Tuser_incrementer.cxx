#include "Tuser_incrementer.h"

#ifdef USER_INCREMENTERS_ENABLED
#include <fstream>
#include <cstdlib>
#include "paths_spy.h"

//****************************************************************************************
Tuser_incrementer::Tuser_incrementer(
  std::string name_of_result,
  std::string name_of_component_a,
  operation_type oper,
  std::string name_of_component_b)
    :
    name_result(name_of_result),
    name_a(name_of_component_a),
    name_b(name_of_component_b),
    operation(oper)
{

  set_parameters(name_of_result,
                 name_of_component_a,
                 oper,
                 name_of_component_b);

}
//*****************************************************************************************
void  Tuser_incrementer::set_parameters(std::string name_of_result,
                                        std::string name_of_component_a,
                                        operation_type   oper,
                                        std::string name_of_component_b)
{
  // at first we try to get information about the components of this arithmetis

  std::string entry_name =  name_of_component_a ;
  Tmap_of_named_pointers::iterator   pos;
  pos = named_pointer.find(name_of_component_a);
  if (pos == named_pointer.end ())
  {
    a_ptr = NULL;   // <--- wrong, it does not exist
    if(!a_ptr) std::cout << "Tuser_incrementer::ctor  -  Component incrementer: "
      << name_of_component_a << " was not found " << std::endl;
    exit(5);
  }
  else
    a_ptr = &(named_pointer[name_of_component_a]);

  //Tmap_of_named_pointers::iterator
  pos = named_pointer.find(name_of_component_b);
  if (pos == named_pointer.end ())
  {
    b_ptr = NULL;
    if(!b_ptr) std::cout << "Tuser_incrementer::ctor   Component incrementer: "
      << name_of_component_b << " was not found " << std::endl;
    exit(6);
  }
  else
    b_ptr = &(named_pointer[name_of_component_b]);

  //=========================
  // now we can create our incrementer
  // searching if it already exists
  pos = named_pointer.find(name_of_result);
  // if it already exist ?
  if (pos != named_pointer.end ())
  {
    // if yes, remove it
    named_pointer.erase(pos);
  }
  // putting once more
  named_pointer[name_of_result]
  = Tnamed_pointer(&result,   &result_validator,
                   0// a_ptr->ptr_detector  // instead of   "this"
                  ) ;

}
//*****************************************************************************
Tuser_incrementer::~Tuser_incrementer()
{}
//*****************************************************************************
void Tuser_incrementer::evaluate_incrementer_for_current_event()
{
  if(a_ptr && b_ptr)
  {
    int flag_a = (a_ptr->data_usable) ?
                 *(a_ptr->data_usable)
                 : true;   // 0 (NULL) means: always valid

    int flag_b = (b_ptr->data_usable) ?
                 *(b_ptr->data_usable)
                 : true;   // 0 (NULL) means: always valid

    result_validator =
      flag_a &&  flag_b;

    if(result_validator)
    {
      switch(operation)
      {
      case plus:
        result =
          *(a_ptr->double_ptr)
          +
          *(b_ptr->double_ptr);
        break;

      case minus:
        result =
          *(a_ptr->double_ptr)
          -
          *(b_ptr->double_ptr);
        //         std::cout << "NEW  tof_21_41_LL_cal_minus_tof_22_41_LL_cal= "
        //         << result
        //         <<", (" <<  *(a_ptr->double_ptr)
        //         << " - " <<  *(b_ptr->double_ptr)
        //         << std::endl;

        break;

      case multiply:
        result =
          *(a_ptr->double_ptr)
          *
          *(b_ptr->double_ptr);
        break;

      case divide:
        if( *(b_ptr->double_ptr))  // not division by zero
        {
          result =
            *(a_ptr->double_ptr)
            /
            *(b_ptr->double_ptr);
        }else result_validator = false;
        break;
      }
      //       std::cout << "Calculating the incremeter: "
      //       << name_result << " = "
      //       << result
      //       <<", (" <<  *(a_ptr->double_ptr)
      //       << " - " <<  *(b_ptr->double_ptr)
      //       << ")  validator = " << result_validator
      //       << std::endl;

    }
  }
  else
  {
    if(!a_ptr) std::cout << "Tuser_incrementer: Component incrementer: "
      << name_a << " was not found " << std::endl;
    if(!b_ptr) std::cout << "Tuser_incrementer: Component incrementer: "
      << name_b << " was not found " << std::endl;
    exit(4);
  }
}
//*****************************************************************************
void Tuser_incrementer::read_in_parameters(std::string name)
{
  std::string pat_name = path.dir_incrementers() + name ;
  if(pat_name.find(".incr") == std::string::npos)
  {
    pat_name += ".incr" ;
  }
  std::ifstream plik(pat_name.c_str() );
  if(!plik)
  {
    // cant open file warning
    std::cout << "Error while opening file: " << pat_name << std::endl;
    exit(1);
  }
	int op_code = -1;
	while(plik.good()){
    std::string aline;
    std::getline(plik,aline);
		//std::cout << aline << std::endl;
    if(aline.empty()) continue;
    if(aline[0]=='#' || aline[0]=='/') continue;
    if(aline[0]=='$'){
		 std::istringstream oneline(aline);
	   std::string keywork, value;
		 oneline >> keywork >> value;
		 if(keywork=="$user_incrementer_name"){name_result=value;}
		 else if (keywork=="$other_incrementer_nameA"){name_a=value;}
		 else if (keywork=="$operation"){op_code=atoi(value.c_str());}
		 else if (keywork=="$other_incrementer_nameB"){name_b=value;}
		 }
  }
		 
 // plik >> keyword;
 // plik >> name_result ;


//  plik >> name_a ;


 // int op_code = 0 ;
//  plik >> op_code ;
	std::cout << "Operation code: " << op_code << std::endl;;
  switch(op_code)
  {
  case 1 :     operation = plus; break;
  case 2 :     operation = minus; break;
  case 3 :     operation = multiply; break;
  case 4 :     operation = divide; break;

  }

  // nr what: incrementer of constant
 // int what = 0;
//  plik >> what;

//  plik  >> name_b ;
//  double constant;
//  plik >> constant ;

  std::cout << name_result << " " << name_a << " " << operation << " " << name_b << std::endl;
  if(name_a.empty() || name_b.empty() || name_result.empty() || op_code <0)
  {
    std::cout << "Tuser_incrementer::read_in_parameters ---> Error during reading the file " << std::endl;
    exit(1);
  }

  set_parameters(name_result, name_a,  operation, name_b);
}

#endif // #ifdef USER_INCREMENTERS_ENABLED
