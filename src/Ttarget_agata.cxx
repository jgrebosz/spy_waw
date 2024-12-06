/***************************************************************************
                          ttarget_agata.cpp  -  description
                             -------------------
    begin                : Fri May 23 2003
    copyright            : (C) 2003 by dr Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/
#include "experiment_def.h"


#include "TGalileoAnalysis.h"
#include "Ttarget_agata.h"
#include "TNeedle.h"
#include "Tdante.h"
#include "Tcristal.h"
//#include "Thector_baf.h"   // for sending the doppler options
//#include "Tagata_elementary_det.h"   // for sending the doppler options

#include "Tnamed_pointer.h"
#include "doppler_enums.h"

#define dcout   if(0)std::cout

//************************************************************************
Ttarget_agata::Ttarget_agata(std::string name_of_target,
                             std::string name_dante
                            ) : TNeedle_element(name_of_target)
{

#ifdef DANTE_PRESENT
    dante_ptr  =
        dynamic_cast<Tdante*>(owner->address_of_device(name_dante));
#endif


    // for the incrementer called:
    // Projectile_energy_calcul_from_frs_beta ;
//   E_zero  = 0 ;
//   E_one  = 0 ;

    // if all was ok.

    create_my_spectra();


//   named_pointer[name_of_this_element + "__projectile_energy_calculated_from_beta"]
//   = Tnamed_pointer(&E_zero, 0, this) ;
//
//   named_pointer[name_of_this_element + "__fragment_energy_after_target"]
//   = Tnamed_pointer(&E_one, 0, this) ;

//   named_pointer[name_of_this_element + "__beta_after_target"]
//   = Tnamed_pointer(&beta_after_target, 0, this) ;



    named_pointer[name_of_this_element + "_angle_theta_radians_scattered_particle"]
    = Tnamed_pointer(&angle_theta_radians_scattered_particle, &flag_doppler_possible, this) ;

    named_pointer[name_of_this_element + "_angle_theta_degree_scattered_particle"]
    = Tnamed_pointer(&angle_theta_degree_scattered_particle, &flag_doppler_possible, this) ;

    named_pointer[name_of_this_element + "_angle_phi_(0_to_2PI)_radians_scattered_particle"]
    = Tnamed_pointer(&angle_phi_radians_scattered_particle, &flag_doppler_possible, this) ;

    named_pointer[name_of_this_element + "_angle_phi_(0_to_360)_degree_scattered_particle"]
    = Tnamed_pointer(&angle_phi_degree_scattered_particle, &flag_doppler_possible, this) ;


    //    named_pointer[name_of_this_element + "_angle_theta_radians_scattered_particle"]
    //    = Tnamed_pointer(&angle_theta_radians_scattered_particle, &flag_doppler_possible, this) ;
    //
    //   named_pointer[name_of_this_element + "_angle_phi_radians_scattered_particle"]
    //    = Tnamed_pointer(&angle_phi_radians_scattered_particle, &flag_doppler_possible, this) ;
    //

    //    named_pointer[name_of_this_element + "_x_extrapolated_to_any_when_ok"]
    //    = Tnamed_pointer(&x_on_any, &flag_xy_extrapolated_ok, this) ;
    //
    //    named_pointer[name_of_this_element + "_y_extrapolated_to_any_when_ok"]
    //    = Tnamed_pointer(&y_on_any, &flag_xy_extrapolated_ok, this) ;




}
//***************************************************************************
Ttarget_agata::~Ttarget_agata()
{}
//***************************************************************************
/** spectra of angles - theta, phi projectile/scattered_particle */
void Ttarget_agata::create_my_spectra()
{
    std::string folder =  std::string(name_of_this_element) + "_folder/" ;

    std::string name ;
    //  = name_of_this_element + "_cos_theta_projectile"  ;
    //  spec_theta_projectile = new spectrum_1D( name,
    //                                   name,
    //                                   201, -1.1, 1.1,
    //                                   folder );
    //  frs_spectra_ptr->push_back(spec_theta_projectile) ;

    //-------------------
//     name = name_of_this_element + "_angle_theta_radians_scattered_particle"  ;
//     spec_theta_scattered_particle = new spectrum_1D( name,
//             name,
//             2000, 0, 2,
//             folder );
//     frs_spectra_ptr->push_back(spec_theta_scattered_particle) ;


    //  name = name_of_this_element + "_angle_phi_radians_scattered_particle"  ;
    //  spec_phi_scattered_particle = new spectrum_1D( name,
    //                                                                                                     name,
    //                                                                                                     2000, 0, 0.2,
    //                                                                                                     folder );
    //  frs_spectra_ptr->push_back(spec_phi_scattered_particle) ;


}
//**************************************************************************
// read the calibration factors, gates
//**************************************************************************
void  Ttarget_agata::make_preloop_action(std::ifstream & s )
{

//    distance =
//        Tfile_helper::find_in_file(s,
//                                   std::string(name_of_this_element + "_distance")
//                                  );



    // reading the options of the doppler correction algorithm
    //  type_of_tracking_requirements    tracking_position_on_target ;
    //  type_of_dante_requirements        tracking_position_on_dante ;
    //  type_of_doppler_algorithm        which_doppler_algorithm ;



    std::string fname = path.dir_options() + "doppler.options" ;

    try
    {
        std::ifstream plik(fname.c_str()) ;
        if (!plik)
        {
            Tfile_helper_exception capsule;
            capsule.message =  "Can't open the file";
            throw capsule ;
        }

        tracking_position_on_target =
            (type_of_tracking_requirements)
            Tfile_helper::find_in_file(plik,"projectile_tracking");

        tracking_position_on_dante =
            (type_of_dante_requirements)
            Tfile_helper::find_in_file(plik,"lycca_tracking");

        which_doppler_algorithm =
            (type_of_doppler_algorithm)
            Tfile_helper::find_in_file(plik,"doppler_corr_algorithm");



        beta_from_where =  (type_of_beta) Tfile_helper::find_in_file(plik, "beta_from_where");
        beta_fixed_value =  Tfile_helper::find_in_file(plik, "beta_fixed_value");

//       energy_deposit_in_target
//       =  Tfile_helper::find_in_file(plik, "energy_deposit_in_target");

//       atima_const = Tfile_helper::find_in_file(plik, "atima_constant");
//       atima_factor = Tfile_helper::find_in_file(plik, "atima_factor");
//       atima_mean = Tfile_helper::find_in_file(plik, "atima_mean_beta_tof");

    }
    catch (Tfile_helper_exception &m)
    {
        std::cout << "\n\nTtarget_agata:: Error while reading the file " << fname
             << m.message << std::endl;
        std::cout << ">>>> Most probably you have too old version of parameters "
             "for Doppler correction\n"
             " Please open the dialog window in the Cracow: \n"
             "   spy_option->Doppler\n"
             "choose any combination of parameteres and press OK" << std::endl;
        exit(123);
        //throw ;
    }

#ifdef DOPPLER_CORRECTION_NEEDED

    // inform them about the current doppler settings
    TEAGLE_crystal::set_doppler_corr_options(
        tracking_position_on_target,
        tracking_position_on_dante,
        which_doppler_algorithm);

#endif // def DOPPLER_CORRECTION_NEEDED


#ifdef HECTOR_PRESENT
    // inform them about the current doppler settings
    Thector_BaF::set_doppler_corr_options(
        tracking_position_on_target,
        tracking_position_on_dante,
        which_doppler_algorithm);
#endif

#ifdef AGATA_PRESENT
    // inform them about the current doppler settings
    Tagata_elementary_det::set_doppler_corr_options(
        tracking_position_on_target,
        tracking_position_on_dante,
        which_doppler_algorithm);
#endif

    if (beta_from_where == beta_type_fixed)
    {

//       double gamma_zero = 1 / sqrt(1- (beta_fixed_value*beta_fixed_value));
//       E_zero = (gamma_zero -1) * 931.5 ;
//       E_one = E_zero - energy_deposit_in_target ;
//       double gamma_one = (E_one + 931.5)/ 931.5 ;

//       beta_after_target = sqrt(1 - (1/(gamma_one * gamma_one)));
    }


    read_beta_playfield(); // for different versions of beta
#ifdef BETA_ANALOG
    read_beta_analog();
#endif

 Tnamed_pointer::add_as_mutative_ALL(
        "ALL_cluster_xia_cryst__beta_analog_(if_enabled)",
        &x_on_dante,
        NULL, // no validator, so always valid
        this
    );


    Tnamed_pointer::add_as_mutative_ALL(
        "ALL_cluster_xia_cryst__core0_energy_dopplered_by_beta_analog_(if_enabled)",
        &x_on_dante,
        NULL, // no validator, so always valid
       this
    );

        Tnamed_pointer::add_as_mutative_ALL(
        "ALL_cluster_xia_cryst__core1_energy_dopplered_by_beta_analog_(if_enabled)",
        &x_on_dante,
        NULL, // no validator, so always valid
        this
    );

}

//**********************************************************************
void Ttarget_agata::analyse_current_event()
{
//   std::cout << "Ttarget_agata::analyse_current_event for " <<  name_of_this_element << std::endl ;

    flag_doppler_possible = true ;

#ifdef BETA_ANALOG
    // New, event - we are removing the old analog beta, because here we will make the new ones
 Tnamed_pointer::remove_from_map_ALL(
        "ALL_cluster_xia_cryst__beta_analog_(if_enabled)" );
    Tnamed_pointer::remove_from_map_ALL(
        "ALL_cluster_xia_cryst__core0_energy_dopplered_by_beta_analog_(if_enabled)" );
        Tnamed_pointer::remove_from_map_ALL(
        "ALL_cluster_xia_cryst__core1_energy_dopplered_by_beta_analog_(if_enabled)");

    vector_beta_analog.clear();
    
#endif



//    dante_ptr->analyse_current_event() ;
    analysis_with_TVector3() ;

    // here as the result the new value of: flag_doppler_possible
    // should be available

    calculations_already_done = true ;
}
//*************************************************************************
//*************************************************************************
//************************************************************************
/** For basic doppler correction algorithm we do not need to calculate */
/*void Ttarget_agata::analysis_for_basic_algorithm()
{
  flag_doppler_possible = true ;  // (only by theta of the cristal)

  return ;
}*/
//*************************************************************************
//***********************************************************************
/** No descriptions */
//Tdante * Ttarget_agata::give_dante_ptr()
//{
//    return dante_ptr ;
//}
/************************************************************************/
// void Ttarget_agata::make_xy_on_dante_distance()
// {
// std::cout << "empty function Ttarget_agata::make_xy_on_dante_distance() " << std::endl;
// }
//***************************************************************************
/** to test the extrapolation algorithm */
//void Ttarget_agata::make_xy_on_any_distance(double dist)
//{
//
//  if(focus_before->was_x_ok() && focus_before->was_y_ok())
//  {
//      x_on_any =
//          focus_before->extrapolate_x_to_distance(dist);
//      y_on_any =
//          focus_before->extrapolate_y_to_distance(dist);
//
//      matr_xy_projected_on_any_distance->increment(x_on_any, y_on_any);
//  }
//}
//*******************************************************************************

//*************************************************************************************************
void Ttarget_agata::give_scattered_particle_vector(double *x, double *y,
        double *z, double *length)
{
    // cartesian vector from target hit ----> to Cate hit
    *x = v_scattered_particle_x;
    *y = v_scattered_particle_y;
    *z = v_scattered_particle_z;
    double len =  sqrt(  (v_scattered_particle_x * v_scattered_particle_x) +
                         (v_scattered_particle_y * v_scattered_particle_y) +
                         (v_scattered_particle_z * v_scattered_particle_z)
                      )   ; // v_scattered_particle_length;
    *length = len ;
}
//************************************************************************************************

/** For Alex algorithm using TVector3 class */

void Ttarget_agata::analysis_with_TVector3()
{
//   std::cout << " Ttarget_agata::analysis_with_TVector3() " << std::endl;

//     std::cout << "\n####################### Target calculations ##############################"
//     << std::endl;

    x_target = 0;
    y_target = 0 ;

    // even if we do not want tracking for doppler corr, it is nice to have the picture...

    x_target = 0 ;
    y_target = 0 ;
    flag_xy_target_ok = true ;

#ifdef NIGDY

    dcout << "No, tracking, so Extrapolated to the position of the target, ("
    << distance << ") x_target = " << x_target << ", y_target = " << y_target << std::endl;
    dcout << "cos vect projectile "
    << " cos_projectile_x = " << cos_projectile_x
    << " cos_projectile_y = " << cos_projectile_y
    << " cos_projectile_z = " << cos_projectile_z
    << std::endl;
#endif

    //====================================================================
    // == direction of cosines for scattered_particle (AFTER  target, on the way to Dante) ==
    //====================================================================


//    if (! dante_ptr->was_only_one_good_hit())    // (dante_ptr->was_only_one_hit())
//    {
//        flag_doppler_possible = false ;
////       std::cout << "dante says, that the xy position was not OK, !!!, so flag_doppler_possible <---- false" << std::endl;
//        return ;
//    }

    // if the dante information is ok
//    dante_vector = dante_ptr->give_general_vector();

    /** all the calculations for gammas are done in their
    respected objects (Tagata, Tagata_elementary_det) */


    flag_doppler_possible = true ;

//  std::cout << "F. Ttarget_agata::analysis_with_TVector3()    flag_doppler_possible <---- TRUE" << std::endl;

#ifdef NIGDY

    expol.SetXYZ(x_target,    y_target,   1) ;    // in fact 0, 0, 1


    outgoing.SetXYZ(v_scattered_particle_x ,  v_scattered_particle_y,  v_scattered_particle_z);

    angle_theta_radians_scattered_particle = expol.Angle(outgoing);
    angle_theta_degree_scattered_particle = angle_theta_radians_scattered_particle * 180 / M_PI ;

    dcout << "so the Scattering theta angle = " << angle_theta_radians_scattered_particle
    << " radians,  ("
    << angle_theta_degree_scattered_particle << " deg)" << std::endl;


// for the phi scatterig angle we make a trick
// 1. we create the vector outgoing, with the z = 0
    TVector2 outgoing_flat( dante_ptr->give_x()  -  x_extr_to_dante,
                            dante_ptr->give_y()  -  y_extr_to_dante);

// 2. we create the unit vector
    TVector2 unit_x(1, 0);

    angle_phi_radians_scattered_particle = outgoing_flat.DeltaPhi(unit_x);
    if (angle_phi_radians_scattered_particle <0)
        angle_phi_radians_scattered_particle +=  2*M_PI ;

    angle_phi_degree_scattered_particle  = (angle_phi_radians_scattered_particle * 180 / M_PI );

    dcout << "so the Scattering PHI angle = " << angle_phi_radians_scattered_particle << " radians,  ("
    << angle_phi_degree_scattered_particle  << " deg)" << std::endl;

    // spec_theta_scattered_particle->increment( angle_theta_degree_scattered_particle);
//      spec_phi_scattered_particle->increment( angle_phi_degree_scattered_particle);

#endif

    return ;
}

//****************************************************************************
/** No descriptions */
void Ttarget_agata::read_beta_playfield()
{
    std::string fname = path.dir_options() + "beta_playfield.options" ;
    try
    {
        std::ifstream plik(fname.c_str()) ;
        if (!plik)
        {
            //            Tfile_helper_exception capsule;
            //            capsule.message =  "Can't open the file";
            //            throw capsule ;
            std::cout << "Can't open the file " << fname << " so I assume 'beta playfield' disabled " << std::endl;
            beta_playfield_enabled = false ;
            return;
        }
        beta_playfield_enabled = (bool) Tfile_helper::find_in_file(plik,"beta_playfield_enabled");
        if (!beta_playfield_enabled)  return ;

        betaN_fixed_value.resize(9);
//       betaN_after_target.resize(9);

        for (int i = 0 ; i < 9 ; i++)
        {
            std::string keyw = "beta_" ;
            keyw += char ('1' + i) ;

            betaN_fixed_value[i] =  (double) Tfile_helper::find_in_file(plik, keyw);

//           double gamma_zero = 1 / sqrt(1- (betaN_fixed_value[i]*betaN_fixed_value[i]));
//           double E_zero = (gamma_zero -1) * 931.5 ;
//           double E_one = E_zero - energy_deposit_in_target ;
//           double gamma_one = (E_one + 931.5)/ 931.5 ;
//           betaN_after_target[i] = sqrt(1 - (1/(gamma_one * gamma_one)));
        }
    }
    catch (Tfile_helper_exception &m)
    {
        std::cout << "Ttarget_agata:: Error while reading the file " << fname
             << m.message << std::endl;
        throw ;
    }
}
//*************************************************************************

//****************************************************************************
/** No descriptions */
void Ttarget_agata::read_beta_analog()
{
#ifdef   BETA_ANALOG

    // some dummy values
    beta_analog_enabled = false;
    begin_beta_analog   = 0.1;
    end_beta_analog  = 0.2;
    how_many_bins_of_beta_analog  = 1;


    std::string fname = path.dir_options() + "beta_analog.options" ;
    try
    {
        std::ifstream plik(fname.c_str()) ;
        if (!plik)
        {
            //      Tfile_helper_exception capsule;
            //      capsule.message =  "Can't open the file";
            //      throw capsule ;
            std::cout << "Can't open the file " << fname << " so I assume 'beta analog' disabled " << std::endl;
            beta_playfield_enabled = false ;
            return;
        }
        beta_analog_enabled =
            (bool) Tfile_helper::find_in_file(plik,"beta_analog_enabled");
        if (!beta_analog_enabled)
        {
            std::cout << "The 'beta analog system'    is set as:  disabled " << std::endl;
            return ;
        }
        // read the start beta
        begin_beta_analog   =
            Tfile_helper::find_in_file(plik,"begin_beta_analog");

        // read the stop beta
        end_beta_analog  =
            Tfile_helper::find_in_file(plik,"end_beta_analog");

        // read bins
        how_many_bins_of_beta_analog  =
            (int) Tfile_helper::find_in_file(plik,"how_many_bins_of_beta_analog");


        size_of_beta_analog_bin = (end_beta_analog - begin_beta_analog) / (how_many_bins_of_beta_analog -1) ;

        for (int i = 0 ; i < how_many_bins_of_beta_analog ; i++)
        {
            beta_analog_fixed_value.push_back( begin_beta_analog + (i *size_of_beta_analog_bin));

            double gamma_zero = 1 / sqrt(1- (beta_analog_fixed_value[i]*beta_analog_fixed_value[i]));
            double E_zero = (gamma_zero -1) * 931.5 ;
            double E_one = E_zero - 0;   // 0 -> energy_deposit_in_target ;
            double gamma_one = (E_one + 931.5)/ 931.5 ;
            beta_analog_after_target.push_back(sqrt(1 - (1/(gamma_one * gamma_one))));

            //             std::cout << "beta_analog_fixed_value[" << i <<"]=" << beta_analog_fixed_value[i];
            //             std::cout << ", beta_analog_after_target[" << i <<"]=" << beta_analog_after_target[i] << std::endl;
        }
        std::cout << "Success in reading the 'beta analog' data" << fname << std::endl;
    }
    catch (Tfile_helper_exception &m)
    {
        std::cout << "Ttarget:: Error while reading the file " << fname
             << m.message << std::endl;
        std::cout << "So the 'beta analog system'    is  disabled " << std::endl;
        //exit(88);
        //throw ;
    }
#endif
}

//*************************************************************************
#ifdef BETA_ANALOG
void Ttarget_agata::remember_to_increment_matrix_beta_vs_energy_dopp(  int i,
        double energy0, double energy1,
        Tincrementer_donnor *ptr_for_selfgate_owner )
{
    // perhaps we want the incrementation under some condition.
    // But the condition can not be tested yet...

    //   points_ont_beta_vs_energy_dop_matrix.push_back
    //   (
    //     std::pair<double, double> (energy,
    //                           begin_beta_analog + (i * size_of_beta_analog_bin))
    //   );

    //----------
    vector_beta_analog.push_back(
        beta_analog_entry(
            begin_beta_analog + (i * size_of_beta_analog_bin),    // beta
            energy0,                      // energy_doppler
            energy1,     // core1
            ptr_for_selfgate_owner)
    );

//   int current = vector_beta_analog.size() -1;

    //Tincrementer_donnor * ptr = &vector_beta_analog[current];

    /*    std::cout << "putting beta " <<  vector_beta_analog[current].beta_value
        << ", energy dop " << vector_beta_analog[current].energy_dopp

        << " size of vector_beta_analog= " << vector_beta_analog.size()
        << std::endl;*/


    Tnamed_pointer::add_as_mutative_ALL(
        "ALL_cluster_xia_cryst__beta_analog_(if_enabled)",
        &vector_beta_analog.back().beta_value,
        NULL, // no validator, so always valid
        &vector_beta_analog.back()
    );


    Tnamed_pointer::add_as_mutative_ALL(
        "ALL_cluster_xia_cryst__core0_energy_dopplered_by_beta_analog_(if_enabled)",
        &vector_beta_analog.back().core0_energy_dopp,
        NULL, // no validator, so always valid
        &vector_beta_analog.back()
    );

        Tnamed_pointer::add_as_mutative_ALL(
        "ALL_cluster_xia_cryst__core1_energy_dopplered_by_beta_analog_(if_enabled)",
        &vector_beta_analog.back().core1_energy_dopp,
        NULL, // no validator, so always valid
        &vector_beta_analog.back()
    );

//   Tnamed_pointer::add_as_mutative_ALL(
//     "ALL_cluster_xia_cryst__beta_analog_(if_enabled)",
//     &vector_beta_analog[current].beta_value,
//     NULL, // no validator, so always valid
//     &vector_beta_analog[current]
//   );
//
//
//   Tnamed_pointer::add_as_mutative_ALL(
//     "ALL_cluster_xia_cryst__energy_dopplered_by_beta_analog_(if_enabled)",
//     &vector_beta_analog[current].energy_dopp,
//     NULL, // no validator, so always valid
//     &vector_beta_analog[current]
//   );

}
#endif // #ifdef BETA_ANALOG
//************************************************************************



