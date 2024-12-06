/***************************************************************************
                          Tuser_spectrum.cpp  -  description
                             -------------------
    begin                : Wed Sep 3 2003
    copyright            : (C) 2003 by dr Jerzy Grebosz IFJ Krakow
    email                :
***************************************************************************/
#include "Tuser_spectrum.h"
#include "Tuser_condition.h"

#include "spectrum.h"
#include "Tnamed_pointer.h"

#include "Tself_gate_ger_descr.h"
#include "Tself_gate_hec_descr.h"
// #include "Tself_gate_mib_descr.h"
#include "Tself_gate_ger_addback_descr.h"
#include "Tself_gate_agata_psa_descr.h"
#include "Tself_gate_ge_galileo_descr.h"


#include "Tincrementer_donnor.h"

//user_spectrum_descripion text_desc;
//*************************************************************************
Tuser_spectrum::Tuser_spectrum()
{
    cond_ptr =0;
    flag_mutative_named_pointers_are_used = false;
}
//*************************************************************************
//*************************************************************************
Tuser_spectrum::~Tuser_spectrum()
{
    delete spec_ptr;
    std::map<std::string, Tself_gate_abstract_descr* >::iterator it;

    for ( it=map_of_selfgates.begin(); it != map_of_selfgates.end(); it++ )
    {
        //std::cout << ( *it ).first << " => " << ( *it ).second << std::endl;
        delete ( *it ).second; //deleting the selfgate pointed by this pointer
    }
    map_of_selfgates.clear();

}
//*************************************************************************
/** Read the definition prepared in the text file (for example by grebware) */
void Tuser_spectrum::read_in_parameters ( std::string s )
{
    desc.set_name ( s );
    desc.read_from ( path.dir_user_def_spectra() + s + ".user_definition" );

    // here we may fill the array of incrementers looking on the map
    set_incrementers();
}

/********************************************************************/
void Tuser_spectrum::create_the_spectrum()
{
    std::string folder = "user/" ;
    std::string sname = desc.get_name();

    if ( desc.give_dimmension() == user_spectrum_description::spec_1D )
    {
        spec_ptr = new spectrum_1D (
            sname,
            sname,
            desc.give_bins_x(), desc.give_beg_x(), desc.give_end_x(),
            folder, "",
            "To check incrementers - go to the User Defined Spectra manager" );
    }
    else
    {
        spec_ptr = new spectrum_2D ( sname,
                                     sname,
                                     desc.give_bins_x(), desc.give_beg_x(), desc.give_end_x(),
                                     desc.give_bins_y(), desc.give_beg_y(), desc.give_end_y(),
                                     folder, "",
                                     "To check incrementers - go to the User Defined Spectra manager" );
    }
    // owner->remember_user_spectrum(spec_ptr);
    // NO, the funcion which calls this funciton will take care
    // of registering it for saving, updating
    // it is so, because now I still do not know if it should
    // be registered in one, or in two vectors

}
//***********************************************************
user_spectrum_description  Tuser_spectrum::give_description()
{
    return desc;
}
//***********************************************************
std::string Tuser_spectrum::give_name()
{
    return desc.get_name();
}
//***********************************************************
/** converting the std::string names into addresses of data */
void Tuser_spectrum::set_incrementers()
{
    x_incr_addresses.clear();
    y_incr_addresses.clear();
    clear_selfgates_map();

    // X ---------------
//     std::cout << " F. set_incrementers() "
//          << ", X incremetners ======================" << std::endl;


    // std::vector<std::string> & x_incr = desc.give_x_incr();
    std::vector< std::pair<std::string,std::string> > & x_incr = desc.give_x_incr();
    for ( unsigned i = 0; i < x_incr.size(); i++ )
    {
        //std::cout << x_incr[i] << std::endl;
        // find this std::string in the table
        Tmap_of_named_pointers::iterator pos = named_pointer.find ( x_incr[i].first );

        //#ifdef NIGDY
        if ( pos == named_pointer.end () ) // if not found
        {
            // look between deprecated names
            pos = perhaps_deprecated_name_of_incrementer ( named_pointer, x_incr[i].first );
        }
        //#endif

        if ( pos != named_pointer.end () )
        {
            // Key found
//             std::cout << "Found std::string " << (pos->first) << std::endl;
//             std::cout << pos->first << " odpowiada_adres "
//                  << ((int) &(pos->second)) << std::endl;


            // try to find the self gate, if necessery

            x_incr_addresses.push_back ( Tincr_entry ( pos->second,   // Tnamed pointer
                                        address_of_self_gate ( x_incr[i].second ) ) );      //  pointer to the descriptor

            int last = x_incr_addresses.size() - 1;
            if ( x_incr_addresses[last].nam_ptr.mutative == true )
            {
                flag_mutative_named_pointers_are_used = true;
            }

            Tnamed_pointer ttt = pos->second;
//             std::cout << "Vek ma dlugosc " << ttt.vek.size();
//       std::cout << " Oto adresy " << std::endl;
//       for(unsigned k = 0; k < ttt.vek.size(); k++)
//       {
//         std::cout << "k= " << k << ") " << ttt.vek[k].v_double_ptr
//          << std::endl;
//       }

        }
        else
        {
            if ( !flag_mutative_named_pointers_are_used ) // if it is a mutative, but in this particular event does not exist, ignore it
            {
                // Key NOT found
                std::cout << "\nERROR: In the definition of user spectrum: "
                     << give_name() << " there is a name of the incrementer (variable):\n"
                     << x_incr[i].first
                     << "\nwhich is not known to the spy.\n"
                     "Please go to the grebware viewer now, open the definition of this spectrum\n"
                     "and choose one of the available incrementers"
                     << std::endl;

                exit ( 1 );
            }
        }
    }


    // Y --------------
//     std::cout << "Y incremetners ======================" << std::endl;
    if ( desc.give_dimmension() == user_spectrum_description::spec_2D )
    {
        // Y
        std::vector< std::pair<std::string,std::string> > & y_incr = desc.give_y_incr();
        for ( unsigned i = 0; i < y_incr.size(); i++ )
        {
            //std::cout << y_incr[i] << std::endl;
            // find this std::string in the table
            Tmap_of_named_pointers::iterator pos = named_pointer.find ( y_incr[i].first );

            if ( pos == named_pointer.end () ) // if not found
            {
                // look between deprecated names
                pos = perhaps_deprecated_name_of_incrementer ( named_pointer, y_incr[i].first );
            }


            if ( pos != named_pointer.end () )
            {
                // Key found
//                 std::cout << "Found std::string " << (pos->first) << std::endl;
//                 std::cout << pos->first << " odpowiada_adres "
//                      << ((int) &(pos->second)) << std::endl;

                y_incr_addresses.push_back ( Tincr_entry ( pos->second,   // Tnamed pointer
                                            address_of_self_gate ( y_incr[i].second ) ) );      //  pointer to the descriptor

                int last = y_incr_addresses.size() - 1;
                if ( y_incr_addresses[last].nam_ptr.mutative == true )
                {
                    flag_mutative_named_pointers_are_used = true;
                }


            }
            else
            {

                if ( !flag_mutative_named_pointers_are_used )
                {
                    // Key NOT found
                    std::cout << "\nERROR: In the definition of user spectrum " << give_name()
                         << " there is a name of the incrementer (variable):\n"
                         << y_incr[i].first
                         << "\nwhich is not known to the spy.\n"
                         "Please go to the grebware viewer now, open the definition of this spectrum\n"
                         "and choose one of the available incrementers"
                         << std::endl;

                    exit ( 1 );
                }
            }
        }

    }

}
/**********************************************************************/
void Tuser_spectrum::make_incrementations()
{
//     std::cout << "Tuser_spectrum::make_incrementations() ---  Spectrum " << desc.get_name() << "  checking" << std::endl;

    if ( flag_mutative_named_pointers_are_used )
    {
//         std::cout << "This spectum has mutative incrementers, so we reread them" << std::endl;
        static int counter = 0;
// 		if ( ! ( ( counter++ ) % 50000 ) )
// 		{
// 			std::cout << "Note: Spectrum called: " << desc.get_name()
// 			<< "\nis using at least one incrementer "
// 			" dynamically created. This makes the analysis slower " << std::endl;
// 		}
        set_incrementers();
    }

//      if(cond_result_ptr)
//      {
//        std::cout << "Condition was assigned" << std::endl;
//        if(*cond_result_ptr == true)
//            std::cout << "Condition was TRUE " << std::endl;
//        else
//            std::cout << "Condition was FALSE " << std::endl;
//       }

    if (
        ( cond_result_ptr && ( *cond_result_ptr == false ) ) // condition for this spectrum is false
        ||

        !desc.spectrum_enabled() )  // or matrix is disabled
    {
        return;
    }

    // if no condition == condition always true

    if ( desc.give_dimmension() == user_spectrum_description::spec_1D )
    {
        make_incr_of_1D_spectrum();
    }
    else          // 2D spectra =============================
    {
        make_incr_of_2D_spectrum();
    }
}
//*******************************************************************
void Tuser_spectrum::save_to_disk()
{
    spec_ptr->save_to_disk();
}
//*******************************************************************
void Tuser_spectrum::remember_address_of_condition ( Tuser_condition *t )
{
    cond_ptr = t;
    if ( cond_ptr )
    {
        cond_result_ptr = &cond_ptr->result;
    }
    else
    {
        cond_result_ptr = 0;
    }
}
//******************************************************************
/** spectrum is 1D, so only X will be incremeted */
void Tuser_spectrum::make_incr_of_1D_spectrum()
{
// 	 std::cout
// 	   << "make_incr_of_1D_spectrum() loop on x_incr_addresses.size() = "
// 	   << x_incr_addresses.size() << std::endl;

    for ( unsigned i = 0; i < x_incr_addresses.size(); i++ )
    {
        // in case of single entry we can say at once if the signal is not valid
        // (in case of multi entry we have to check all of entries

        if ( x_incr_addresses[i].nam_ptr.flag_multi_entry == false )
        {

            if ( x_incr_addresses[i].nam_ptr.data_usable != 0
                    &&
                    * ( x_incr_addresses[i].nam_ptr.data_usable ) == false )
            {
                continue; // this is blocked (because for example not fired)
            }

            // here we may check the self_gate of this incrmentor

            if ( x_incr_addresses[i].self_gate_descr_ptr != NULL) // if selfgate was set
            {
                if ( x_incr_addresses[i].self_gate_descr_ptr->not_for_1Dspectrum() )
                {
                    std::cout << "NOTE: The self gate called "
                         //<< x_incr_addresses[i].self_gate_descr_ptr->name
                         << "  has settings option 'time difference between 2 gammas' as  ON "
                         "which is  nonsense in case of 1D spectrum: " << give_name()
                         << std::endl;
                }
                if ( !x_incr_addresses[i].nam_ptr.ptr_detector->
                        check_selfgate ( x_incr_addresses[i].self_gate_descr_ptr ) )
                {
                    continue; // selfgate was false;
                }
            }


            //    int mmm = x_incr_addresses[i].nam_ptr.what_type;
            //   std::cout << "================ Incremet USER with  value  "
            //        <<(*(x_incr_addresses[i].nam_ptr.double_ptr))
            //        <<std::endl;

            switch ( x_incr_addresses[i].nam_ptr.what_type )
            {
            case Tnamed_pointer::is_double:
                spec_ptr->increment ( * ( x_incr_addresses[i].nam_ptr.double_ptr ) );
                break;

            case   Tnamed_pointer::is_int:
                // std::cout << "Incremet USER with value " << *(x_incr_addresses[i].nam_ptr.int_ptr) << std::endl;
                spec_ptr->increment ( * ( x_incr_addresses[i].nam_ptr.int_ptr ) );
                break;

            case   Tnamed_pointer::is_bool:
                spec_ptr->increment ( * ( x_incr_addresses[i].nam_ptr.bool_ptr ) );
                break;


            } // switch

        } //
        else    // this is multi
        {
// 			std::cout << "so many multi " <<  x_incr_addresses[i].nam_ptr.vek.size() << std::endl;

            for ( unsigned m = 0; m < x_incr_addresses[i].nam_ptr.vek.size(); m++ )
            {
                if ( x_incr_addresses[i].nam_ptr.vek[m].v_data_usable != 0  // when defined
                        &&
                        * ( x_incr_addresses[i].nam_ptr.vek[m].v_data_usable ) == false )   // and  not valid
                {
                    continue;
                }  // these were the condition to no


                // here we may check the self_gate of this incrmentor ----------

                if ( x_incr_addresses[i].self_gate_descr_ptr ) // if selfgate was set
                {
                    if ( !x_incr_addresses[i].nam_ptr.vek[m].v_ptr_detector->
                            check_selfgate ( x_incr_addresses[i].self_gate_descr_ptr ) )
                    {
                        continue; // selfgate was false;
                    }
                }

                // -------- lets increment

                double value = 0;
                switch ( x_incr_addresses[i].nam_ptr.vek[m].v_what_type )
                {
                case    Tnamed_pointer::is_double:
                    value = ( * ( x_incr_addresses[i].nam_ptr.vek[m].v_double_ptr ) );
                    break;
                case    Tnamed_pointer::is_int:
                    value = ( * ( x_incr_addresses[i].nam_ptr.vek[m].v_int_ptr ) );
                    break;
                case    Tnamed_pointer::is_bool:
                    value = ( * ( x_incr_addresses[i].nam_ptr.vek[m].v_bool_ptr ) );
                    break;
                }
// 				           std::cout << "USER multi incr with " << value
// 				             << "taken from address "
// 				             << (x_incr_addresses[i].nam_ptr.vek[m].v_double_ptr)
// 				             << std::endl;
                spec_ptr->increment ( value );

            }
            // ????? break;
        }
    } // for incrementers

}
//*******************************************************************
/** Has X and Y incrementers */
void Tuser_spectrum::make_incr_of_2D_spectrum()
{
//     std::cout << "Tuser_spectrum::make_incr_of_2D_spectrum() "
//          << ", there are " << x_incr_addresses.size() << " incrementers "
//          << std::endl;

    for ( unsigned x = 0; x < x_incr_addresses.size(); x++ )
    {
        //     if(x_incr_addresses[x].nam_ptr.mutative)
        //     {
        //       std::cout << "Rereading the mutative incrementer" << std::endl;
        //       set_incrementers();
        //     }

        double x_value = 0;

        //     std::cout << "X incremeter  is multi ? "
        //     << x_incr_addresses[x].nam_ptr.flag_multi_entry << std::endl;
        //     std::cout << "so many multi = " <<  x_incr_addresses[x].nam_ptr.vek.size() << std::endl;

        if ( x_incr_addresses[x].nam_ptr.flag_multi_entry == false )
        {

            if ( x_incr_addresses[x].nam_ptr.data_usable != 0
                    &&
                    * ( x_incr_addresses[x].nam_ptr.data_usable ) == false )
            {
                continue; // this is blocked (because for example not fired)
            }


            //-----------------------------------
            // here we may check the self_gate of this incrmentor

            if ( x_incr_addresses[x].self_gate_descr_ptr ) // if selfgate was set
            {
                x_incr_addresses[x].self_gate_descr_ptr-> this_is_x_incrementer();
                if ( !x_incr_addresses[x].nam_ptr.ptr_detector->
                        check_selfgate ( x_incr_addresses[x].self_gate_descr_ptr ) )
                {
                    continue; // selfgate was false;
                }
            }
            //------------------------------


            //    int mmm = x_incr_addresses[x].nam_ptr.what_type;
            //std::cout << "Incremet USER with what value ? " <<std::endl;
            switch ( x_incr_addresses[x].nam_ptr.what_type )
            {
            case Tnamed_pointer::is_double:
                x_value = * ( x_incr_addresses[x].nam_ptr.double_ptr );
                break;

            case Tnamed_pointer::is_int:
                //std::cout << "Incremet USER with value "
                //   << *(x_incr_addresses[x].nam_ptr.int_ptr) << std::endl;
                x_value = * ( x_incr_addresses[x].nam_ptr.int_ptr );
                break;

            case Tnamed_pointer::is_bool:
                x_value = * ( x_incr_addresses[x].nam_ptr.bool_ptr );
                break;

            } // switch

            // second argument is the addres of the detecors whic delvered the data
            find_y_to_increment_with ( x_value, ( x_incr_addresses[x].nam_ptr.ptr_detector ) );
        } //
        else    // this is multi
        {
//             std::cout << "so many multi = " <<  x_incr_addresses[x].nam_ptr.vek.size() << std::endl;

            for ( unsigned m = 0; m < x_incr_addresses[x].nam_ptr.vek.size(); m++ )
            {
//                 if (x_incr_addresses[x].nam_ptr.vek.size() > 8000)
//                 {
//                     std::cout << " m = " << m
//                     << " ptr x_incr_addresses[x].nam_ptr.vek[m].v_double_ptr = "
//                     <<  (int)(x_incr_addresses[x].nam_ptr.vek[m].v_double_ptr)
//                     << std::endl;
//                 }
                if ( x_incr_addresses[x].nam_ptr.vek[m].v_data_usable != 0 )
                {
                    if (
                        * ( x_incr_addresses[x].nam_ptr.vek[m].v_data_usable ) == false    // when not valid
                    )   // and defined
                    {
                        continue;
                    }  // these were the condition to no
                }

                // here we may check the self_gate of this incrmentor ----------

                if ( x_incr_addresses[x].self_gate_descr_ptr ) // if selfgate was set
                {
                    x_incr_addresses[x].self_gate_descr_ptr-> this_is_x_incrementer();

// 					std::cout << "Before checking the selfgate for " << give_name()
// 					<< ", addr = " << std::hex << (int)(x_incr_addresses[x].nam_ptr.vek[m].v_ptr_detector) << std::dec
// 					<< std::endl;
//
// 					std::cout << " while x_incr_addresses[x].self_gate_descr_ptr  = " << std::hex << x_incr_addresses[x].self_gate_descr_ptr  << std::dec
// 					<< std::endl;
//
                    if ( !x_incr_addresses[x].nam_ptr.vek[m].v_ptr_detector->
                            check_selfgate ( x_incr_addresses[x].self_gate_descr_ptr ) )
                    {
// 					  std::cout << "After1 " << give_name() << std::endl;
                        continue; // selfgate was false;
                    }
// 					  std::cout << "After2 " << give_name() << std::endl;
                }

                // -------- lets increment



                switch ( x_incr_addresses[x].nam_ptr.vek[m].v_what_type )
                {
                case    Tnamed_pointer::is_double:
                    x_value = ( * ( x_incr_addresses[x].nam_ptr.vek[m].v_double_ptr ) );
                    break;

                case    Tnamed_pointer::is_int:
                    x_value = ( * ( x_incr_addresses[x].nam_ptr.vek[m].v_int_ptr ) );
                    break;
                case    Tnamed_pointer::is_bool:
                    x_value = ( * ( x_incr_addresses[x].nam_ptr.vek[m].v_bool_ptr ) );
                    break;
                }
                //            std::cout << "USER multi incr with " << value
                //              << "taken from address "
                //              << (x_incr_addresses[x].nam_ptr.vek[m].v_double_ptr)
                //              << std::endl;

                // second argument is the addres of the detecors whic delvered the data
                find_y_to_increment_with ( x_value, ( x_incr_addresses[x].nam_ptr.vek[m].v_ptr_detector ) );

            } // end for
        } // else multi
    } // end for increm


}
//******************************************************************

/** No descriptions */
void Tuser_spectrum::find_y_to_increment_with ( double x_value, void *det_address )
{
    double y_value = 0;

    for ( unsigned i = 0; i < y_incr_addresses.size(); i++ )
    {
        // in case of single entry we can say at once if the signal is not valid
        // (in case of multi entry we have to check all of entries

        if ( y_incr_addresses[i].nam_ptr.flag_multi_entry == false )
        {
            if ( y_incr_addresses[i].nam_ptr.data_usable != 0
                    &&
                    * ( y_incr_addresses[i].nam_ptr.data_usable ) == false )
            {
                continue; // this is blocked (because for example not fired)
            }

            //-----------------------------------
            // here we may check the self_gate of this incrmentor

            if ( y_incr_addresses[i].self_gate_descr_ptr ) // if selfgate was set
            {
                y_incr_addresses[i].self_gate_descr_ptr->this_is_y_incrementer();
                if ( !y_incr_addresses[i].nam_ptr.ptr_detector->
                        check_selfgate ( y_incr_addresses[i].self_gate_descr_ptr ) )
                {
                    continue; // selfgate was false;
                }
            }
            //------------------------------




            //    int mmm = y_incr_addresses[i].nam_ptr.what_type;
            //std::cout << "Incremet USER with what value ? " <<std::endl;
            switch ( y_incr_addresses[i].nam_ptr.what_type )
            {
            case Tnamed_pointer::is_double:
                y_value = * ( y_incr_addresses[i].nam_ptr.double_ptr );
                break;

            case   Tnamed_pointer::is_int:
                //std::cout << "Incremet USER with value " << *(y_incr_addresses[i].nam_ptr.int_ptr) << std::endl;
                y_value = * ( y_incr_addresses[i].nam_ptr.int_ptr );
                break;

            case   Tnamed_pointer::is_bool:
                y_value = * ( y_incr_addresses[i].nam_ptr.bool_ptr );
                break;

            } // switch

            // ========= now we can try to increment ============
            switch ( desc.give_policy() )
            {
            case 0:  // always
                ( ( spectrum_2D* ) ( spec_ptr ) )->increment ( x_value, y_value );
                break;

            case 1:  // only when different
                if ( det_address != ( y_incr_addresses[i].nam_ptr.ptr_detector ) )
                {
                    ( ( spectrum_2D* ) ( spec_ptr ) )->increment ( x_value, y_value );
                }
                break;

            case 2:  // only when the same
                if ( det_address == ( y_incr_addresses[i].nam_ptr.ptr_detector ) )
                {
                    ( ( spectrum_2D* ) ( spec_ptr ) )->increment ( x_value, y_value );
                }
                break;
            } // switch
        } //
        else    // this is multi
        {
            //std::cout << "so many multi " <<  y_incr_addresses[i].nam_ptr.vek.size() << std::endl;

            for ( unsigned m = 0; m < y_incr_addresses[i].nam_ptr.vek.size(); m++ )
            {
                if (
                    y_incr_addresses[i].nam_ptr.vek[m].v_data_usable != 0
                    &&
                    * ( y_incr_addresses[i].nam_ptr.vek[m].v_data_usable ) == false    // when not valid
                )   // and defined
                {
                    continue;
                }  // these were the condition to no


                // here we may check the self_gate of this incrmentor ----------

                if ( y_incr_addresses[i].self_gate_descr_ptr ) // if selfgate was set
                {
                    y_incr_addresses[i].self_gate_descr_ptr->this_is_y_incrementer();
                    if ( !y_incr_addresses[i].nam_ptr.vek[m].v_ptr_detector->
                            check_selfgate ( y_incr_addresses[i].self_gate_descr_ptr ) )
                    {
                        continue; // selfgate was false;
                    }
                }

                // -------- lets increment

                switch ( y_incr_addresses[i].nam_ptr.vek[m].v_what_type )
                {
                case    Tnamed_pointer::is_double:
                    y_value = ( * ( y_incr_addresses[i].nam_ptr.vek[m].v_double_ptr ) );
                    break;
                case    Tnamed_pointer::is_int:
                    y_value = ( * ( y_incr_addresses[i].nam_ptr.vek[m].v_int_ptr ) );
                    break;
                case    Tnamed_pointer::is_bool:
                    y_value = ( * ( y_incr_addresses[i].nam_ptr.vek[m].v_bool_ptr ) );
                    break;
                }
                //            std::cout << "USER multi incr with " << value
                //              << "taken from address "
                //              << (y_incr_addresses[i].nam_ptr.vek[m].v_double_ptr)
                //              << std::endl;



                //     // below we incremt only when these two addresses are different
                //     void *adresik =y_incr_addresses[i].nam_ptr.vek[m].v_ptr_detector;
                //
                //        if(x_value == y_value)
                //        {
                //            std::cout << "uwaga !, x_value="
                //            << x_value
                //            << ", y_value="<< y_value
                //            << ",   x_adr = "
                //            << address
                //            << ", y_adr = "
                //            << adresik << std::endl;
                //
                //        }


                // ========= now we can try to increment ============
                switch ( desc.give_policy() )
                {
                case 0:  // always
                    ( ( spectrum_2D* ) ( spec_ptr ) )->increment ( x_value, y_value );
                    break;

                case 1:  // only when different
                    if ( det_address !=  y_incr_addresses[i].nam_ptr.vek[m].v_ptr_detector )
                    {
                        ( ( spectrum_2D* ) ( spec_ptr ) )->increment ( x_value, y_value );
                    }
                    break;

                case 2:  // only when the same
                    if ( det_address ==  y_incr_addresses[i].nam_ptr.vek[m].v_ptr_detector )
                    {
                        ( ( spectrum_2D* ) ( spec_ptr ) )->increment ( x_value, y_value );
                    }
                    break;
                } // switch


            } // for

        } // else multi

    } // for incrementers

}
//*********************************************************************
/** This function finds in the special vector the selfgate description
If this description is on in the vector, reads it from the disk, stores
in the vector and gives the address */
Tself_gate_abstract_descr*  Tuser_spectrum::address_of_self_gate ( std::string sg_name )
{

    // finding in the map such a selfgate
// 	std::map<std::string, Tself_gate_abstract_descr* >::iterator pos =
    std::map<std::string, Tself_gate_abstract_descr*  >::iterator pos =
        map_of_selfgates.find ( sg_name );

    if ( pos != map_of_selfgates.end () )
    {
        return pos->second;
    }

    // not found, try to find it on the disk
    if ( sg_name.find ( ".self_gate_ger_crystal" ) != std::string::npos )
    {
        std::string pathed_sg_name = path.dir_conditions() + sg_name;

        Tself_gate_abstract_descr* d = new Tself_gate_ger_descr;
        d->read_definition_from ( pathed_sg_name );

        map_of_selfgates[sg_name] = d;

        return d;
    }

    if ( sg_name.find ( ".self_gate_hec_crystal" ) != std::string::npos )
    {
        std::string pathed_sg_name = path.dir_conditions() + sg_name;

        Tself_gate_abstract_descr* d = new Tself_gate_hec_descr;
        d->read_definition_from ( pathed_sg_name );

        map_of_selfgates[sg_name] = d;

        return d;
    }
    // else if..  other types of selfgates

// 	if ( sg_name.find ( ".self_gate_mib_crystal" ) != std::string::npos )
// 	{
// 		std::string pathed_sg_name = path.dir_conditions() +" + sg_name;
//
// 		Tself_gate_abstract_descr* d = new Tself_gate_mib_descr;
// 		d->read_definition_from ( pathed_sg_name );
//
// 		map_of_selfgates[sg_name] = d;
//
// 		return d;
// 	}
    // else if..  other types of selfgates
    if ( sg_name.find ( ".self_gate_ger_addback_cluster" ) != std::string::npos )
    {
        std::string pathed_sg_name = path.dir_conditions()  + sg_name;

        Tself_gate_abstract_descr* d = new Tself_gate_ger_addback_descr;
        d->read_definition_from ( pathed_sg_name );

        map_of_selfgates[sg_name] = d;

        return d;
    }
    // not found, try to find it on the disk
    if ( sg_name.find ( ".self_gate_agata_psa" ) != std::string::npos )
    {
        std::string pathed_sg_name = path.dir_conditions() + sg_name;
        Tself_gate_abstract_descr* d = new Tself_gate_agata_psa_descr;
        d->read_definition_from ( pathed_sg_name );

        map_of_selfgates[sg_name] = d;

        return d;
    }

       // not found, try to find it on the disk
    if ( sg_name.find ( ".self_gate_galileo_ge" ) != std::string::npos )
    {
        std::string pathed_sg_name = path.dir_conditions() + sg_name;

        Tself_gate_abstract_descr* d = new Tself_gate_ge_galileo_descr;
        d->read_definition_from ( pathed_sg_name );

        map_of_selfgates[sg_name] = d;

        return d;
    }
    
    
    
    
    
    //...
    std::string lowercase_name = sg_name;
    for ( unsigned i = 0; i < lowercase_name.size(); i++ )
    {
        lowercase_name[i] = tolower ( lowercase_name[i] );
    }
    if ( lowercase_name != "no_self_gate" )
    {
        std::cout << "User spectrum definiton "
             << desc.get_name()
             << " requires a self-gate named: " << sg_name
             << " which does not have proper extension "
             " *.self_gate_ger_crystal"
             << std::endl;

        exit ( 1 );
    }
    return 0;
}
/***************************************************************************/
void Tuser_spectrum::clear_selfgates_map()
{
    std::map<std::string, Tself_gate_abstract_descr* >::iterator it =
        map_of_selfgates.begin();

    for (; it != map_of_selfgates.end(); it++ )
    {
        delete it->second;
    }
    map_of_selfgates.clear();
}
//*******************************************************************
/** in case if the name of the incrementer was not found, perhaps this name is
on the list of the deprecated pointers, obsolate, deprecated */

Tmap_of_named_pointers::iterator
Tuser_spectrum::perhaps_deprecated_name_of_incrementer ( Tmap_of_named_pointers & np, std::string orgstr )
{
    //  std::cout << "Looking between the deprecated names... the name "
    //   << orgstr << std::endl;
    // modifying the string
    std::vector<std::string> stary;
    std::vector<std::string> nowy;

    stary.push_back ( "succesfull" );
    nowy.push_back ( "successful" );

    stary.push_back ( "cluster_crys" );
    nowy.push_back ( "cluster_" );

    stary.push_back ( "time_LR_diff_cal" );
    nowy.push_back ( "time_LR_cal" );

    stary.push_back ( "time_diff_calibrated" );
    nowy.push_back ( "time_cal" );

    stary.push_back ( "stopper_implantation_" );
    nowy.push_back ( "stopper_AllModules__implantation_" );

    stary.push_back ( "stopper2_implantation_" );
    nowy.push_back ( "stopper2_AllModules__implantation_" );

    stary.push_back ( "addback" );
    nowy.push_back ( "multi_addback" );

    stary.push_back ( "music" );  // to allow spectra working with old music (musicBig) and new, munich - just (music)
    nowy.push_back ( "musicBig" );

    stary.push_back ( "musicBig" );
    nowy.push_back ( "music" );

    stary.push_back ( "Adback" );
    nowy.push_back ( "Addback" );

    for ( unsigned int i = 0; i < stary.size(); i++ )
    {
        std::string str = orgstr;
        std::string::size_type pos  = str.find ( stary[i] );
        //std::cout << "Checking " << stary[i] << std::endl;
        if ( pos != std::string::npos )
        {
            //        std::cout << "Found!" << std::endl;
            // if old was found - replace it
            str.replace ( pos, stary[i].size(), nowy[i] );
            //       std::cout << "After replace str = >"<< str << "<" << std::endl;

            return   np.find ( str ); // and return the iterator to the new name
        }
        // if the old std::string not found, keep on searching

    }
    // not found at all - return the iterator to nowhere
    return   np.find ( orgstr );
}
