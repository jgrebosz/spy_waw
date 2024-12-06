#ifndef TBETA_ANALOG_WORKSHOP_H
#define TBETA_ANALOG_WORKSHOP_H

#include <Tincrementer_donnor.h>
#include "spectrum.h"
#include "TEAGLE_crystal.h"
#include <list>



class Tbeta_analog_workshop : public Tincrementer_donnor
{

  
//==================
		double give_beta_analog_after_target ( int n ) { return beta_analog_after_target[n]; }
		bool is_beta_analog_enabled()  { return beta_analog_enabled; }
//==================

  
  //==================

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
        void remember_to_increment_matrix_beta_vs_energy_dopp ( int i, double energy, TEAGLE_crystal *ptr_for_selfgate );

		int  give_beta_analog_bins()
		{   return  how_many_bins_of_beta_analog; }

		//----------------
	struct beta_analog_entry : public Tincrementer_donnor
		{
			double beta_value;
			double energy_dopp;
            TEAGLE_crystal *ptr_for_selfgate;
            beta_analog_entry ( double b, double e, TEAGLE_crystal *ptr ) :
					beta_value ( b ), energy_dopp ( e ), ptr_for_selfgate ( ptr )
			{}

			bool check_selfgate ( Tself_gate_abstract_descr * descr )
			{
				return ptr_for_selfgate->check_selfgate ( descr );
			}
		};
		//------------------
//   std::vector<beta_analog_entry>  vector_beta_analog;
		list<beta_analog_entry>  vector_beta_analog;

//   std::vector< std::pair<double, double> > points_ont_beta_vs_energy_dop_matrix;
	protected:
//==================

  
public:
    virtual bool check_selfgate(Tself_gate_abstract_descr* );
    void  during_analyse_current_event();
    void  during_preloop();    
    
    void remember_to_increment_matrix_beta_vs_energy_dopp(  int i,
    double energy,
    Tself_gate_abstract_descr *ptr_for_selfgate);
};

#endif // TBETA_ANALOG_WORKSHOP_H
