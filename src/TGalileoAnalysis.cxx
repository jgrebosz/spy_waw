#include "TGalileoAnalysis.h"
#include "spectrum.h"
#include <sstream>
#include <filesystem>  // for file exists

#include <fcntl.h>

#include "TGalileoCalibrateProcessor.h"
#include "Tgrebware_exception.h"
#include "Tnamed_pointer.h"
#include <string>
#include "user_spectrum_description.h"
#include "Tuser_spectrum.h"
#include "Tuser_condition.h"
#include "Tcondition_description.h"

#include "Tuser_incrementer.h"
#include "Tfile_helper.h"

// Joern Adamczewski suggest to definie it here and refer as extern
// from MainUserAnalysis
//    (it was other way round before)
bool go4_gui_mode;  // Take does not want to save *.spc files in Go4 mode


#include "randomizer.h"
int randomizer::randomizer_word  ;  // static

//#define COTO  std::cout<<" I am at line  "<<__LINE__<< std::endl;
//#define ANYKEY { int nic; std::cout << "press 1 and Enter " << std::endl; std::cin >> nic;}

//#define WHERE    __FILE__ << "  line  "<<__LINE__

TGalileoCalibratedEvent * TGalileoAnalysis::fxCaliEvent;   // static
TGalileoAnalysis *GalileoAnalysis_ptr;

extern unsigned int starting_event; // global to be set in main and accessed from constructor of TGo4Analysis.cxx

extern void unpack_one_event();

char* bufDiamant;

extern int argcounter;
extern string string_arguments[10];
bool flag_this_is_diamant_data = false;
//****************************************************************************
TGalileoAnalysis::TGalileoAnalysis()
    : fxEbEvent ( 0 ) , gal ( this )
{
    // disabled default ctor JA
}
//****************************************************************************
TGalileoAnalysis::TGalileoAnalysis (
        std::string  nam1,
        std::string  nam2,
        std::string  nam3,
        std::string  nam4 )  :
    fxEbEvent ( 0 ) , gal ( this )
{

    GalileoAnalysis_ptr = this;
    general_flag_verbose_events = false;
    nr_events_analysed = 0 ;

    print_ratios_every_n_events = 5000;
    previous_print_ratios_time = time(NULL);
    previous_print_event_nr = 0 ;

    step1_output_event  = NULL ;  //

    //    source_of_events = NULL; // will be defined below

    // nam1: input file name (*.lmd)
    // nam2: output file name of first analysis step  (*.root)
    // nam3: output file name of second analysis step (*.root)
    // nam4:
    //ntuple_RAW_is_collected = false;
    //ntuple_CAL_is_collected = false;

    info_to_grebware ( "Spy is going to start soon..." );


    //=========================================================
    // first step definitions:
    //=========================================================


    flag_sorting_from_file = false;  // later we set it really

    char ptr1[250] = "currrent.lmd";
    char ptr2[250] = "currrent_RAW_tree";   // was ASF by mistake
    char ptr3[250] = "currrent_CAL_tree";


    // look at the first argument
    std::string kawalek = std::string ( nam1 ).substr ( 0,4 );  // "-remserver"

#if DIAMANT_PRESENT
    if ( nam1.substr(0, 8) == "-rem_dia" )  // "-remote-event-server"
    {
        //      from the server - whien this is nr of port
        std::cout << "Before connecting to a DIAMANT remote SERVER " << std::endl;
        source_of_events = new TDiamantRevServParameter ( nam2, nam3 );   // online
        std::cout << "after connecting " << std::endl;
        flag_this_is_diamant_data = true;
    }
    else
#endif

        // if it is "-rem" the data will come online from the experiment.  +++++++++++++++
        if ( kawalek == "-rem" )  // "-remote-event-server"
        {
            //      from the server - whien this is nr of port
            std::cout << "Before connecting to a remote server " << std::endl;
            source_of_events = new TGo4RevServParameter ( nam2, nam3 );   // online
            std::cout << "after connecting " << std::endl;

        }

#if DIAMANT_PRESENT


        else if ( nam1.substr(0, 8) == "-fil_dia" )  // "-file diamant"
        {
            flag_this_is_diamant_data = true;
            //      from the server - whien this is nr of port
            flag_sorting_from_file = true; // important for timeout
            // TGo4MbsFileParameter*
            if(string_arguments[2].empty())
            {
                cout << "\n\nError: No argument with a diamant file to analyse? " << endl;
                exit(20);
            }else{
                source_of_events= new TDiamantFileParameter ( string_arguments[2] );
                std::cout << "starting event  ===============" << starting_event << std::endl;

                ( ( TDiamantFileParameter* ) source_of_events )->SetStartEvent ( starting_event );
            }
        }
#endif

        else   // data will come from the disk file, normally ++++++++++++++++
        {
            flag_sorting_from_file = true; // important for timeout
            // TGo4MbsFileParameter*
            source_of_events= new TGo4MbsFileParameter (string_arguments[1]);
            std::cout << "starting event  ===============" << starting_event << std::endl;

            ( ( TGo4MbsFileParameter* ) source_of_events )->SetStartEvent ( starting_event );
        }


    std::cout << "Connection with the source of events - established." << std::endl;

    //__________________ input data - established successfuly -------------------------

    // STEP 1
    TGalileoEventProcessor* procpar1= new TGalileoEventProcessor ;
    TGalileoCalibrateProcessor* procpar2 = new TGalileoCalibrateProcessor;

    TGalileoEvent *evptr1  =  new TGalileoEvent;
    /*    std::cout << "After creating the TGalileoEvent "
   << "object in address-------------------------->>>--> "
   << std::hex << (int) evptr1 << std::dec << std::endl;
   */
    TGalileoCalibratedEvent * evptr2 =  new TGalileoCalibratedEvent;

    set_all_steps ( procpar1, procpar2,
                    evptr1,
                    evptr2 );

    //================================================
    create_my_spectra();
    restore_times_of_zeroing_from_disk();

}
//*****************************************************************************
/** No descriptions */
void TGalileoAnalysis::create_my_spectra()
{
    // constructing the "spectra" for timestammps

}
//*****************************************************************************
TGalileoAnalysis::~TGalileoAnalysis()
{


    for ( uint i = 0 ; i < list_of_histograms.size() ; i++ )
    {
        delete  list_of_histograms[i];
    }

    for ( uint i = 0 ; i < spectra_user.size() ; i++ )
    {
        delete  spectra_user[i];
    }

    for ( uint i = 0 ; i < user_condition.size() ; i++ )
    {
        delete  user_condition[i];
    }

    delete source_of_events ;

    delete  step1;
    delete  step2;

    //     std::cout << "Before deleting the object in address" << step1_output_event << std::endl;
    delete  step1_output_event;
    delete  step2_output_event;

    step1 =  0;
    step2 = 0;
    step1_output_event = 0;
    step2_output_event =0 ;

}
//****************************************************************************
int TGalileoAnalysis::UserPreLoop()
{
    //// This function is processed once before the implicit loop.
    //// For online (gui-mode), it is called on every start of the analysis
    std::cout<< "*****  Galileo Analysis: User Preloop ***** "<<std::endl;
    info_to_grebware ( "Spy is reading current parameters" );

    // we update the pointers to the current event structurese here:

    // jurek
    //   std::cout << "Rezerwacja obiektow Event" << std::endl;
    //fxMbsEvent=0 ; //new TGo4MbsEvent;
    fxEbEvent  = step1_output_event;
    fxCaliEvent= step2_output_event;


    ptr_to_mbs_nr_of_the_current_event =
            step1->give_addres_of_mbs_event_number();

    // here we can sent pointers to the frs class
    gal.make_preloop_action ( fxEbEvent , fxCaliEvent );

#ifdef USER_INCREMENTERS_ENABLED
    // read in the definition of user incrementers
    read_in__user_incrementers();
    std::cout << "Successfuly defined incrementers..."<< std::endl;

#endif // #ifdef USER_INCREMENTERS_ENABLED


    //########################
    //################################################
    // storing list of names of variables for cracow Wizard
    std::ofstream plik(path.dir_commands() + "list_of_variables.txt");
    std::ofstream plikselfgates(path.dir_commands() + "list_of_variables_with_selfgates.txt");
    Tmap_of_named_pointers::iterator pos;
    for(pos = named_pointer.begin(); pos != named_pointer.end(); ++pos){
        plik << pos->first << std::endl;
        //std::cout << pos->first << " odpowiada_adres " << ( reinterpret_cast<long> (&(pos->second) )) << std::endl;
        auto drugi = pos->second ;
        //         std::cout << "UserPreLoop()   drugi " << drugi.what_type << std::endl;
        if(pos->second.ptr_detector != nullptr){
            if(pos->second.ptr_detector->selfgate_type_is_not_avaliable() == false  ) {
                plikselfgates
                        << pos->first 					// name of incremener
                        << "     "
                        << static_cast<int>(pos->second.ptr_detector->give_selfgate_type()) // code of enum
                        << std::endl;
            }
        }
    }


    //  read_in_verbose_parameters();     // for listing someof the events
    read_autosave_time();  // and so called -other options

    // adding my user spectra h-----------------
    read_definitions_of_user_spectra();

    std::cout << "\nAfter finishing to define the spectra, there are such conditions"
              << std::endl;
    for ( unsigned i = 0; i < user_condition.size(); i++ ) {
        std::cout << i << ") " <<  user_condition[i]->give_name() << std::endl;
    }

    cycle_on_real_data = false;  // initially
    return 0;
}
//****************************************************************************
int TGalileoAnalysis::UserPostLoop()
{
    //// This function is processed once after the implicit loop.
    //// For online (gui-mode), it is called on every stop of the analysis
    std::cout<< "***** Agata Analysis: User Postloop ***** "<<std::endl;
    // if this is batch (also grebware) mode
    time_t begin  = time ( 0 );
    gal.make_postloop_action();
    time_t end = time ( 0 );
    std::cout << "Saving spectra took " << end - begin << " seconds " << std::endl;

    // user defined spectra --------------------
    for ( unsigned i = 0; i < spectra_user.size(); i ++ )
    {
        spectra_user[i]->save_to_disk();
    }

    //delete fxMbsEvent;
    //fxMbsEvent=0; // reset to avoid invalid pointer if analysis is changed in between
    proc_remembered = 0;
    return 0;
}

//*****************************************************************************
int TGalileoAnalysis::UserEventFunc()
{
    //// This function is called once for each event.
    //// In addition to the event processor operations, eventwise analysis
    //// can be done here

    //      std::cout<< "***** Agata Analysis: User Event Func ***** "<<std::endl;



    // fill user histograms:
    int value=0;

    // user defined spectra --------------------
    if ( true  /*cycle_on_real_data */)
    {
        //         std::cout << "New event check ================================" << std::endl;


        // Here was the check of the user defined conditions. Now it is as a special function
        // called
        check_all_user_def_conditions();
        // and it is called earlier, on the end of the step2, because we need to know the result
        // earlier, just before storing the data in the n-tuple


        for ( unsigned i = 0; i < spectra_user.size(); i ++ )
        {
            //     std::cout << "Working with the user spectrum nr " << i
            //     << " called " << spectra_user[i]->give_name()
            //     << std::endl;
            spectra_user[i]->make_incrementations();
        }


        //       make_ratio_spectra();
    }
    cycle_on_real_data = false;
    return 0;
}

//****************************************************************************
bool TGalileoAnalysis::InitEventClasses()
{
    // std::cout << "fcja Initevent classes =======calling inheritated ====== "
    // << std::endl;

    return 777;
}
//****************************************************************************


//*****************************************************************************
void TGalileoAnalysis::CloseAnalysis()   // hiding the one from the base class
{
    // calling the base class version
    //  TGo4Analysis::CloseAnalysis();

    //std::cout << "- After   TGo4Analysis::CloseAnalysis(),
    // JUREK -----My, empty  version of Close Analysis ------" << std::endl;

}

//*****************************************************************************
TGalileoCalibrateProcessor* TGalileoAnalysis::give_call_processor_address()
{
    //std::cout << "Trying to acquire the current address of the callibration processor "
    //     << std::endl;
    //  fxCaliEvent=dynamic_cast<TRisingCalibratedEvent*> ( GetOutputEvent("Calibrate") );
    //   std::cout << " after funtion   GetOutputEvent(Calibrate) "<< std::endl;

    return 0;
}

//***************************** when **********************************************

/** this is such a version which replaces the RunImplicitLoop,
 but is able to handle the pause and continue exceptions */
int TGalileoAnalysis::run_jurek_ImplicitLoop(int times)
{

    const long int infinity = 999999999L;

    if(times == 0){
        times = infinity; // was bug in go4
    }
    // automatic resources =====================================================
    class spy_works{
        std::string nazwa;
    public:
        spy_works(){
            nazwa = path.dir_commands() + "spy_works.command";
            std::ofstream plik(nazwa.c_str());
            plik << "running" << std::endl;
            plik.close();
        }
        void tell_gui_that_spy_is_running(bool flag_running){
            std::ofstream plik(nazwa.c_str());
            plik << (flag_running ? "running" : "paused")  << std::endl;
            plik.close();
        }

        ~spy_works() {
            unlink(nazwa.c_str());
        }
    };

    spy_works automatic;


    //=========================================================================

    bool flag_pause = false;

    int curr_ev_nr = 0; // number of actually processed events
    try{
        user_batch();
        PreLoop();

        //         Message ( 0,"G-OOOO-> Analysis Implicit Loop for %d cycles is starting... <-OOOO-G",
        //                   times );

        last_ev_mbs_long_term_nr = 0;
        last_ev_analsyed_long_term_nr = 0;


        //times = 4;  // for testing
        for(curr_ev_nr = 0; curr_ev_nr < times  || (curr_ev_nr == infinity); ++curr_ev_nr){
            if(curr_ev_nr == infinity) curr_ev_nr = 0 ;

            try{
                // here we could check if there are some spectra to delete
                perhaps_delete_some_spectra();
                check_for_commands(curr_ev_nr);
                save_just_observed(); // only the selected spectra just observed in cracow


                // ------------ in PAUSED state ------------------
                if(flag_pause){
                    --curr_ev_nr;
                    // how often to look at the disk

                    static time_t last = time(0);
                    time_t now = time(0);
                    if(now - last > 15 /*3*/ ){
                        last = now;
                        // check on the disk for the command "continue"
                        std::string nazwa = path.dir_commands() + "spy_continue.command";
                        std::ifstream plik(nazwa.c_str());
                        if(plik){
                            info_to_grebware(" prepares to run again");

                            unlink(nazwa.c_str());
                            PreLoop();
                            flag_pause = false;
                            std::cout << "-------------------------- continue the analysis" << std::endl;
                            plik.close();
                            unlink(nazwa.c_str());    // second time, to be sure, because prelop takes time
                            automatic.tell_gui_that_spy_is_running(true);

                            last_ev_mbs_long_term_nr =  0;
                            last_ev_analsyed_long_term_nr = curr_ev_nr;

                            info_to_grebware("Spy prepares to run again");

                        } else{
                            std::cout << "Spy paused by the greWare viewer request" << std::endl;
                            info_to_grebware(" is PAUSED  on ", curr_ev_nr);
                        }
                    }
                } //----------------------------- end if flag pause
                else{
                    // I had a situation of the forgotten continue - which did not allowed
                    // to refresh in Greware, but analysis was running ok.
                    if(curr_ev_nr %25000 == false) {
                        unlink( (path.dir_commands() + "spy_continue.command").c_str());
                    }



                    /** Flag telling that now we want to dump information about the current event */
                    //     if(verbose_mode_enabled)
                    //     {
                    //      if(i >= verbose_begins_with_event_nr
                    //        &&
                    //        i < (verbose_begins_with_event_nr + verbose_how_many_events)
                    //        )
                    //             {
                    //             flag_verbose_events = true;
                    //             // std::cout << "======= CyEvent nr " << i << " verbose listing =========" << std::endl;
                    //             // i is not an event nr. Events are those which pass to the calibration step.
                    //             }
                    //       else
                    //         { flag_verbose_events = false;}
                    //     }
                    perhaps_conditions_report();

                    MainCycle();

                    perhaps_info_of_events(curr_ev_nr);

                    //============== RATIO DISPLAY ===============
                    //                     static int print_ratios_every_n_events = 5000;
                    //                     static time_t previous_print_ratios_time = time(NULL);
                    //                     static int previous_print_event_nr = 0 ;

                    if(!(curr_ev_nr % print_ratios_every_n_events))
                    {
                        display_procent_ratios_and_speed(curr_ev_nr);

                    } // endif how often display rates


                    //=============================================

                    static time_t   time_last = time(0);    // so always at first save

                    time_t  time_now = time(0);

                    if(time_now - time_last > (autosave_period_in_minutes * 60))
                    {
                        info_to_grebware(" makes autosave");
                        PostLoop();
                        std::cout << "Autosave from  - finished (every " << autosave_period_in_minutes
                                  << " minutes) - as decided in greware Spy-options->Other SPY options." << std::endl;
                        time_last = time_now;
                        info_to_grebware(" finished autosave");
                    }

                    //save_just_observed(); // only the selected spectra just observed in cracow

                } // end else --- not paused

            } // end try
            //-------------------------

            //----------------------------
            catch(Tgrebware_exception & kkk)
            {
                if(kkk.message == "pause")
                {
                    info_to_grebware("is PAUSED  on ", curr_ev_nr);
                    PostLoop();
                    flag_pause = true;
                    automatic.tell_gui_that_spy_is_running(false);
                }
                else  if ( kkk.message == "pause_quick" )
                {
                    info_to_grebware ( "is PAUSED  on ", curr_ev_nr );
                    std::cout << "skipping spectra saving " << std::endl; // ----> no saving spectra in this option    PostLoop();
                    flag_pause = true;
                    automatic.tell_gui_that_spy_is_running ( false );
                }
                else  if ( kkk.message == "continue" )
                {
                    info_to_grebware ( "is continuing  on ", curr_ev_nr );
                    PreLoop();
                    //std::cout << "skipping spectra saving " << std::endl; // ----> no saving spectra in this option    PostLoop();
                    flag_pause = false;
                    automatic.tell_gui_that_spy_is_running ( true );
                }
                else  if ( kkk.message == "update" )
                {
                    info_to_grebware ( "is updating  on ", curr_ev_nr );
                    PostLoop();
                    PreLoop();
                    //std::cout << "skipping spectra saving " << std::endl; // ----> no saving spectra in this option    PostLoop();
                    flag_pause = false;
                    automatic.tell_gui_that_spy_is_running ( true );
                }
                else
                {
                    perhaps_conditions_report(true);    // parforce
                    throw;  // probably 'finish'
                }
            }
            //----------------------------
            //----------------------------
            catch(std::bad_alloc & ex)      // treat standard library exceptions
            {

                //                std::cout << "ERROR: Not enough memory to reserve the spectrum! " << std::endl;
                //                 Message ( 0,"G-OOOO-> (1.9) bad_alloc exception %s appeared after %d cycles.  <-OOOO-G",
                //                           ex.what(), i );
            }
            catch(std::exception& ex)        // treat standard library exceptions
            {

                //                 Message ( 0,"G-OOOO-> (2) standard exception %s appeared after %d cycles.  <-OOOO-G",
                //                           ex.what(),i );
            }

            //----------------------------------
            catch(TjurekEventEndException & obj)
            {
                std::cout << "TjurekEventEndException:: End of events in the file " << std::endl;
                info_to_grebware(" End of events in the file, saving ");
                PostLoop();
                info_to_grebware(" finished events");
                throw;
            }
            catch(Texception_input_file &obj)
            {
                if(obj.error_nr ==   myEOF)
                {
                    std::cout << "End of events in the file " << std::endl;
                    info_to_grebware(" End of events in the file, saving ");
                }

                PostLoop();
                info_to_grebware(" finished events");
                throw;
            }
            catch(...)
            {
                //std::cout << "Unknown exception - ignored (timeout ?, no events after "
                // << i << " cycles ?)" << std::endl;
                if(!flag_sorting_from_file)
                {
                    std::cout << "Timeout ?  no events comming from MBS now... (after "
                              << curr_ev_nr << " cycles )" << std::endl;
                    info_to_grebware("Timeout? No events from MBS now...  ", curr_ev_nr);
                    automatic.tell_gui_that_spy_is_running(true);     // when MBS gives constant Timeout
                    // cracow would disable the icons of SPY
                    perhaps_delete_some_spectra();
                }
                else
                {
                    std::cout << "Unknown exception - after " << curr_ev_nr << " cycles ?)"
                              << " (can be also when timestamp is noticed as corrupted )" << std::endl;
                    //     std::cout << "Unknown exception - ignored (timeout ?, no events after "
                    //<< i << " cycles ?)" << std::endl;
                    //     std::cout << "End of file ?????? " << std::endl;
                    // throw;
                }
            }
            ////// end inner catch


        }// for
        // return 0; //times = 0;
        //         Message ( 0,"G-OOOO-> Analysis Implicit Loop has finished after %d cycles. <-OOOO-G",
        //                   i );
        PostLoop();
    } //  try


    catch(Texception_input_file & kkk)
    {
        if(kkk.error_nr == myEOF)
        {
            //info_to_grebware ( " saves spectra before finishing" );
            //PostLoop();  // << -was done above in (...) catch
        }
    }

    catch(Tgrebware_exception & kkk)
    {
        if(kkk.message == "finish")
        {
            info_to_grebware(" saves spectra before finishing");
            PostLoop();  // << -was done above in (...) catch
        }
        else
        {
            std::cout << "unrecognized mesaage from Tgrebware_exception " << std::endl;
            perhaps_conditions_report(true);    // parforce
            throw;
        }
    }
    catch(std::bad_alloc & ex)        // treat standard library exceptions
    {

        std::cout << "ERROR: Not enough memory to reserve the spectrum or buffer ! " << std::endl;
        //         Message ( 0,"G-OOOO-> (1.9) bad_alloc exception %s appeared after %d cycles.  <-OOOO-G",
        //                   ex.what(), i );
    }
    catch(std::exception& ex)        // treat standard library exceptions
    {
        std::cout << "Standard  exception after " <<  curr_ev_nr << " cycles !!!  <-OOOO-G" << std::endl;
        //         Message ( 0,"G-OOOO-> (2) standard exception %s appeared after %d cycles.  <-OOOO-G",
        //                   ex.what(),i );
    }

    catch(...)
    {
        std::cout << "Unexpected exception after " <<  curr_ev_nr << " cycles !!!  <-OOOO-G"  << std::endl;
        //         Message ( 0,"G-OOOO-> !!! (1) Unexpected exception after %d cycles !!!  <-OOOO-G", i );
    }
    perhaps_conditions_report(true);    // parforce
    /////////// end outer catch block
    info_to_grebware("Finished after ", curr_ev_nr);
    return curr_ev_nr;
}
//***************************************************************************

//***************************************************************************
/**
 * @brief every  5000 events or every 10 seceonds on the screen we dispaly
 * the statistics of events (how many percent of all incoming data is ananlysed
 * and  what is the speed (events per second)
 *
 * @param curr_ev_nr   number of currently analysed event
 * @return void
 **/
void TGalileoAnalysis::display_procent_ratios_and_speed(const int curr_ev_nr)
{

    //============== RATIO DISPLAY ===============
    static int previously_displayed_ev_mbs_nr = 0;
    time_t now = time(nullptr);

    if(previously_displayed_ev_mbs_nr != 0)      // to avoid first print
    {
        static time_t  speed_ratio__time_before = 0;
        std::cout << "\n--- Read " << curr_ev_nr << " events ";


        double recent_ratio =
                (100.0 * (curr_ev_nr - previous_print_event_nr))
                /
                ((*ptr_to_mbs_nr_of_the_current_event) - previously_displayed_ev_mbs_nr);


        double ratio_long_term =
                (100.0 * (curr_ev_nr - last_ev_analsyed_long_term_nr))
                /
                ((*ptr_to_mbs_nr_of_the_current_event) - last_ev_mbs_long_term_nr);

#if 0
        if(recent_ratio < 0)
        {
            std::cout << "NEGATIVE recent_ratio? "
                      << " curr_ev_nr = " << curr_ev_nr
                      << " previous_print_event_nr = " << previous_print_event_nr
                      << " *ptr_to_mbs_nr_of_the_current_event = " << (*ptr_to_mbs_nr_of_the_current_event)
                      << " previously_displayed_ev_mbs_nr = " << previously_displayed_ev_mbs_nr
                      << std::endl;

        }

        if(ratio_long_term < 0)
        {
            std::cout << "NEGATIVE ratio_long_term? "
                      << " curr_ev_nr = " << curr_ev_nr
                      << " last_ev_analsyed_long_term_nr = " << last_ev_analsyed_long_term_nr

                      <<  "(roznica = " << (curr_ev_nr - last_ev_analsyed_long_term_nr)
                       << "\nn *ptr_to_mbs_nr_of_the_current_event = " << (*ptr_to_mbs_nr_of_the_current_event)
                       << " last_ev_mbs_long_term_nr = " << last_ev_mbs_long_term_nr
                       <<  "  roznica = " << ((*ptr_to_mbs_nr_of_the_current_event) - last_ev_mbs_long_term_nr)
                        << "\n in result " << ratio_long_term
                        << std::endl;
            //exit(33);

        }
#endif
        // Long term statistics
        if(last_ev_mbs_long_term_nr != 0)
        {
            std::cout
                    << ", (ratio "
                    << ((ratio_long_term > 98)? 100.0 : ratio_long_term )
                    << "% of all coming)"
                    << "   [recently "
                    << ((recent_ratio > 98)? 100.0 : recent_ratio )
                    << "%]   ";
            //--- speed ---------------------
            static int previous_speed_event_nr = 0;
            int speed__time_diff = now - speed_ratio__time_before;
            int how_many_seconds_to_calculate_speed = 2;

            if(speed__time_diff >= how_many_seconds_to_calculate_speed)
            {
                double speed = (1.0 * (curr_ev_nr - previous_speed_event_nr)  / speed__time_diff);
                if(speed > 1)
                {
                    std::cout
                            //                    <<  "(curr_ev_nr - previous_print_event_nr) = " <<  (curr_ev_nr - previous_print_event_nr)
                            //          << " how_many_seconds_to_calculate_speed = " << how_many_seconds_to_calculate_speed
                            //           <<  " diff = " <<  speed_ratio__time_diff
                            //            << "\n(1.0 * (curr_ev_nr - previous_print_event_nr)  / diff)     "
                            << (1.0 * (curr_ev_nr - previous_speed_event_nr)  / speed__time_diff)
                            << " ev/s ";
                }
                speed_ratio__time_before = now;
                previous_speed_event_nr = curr_ev_nr;
            }
            else
            {
                //std::cout << " speed_ratio__time_diff " << speed__time_diff << std::endl;
            }

            int since_last_display = now - previous_print_ratios_time;
            if(since_last_display < 2)      // every n seconds
            {
                //print_ratios_every_n_events += 3000;
                print_ratios_every_n_events *= 2;
                //std::cout << "----------Mult Changed print_every_n_events, now is " << print_ratios_every_n_events << std::endl;
            }
            else if(since_last_display < 10)        // every n seconds
            {
                print_ratios_every_n_events += 3000;
                //                                  print_ratios_every_n_events *= 2;
                //                                  std::cout << "----------Added  print_every_n_events, now is " << print_ratios_every_n_events << std::endl;
            }

            previous_print_event_nr = curr_ev_nr;
            previous_print_ratios_time = now;

            //if(ntuple_RAW_is_collected)
            //  {
            //std::cout << "\nNotice: The option to produce the n - tuple file(*_RAW_tree.root) is  ON.";
            // comented because the proper text is displayed form tree making function ;
            //   }
            //   if(ntuple_CAL_is_collected)
            //  {
            //std::cout << "\nNotice: The option to produce the n - tuple file(*_CAL_tree.root) is  ON.";
            //   }

        }
        else
        {
            last_ev_mbs_long_term_nr =  *ptr_to_mbs_nr_of_the_current_event;
            last_ev_analsyed_long_term_nr = curr_ev_nr;
        }
        std::cout  << std::endl;
    } // endif how often display rates
    previously_displayed_ev_mbs_nr = *ptr_to_mbs_nr_of_the_current_event;
}
//***************************************************************************
/** GreWARE viewer may send request to delete list of spectra */
void TGalileoAnalysis::perhaps_delete_some_spectra()
{

    static int how_many_wrongly_finished_command_files;
    static time_t last = 0;
    time_t now = time ( 0 );
    std::string full_name_of_spectrum;

    if ( now-last > 2 ) // every 3 seconds
    {
        last = now;
        // check on the disk for the command "continue"

        std::string nazwa =path.dir_commands() + "spy_spectra_for_zeroing.command";
        std::ifstream plik ( nazwa.c_str() );
        if ( plik )
        {
            info_to_grebware ( " is deleting spectra" );
            std::string spec_name;
            while ( plik )
            {
                plik >> spec_name;
                if ( !plik )
                {
                    // this trick is made to compatibility with the old grebware programs
                    how_many_wrongly_finished_command_files++;
                    std::cout << "Error while reading the spec_name";
                    if ( how_many_wrongly_finished_command_files < 20 )
                    {
                        // most probably file is not finished yet - come later here ! (return without deleting the file)
                        return;
                    }
                    how_many_wrongly_finished_command_files = 0;
                    break;  // for old grebware - after 10 tries - give up !
                }// if plik
                std::cout << "Zeroing spectrum: " << spec_name << "\n" ;

                if ( spec_name == "this_is_the_end_of_the_list_of_spectra_which_greware_wants_to_zero"
                     || spec_name == "this_is_the_end_of_the_list_of_spectra_which_cracow_wants_to_zero" )
                {
                    break;  // this was the last spectrum to delete
                }
                full_name_of_spectrum = spec_name;
                // remove extension
                std::string::size_type pos = spec_name.rfind ( ".mat" );
                if ( pos == std::string::npos )
                    pos = spec_name.rfind ( ".spc" );
                if ( pos != std::string::npos )
                    spec_name.erase ( pos, 100 );

                //std::cout << "Zeroing spectrum " << spec_name << std::endl;
                TH1 * wskaznik = GetHistogram ( spec_name.c_str() );

                if ( wskaznik )
                {
                    wskaznik->Reset();
                    store_time_of_zeroing_spectrum ( full_name_of_spectrum, now );
                }
                else
                {
                    std::cout << "!!!!!!!!!!!!!!  Spectrum " << spec_name
                              << " is not currently produced by the spy !!!!!!!!!!!!!!!!!!!!"
                              << std::endl;
                }
                // " After Zeroing " << std::endl;
            } // while plik
            unlink ( nazwa.c_str() );
            //info_to_grebware(" is saving new, deleted specta");
            //PostLoop();  // saving the new zeroed version
            plik.close();
            std::cout << std::endl;   // flushing
            save_just_observed ( true ); // means NOW, do not wait for 10 s period
            save_times_of_zeroing_on_disk();
        } // if plik
    }
}
//**************************************************************************
/** If the grebware viewer is just observing some spectra, we will save them
 more often */
void TGalileoAnalysis::save_just_observed ( bool unconditionally )
{
    static int matrix_save_multiplier;

    if ( unconditionally )
        matrix_save_multiplier = 0;


    static time_t last = 0;
    time_t now = time ( 0 );

    if ( now-last > 10 // every 10 seconds
         ||                 // OR
         unconditionally ) // when we must save NOW (just after deleting)
    {
        last = now;
        // check on the disk for the command "continue"
        //        std::cout << "Saving spectra currently watched by grebware..." << std::flush;
        std::string nazwa =path.dir_commands() + "spectra_currently_watched.names";
        std::ifstream plik ( nazwa.c_str() );
        if ( plik )
        {
            std::string spec_name;
            while ( plik ){
                plik >> spec_name;
                if ( !plik )
                    break;
                // remove extension
                std::string::size_type pos = spec_name.rfind ( ".mat" );
                if ( pos == std::string::npos ) {
                    pos = spec_name.rfind ( ".spc" );
                }
                else {
                    // if it was matrix, check now - to not to save it too often
                    if ( ( matrix_save_multiplier % 1 ) ) { // 1 times 'seldomer'
                        continue;
                    }
                }

                if ( pos != std::string::npos )
                    spec_name.erase ( pos, 100 );

                // the trick below is to stop searching if it was found
                if ( gal.save_selected_spectrum ( spec_name ) )
                {}
                // // //                 else if ( rising.save_selected_spectrum ( spec_name ) )
                // // //                     {}

                else if ( save_analysis_local_spectrum ( spec_name ) )
                {}
                else{

                    if(spec_name.find("@over") == string::npos
                            &&
                            spec_name.find("_projection_") == string::npos )
                    {
                        std::cout << "Spectrum " << spec_name
                                  << " not found, among spectra currently produced by the spy"
                              << std::endl;

                    }
                }
            }

            // unlink(nazwa);
            // PostLoop();  // saving the new zeroed version
            plik.close();
            //  std::cout << " done." << std::endl;

        }
        matrix_save_multiplier++;
    }
}
//***********************************************************************
/** If some spectra are defined here in the analysis class */
bool TGalileoAnalysis::save_analysis_local_spectrum ( std::string specname )
{
    for ( unsigned i = 0; i < spectra_local.size(); i++ ){
        if ( specname == spectra_local[i]->give_name() ){
            spectra_local[i]->save_to_disk();
            return true;
        }
    }

    for ( unsigned i = 0; i < spectra_user.size(); i++ ){
        if ( specname == spectra_user[i]->give_name() ){
            spectra_user[i]->save_to_disk();
            return true;
        }
    }
    return false;
}


//**************************************************************************
/** to checking if greware gui is giving any pause/go command */
void TGalileoAnalysis::check_for_commands ( int nr_events )
{
    //  static time_t   time_last = time(0);

    time_t  time_now = time ( 0 );

    // here we can try if the grebware viewer asked to finish the analysis
    // we look at this every 5 seconds

    int command_check_period_in_seconds = 2;
    static time_t   time_last_command_check = time ( 0 );

    if ( time_now - time_last_command_check > command_check_period_in_seconds ){
        time_last_command_check = time_now;
        //    std::cout << "Command check done " << command_check_period_in_seconds
        //        << " seconds)" << std::endl;

        //---------------
        bool flag_spy_asked_to_quit = 0;
        //     info_to_grebware(" is running ", nr_events);


        //      ofstream plikinfo( path.dir_commands() + "spy_events.info");
        //      if(plikinfo &&  flag_spy_asked_to_quit == false )
        //      {
        //        plikinfo << "Spy is running, " << nr_events << " events "<< std::endl;
        //      }
        //      plikinfo.close();




        std::ifstream plik1 (  path.dir_commands() + "spy_finish.command" );
        if ( plik1 )
        {

            info_to_grebware ( "Finished after ", nr_events );

            flag_spy_asked_to_quit = true;

            std::cout << "Throwing the grebware exception" << std::endl;
            plik1.close();
            unlink
                    (  (path.dir_commands() + "spy_finish.command").c_str() );

            Tgrebware_exception kkk;
            kkk.message = "finish";
            throw kkk;
        }

        //---------------
        std::ifstream plik2 (  path.dir_commands() + "spy_pause.command" );
        if ( plik2 )
        {
            plik2.close();
            unlink
                    (  (path.dir_commands() + "spy_pause.command").c_str() );


            info_to_grebware ( "Spy: asked to pause after ", nr_events );

            std::cout << "Throwing the grebware pause exception" << std::endl;
            Tgrebware_exception kkk;
            kkk.message = "pause";
            throw kkk;
        }
        std::ifstream plik3 (  path.dir_commands() + "spy_continue.command" );
        if ( plik3 )
        {
            plik3.close();
            unlink
                    (  (path.dir_commands() + "spy_continue.command").c_str() );
            info_to_grebware ( "Spy: asked to continue after ", nr_events );
            std::cout << "Throwing the grebware pause exception" << std::endl;
            Tgrebware_exception kkk;
            kkk.message = "continue";
            throw kkk;
        }
        // the exception continue is checked in the jurek_implit_loop()
        std::ifstream plik4 (  path.dir_commands() + "spy_update.command" );
        if ( plik4 )
        {
            plik4.close();
            unlink
                    (  (path.dir_commands() + "spy_update.command").c_str() );
            info_to_grebware ( "Spy: asked to update  ", nr_events );
            std::cout << "Throwing the grebware update exception" << std::endl;
            Tgrebware_exception kkk;
            kkk.message = "update";
            throw kkk;
        }

    }
}
//*******************************************************************
/** sending an info message to the greWARE GUI */
void TGalileoAnalysis::info_to_grebware ( std::string s, int i )
{
    std::ofstream plikinfo (  path.dir_commands() + "spy_events.info" );
    if ( plikinfo )
    {
        plikinfo << "SPY: " << s;
        if ( i != -1 )
        {
            plikinfo << i
                     << " (created "
                        //  << " SUBevents read ("
                     << nr_events_analysed
                     << " Events)"
                        ;
        }
        plikinfo << std::endl;
        plikinfo << time ( 0 ) << std::endl;
    }
    plikinfo.close();

}
//********************************************************************
/** No descriptions */
void TGalileoAnalysis::perhaps_info_of_events ( int nr_events )
{
    // static time_t   time_last = time(0);
    time_t  time_now = time ( 0 );


    int command_check_period_in_seconds = 2;
    static time_t   time_last_command_check = time ( 0 );
    if ( time_now - time_last_command_check > command_check_period_in_seconds ){
        time_last_command_check = time_now;
        info_to_grebware ( " is analysing, ",  nr_events );
    }
}

//**************************************************************
//**************************************************************
/** In preLoop moment */
void TGalileoAnalysis::read_definitions_of_user_spectra()
{
    Tuser_condition::owner = this;

    //====================================================
    // BUT AT FIRST WE READ USER CONDITIONS DEFINITONS ---------
    // retrieving the names.
    std::vector<std::string> names =
            FH::find_files_in_directory (path.dir_conditions(),
                                         ".cnd" );

    // we always build new condition vector, so perhaps we should delete the conditions
    // from the old version
    for ( unsigned i = 0; i < user_condition.size(); i++ )
    {
        delete user_condition[i];
    }
    user_condition.clear();


#ifdef NIGDY

    for ( unsigned nr = 0; nr < names.size(); nr++ )
    {
        std::cout << "User defined condition definition file :" << names[nr]
                     << std::endl;
        create_user_condition ( names[nr] );
    }
#endif


    //========================================================
    // NOW WE REALLY READ USER SPECTRA DEFINIIONS ------------
    // retrieving the names.
    names =
            Tfile_helper::find_files_in_directory ( path.dir_user_def_spectra(),
                                                    ".user_definition" );

    // loop which is looking into the directory for a specified definions *.

    for ( unsigned nr = 0; nr < names.size(); nr++ )
    {
        //        std::cout << "User defined spectrum definition file :" << names[nr] <<
        //                     std::endl;
        create_user_spectrum ( names[nr] );
    }
}
//****************************************************************************************************************
/** No descriptions */
void TGalileoAnalysis::create_user_spectrum ( std::string name_of_description )
{

    std::string pathed_name_of_description = path.dir_user_def_spectra() + name_of_description;

    //  std::cout << "In the std::string " << name
    //        << " lookin for the extension "
    //        << std::endl;

    std::string name_without_extension = name_of_description;

    std::string::size_type  extension_starts =
            name_of_description.find ( ".user_definition" );

    if ( extension_starts == std::string::npos )
    {
        std::cout << "In the std::string " << name_of_description
                  << " the extension was not found"
                     //<< " this should never happen "
                  << std::endl;
        //return;
    }
    else
    {
        name_without_extension = name_of_description.erase ( extension_starts,
                                                             100 );
    }

    user_spectrum_description  desc;
    desc.set_name ( pathed_name_of_description );
    desc.read_from ( pathed_name_of_description );

    //check if such a spectrum already exists in our vector
    bool known = false;
    int position =  0;
    Tuser_spectrum *spec = 0;

    for ( unsigned i = 0; i < spectra_user.size(); i ++ )
    {

        //    std::cout << "searching ih the table of specta: current item is "
        //          << spectra_user[i]->give_name() << std::endl;
        if ( spectra_user[i]->give_name() == name_without_extension )
        {
            known = true;
            position = i;
            spec = spectra_user[i];
            break;
        }
    }


    bool should_be_created = false;

    if ( !known )
    {
        // if not = create it and put into the vector
        should_be_created = true;
    }
    else // if it is known
    {
        Tuser_spectrum *uspec = dynamic_cast<Tuser_spectrum *>
                ( spectra_user[position] );
        //  check dimmension, binning, beg, end (on x and y if necessery)

        if ( desc.are_parameters_identical ( uspec->give_description() ) == false )
            // ONLY BINNING etc  was tested. (Not enable, incrementers!)
        {
            // if different:  delete the old spectrum from root,
            //    !!!! not important - the function create is doing this !!!

            // delete this entry from spectum list (user conditional spectra)
            spectra_user.erase ( spectra_user.begin() + position );

            //and create the new one
            should_be_created = true;
        }
    }

    //---------------------------------------
    if ( should_be_created )
    {
        //        std::cout << "user Spectrum had to be created " << std::endl;
        spec = new Tuser_spectrum();
        spec->read_in_parameters ( name_without_extension );  // the path will be added automatically

        spec->create_the_spectrum();
        // during creation the spectrum registers itself
        // by calling  "remember_user_spectrum" from this class


        // put spectrum on the list user spectra
        spectra_user.push_back ( spec );
    }
    else
    {
        // above spec was created with:  new Tspectrum_1D_conditional;
        //        std::cout << "Spectrum already existed" << std::endl;
        // somebody may change the enable/disable/incrementer list
        spec->read_in_parameters ( name_without_extension );  // the path will be added automatically

    }


    //==============================================
    // attaching the conditon - we do it always, because
    // vector of conditions is evertime new
    //==============================================

    // here we should find the condition desired by this spectrum
    // and send its address


    std::string cn = ( spec->give_description() ).give_conditon_name();

    std::string::size_type pos = cn.rfind ( ".cnd" );
    if ( pos != std::string::npos )
        cn.erase ( pos, 100 );


    //    std::cout << "Trying to assign a condition named : " << cn << std::endl;
    if ( cn == "No_condition" )
    {
        spec->remember_address_of_condition ( 0 );
    }
    else
    {
        unsigned i = 0;
        for ( i = 0; i < user_condition.size(); i++ )
        {
            if ( user_condition[i]->desc.give_name() == cn )
            {
                spec->remember_address_of_condition ( user_condition[i] );
                break;
            }
        }

        if ( i >=user_condition.size() )
        {

            //            std::cout << "Condition with the name " << cn << " not found on the list"
            //                      << " try to read it from the disk " << std::endl;

            // if not existing, try to create it
            spec->remember_address_of_condition (
                        create_user_condition ( ( spec->give_description() ).give_conditon_name() ) );
        }

    } // end else no condition
}
//******************************************************************************
/** Conditions live independently. One condition can be shared by many spectra */
Tuser_condition*  TGalileoAnalysis::create_user_condition ( std::string name_of_description )
{

    std::string pathed_name_of_description =  path.dir_conditions() + name_of_description;

    //  std::cout << "In the std::string " << name
    //        << " lookin for the extension "
    //        << std::endl;

    std::string name_without_extension = name_of_description;
    std::string::size_type  extension_starts = name_of_description.find ( ".cnd" );
    if ( extension_starts != std::string::npos )
    {
        name_without_extension =
                name_of_description.erase ( extension_starts, 100 );
    }


    // Check if it is not the snake situation

    static int level_of_nesting;
    static std::vector<std::string> nesting_trace;
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    for ( unsigned int i = 0; i < nesting_trace.size(); i++ )
    {
        if ( nesting_trace[i] == name_without_extension )
        {

            std::cout << "\nCondition nesting trace: \n";
            for ( unsigned int k = 0; k < nesting_trace.size(); k++ )
            {
                std::cout << "(" << k
                          << ") " << nesting_trace[k] << " --> ";
            }
            std::cout << name_without_extension;

            std::cout << "\nERROR: Infinite nesting of the conditions at the level "
                      << level_of_nesting
                      << "\n - the condition called " << name_without_extension
                      << " was already on the level " << i
                      << "\n This \"Snake is eating its own tail\" " << std::endl;
            exit ( 1 );

        }
    }

    level_of_nesting++;
    if ( level_of_nesting > 1000 )
    {
        std::cout << "Nesting the conditions reach the level " << level_of_nesting
                  << "\n - it is unrealistic, most probably there is a situation "
                     "\n 'Snake is eating its own tail' " << std::endl;
        exit ( 1 );
    }


    nesting_trace.push_back ( name_without_extension );

    //  conditions are not registered in root, they do not have to have
    //  any continuity after any PAUSE, so we can create them new everytime


    Tuser_condition *c = new Tuser_condition();
    c->read_in_parameters ( name_without_extension );  // the path will be added automatically
    // here above the condition can be nested with the name of other conditions, which will be created.

    // during creation the spectrum registers itself
    // by calling  "remember_user_spectrum" from this class

    // put spectrum on the list user spectra
    user_condition.push_back ( c );

    level_of_nesting--;
    nesting_trace.pop_back();
    return c;

}
//************************************************************************

//***********************************************************************
/** No descriptions */
std::string TGalileoAnalysis::give_current_filename()
{
    // this function should return of currently analysed disk file

    // std::string nazwa = "NotImplemented.Yet";
    return source_of_events->give_name_of_source() ;
}
//**********************************************************************
/** No descriptions */
Tuser_condition* TGalileoAnalysis::give_ptr_to_cond ( std::string cname )
{
    //std::cout << "Searching for cond node " << cname << std::endl;
    for ( unsigned i = 0; i < user_condition.size(); i++ )
    {
        //std::cout << "    " << user_condition[i]->give_name()<< std::endl;
        if ( cname == user_condition[i]->give_name() )
            return user_condition[i];
    }


    return    create_user_condition ( cname );
}

/**********************************************************************/
// when par_force is true - we do not wait for 15 seconds period, but
// just do int immediately
void TGalileoAnalysis::perhaps_conditions_report ( bool par_force )
{
    time_t  time_now = time ( 0 );
    const int period_in_seconds = 15;
    static time_t   time_last = time ( 0 );
    if ( par_force || ( time_now - time_last > period_in_seconds ) )
    {
        time_last = time_now;


        struct tm * timeinfo;
        timeinfo = localtime ( &time_now );
        std::ostringstream iii;
        iii
                << ( par_force?  " (par force) " : " (normal) " )
                << "Statistics \"Tested/True\" made at "
                <<  asctime ( timeinfo )
                    ;
        std::string information = iii.str();


        std::ofstream plik ( path.dir_conditions() + "report.txt" );

        plik << information << "\n";

        for ( unsigned i = 0; i < user_condition.size(); i++ )
        {
            plik << user_condition[i]->give_name()
                 << "\t"
                 << user_condition[i]->give_how_many_tested()
                 << "\t" << user_condition[i]->give_how_many_true()
                 << "\n";

        }
        plik.close();

    }
}
//************************************************************************************

//******************************************************************************************************
/** No descriptions */
void TGalileoAnalysis::incr_synchronisation_matices()
{
    //int f = gal.give_synchro_data();

}
//**********************************************************************************************

//*****************************************************************************************
/** Autosave - all spectra may be saved every 20 minutes  */
void TGalileoAnalysis::read_autosave_time()
{
    std::string fname = path.dir_options() + "other_options.dat";
    std::ifstream plik ( fname.c_str() );

    if ( !plik ) {
        std::cout << "file does not exist, so I assume default state" << std::endl;
        autosave_period_in_minutes = 60;  // every hour
        return;
    }

    try {
        autosave_period_in_minutes =
                ( int ) Tfile_helper::find_in_file<int>( plik, "autosave_period_in_minutes" );
    }
    catch ( ... ) {
        autosave_period_in_minutes = 60;
    }
    plik.close();
    return;

}
//******************************************************************************
/** This function calculates the value of all the user defined conditions.
 It may be used by the user defined spectra, but also as the decision
 if the event should go to the ntuple. */
void TGalileoAnalysis::check_all_user_def_conditions()
{

    for ( unsigned i = 0; i < user_condition.size(); i ++ )
        user_condition[i]->new_event_init();

    for ( unsigned i = 0; i < user_condition.size(); i ++ ) {
        //     std::cout << "============ Checking the value of the condition nr "
        //     << i
        //     << " called "
        //     << user_condition[i]->desc.give_name()
        //     << std::endl;

        user_condition[i]->calculate_value_of_condition();

        //     std::cout << "Condition nr "
        //     << i
        //     << " called "
        //     << user_condition[i]->give_name()
        //     << " is  "
        //     << (user_condition[i]->give_result() ? " TRUE " : " FALSE ")
        //     << std::endl;
    }
}

//*****************************************************************************
/** No descriptions */
void TGalileoAnalysis::user_batch()
{
    std::string next_name  = give_current_filename();


    // finding the extension
    std::string::size_type kropka  = next_name.rfind ( ".adf" );
    if ( kropka != std::string::npos )
    {
        next_name.erase ( kropka );
        //    std::cout << "Po wymazaniu extension " << next_name << std::endl;
    }
    // erasing the path
    std::string::size_type slash  = next_name.rfind ( "/" );
    if ( slash != std::string::npos )
    {
        next_name.erase ( 0, slash+1 );
    }
    next_name += ".bat" ;
    //  std::cout << "Name of the batch =>" << next_name << "<" << std::endl;
    std::string komenda = "source " + next_name;
    std::cout << "Trying to execute a user batch file =>" << komenda << "<  (if it exists)" << std::endl;

    ifstream file(next_name);
    if(file){
        cout << "File exists = " << next_name << endl;
        file.close();
        system ( komenda.c_str() );
    }else{ cout << "The batch file does not exist = " << next_name << endl;}

    //  std::cout << "Result was = " << result << std::endl;
}
//***********************************************************************
void TGalileoAnalysis::save_times_of_zeroing_on_disk()
{
    //return ; // because destroy does not work properly (see below)
    std::cout << "F. save_times_of_zeroing_on_disk() " << std::endl;

    std::ofstream plik ( path.dir_spectra() + "zeroing_times.bin", std::ios::binary );
    if ( !plik )
    {
        std::cout << "Can't open file  for storing zeroing times " << std::endl;
        return;
    }

    std::string nazwa_wid;
    time_t when = 0 ;
    const unsigned int max_length_of_name = 200;

    std::map<std::string, time_t >::iterator it =
            map_of_zeroing.begin();

    for ( ; it != map_of_zeroing.end() ; it++ )
    {

        nazwa_wid = it->first ;
        if ( nazwa_wid.size() > max_length_of_name )
            continue;

        char tablica[max_length_of_name] = { 0 } ;  // name can be max for ex. 200 bytes long
        nazwa_wid.copy ( tablica, nazwa_wid.size() );
        plik.write ( ( char* ) tablica, sizeof ( tablica ) );

        when = it->second ;
        plik.write ( ( char* ) &when, sizeof ( when ) );
        if ( !plik )
            break;
    }
}
//***********************************************************************
void TGalileoAnalysis::restore_times_of_zeroing_from_disk()
{
    //     std::cout << "F. restore_times_of_zeroing_from_disk() " << ((int)time(0)) << std::endl;

    map_of_zeroing.clear();
    std::ifstream plik ( path.dir_spectra() + "zeroing_times.bin", std::ios::binary );
    if ( !plik )
    {
        std::cout << "Can't open file  for restoring zeroing times " << std::endl;
        return;
    }

    std::string nazwa_wid;
    time_t when = 0 ;
    const int max_length_of_name = 200;

    while ( plik ) // for(unsigned int i = 0 ;/* i < map_of_zeroing.size()*/ ; i++)
    {
        char tablica[max_length_of_name] = { 0 } ;  // name can be max for ex. 200 bytes long
        plik.read ( ( char* ) tablica, sizeof ( tablica ) );
        nazwa_wid = tablica;

        plik.read ( ( char* ) &when, sizeof ( when ) );
        if ( !plik )
            break;
        map_of_zeroing[nazwa_wid] =when ;
    }
    //  std::cout << "Size of zeroing map is " << map_of_zeroing.size() << std::endl;

}

//***********************************************************************
void TGalileoAnalysis::read_in__user_incrementers()
{

#ifdef USER_INCREMENTERS_ENABLED

    //#################################################
    std::cout << "Now, user defined incrementers..."<< std::endl;
    vector_of_user_incrementers.clear();

#ifdef NIGDY
    vector_of_user_incrementers.push_back (
                new Tuser_incrementer ( "!suma",
                                        "tpc41_energy_deposit",
                                        Tuser_incrementer::plus,
                                        "tpc42_energy_deposit" )
                );


    vector_of_user_incrementers.push_back (
                new Tuser_incrementer ( "tof_21_22LL_cal_diff",
                                        "tof_21_41_tof_LL_cal",
                                        Tuser_incrementer::minus,
                                        "tof_22_41_tof_LL_cal" )
                );
    //
    vector_of_user_incrementers.push_back (
                new Tuser_incrementer ( "tof_21_22RR_cal_diff",
                                        "tof_21_41_tof_RR_cal",
                                        Tuser_incrementer::minus,
                                        "tof_22_41_tof_RR_cal" )
                );
    //
    vector_of_user_incrementers.push_back (
                new Tuser_incrementer ( "tof_21_22_calculated",
                                        "tof_21_22LL_cal_diff",
                                        Tuser_incrementer::plus,
                                        "tof_21_22RR_cal_diff" )
                );
#endif
    /***
   vector_of_user_incrementers.push_back(
   new Tuser_incrementer("sum_for_plamen_tof_21_41_LL_cal_minus_tof_22_41_LL_cal_"
   "PLUS_tof_21_41_RR_cal_minus_tof_22_41_RR_cal",
   "tof_21_41_LL_cal_minus_tof_22_41_LL_cal",
   Tuser_incrementer::plus,
   "tof_21_41_RR_cal_minus_tof_22_41_RR_cal")
   );
   ****/


    //========================================================
    // retrieving the names.

    try
    {
        std::cout << "Trying to open the user incrementes directory:" + path.dir_incrementers() << std::endl;
        std::vector<std::string> names =
                Tfile_helper::find_files_in_directory ( path.dir_incrementers(),
                                                        ".incr" );

        // loop which is looking into the directory for a specified definions *.

        for ( unsigned nr = 0; nr < names.size(); nr++ )
        {
            std::cout << "User defined spectrum definition file :" << names[nr] <<
                         std::endl;
            create_user_incrementer ( names[nr] );
        }
    }
    catch ( ... )
    {
        std::cout << "Error while reading the users incrementers. "<< std::endl;
    }

#endif // #ifdef USER_INCREMENTERS_ENABLED

}
//*************************************************************************************************
//**************************************************************
/** No descriptions */
void TGalileoAnalysis::create_user_incrementer ( std::string name_of_description )
{
#ifdef USER_INCREMENTERS_ENABLED

    std::string pathed_name_of_description = "./incrementer_user_def/" + name_of_description;

    //  std::cout << "In the std::string " << name
    //        << " lookin for the extension "
    //        << std::endl;

    std::string name_without_extension = name_of_description;
    std::string::size_type  extension_starts = name_of_description.find ( ".incr" );
    if ( extension_starts != std::string::npos )
    {
        name_without_extension =
                name_of_description.erase ( extension_starts, 100 );
    }

    // Check if it is not the snake situation

    static int level_of_nesting;
    static std::vector<std::string> nesting_trace;
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    for ( unsigned int i = 0; i < nesting_trace.size(); i++ )
    {
        if ( nesting_trace[i] == name_without_extension )
        {

            std::cout << "\nCondition nesting trace: \n";
            for ( unsigned int k = 0; k < nesting_trace.size(); k++ )
            {
                std::cout << "(" << k
                          << ") " << nesting_trace[k] << " --> ";
            }
            std::cout << name_without_extension;

            std::cout << "\nERROR: Infinite nesting of the incrementers at the level "
                      << level_of_nesting
                      << "\n - the incrementer called " << name_without_extension
                      << " was already on the level " << i
                      << "\n This \"Snake is eating its own tail\" " << std::endl;
            exit ( 1 );

        }
    }

    level_of_nesting++;
    if ( level_of_nesting > 1000 )
    {
        std::cout << "Nesting the incrementers reach the level " << level_of_nesting
                  << "\n - it is unrealistic, most probably there is a situation "
                     "\n 'Snake is eating its own tail' " << std::endl;
        exit ( 1 );
    }


    nesting_trace.push_back ( name_without_extension );

    //  incrementers are not registered in root, they do not have to have
    //  any continuity after any PAUSE, so we can create them new everytime


    Tuser_incrementer *c = new Tuser_incrementer();
    c->read_in_parameters ( name_without_extension );  // the path will be added automatically

    // put spectrum on the list
    vector_of_user_incrementers.push_back ( c );

    level_of_nesting--;
    nesting_trace.pop_back();
    return /*c*/ ;
#endif // #ifdef USER_INCREMENTERS_ENABLED

}
//******************************************************************************
TjurekPolyCond* TGalileoAnalysis::give_ptr_to_polygon ( std::string name )
{
    for ( int i = 0 ; i < list_of_polygons.size() ;i++ )
    {
        if ( list_of_polygons[i]->give_name() == name )
            return list_of_polygons[i];
    }
    return NULL;
}
//_____________________________________________________________________________
void TGalileoAnalysis::AddHistogram ( TH1* ptr_spec, const char* )
{
    //flag_sorting_from_file = true;
    list_of_histograms.push_back ( ptr_spec );
}
//_____________________________________________________________________________
