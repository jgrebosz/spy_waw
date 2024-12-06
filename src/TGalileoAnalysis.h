
#ifndef TEBANALYSIS_H
#define TEBANALYSIS_H

#include "experiment_def.h"

//#if DIAMANT_PRESENT
////========= Diamant - Begin ----------
//#include "MFMAllFrames.h"
//#include "DataParameters.h"
//#include "DataScalers.h"
//#include "MError.h"
//#include "Cobo.h"
//#include <cstring>
//#include <cstdlib>
//#include <time.h>
//#include "MFMlib.in.h"
////#include "diamant_data.h"
//////========= Diamant - End ----------
//#endif


#ifdef AGATA_PRESENT
#include "Tagata.h"
#endif


#include <string>
#include <cstring>
#include <cstdlib>

#include <vector>
#include <map>  // for map of zeroing
#include <ctime> // zeroing time
//#include "Tuser_incremeter.h"

// Fake version of Go4 function

class TGo4EventProcessor
{
  std::string name;

 public:
  TGo4EventProcessor ( char *name_ ) : name ( name_ )
  {}
};


#include "TGalileoEventFactory.h"
#include "TGalileoCalibrateFactory.h"

class spectrum_abstr ;
class spectrum_1D;
class spectrum_2D;

class Tuser_spectrum ;
class Tuser_condition;
class Tuser_incrementer;

#include "TNeedle.h"
#include "TEAGLE_array.h"

#include "Tsynchro_watchdog_curved.h"

#include "tsocket_for_data.h"

class Tuser_condition;

#include "tbuffer_of_data_block.h"
#include "tunpacked_event_data.h"

extern Tbuffer_of_data_block bufAgata;
extern Tunpacked_event_data event;

extern  char* bufDiamant;

int unpack_frame ( );




//class TjurekEventSourceException
//{ }
//;

//class TjurekEventEndException : public TjurekEventSourceException
//{

//};


#include "Tgo4eventsourceparameter.h"

////////////////////////////////////////////////////////////////////
class TGo4FileStoreParameter
{
  std::string file_name;
 public:
  TGo4FileStoreParameter ( std::string nam, int val = 0 )
    :file_name ( nam )
  {}

  void SetOverwriteMode ( bool v )
  {
    // empty so far
  }
};

// abstract class for event processor of particular step
class TGo4EventProcessorParameter
{

};

class TGo4FileSourceParameter
{
 public:
  TGo4FileSourceParameter ( std::string name )
    {}
};
///////////////////////////////////////////////////////////////////////////////////////
class TjurekFactory
{
  std::string name;
 public:
  TjurekFactory ( std::string n ) : name ( n )
  {}
};
/////////////////////////////////////////////////////////////////////////////////////////////////
class TGo4AnalysisStep
{
  std::string name;
  bool enabled;

  TGo4EventSourceParameter *sourceptr; // <-- where from take the data
  TGo4FileStoreParameter *storeptr; // <--- where to put the result RAW root tree ( name_RAW.root)
  TjurekAbstractEventProcessor * procptr ;
  TjurekFactory *factoryptr;

 public:

  TGo4AnalysisStep (
		    std::string n,       // <--- name
		    TjurekFactory *factory1,
		    TGo4EventSourceParameter *sourcepar, // <-- where from take the data (file or remote server) pointer to the object
		    TGo4FileStoreParameter *storepar, // <--- where to put the result RAW root tree ( name_RAW.root)
		    TjurekAbstractEventProcessor * procpar ) : // NULL?
  name ( n ),
    factoryptr ( factory1 ),
    sourceptr ( sourcepar ),
    storeptr ( storepar ),
    procptr ( procpar )
    {
      enabled = true;
    }
  //----------------------------------
  void SetSourceEnabled ( bool t )
  {
    enabled  = t;
  }
  void SetStoreEnabled ( bool t )
  {}   // leave it true, even it will be disabled, otherwise the file
  // is not opened, and enabling later will not be possible - this is my observation JG

  void SetProcessEnabled ( bool t )
  {}
  void SetErrorStopEnabled ( bool t )
  {}

  void Process()
  {
    //        procptr->Process();
  }
};

//**************************************************
///////////////////////////////////////////////////////////////////////////////
class TGalileoAnalysis  //: public TGo4Analysis
{

  TGalileoEventProcessor *step1;
  TGalileoCalibrateProcessor *step2;

  TGalileoEvent *step1_output_event;
  TGalileoCalibratedEvent *step2_output_event;

  void set_all_steps ( TGalileoEventProcessor *p1, TGalileoCalibrateProcessor *p2,
		       TGalileoEvent *targ1, TGalileoCalibratedEvent *targ2 )
  {
    step1 = p1;
    step2 = p2;
    step1_output_event = targ1;
    step2_output_event = targ2;
  }

 public:
  TGalileoAnalysis();
  TGalileoAnalysis ( std::string lmdname, std::string rawname,
		     std::string calname, std::string   userfname );

  virtual ~TGalileoAnalysis() ;
  virtual int UserPreLoop();
  virtual int UserEventFunc();
  virtual int UserPostLoop();
  virtual bool InitEventClasses();
  void analyse_event()
  {
    gal.analyse_current_event();
  }
  //     void make_rising_calibration_step()
  //     {
  //       rising.analyse_current_event();
  //       cycle_on_real_data = true;
  //     }



  void make_watchdog_step()
  {
#ifdef GSI_STUFF
    synchro_watchdog.analyse_current_event();
#endif
  }

  std::vector<spectrum_abstr*> spectra_local ;  // timestamp etc.

  std::vector<Tuser_spectrum*> spectra_user ;  //!    // user defined spectra
  std::vector<Tuser_condition*> user_condition ;  //!    // user defined conditions
  std::vector<Tuser_incrementer*> vector_of_user_incrementers; //!

  // for time stamp

  bool flag_sorting_from_file;

  /** This is the value which is stored in every event.
      We need it to estimate the ratio of how many events are analysed
      to all the events coming */
  int *ptr_to_mbs_nr_of_the_current_event; //!




  //bool set_event_unpacked_and_valid(bool x)
  // {flag_event_unpacked_and_is_valid = x ;}

  static TGalileoCalibratedEvent * fxCaliEvent;

  int nr_events_analysed;  // Cal Processor will set its value here

  void SetAutoSaveFile ( std::string nazwa )
  {
    std::cout << "empty f. SetAutoSaveFile" << std::endl;
  }

  void SetAutoSaveInterval ( int value )     { }


  TjurekPolyCond* give_ptr_to_polygon ( std::string name );
  void AddAnalysisCondition ( TjurekPolyCond* ptr )
  {
    list_of_polygons.push_back ( ptr );
  }

  void RemoveHistogram ( std::string name_root )
  {
    std::cout << "Empty f. RemoveHistogram() " << std::endl;
  }
  void AddHistogram ( std::string name_root )
  {
    std::cout << "Empty f. AddHistogram() " << std::endl;
  }
  void AddHistogram ( TH1* ptr_spec, const char* );

  TGo4EventSourceParameter* give_source_of_events()
  { return source_of_events;}

  //-----------------
 private:
  bool flag_event_unpacked_and_is_valid ;
  std::vector<TGo4AnalysisStep *> steps_of_analysis;

  void AddAnalysisStep ( TGo4AnalysisStep * step )
  {
    steps_of_analysis.push_back ( step );
  }

  TGo4EventSourceParameter* source_of_events;   // null

  std::vector<TjurekPolyCond*> list_of_polygons;

  std::vector<TH1*> list_of_histograms;

  //    TjurekMbsEvent * fxMbsEvent;
  TGalileoEvent * fxEbEvent;
  TGalileoCalibrateProcessor* proc_remembered ;



  /** If some spectra are defined here in the analysis class */
  bool save_analysis_local_spectrum ( std::string specname );

#ifndef __CINT__

 public:
  TNeedle * give_gal_ptr()
  {
    return &gal ;
  }

  std::vector<spectrum_abstr*> * local_spectra_pointer()
    {
      return  &spectra_local;
    }


  /** this is such a version which replaces the RunImplicitLoop,
      but is able to handle the pause and continue exceptions */
  int run_jurek_ImplicitLoop ( int times );

  int print_ratios_every_n_events;            // initaially  = 5000, ten mdified
  time_t previous_print_ratios_time;
  int previous_print_event_nr;
  int last_ev_analsyed_long_term_nr;
  int last_ev_mbs_long_term_nr;

  void display_procent_ratios_and_speed(const int ev_nr);

  /** If the grebware viewer is just observing some spectra, we
   * will save them more often */
  void save_just_observed(bool unconditionally = false);
  /** sending an info message to the grebware GUI */
  void info_to_grebware(std::string s, int i = -1);


  /** If the grebware viewer is just observing some spectra, we
   * will save them more often */





  TNeedle          gal ;      //!     fragment separtor + target +cate

  /** perhaps it was idle cycle ? */
  bool cycle_on_real_data;



#endif

  //---------------------
  void CloseAnalysis() ;  // hiding the one from the base class

  //transporter * ptr_transporter ; //!  -----------------------!
  TGalileoCalibrateProcessor* give_call_processor_address() ;
  /** grebware viewer may send request to delete list of spectra */
  void perhaps_delete_some_spectra();

  /** to checking if grebware gui is giving any pause/go command */
  void check_for_commands ( int nr_events );

  /** No descriptions */
  inline void perhaps_info_of_events ( int nr_events );
  void perhaps_conditions_report ( bool par_force = false );

  /** In preLoop moment */
  void read_definitions_of_user_spectra();
  /** No descriptions */
  void create_user_spectrum ( std::string name );

  /** Conditions live independently. One condition can be shared by many
      spectra */
  Tuser_condition* create_user_condition ( std::string name );

  TGalileoEvent* give_unpacked_event_ptr()
  {
    return fxEbEvent;
  }


  //    std::vector<Tuser_sorting_wish_abstract*> sorting_wish;

  std::string give_current_filename();
  Tuser_condition* give_ptr_to_cond ( std::string );



  // the new style, user def condition
  Tuser_condition *step2_cond_ptr;
  //  int nicto;
  bool *step2_cond_result_ptr; //!

  /** No descriptions */
  void incr_synchronisation_matices();
  /** No descriptions */
  void make_ratio_spectra();
  /** No descriptions */
  void prepare_ratio_vector();

  struct single_ratio
  {
    std::string name ;
    bool *test_it;
    int nr_success;
    int spectrum_to_increment ;   // there may be more than one spectrum
    int channel_to_increment;
    std::string explanation;

    // constructor
    single_ratio ( std::string name_of_value = "", int spec_to_increment = 0,
		   int chan_to_increment = 0, std::string explanation_text = "" ) :
    name ( name_of_value ),
      spectrum_to_increment ( spec_to_increment ),
      channel_to_increment ( chan_to_increment ),
      explanation ( explanation_text )
    {
      nr_success = 0 ;
      if ( explanation == "" )
	explanation = name ;
    }

  };

  std::vector<single_ratio> ratio_vector ;

  bool general_flag_verbose_events;
  bool is_verbose_on(){
    return  general_flag_verbose_events;
  }
  void set_verbose ( bool b ){
    general_flag_verbose_events = b ;
  }

  /** No descriptions */
  void create_my_spectra();
  /** No descriptions */
  int read_how_many_branches();
  /** No descriptions */
  void read_autosave_time();
  int autosave_period_in_minutes;

  /** This function calculates the value of all the user defined conditions.
   * It may be used by the user defined spectra, but also as the decision
   * if the event should go to the ntuple. */
  void check_all_user_def_conditions();

  /** This function checks if the user wanted any condtion on
   * the step2 ntuple
   * and sets a special pointer */
  void prepare_condition_for_ntuple ( std::string condition_name );
  /** No descriptions */
  void user_batch();

  /** This member remembers the time of the last zeroing */
  std::map<std::string, time_t> map_of_zeroing;
  void store_time_of_zeroing_spectrum ( std::string name, time_t when )
  {
    map_of_zeroing[name] = when;
  }
  void save_times_of_zeroing_on_disk();
  void restore_times_of_zeroing_from_disk();

  void read_in__user_incrementers();
  void create_user_incrementer ( std::string name_of_description );
  //--------------------------------
  void PreLoop()
  {
    std::cout << "f. TRisingAnalysis::PreLoop() " << std::endl;
    step1->preLoop();
    step2->preLoop();
    UserPreLoop();
  }
  //--------------------------------
  void MainCycle()
  {
    // std::cout << "f. TRisingAnalysis::MainCycle() " << std::endl;


    //         1. rozpakowanie - step1
    step1->BuildEbEvent ( source_of_events, step1_output_event );
    //         2. kalibracja - step2
    step2->BuildCalibratedEvent ( step2_output_event );
    //         3. user functions - step2a
    // go4 was calling it automatically, here in exotic - we must make it on our own
    UserEventFunc();
  }

  //--------------------------------
  void PostLoop()
  {
    //std::cout << "Empty f. TRisingAnalysis::PostLoop() " << std::endl;

    std::cout << "f. TRisingAnalysis::PostLoop() " << std::endl;
    step1->postLoop();
    step2->postLoop();
    UserPostLoop();
  }


  TH1 * GetHistogram ( std::string name )
  {
    //        std::cout << "Empty f. TRisingAnalysis::GetHistogram()  for " << name << std::endl;
    for ( int i = 0 ; i < list_of_histograms.size() ; i++ )
      {
	if ( list_of_histograms[i]->GetName() == name )
	  return list_of_histograms[i];
      }
    // not found
    //         std::cout << "F. TRisingAnalysis::GetHistogram()  for " << name
    //         << " - such a spectrum is not found on the list "
    //         << std::endl;
    return 0;
  }

  //    ClassDef(TRisingAnalysis,1)

};

///////////////////////////////////////////////////////////
extern TGalileoAnalysis *  GalileoAnalysis_ptr ;

#endif //TEBANALYSIS_H


