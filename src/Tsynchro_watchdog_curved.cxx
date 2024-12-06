#include "Tsynchro_watchdog_curved.h"
#include "Tnamed_pointer.h"

#ifdef GSI_STUFF

#include "TGalileoAnalysis.h"
//***********************************************************************
//  constructor
Tsynchro_watchdog::Tsynchro_watchdog (std::string _name)
:
name_of_this_element()

{
  
  analysis_ptr = RisingAnalysis_ptr  ;
  create_my_spectra();
  
  
  struct one_entry
  {
    int how_many_times_arrived ;   // A
    double sum_of_current_ratios;  // B
    double long_term_mean_ratio;  // B:A
  };
  
  static one_entry dum ;
  
  dum.how_many_times_arrived = 0 ;
  dum.sum_of_current_ratios = 0 ;
  dum.long_term_mean_ratio = 0 ;
  
  /*
   std::vector<  > line;
   for(int m = 0 ; m < 128 ; m++)
   {
   line.push_back(dummy); // just to test if the reservation was OK
   }
   */
  
  ratios_bank.resize(how_many_variants);
  //   std::cout << "how_many_variants=" << how_many_variants << std::endl;
  //      std::cout << "ratios_bank.capacity() =" << ratios_bank.capacity() << std::endl;
  
  for(int i =0 ; i < how_many_variants ; i++)
  {
    ratios_bank[i].resize(128);
    //              std::cout << "ratios_bank[" << i << "].capacity()="
    //                              << ratios_bank[i].capacity() << std::endl;
    
    
    // just to test
    for(int m = 0 ; m < 128 ; m++)
    {
      //                      ratios_bank[i].push_back(dum);
      
      ratios_bank[i][m].how_many_times_arrived = 0 ; // just to test if the reservation was OK
    }
  }
  
  
}
//************************************************************
void Tsynchro_watchdog::create_my_spectra()
{
  
  //the spectra have to live all the time, so here we can create
  // them during  the constructor
  
  std::string folder = "synchronisation/" + name_of_this_element ;
  //----------------------------
  
  // NOTE: The spectra are created as the user defined spectra
  
  
}
//*************************************************************
void Tsynchro_watchdog::analyse_current_event()
{
  
  
  // std::cout << "analyse_current_event()  for "
  //         <<  name_of_this_element
  //          << std::endl ;
  
  //#ifdef NIGDY
  
#ifdef GSI_STUFF
  frs_synchro_data =  analysis_ptr->give_frs_ptr()->give_synchro_data() ;
  ger_synchro_data =  analysis_ptr->give_rising_ptr()->give_synchro_data() ;
  
#ifdef DGF_SEPARATE_BRANCH_PRESENT
  dgf_synchro_data =  analysis_ptr->give_dgf_ptr()->give_synchro_data() ;
#endif
  
#ifdef HECTOR_PRESENT
  hec_synchro_data =  analysis_ptr->give_hector_ptr()->give_synchro_data() ;
#endif
  
#ifdef AGATA_PRESENT
  aga_synchro_data =  analysis_ptr->give_aga_ptr()->give_synchro_data() ;
#endif
  
  // this depends of the type of matching
  
  check_combination(1, frs_synchro_data, ger_synchro_data,
                    "FRS_synchronisation_data", "GERMANIUMS_synchronisation_data", 4000, 4000);
  
#ifdef HECTOR_PRESENT
  check_combination(0, hec_synchro_data, frs_synchro_data,
                    "HECTOR_synchronisation_data", "FRS_synchronisation_data", 4000, 4000);
  
  check_combination(2, hec_synchro_data, ger_synchro_data,
                    "HECTOR_synchronisation_data", "GERMANIUMS_synchronisation_data", 4000, 4000);
#endif
  
  
#ifdef DGF_SEPARATE_BRANCH_PRESENT
  check_combination(3, dgf_synchro_data, ger_synchro_data,
                    "DGF_synchronisation_data", "GERMANIUMS_synchronisation_data", 4000, 4000);
  check_combination(4, dgf_synchro_data, frs_synchro_data,
                    "DGF_synchronisation_data", "FRS_synchronisation_data", 4000, 4000);
#endif
  
#ifdef AGATA_PRESENT
  check_combination(5, aga_synchro_data, ger_synchro_data,
                    "AGA_synchronisation_data", "GERMANIUMS_synchronisation_data", 4000, 4000);
  check_combination(6, aga_synchro_data, frs_synchro_data,
                    "AGA_synchronisation_data", "FRS_synchronisation_data", 4000, 4000);
#endif
  
  
  
#endif   //#ifdef GSI_STUFF
  
}
//**************************************************************
// read the calibration factors, gates
void Tsynchro_watchdog::make_preloop_action()
{
  
}
//***************************************************************
/** No descriptions */
void Tsynchro_watchdog::fill_the_moving_paper_graph(
                                                    Tratio_nr  which_quotient, double value)
{
  
}

//***********************************************************************************
void Tsynchro_watchdog::check_combination(int nr, int val1, int val2, std::string nam1, std::string nam2, int max1, int max2)
{
  int bin = val2 >> 5 ;// devide by 32 - so there are only 128 bins
  
  if(bin > 127) std::cout << "Something wrong with the bins of synchro watchdog"
    << std::endl;
  
  if(val1 > 300 &&  val2 > 300 && (val1 < max1 && val2 < max2))
  {
    double ratio_current = (double)val1 / val2;
    
    
    ratios_bank[nr][bin].how_many_times_arrived++;
    ratios_bank[nr][bin].sum_of_current_ratios += ratio_current;
    ratios_bank[nr][bin].long_term_mean_ratio =
    ratios_bank[nr][bin].sum_of_current_ratios
    /
    ratios_bank[nr][bin].how_many_times_arrived ;
    
    
    // sensible test is after 100 entries to this bin
    if(ratios_bank[nr][bin].how_many_times_arrived > 100
       &&
       ratios_bank[nr][bin].long_term_mean_ratio != 0)
    {
      double quotient = ratio_current /  ratios_bank[nr][bin].long_term_mean_ratio;
      if(quotient  < 0.7  ||  quotient   > 1.3)
      {
        // this is the alarming situation
        
        static time_t last ;
        time_t now = time(0) ;
        
        if(now - last < 15)
        {
          std::cout
          << "ALARM !!!!!!!!!!!!!!!!!!!!!!!!!!!! quotient = "
          << quotient << std::endl;
          
          std::cout
          << nam1 << ": "  << val2
          << nam2 << ": "  << val2
          << "    ratio_current: "
          << ratio_current
          << " \a\a\a" << std::endl ;
          
          
          std::string file_sorted = analysis_ptr->give_current_filename() ;
          
          if(file_sorted == "")
          {
            //std::cout << "This is sorting online " << std::endl;
            
            std::ofstream plik(path.dir_commands() + "synchronisation_lost.txt");
            plik
            << "Synchronisation Watchdog barks !\n"
            << "Please check synchronisation spectrum corresponding to\n  " << nam1
            << " <---> " << nam2
            << "\nIf it is only one point which is wrong - No problem\n"
            << "If the whole spectrum is covered with a grid of points - "
            << "this is really lost of the synchronisation"
            << std::endl;
            plik.close();
          }else{
            std::cout << "This is sorting offline, file  " << file_sorted
            << ", so no watchdog will bark !!! " << std::endl;
            // in the offline version we do not make watchdog in grebware viewer
          }
          
          ratios_bank[nr][bin].how_many_times_arrived = 0;
          ratios_bank[nr][bin].sum_of_current_ratios = 0.0;
        }
        last = now ;
      }
    }
  }
  
  if(ratios_bank[nr][bin].how_many_times_arrived  > 10000)
  {
    ratios_bank[nr][bin].how_many_times_arrived = 0;
    ratios_bank[nr][bin].sum_of_current_ratios = 0.0;
  }
}
//*******************************************************************************************
#endif  // #ifdef GSI_STUFF
