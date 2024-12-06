/***************************************************************************
                          ttarget_agata.h  -  description
                             -------------------
    begin                : Fri May 23 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#ifndef TTARGET_AGATA_H
#define TTARGET_AGATA_H


#include <TNeedle_element.h>
#include <Tdante.h>
#include <Tcristal.h>   // to have an enum about doppler correction

#include <spectrum.h>
#include "Tincrementer_donnor.h"
#include "Tself_gate_abstract_descr.h"

#include "TVector3.h"

#include <vector>
#include <list>
//#include "tself_gate_holder.h"

class Tfocal_plane ;
//class Tdante ;
class Tagata_core_energy;

class Tself_gate_holder;
/**this class is to perform operation of tracing the projectile hitting
   the target_agata and tracing the scattered_particle leaving the target
   *@author dr. Jerzy Grebosz (IFJ Krakow, Poland)
   */


////////////////////////////////////////////////////////////////////////////
class Ttarget_agata : public TNeedle_element, public Tincrementer_donnor
  {
  public:
    Ttarget_agata(std::string name_of_target,
                  std::string name_dante
                 );

    ~Ttarget_agata();

    // for the incrementer called:
    // Projectile_energy_calcul_from_frs_beta ;
//     double E_zero ;
//     double E_one ; //fragement_energy_after_target

    void make_preloop_action(std::ifstream & /* s */);

    /** doppler correction algorithm will ask for this */
    inline void give_cos_scattered_particle (double * cx, double *cy, double *cz)
    {
      *cx = cos_scattered_particle_x;
      *cy = cos_scattered_particle_y;
      *cz = cos_scattered_particle_z ;
    }
    /** for doppler correction algorithms */
//   inline double give_y_target(){
//     return y_target ;
//   }
    /** for doppler correction algorithms */
    /*inline double give_x_target()
    {
     return x_target ;
    }*/
    /** No descriptions */
    /*   inline double give_z()
       {
         return distance ;
       }
    */   /** No descriptions */

//    /** for doppler correction algorithms */
//    inline double give_y_tracked(){
//      return y_tracked ;
//    }
//    /** for doppler correction algorithms */
//    inline double give_x_tracked()
//    {
//     return x_tracked ;
//    }

    //bool projectile_calculations_ok() { return flag_projectile_ok ;}
    //bool scattered_particle_calculations_ok() { return flag_scattered_particle_ok ;}
    bool is_doppler_possible()
    {
      return flag_doppler_possible ;
    }

//    double give_dante_hit_pos_x()
//    {
//      return dante_ptr->give_x() ;
//    }
//    double give_dante_hit_pos_y()
//    {
//      return dante_ptr->give_y() ;
//    }
//    unsigned int give_dante_timestamp()
//    {
//      return dante_ptr->give_timestamp();
//    }

    /** Crystal will ask for a vector of resiude (target_agata----->dante) */
    void give_scattered_particle_vector(double *x, double *y,
                                        double *z, double *length);

    /** calculates modified beta for each event, or gives modified 'fixed' value */
//     double give_beta_after_target() { return  beta_after_target ; }
    double give_fixed_beta()
    {
      return  beta_fixed_value ;
    }
    
    //--------------------------------------------------------------------------
    double give_fixed_betaN(unsigned int i)
    {
      if (i <  betaN_fixed_value.size() )
        {
          return  betaN_fixed_value[i] ;
        }else return 0;
    }
    
//     double give_betaN_after_target(int n)
//     {
//       return betaN_after_target[n];
//     }
    bool is_beta_playfield_enabled()
    {
      return beta_playfield_enabled;
    }


#ifdef   BETA_ANALOG
		double give_beta_analog_after_target ( int n ) { return beta_analog_after_target[n]; }
		bool is_beta_analog_enabled()  { return beta_analog_enabled; }
#endif

    /** For basic doppler correction algorithm we do not need to calculate */
//     void analysis_for_basic_algorithm();


    //-----------------------------------
    double give_theta_scattering2()
    {
      std::cout << "give_theta_scattering() Not implemented yet" << std::endl ;
      return 0 ; // put here the result of calculations
    }
    //-----------------------------------
    double give_phi_scattering2()
    {
      std::cout << "give_phi_scattering() Not implemented yet" << std::endl ;
      return 0 ; // put here the result of calculations
    }
    /** To incremet spectra gated by the identified isotope */
//  void isotope_was_identified();
    /** No descriptions */
    Tdante * give_dante_ptr();
    /** No descriptions */
//   void make_xy_on_dante_distance();
    /** position where the projectile hits the dante */

    double give_x_on_dante()
    {
      return x_on_dante;
    }
    double give_y_on_dante()
    {
      return y_on_dante;
    }
    /** to test the extrapolation algorithm */
    void make_xy_on_any_distance(double dist);
    /** No descriptions */
    void read_beta_playfield();
    std::vector<Tagata_core_energy>  give_core_energies_array();
    /** No descriptions */
    // bool calculate_scattered_particle_vector();
    /** we need this in many places */
    // bool make_projectile_vector();

// TVector3 expol , outgoing ;

    TVector3  give_dante_wektor()
    {
      return dante_vector;
    }



  protected: // Protected methods

    /** position where the projectile hits the dante */
    double x_on_dante;
    double y_on_dante;

    //double x_on_any;
    //double y_on_any;


    /** spectra of angles - theta, phi projectile/residum */
    void create_my_spectra();
    void analyse_current_event();

    /** For Samit algorithm  */
//  void analysis_in_polar_coordinates();
    /** For Take algorithm */
//  void analysis_in_cartesian_coordinates();
    /** For Alex algorithm using TVector3 class */
    void analysis_with_TVector3();


  protected: // Protected attributes

    TVector3 expol , outgoing ;


    TVector3 dante_vector;



    /** to trace the projectile coming to the target */
//   Tfocal_plane *focus_before;
    /** to trace the scattered_particle coming out of target */
    //Teuclides *recoil_ptr;
    //double distance;

    double cos_projectile_x ;
    double cos_projectile_y ;
    double cos_projectile_z ;

    double v_projectile_length  ;

    //  normalized vector
//     double dnorm_projectile ;
//     double dnorm_scattered_particle ;


    // angles
    double cos_theta_scattered_particle  ;
    double cos_theta_projectile  ;

    double angle_theta_radians_scattered_particle ;
    double angle_phi_radians_scattered_particle ;

    double angle_theta_degree_scattered_particle ;
    double angle_phi_degree_scattered_particle ;

    // for Cartesian coordinates algorithm

    // catesian vector from target hit ----> to Cate hit
    double
    v_scattered_particle_x,
    v_scattered_particle_y,
    v_scattered_particle_z,
    v_scattered_particle_length;

    // for vector of the scattered_particle   target----->dante
    double cos_scattered_particle_x ;
    double cos_scattered_particle_y ;
    double cos_scattered_particle_z ;


    /** position where the projectile hits the target (x coordinate) */
// double x_extrapolated;
    /** position where the projectile hits the target (y coordinate) */
    //double y_extrapolated;

    //bool flag_xy_extrapolated_ok;

    // this below is different form this above (x,y) only by options set in the grebware
    // viewer. If the user wants no tracking, the x,y contains extrapolated value
    // but x_target, and y_target contains zero (0,0)

    /** position where the projectile is tracked on the target (x coordinate) */
    double x_target;
    /** position where the projectile hits the target (y coordinate) */
    double y_target;

    bool flag_xy_target_ok;



    type_of_tracking_requirements    tracking_position_on_target ;
    type_of_dante_requirements        tracking_position_on_dante ;
    type_of_doppler_algorithm        which_doppler_algorithm ;

    enum type_of_beta { beta_type_frs = 1, beta_type_fixed, beta_type_atima };
    type_of_beta   beta_from_where;

    /** beta modified by the energy loss in the target */
//     double beta_after_target ;
    double beta_fixed_value ;
//     double energy_deposit_in_target ;

//     double atima_const, atima_factor, atima_mean ;

    std::vector<double> betaN_fixed_value ;  // for the Beta Playfield purposes
//     std::vector<double> betaN_after_target ;
    bool beta_playfield_enabled;

    // ---- spectra ----------------------
//  spectrum_1D * spec_theta_projectile ;
//     spectrum_1D * spec_theta_scattered_particle ;

    //spectrum_2D * matr_position_xy ;
    //spectrum_2D * matr_xy_projected_on_dante;
//  spectrum_2D * matr_xy_projected_on_any_distance;

    /** if the information in the event is sufficient to perform such doppler
     correction as we demand (accurate, approximate) */
    bool flag_doppler_possible;

    #ifdef   BETA_ANALOG

		std::vector<double> beta_analog_fixed_value ;  // for the Beta Playfield purposes
		std::vector<double> beta_analog_after_target ;
		bool beta_analog_enabled;
		spectrum_2D  * spec_beta_analog_vs_energy_doppler;


		// Data to the bins of spectrum, and region of  beta;
		double begin_beta_analog ;
		double end_beta_analog ;
		int how_many_bins_of_beta_analog;
		double size_of_beta_analog_bin;

//   Tuser_condition *beta_analog_special_condition;
//   std::string beta_analog_special_condition_name;
	public:
		//void remember_to_increment_matrix_beta_vs_energy_dopp(int i, double energy);
		void remember_to_increment_matrix_beta_vs_energy_dopp ( int i, double core0_energy_dop, double core1_energy_dop, Tincrementer_donnor *ptr_for_selfgate_owner );

		int  give_beta_analog_bins()
		{   return  how_many_bins_of_beta_analog; }

		//----------------
	struct beta_analog_entry : public Tincrementer_donnor
		{
			double beta_value;
			double core0_energy_dopp;
			double core1_energy_dopp;
			Tincrementer_donnor *ptr_for_selfgate_owner;
			beta_analog_entry ( double b, double e0, double e1, Tincrementer_donnor *ptr ) :
					beta_value ( b ), core0_energy_dopp ( e0 ), core1_energy_dopp ( e1 ), ptr_for_selfgate_owner ( ptr )
			{}

			bool check_selfgate ( Tself_gate_abstract_descr * descr )
			{
				return ptr_for_selfgate_owner->check_selfgate ( descr );
			}
		};
		//------------------
//   std::vector<beta_analog_entry>  vector_beta_analog;
		list<beta_analog_entry>  vector_beta_analog;

//   std::vector< std::pair<double, double> > points_ont_beta_vs_energy_dop_matrix;
	protected:
	  void read_beta_analog();
#endif

    
    
  };
/////////////////////////////////////////////////////////////////////////////

#endif

