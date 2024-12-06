//---------------------------------------------------------------
//----------------------------------------------------------------


#include "TGalileoCalibrateProcessor.h"

#include "TGalileoCalibratedEvent.h"
//#include "Go4Analysis/TGo4Analysis.h"
#include "TGalileoAnalysis.h"
#include <locale>
using namespace std;


//******************************************
struct my_numpunct : std::numpunct<char> {
    std::string do_grouping() const {return "\03";}
};

const int debug_level = 1 ;

#define Dcout(nnn) if(debug_level >= nnn) std::cout << nnn << std::endl;


#define COTO  std::cout<<" I am in " << __FILE__ << " at line  "<<__LINE__<< std::endl ;
#define ANYKEY  { std::cout << "Stop.  Press: '1' and Enter " ; int iii ; std::cin >> iii ; }

//-------------------------------------------------------------------------
/*------------------------------------------------------------------------*/
std::istream & zjedz ( std::istream & plik )
{
    char c = 'x' ;
    do
    {
        plik >> c  ;
        if (( c == '/' && ( plik.peek() == '/' )) || c == '#')
        {
            std::string schowek;
            getline ( plik, schowek );
        }
        else
        {
            plik.putback ( c ) ;
            return plik ;
        }
    }
    while ( plik ) ;
    return plik ;
}
/**************************************************************************/
//**************************************************************************
TGalileoCalibrateProcessor::TGalileoCalibrateProcessor ( std::string  name )
    :TjurekAbstractEventProcessor ( name )
{
    //   TRACE((14,"TGalileoCalibrateProcessor::TGalileoCalibrateProcessor(string)",
    //          __LINE__, __FILE__));

    event_nr = 0  ;

}
//**************************************************************************
/* default constructor */
TGalileoCalibrateProcessor::TGalileoCalibrateProcessor()
    : TjurekAbstractEventProcessor ( "GalileoCalibrateProcessor" )
{
    //   TRACE((14,"TGalileoCalibrateProcessor::TGalileoCalibrateProcessor()",
    //          __LINE__, __FILE__));

    std::cout << "The default constuctor for the class TGalileoCalibrateProcessor() " << std::endl ;
    event_nr = 0;
}

//**********************************************************************************
/* destructor */
TGalileoCalibrateProcessor::~TGalileoCalibrateProcessor()
{
    //   TRACE((14,"TGalileoCalibrateProcessor::~TGalileoCalibrateProcessor()",
    //          __LINE__, __FILE__));

    //std::cout << "~TGalileoCalibrateProcessor  Destructor begins" << std::endl ;

    // // WARNING: never delete the histograms which are added to the framework
    //   // with AddHistogram. They are adopted by the Go4 Folders!!!
    //   // causes SEGV if analysis is closed and initialized again!!!
    //   // the proper way is to remove the histograms from the framework...

    // std::cout << "Przed wywol w destruktorze " << std::endl ;
    //  dynamic_cast<TRisingAnalysis*> (TGo4Analysis::Instance())     ->analysis_submited(false) ;
    //std::cout << " End of destructor " <<std::endl ;
}
//************************************************************************

//***********************************************************************
//***********************************************************************
//***********************************************************************
// working on every event
//***********************************************************************
//***********************************************************************
void TGalileoCalibrateProcessor::BuildCalibratedEvent ( TGalileoCalibratedEvent* target )
{

    //   TRACE((11,"TGalileoCalibrateProcessor::TGalileoCalibrateProcessor(Int_t)",
    //          __LINE__, __FILE__));

    //Dcout ( 5 )  << "\nFunction Step2 : Build CalibratedEvent%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl ;

    //     TGalileoEvent* input = 0;
    //     TRisingCalibration* cal = 0;
    TGalileoPar* ps_par = 0 ;

    //target->SetValid(kFALSE);       // events are not stored until kTRUE is set

    //     input= ( TGalileoEvent* ) GetInputEvent();
    //     cal= ( TRisingCalibration* ) GetCalibration();

    //  target->set_value(-250);
    /*    if ( ! input->IsValid() )
        {
    //std::cout << "Not valid event " << std::endl;
    target->SetValid ( false );    // do not save the ntuple event
    return ;
        }
  */


    GalileoAnalysis_ptr->nr_events_analysed =  ++event_nr ;

    // we check this true event number
    /** Flag telling that now we want to dump information about the current event */
    if ( verbose_mode_enabled )
    {
        if ( event_nr >= verbose_begins_with_event_nr
             &&
             event_nr < ( verbose_begins_with_event_nr + verbose_how_many_events )
             ){
            flag_verbose_events = true;
        }
        else {
            flag_verbose_events = false ;
        }
        GalileoAnalysis_ptr->set_verbose ( flag_verbose_events ) ;
    }

    //     std::cout << "\n---- created Valid Event nr " << (event_nr) << std::endl;

    constexpr int how_often = 250000;
    if ( ! ( event_nr % how_often )  || flag_verbose_events
         || event_nr == 100)
    {
        //    TRisingAnalysis* ptr_anal =
        //    dynamic_cast<TRisingAnalysis*> (TGo4Analysis::Instance());
        std::locale loc (std::cout.getloc(),new my_numpunct);
        std::cout.imbue(loc);
        std::cout << "                \r" << ( event_nr )
                  << "       events treated " ;

        if(event_nr % (how_often * 3) == 0
                && !GalileoAnalysis_ptr->give_current_filename().empty()
                )
        {    cout << "   currently from File= "
                  << GalileoAnalysis_ptr->give_current_filename()
                            ;


        }
        std::cout.flush();

        if ( flag_verbose_events )
            std::cout << "VERBOSE MODE: " << std::endl ;
    }

    //     target-> ( TRUE );    // events are not stored until kTRUE is set

    TGalileoAnalysis* ptr_anal = GalileoAnalysis_ptr ;
    //    dynamic_cast<TRisingAnalysis*> (TGo4Analysis::Instance());


    // bool multiplicity_ok = ptr_anal ->rising.first_check_cluster_multiplicity();
    // ptr_anal ->frs.remember_rising_cluster_multiplicity(multiplicity_ok);
    // this multiplicity is also used for CATE


    ptr_anal -> analyse_event();

    ptr_anal -> make_watchdog_step();

    // if(! (event_nr % 50000)) save_member_spectra();
    


    return ;
}
//******************************************************************
//*******************************************************************
/** For Franco Camera wish to make dump of selected events */
void TGalileoCalibrateProcessor::read_in_verbose_parameters()
{
    flag_verbose_events = false ;
    GalileoAnalysis_ptr->set_verbose ( flag_verbose_events ) ;


    std::string nazwa =path.dir_commands() + "/verbose_mode.command" ;
    std::ifstream plik ( nazwa.c_str() );
    if ( plik )
    {
        try
        {
            verbose_mode_enabled =
                    ( bool ) FH::find_in_file<bool> ( plik, "verbose_mode_enabled" );

            verbose_begins_with_event_nr  =
                     FH::find_in_file<int> ( plik,
                                               "verbose_begins_with_event_nr" );

            verbose_how_many_events =
                    ( int ) FH::find_in_file <int>( plik,
                                               "verbose_how_many_events" );
        }
        catch ( Tfile_helper_exception &m )
        {
            std::cout << "Error while trying to read the file " << nazwa
                      << "\n Reason is: " << m.message << std::endl;
            exit ( 1 );
        }

    }
    else
    {
        verbose_begins_with_event_nr  = 0 ;
        verbose_how_many_events = 0;
        verbose_mode_enabled = false ;
    }

}

//****************************************************************************
// this function is called from TRisingAnalysis

//****************************************************************************
/** No descriptions */
void TGalileoCalibrateProcessor::preLoop()
{
    read_in_verbose_parameters();
}
//**********************************************************


//ClassImp ( TGalileoCalibrateProcessor )


