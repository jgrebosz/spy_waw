/***************************************************************************
                          Tuser_condition.cpp  -  description
                             -------------------
    begin                : Thu Sep 4 2003
    copyright            : (C) 2003 by dr Jerzy Grebosz, IFJ   Cracow
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/


#include "Tuser_condition.h"
#include "Tcondition_description.h"
#include "Tnamed_pointer.h"
#include "Tfile_helper.h"
#include "Tuser_spectrum.h"   // for deprecated...

#include "TGalileoAnalysis.h"
TGalileoAnalysis *  Tuser_condition::owner ;
/***********************************************************************/
void Tuser_condition::read_in_parameters(std::string s)
{
    desc.set_name(s);
    desc.read_in_description() ;

//    std::cout << "Reading in the parameters for the condition named "
//         << desc.give_name()
//         << std::endl;

    condition_item_1D_AND.clear();
    condition_item_1D_OR.clear();
    condition_item_2D_AND.clear();
    condition_item_2D_OR.clear();


    other_condition_AND_ptr.clear();
    other_condition_OR_ptr.clear();
    other_condition_NAND_ptr.clear();
    other_condition_NOR_ptr.clear();

    // for statistics
    how_many_tested = 0;
    how_many_true = 0 ;




    // =========   1D AND ==============================
    // loop over all 1D items of descrption
    for (unsigned i = 0; i < desc.item_1D_AND.size() ; i++)
    {

        // std::vector<Tcondition item_1D_AND>
        Tcondition_item_1D tmp ;
        tmp.min_value = desc.item_1D_AND[i].min_value ;
        tmp.max_value = desc.item_1D_AND[i].max_value ;
        tmp.must_be_valid = desc.item_1D_AND[i].must_be_valid ;

        // finding in the map such a Tnamed_pointer

        Tmap_of_named_pointers::iterator pos = named_pointer.find(desc.item_1D_AND[i].var_name);

        if (pos == named_pointer.end ())   // if not found
        {
            // look between deprecated names
            pos = Tuser_spectrum::perhaps_deprecated_name_of_incrementer
                  (
                      named_pointer, desc.item_1D_AND[i].var_name);
        }

        if (pos != named_pointer.end ())
        {
            // Key found
            // std::cout << "Found std::string " << (pos->first) << std::endl;
            // std::cout << pos->first << " odpowiada_adres " << ((int) &(pos->second)) << std::endl;
            tmp.named_ptr = pos->second ;
        }
        else
        {
            // Key NOT found
            std::cout << "\nERROR: In the definition of user condition: "
                 << desc.give_name()
                 << "  there is a name of the incrementer (variable):"
                 << desc.item_1D_AND[i].var_name
                 << "\nwhich is not known to the spy.\n"
                 "Please go to the grebware viewer now, open the definition of this condition\n"
                 "and choose one of the available incrementers"
                 << std::endl;
            exit(1);
        }

        condition_item_1D_AND.push_back(tmp) ;
    }
    // now we have to


    // =========   1D OR ==============================
    // loop over all 1D items of descrption
    for (unsigned i = 0; i < desc.item_1D_OR.size() ; i++)
    {

        // std::vector<Tcondition item_1D_OR>
        Tcondition_item_1D tmp ;
        tmp.min_value = desc.item_1D_OR[i].min_value ;
        tmp.max_value = desc.item_1D_OR[i].max_value ;
        tmp.must_be_valid = desc.item_1D_OR[i].must_be_valid ;

        // finding in the map such a Tnamed_pointer

        Tmap_of_named_pointers::iterator pos =
            named_pointer.find(desc.item_1D_OR[i].var_name);

        if (pos == named_pointer.end ())   // if not found
        {
            // look between deprecated names
            pos = Tuser_spectrum::perhaps_deprecated_name_of_incrementer(named_pointer, desc.item_1D_OR[i].var_name);
        }

        if (pos != named_pointer.end ())
        {
            // Key found
            // std::cout << "Found std::string " << (pos->first) << std::endl;
            // std::cout << pos->first << " odpowiada_adres "
            // << ((int) &(pos->second)) << std::endl;
            tmp.named_ptr = pos->second ;
        }
        else
        {
            // Key NOT found
            std::cout << "\nERROR: In the definition of user condition: "
                 << desc.give_name()
                 << "  there is a name of the incrementer (variable):"
                 << desc.item_1D_OR[i].var_name
                 << "\nwhich is not known to the spy.\n"
                 "Please go to the grebware viewer now, open the definition of this condition\n"
                 "and choose one of the available incrementers"
                 << std::endl;
            exit(1);
        }

        condition_item_1D_OR.push_back(tmp) ;
    }



    //------------------------------------------------------
    // ---------- 2D items from the description ------------
    //------------------------------------------------------

    //============= 2D AND ===============================

    // loop over all 2D items of descrption
    for (unsigned i = 0; i < desc.item_2D_AND.size() ; i++)
    {

        // std::vector<Tcondition item_1D>
        Tcondition_item_2D tmp ;
        tmp.must_be_valid = desc.item_2D_AND[i].must_be_valid;

        // X variable ----------------
        // finding in the map such a Tnamed_pointer
        Tmap_of_named_pointers::iterator pos =
            named_pointer.find(desc.item_2D_AND[i].var_x_name);

        if (pos == named_pointer.end ())   // if not found
        {
            // look between deprecated names
            pos = Tuser_spectrum::perhaps_deprecated_name_of_incrementer(named_pointer,
                    desc.item_2D_AND[i].var_x_name);
        }

        if (pos != named_pointer.end ())
        {
            tmp.x_named_ptr = pos->second ;
        }
        else
        {   // Key NOT found
            std::cout
                << "\nERROR: In the definition of user condition: "
                << desc.give_name()
                << "  there is a name of the incrementer (variable):\n"
                << desc.item_2D_AND[i].var_x_name
                << "\nwhich is not known to the spy.\n"
                "Please go to the grebware viewer now, open the definition of this condition\n"
                "and choose one of the available incrementers"
                << std::endl;

            exit(1);
        }

        // Y variable ----------------
        pos = named_pointer.find(desc.item_2D_AND[i].var_y_name);

        if (pos == named_pointer.end ())   // if not found
        {
            // look between deprecated names
            pos = Tuser_spectrum::perhaps_deprecated_name_of_incrementer(named_pointer,
                    desc.item_2D_AND[i].var_y_name);
        }

        if (pos != named_pointer.end ())
        {
            tmp.y_named_ptr = pos->second ;
        }
        else
        {   // Key NOT found
            std::cout
                << "\nERROR: In the definition of user condition: "
                << desc.give_name()
                << "  there is a name of the incrementer (variable):\n"
                << desc.item_2D_AND[i].var_y_name
                << "\nwhich is not known to the spy.\n"
                "Please go to the grebware viewer now, open the definition of this condition\n"
                "and choose one of the available incrementers"
                << std::endl;

            exit(1);
        }


        tmp.polygon_name = desc.item_2D_AND[i].polygon_name ;

        // reading the polygon
        if (!read_banana(tmp.polygon_name, &tmp.polygon))
        {
            std::cout << "During Reading-in the condition named "
                 << desc.give_name()
                 << "\n  Impossible to read polygon gate: " << tmp.polygon_name
                 << "\nMost probably it does not exist (anymore?)"
                 << std::endl;
            exit(1);

        };


        condition_item_2D_AND.push_back(tmp) ;
    }

    //============= 2D OR ===============================

    // loop over all 2D items of descrption
    for (unsigned i = 0; i < desc.item_2D_OR.size() ; i++)
    {

        // std::vector<Tcondition item_1D>
        Tcondition_item_2D tmp ;
        tmp.must_be_valid = desc.item_2D_OR[i].must_be_valid;

        // X variable ----------------
        // finding in the map such a Tnamed_pointer
        Tmap_of_named_pointers::iterator pos =
            named_pointer.find(desc.item_2D_OR[i].var_x_name);

        if (pos == named_pointer.end ())   // if not found
        {
            // look between deprecated names
            pos = Tuser_spectrum::perhaps_deprecated_name_of_incrementer(named_pointer,
                    desc.item_2D_OR[i].var_x_name);
        }

        if (pos != named_pointer.end ())
        {
            tmp.x_named_ptr = pos->second ;
        }
        else
        {   // Key NOT found
            std::cout
                << "\nERROR: In the definition of user condition: "
                << desc.give_name()
                << "  there is a name of the incrementer (variable):\n"
                << desc.item_2D_OR[i].var_x_name
                << "\nwhich is not known to the spy.\n"
                "Please go to the grebware viewer now, open the definition of this condition\n"
                "and choose one of the available incrementers"
                << std::endl;

            exit(1);
        }

        // Y variable ----------------
        pos = named_pointer.find(desc.item_2D_OR[i].var_y_name);

        if (pos == named_pointer.end ())   // if not found
        {
            // look between deprecated names
            pos = Tuser_spectrum::perhaps_deprecated_name_of_incrementer(named_pointer,
                    desc.item_2D_OR[i].var_y_name);
        }

        if (pos != named_pointer.end ())
        {
            tmp.y_named_ptr = pos->second ;
        }
        else
        {   // Key NOT found
            std::cout
                << "\nERROR: In the definition of user condition: "
                << desc.give_name()
                << "  there is a name of the incrementer (variable):\n"
                << desc.item_2D_OR[i].var_y_name
                << "\nwhich is not known to the spy.\n"
                "Please go to the grebware viewer now, open the definition of this condition\n"
                "and choose one of the available incrementers"
                << std::endl;

            exit(1);
        }


        tmp.polygon_name = desc.item_2D_OR[i].polygon_name ;

        // reading the polygon
        if (!read_banana(tmp.polygon_name, &tmp.polygon))
        {
            std::cout << "During Reading-in the condition named "
                 << desc.give_name()
                 << "\n  Impossible to read polygon gate: "
                 << tmp.polygon_name
                 << "\nMost probably it does not exist (anymore?)"
                 << std::endl;
            exit(1);

        };


        condition_item_2D_OR.push_back(tmp) ;
    }

    // read in the nodes =======================================
    read_in_AND_node();
    read_in_OR_node();
    read_in_NAND_node();
    read_in_NOR_node();



}
//*****************************************************************
/** Checking is done before working with any (all) user spectrum */
void Tuser_condition::calculate_value_of_condition()
{
    if (already_calculated)
    {
        return ;
    }

    // for statistics
    how_many_tested++;


    result = true ;

//    std::cout << "Checking the condition " << desc.give_name()
//              << std::endl;

    if (!check_1D_AND())
    {
        result = false ;
        already_calculated = true ;
        return ;
    }

    if (!check_1D_OR())
    {
        result = false ;
        already_calculated = true ;
        return ;
    }

    if (!check_2D_AND())
    {
        result = false ;
        already_calculated = true ;
        return ;
    }

    if (!check_2D_OR())
    {
        result = false ;
        already_calculated = true ;
        return ;
    }


    if (!check_nodes_AND())
    {
        result = false ;
        already_calculated = true ;
        return ;
    }

    if (!check_nodes_OR())
    {
        result = false ;
        already_calculated = true ;
        return ;
    }

    if (!check_nodes_NAND())
    {
        result = false ;
        already_calculated = true ;
        return ;
    }

    if (!check_nodes_NOR())
    {
        result = false ;
        already_calculated = true ;
        return ;
    }


    how_many_true++ ;
    already_calculated = true ;

}
//***************************************************************************
/** No descriptions */
bool Tuser_condition::check_1D_AND()
{
    if (!condition_item_1D_AND.size()) return true ;

    int flag_something_was_true = 0 ;

    // ============ 1D gates  AND =================
    for (unsigned row = 0 ; row < condition_item_1D_AND.size() ; row++)
    {
        switch (condition_item_1D_AND[row].check_item_AND())
        {
        case 0:
            return false ;

        case 1:
            flag_something_was_true ++;
            break ;

        case -1:    
            // not fired, but was not obligatory, so just ignore it
            break;
        }
    }
    return flag_something_was_true ? true :false  ; // here we are when all were in gates
}

//***************************************************************************
bool Tuser_condition::check_1D_OR()
{

    if (!condition_item_1D_OR.size()) return true ;

    //    int flag_something_was_true = 0 ;

    // ============ 1D gates  OR =================
    for (unsigned row = 0 ; row < condition_item_1D_OR.size() ; row++)
    {
        switch (condition_item_1D_OR[row].check_item_OR())
        {
        case 0:
            break ;

        case 1:
            return true ;

        case -1:
            // not fired, and was not obligatory - so just continue checking
            break;
        }
    }
    return false  ; // here we are when nothing was true

}
//**************************************************************************

/*****************************************************************************/
bool Tuser_condition::check_2D_AND()
{
    // ============ 2D gates =================
    if (!condition_item_2D_AND.size()) return true ;

    for (unsigned row = 0 ; row < condition_item_2D_AND.size() ; row++)
    {

        double x_value = 0 ;


        if (

            (condition_item_2D_AND[row].x_named_ptr.data_usable != 0
             &&
             *condition_item_2D_AND[row].x_named_ptr.data_usable == false)

            ||

            (condition_item_2D_AND[row].y_named_ptr.data_usable != 0
             &&
             *condition_item_2D_AND[row].y_named_ptr.data_usable == false)
        )
        {

            if (condition_item_2D_AND[row].must_be_valid)
            {
                return (result = false);
            }
            else
            {
                continue ;
            }
        }




        switch (condition_item_2D_AND[row].x_named_ptr.what_type )
        {
        case Tnamed_pointer::is_int:
            x_value = *(condition_item_2D_AND[row].x_named_ptr.int_ptr) ;
            break;

        case Tnamed_pointer::is_double:
            x_value = *(condition_item_2D_AND[row].x_named_ptr.double_ptr) ;
            break;

        case Tnamed_pointer::is_bool:
            x_value = *(condition_item_2D_AND[row].x_named_ptr.bool_ptr) ;
            break;


        }


        double y_value = 0 ;

        switch (condition_item_2D_AND[row].y_named_ptr.what_type )
        {
        case Tnamed_pointer::is_int:
            y_value = *(condition_item_2D_AND[row].y_named_ptr.int_ptr) ;
            break;

        case Tnamed_pointer::is_double:
            y_value = *(condition_item_2D_AND[row].y_named_ptr.double_ptr) ;
            break;

        case Tnamed_pointer::is_bool:
            y_value = *(condition_item_2D_AND[row].y_named_ptr.bool_ptr) ;
            break;

        }


        // std::cout << "Point " << x_value << ", " << y_value << std::endl ;  // testjur
        // condition_item_2D_AND[row].polygon->PrintCondition(true);  // testjur

        if (condition_item_2D_AND[row].polygon->Test(x_value, y_value) == false)
        {
            // std::cout << " is outside " << std::endl;;
            return (result = false);
        }

    }
    return true ;// all were in gates
}
//***************************************************************************

//***************************************************************************
bool Tuser_condition::check_2D_OR()
{

    // ============ 2D gates =================
    if (!condition_item_2D_OR.size()) return true ;// no condition, so true

    for (unsigned row = 0 ; row < condition_item_2D_OR.size() ; row++)
    {

        double x_value = 0;

        if (

            //        condition_item_2D_OR[row].x_named_ptr.data_usable == false)
            //            ||
            //                (condition_item_2D_OR[row].y_named_ptr.data_usable == false))

            (condition_item_2D_OR[row].x_named_ptr.data_usable != 0
             &&
             *condition_item_2D_OR[row].x_named_ptr.data_usable == false)

            ||

            (condition_item_2D_OR[row].y_named_ptr.data_usable != 0
             &&
             *condition_item_2D_OR[row].y_named_ptr.data_usable == false)
        )


        {

            // OR gives no obligatory requirement
            //            if(condition_item_2D_OR[row].must_be_valid)
            //            {
            //              return (result = false);
            //            }
            //            else
            {
                continue ;
            }
        }




        switch (condition_item_2D_OR[row].x_named_ptr.what_type )
        {
        case Tnamed_pointer::is_int:
            x_value = *(condition_item_2D_OR[row].x_named_ptr.int_ptr) ;
            break;

        case Tnamed_pointer::is_double:
            x_value = *(condition_item_2D_OR[row].x_named_ptr.double_ptr) ;
            break;

        case Tnamed_pointer::is_bool:
            x_value = *(condition_item_2D_OR[row].x_named_ptr.bool_ptr) ;
            break;


        }


        double y_value =0 ;

        switch (condition_item_2D_OR[row].y_named_ptr.what_type )
        {
        case Tnamed_pointer::is_int:
            y_value = *(condition_item_2D_OR[row].y_named_ptr.int_ptr) ;
            break;

        case Tnamed_pointer::is_double:
            y_value = *(condition_item_2D_OR[row].y_named_ptr.double_ptr) ;
            break;

        case Tnamed_pointer::is_bool:
            y_value = *(condition_item_2D_OR[row].y_named_ptr.bool_ptr) ;
            break;

        }


        //     std::cout << "Point " << x_value << ", " << y_value << std::endl ;
        //     condition_item_2D_OR[row].polygon->PrintCondition(true);

        if (condition_item_2D_OR[row].polygon->Test(x_value, y_value))
        {
            //       std::cout << " is inside " << std::endl;;
            result = true ;
            return true ;
        }
    } // end of for

    // here we are when none of alternative was true, so
    result = false ;
    return false ;

}
//************************************************************
//************************************************************
/** No descriptions */
bool Tuser_condition::check_nodes_AND()
{
    if (!other_condition_AND_ptr.size()) return true ;

    int flag_something_was_true = 0 ;

    // ============ 1D gates  AND =================
    for (unsigned row = 0 ; row < other_condition_AND_ptr.size() ; row++)
    {
        switch (other_condition_AND_ptr[row]->give_result())
        {
        case 0:
            return false ;

        case 1:
            flag_something_was_true ++;
            break ;


// note, the result is bool, so it can not be -1   !!!
//     case -1:       // not fired
//       break;
        }
    }
    return flag_something_was_true ? true :false  ; // here we are when all were in gates
}
/***************************************************************/
bool Tuser_condition::check_nodes_OR()
{
    if (!other_condition_OR_ptr.size()) return true ;

    //int flag_something_was_true = 0 ;

    for (unsigned row = 0 ; row < other_condition_OR_ptr.size() ; row++)
    {
        switch (other_condition_OR_ptr[row]->give_result())
        {
        case 0:  // keep on testing
            break ;

        case 1:  // was true, OK
            return true ;

// note, the result is bool, so it can not be -1   !!!
//     case -1:
//       // not fired , continue checking,
//       break;
        }
    }
    return false  ; // here we are when nothing was true

}
/***************************************************************/
bool Tuser_condition::check_nodes_NAND()
{
    if (!other_condition_NAND_ptr.size()) return true ;

    int flag_something_was_true = 0 ;

    // ============ Nodes NAND =================
    for (unsigned row = 0 ; row < other_condition_NAND_ptr.size() ; row++)
    {
        switch (other_condition_NAND_ptr[row]->give_result())
        {
        case 0:
            return true ; // something was false, so NAND = true ;

        case 1:
            flag_something_was_true ++;
            break ;

// note, the result is bool, so it can not be -1   !!!
//     case -1:
//       // not fired
//       break;
        }
    }
    return flag_something_was_true ? false : true  ; // here we are when all were in gates
}
/****************************************************************/
bool Tuser_condition::check_nodes_NOR()
{
    if (!other_condition_NOR_ptr.size()) return true ;

    //int flag_something_was_true = 0 ;

    for (unsigned row = 0 ; row < other_condition_NOR_ptr.size() ; row++)
    {
        switch (other_condition_NOR_ptr[row]->give_result())
        {
        case 0:  // keep on testing
            break ;

        case 1:  // was true, OK
            return false ; // one was true, so NOR = false ;

//     case -1:   // note, the result is bool, so it can not be -1   !!!
//       // not fired , continue checking
//       break;
        }
    }
    return true  ; // here we are when nothing was true, so NOR = true
}
/****************************************************************/
void Tuser_condition::read_in_AND_node()
{


    // loop over all 1D items of descrption
    for (unsigned i = 0; i < desc.other_condition_AND.size() ; i++)
    {
        //std::cout <<  "AND Node called " << desc.other_condition_AND[i] << std::endl;

        // trying to find such a cond on the list
        // cut the extension
        std::string nname = desc.other_condition_AND[i] ;
        std::string::size_type pos = nname.rfind(".cnd");
        if (pos != std::string::npos)
        {
            nname.erase(pos);
        }

        Tuser_condition* cond = owner->give_ptr_to_cond(nname);
        if (!cond)
        {
            std::cout << "Error: Condition "
                 << nname
                 << " (which is used on the 'node list' of the conditon "
                 << give_name()
                 << ") does not exist on the disk "
                 << std::endl;
            exit(1) ;
        }
        other_condition_AND_ptr.push_back(cond);
    }

}
//*********************************************************************
/****************************************************************/
void Tuser_condition::read_in_OR_node()
{

    // loop over all 1D items of descrption
    for (unsigned i = 0; i < desc.other_condition_OR.size() ; i++)
    {
        //std::cout <<  "OR Node called " << desc.other_condition_OR[i] << std::endl;

        // trying to find such a cond on the list
        // cut the extension
        std::string nname = desc.other_condition_OR[i] ;
        std::string::size_type pos = nname.rfind(".cnd");
        if (pos != std::string::npos)
        {
            nname.erase(pos);
        }

        Tuser_condition* cond = owner->give_ptr_to_cond(nname);
        if (!cond)
        {
            std::cout << "Error: Condition "
                 << nname
                 << " (which is used on the 'node list' of the conditon "
                 << give_name()
                 << ") does not exist on the disk "
                 << std::endl;
            exit(1) ;
        }
        other_condition_OR_ptr.push_back(cond);
    }
}
/****************************************************************/
void Tuser_condition::read_in_NAND_node()
{
    // loop over all 1D items of descrption
    for (unsigned i = 0; i < desc.other_condition_NAND.size() ; i++)
    {
        //std::cout <<  "NAND Node called " << desc.other_condition_NAND[i] << std::endl;

        // trying to find such a cond on the list
        // cut the extension
        std::string nname = desc.other_condition_NAND[i] ;
        std::string::size_type pos = nname.rfind(".cnd");
        if (pos != std::string::npos)
        {
            nname.erase(pos);
        }

        Tuser_condition* cond = owner->give_ptr_to_cond(nname);
        if (!cond)
        {
            std::cout << "Error: Condition "
                 << nname
                 << " (which is used on the 'node list' of the conditon "
                 << give_name()
                 << ") does not exist on the disk "
                 << std::endl;
            exit(1) ;
        }
        other_condition_NAND_ptr.push_back(cond);
    }
}
/****************************************************************/
void Tuser_condition::read_in_NOR_node()
{
    // loop over all 1D items of descrption
    for (unsigned i = 0; i < desc.other_condition_NOR.size() ; i++)
    {
        //std::cout <<  "NOR Node called " << desc.other_condition_NOR[i] << std::endl;

        // trying to find such a cond on the list
        // cut the extension
        std::string nname = desc.other_condition_NOR[i] ;
        std::string::size_type pos = nname.rfind(".cnd");
        if (pos != std::string::npos)
        {
            nname.erase(pos);
        }

        Tuser_condition* cond = owner->give_ptr_to_cond(nname);
        if (!cond)
        {
            std::cout << "Error: Condition "
                 << nname
                 << " (which is used on the 'node list' of the conditon "
                 << give_name()
                 << ") does not exist on the disk "
                 << std::endl;
            exit(1) ;
        }
        other_condition_NOR_ptr.push_back(cond);
    }

}
//********************************************************************

