#ifndef _synchro_watchdog_H_
#define _synchro_watchdog_H_

#ifdef GSI_STUFF

//#ifdef NIGDY

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TGalileoEvent class
#ifndef  __CINT__



#include "TNeedle_element.h"
#include "spectrum.h"
#include <string>
#include <vector>

#include "TGalileoEvent.h"
#include "Tincrementer_donnor.h"

class TRisingAnalysis ;

//////////////////////////////////////////////////////////////////////////////
class Tsynchro_watchdog : public Tincrementer_donnor
{

  std::string name_of_this_element ;

  int frs_synchro_data;
  int ger_synchro_data;
  int hec_synchro_data;
  int dgf_synchro_data;
  int aga_synchro_data;

  //----------------------------

  TRisingAnalysis *analysis_ptr;

  enum Tratio_nr
  {
      hec_ger,
      hec_frs,
      frs_ger,
      dgf_ger,
      dgf_frs,
      aga_ger,
      aga_frs,
      how_many_variants    // <----  to know it automatically
      };

  int channel_to_increment[how_many_variants];
  double mean_ratio[how_many_variants];
  double sum_ratios[how_many_variants];
  int how_many_times_added[how_many_variants];

struct one_entry
{
        int how_many_times_arrived ;   // A
        double sum_of_current_ratios;  // B
        double long_term_mean_ratio;  // B:A
};

std::vector< std::vector<one_entry>  > ratios_bank;

 public:
  //  constructor
  Tsynchro_watchdog (std::string _name) ;


  void analyse_current_event() ;
  void make_preloop_action();  // read the calibration factors, gates
  /** No descriptions */
  void fill_the_moving_paper_graph(Tratio_nr nr_ratio, double value);
  /** No descriptions */
  void check_combination(int nr, int val1, int val2, std::string nam1, std::string nam2, int max1, int max2);

 protected:
  void create_my_spectra();

};

////////////// ///////////////////////////////////////////////////////////

#endif // CINT

#endif  // #ifdef GSI_STUFF

#endif // _synchro_watchdog_H_
