//--------------------------------------------------------------- Go4
#include "experiment_def.h"

#include "tjurekabstracteventprocessor.h"

#include "TGalileoEvent.h"
#include "TGalileoAnalysis.h"

// #include "Go4EventServer/TjurekMbsEvent.h"

#include <iostream> // for test printouts
#include <iomanip> // for test printouts
#include <sstream>
using namespace  std;

#include "spectrum.h"
#include "Tfile_helper.h"
#include "TEAGLE_array.h"  // for cluster characters

#ifdef ACTIVE_STOPPER_PRESENT
//#include "Tactive_stopper_full.h"
#endif

#define FOUR_BRANCHES_VERSION   1
#define NEW_FRS_LOOKUP_TABLE   true

// for debugging purposes
#define POKAZ 0

//#define LOOKING 1
static const unsigned TIMESTAMP_QUEUE_MAX = 10;   // was 10

extern std::istream & zjedz ( std::istream & plik );
#define DBG(x)     // std::cout << x << std::endl

extern void unpack_one_event();
extern bool unpacking_oliver_event_was_successful();
extern bool flag_this_is_diamant_data;

#include "tgeneric_data.h"
#include "TDiamant_array.h"
//******************************************************************
TGalileoEventProcessor::TGalileoEventProcessor ( std::string  name )
    :TjurekAbstractEventProcessor ( name )
{
    flag_old_style_pulser_in_b7 = true; // during commisioninig experiment
    // after discovering first pulser in Master trigger word -
    // this flag will be switched automatically

    // we remember for private functions
    target_event = nullptr;
    // TjurekMbsEvent* input_event ;
    //input_subevent = NULL;
    input_event_data = nullptr; // one event buffer
    how_many_data_words = 0 ; // how many sensible data in the buffer
    memset(&vmeOne[0][0], 0, sizeof(vmeOne));
    memset(&vmeUser[0][0], 0, sizeof(vmeUser));

}
//******************************************************************
//******************************************************************
TGalileoEventProcessor::~TGalileoEventProcessor()
{
}
//******************************************************************
// unpacking
// *****************************************************************
void TGalileoEventProcessor::BuildEbEvent (
        TGo4EventSourceParameter* source_of_events,
        TGalileoEvent* target )
{
    // std::cout << "Function TGalileoEventProcessor::BuildEbEvent================================" << std::endl;

top:
    target_event = target;  // to remember for other private functions
    target_event->Clear();

    Tgeneric_data::event_ptr  = target_event;   // to allow them to unpack to the event

#if 0
    input_event_data = source_of_events-> give_next_event ( &how_many_data_words );

    std::cout << "Size of event " <<  how_many_data_words  << std::endl;
    if ( !input_event_data)
    {
        std::cout << "BuildEvent in TGalileoEventProcessor: no input event !"
                  << std::endl;
        std::cout << "Event False" << std::endl;
        target_event->SetValid ( false );
        return;
    }
    // else {unpack_frame();}

    current_ev_nr = source_of_events->give_event_nr();
    //std::cout << "Current event number " << current_ev_nr << std::endl;
    //     if(current_ev_nr % 500000 == 0 ) std::cout << current_ev_nr << std::endl;
    
    // std::cout  << "\n\n/////////////////////////////// New event nr "
    //    << source_of_events->GetCount()
    //      << "  ////////////////////////////////"
    //          << std::endl;
    //
    //  input->PrintEvent();
    //  input->ResetIterator(); // this above was destroing it
    //
    //  std::cout << "after text, tring to analyse it ///////////////////"
    //       << std::endl;
#endif // 0

    if(flag_this_is_diamant_data == false)
    {
        unpack_one_event();
        if (unpacking_oliver_event_was_successful() )   // <<--- seams it is always true
            //     if ( unpack_the_fast_beam_campaign_event() )
        {

            target_event->SetValid ( true );
            //   std::cout << "Correctly reconstructed event ------------" << std::endl;
        }
        else
        {

            target_event->SetValid ( false );
            std::cout << "Event not constructed, wrong event -------" << std::endl;
        }
    }
    else   // This is a DIAMANT data -------------------------
    {

        // cout << "before next event " <<endl ;
        input_event_data = source_of_events-> give_next_event ( &how_many_data_words );


        // int input event data[10] <=== is a buffer with results (board, channel, energy, top)

        if(input_event_data[2])   // if nonzero data
            // board)
        {
            //            int b = 102;
            //            int c = 15;
            //            int n = GalileoAnalysis_ptr->give_gal_ptr()->
            //                    give_diamant_pointer()
            //                    ->
            //                    give_lookup_table(
            //                        b,    // board
            //                        c     // channel
            //                        );

            //            cout << " n = " << n << endl;

            //            for(int i = 0 ; i < 10 ; ++i)
            //            {
            //                cout << " input_event_data[" << i << "] = " << input_event_data[i]
            //                        << endl;
            //            }

            // Decode this combination of lookup table for diamond
            int nr =
                    GalileoAnalysis_ptr->give_gal_ptr()->
                    give_diamant_pointer()
                    ->
                    give_lookup_table(
                        input_event_data[0],    // board
                    input_event_data[1]     // channel
                    );


            //            cout << "Input to the event"
            //                 << "Board = " << input_event_data[0]
            //                 << ", channel = " << input_event_data[1]
            //                 << ", (decoded as det nr " << nr
            //                 << ") ---- energy = " <<input_event_data[2]
            //                 << " top = " << input_event_data[3]
            //                 << endl;


            target_event->diamant_fired[nr] = true;

            target_event->diamant_data[nr][0] =
                    input_event_data[2]; // energy

            target_event->diamant_data[nr][1] =
                    input_event_data[3];  // top

        }
    }

    // goto top;
    //     std::cout << "end of event processor function." << std::endl;
}

//***********************************************************
//***********************************************************
void  TGalileoEventProcessor::swap_nr_words ( short int* pointer,
                                              int how_many )
{
    int tmp;
    //std::cout << "Swapping " << how_many  << " words " << std::endl;

    for ( int i = 0; i < how_many; i+=2, pointer+=2 )
    {
        tmp = * ( pointer+1 );
        * ( pointer+1 ) = * ( pointer );
        * ( pointer ) = tmp;
    }

}
//**********************************************************

//************************************************************

//*******************************************************************************************************************
void TGalileoEventProcessor::distribute_frs_vme_words_to_event_members (
        TGalileoEvent *target_event )
{

    /*  for ( int i = 0; i < 32; i++ )
    {
      target_event->module_vmeOne_2[i] = vmeOne[2][i];
      target_event->module_vmeOne_3[i] = vmeOne[3][i];
      target_event->module_vmeOne_4[i] = vmeOne[4][i];

      target_event->module_vmeOne_5[i] = vmeOne[5][i];
      target_event->module_vmeOne_6[i] = vmeOne[6][i];
      target_event->module_vmeOne_8[i] = vmeOne[8][i];
      target_event->module_vmeOne_9[i] = vmeOne[9][i];
      target_event->module_vmeOne_11[i] = vmeOne[11][i];
      target_event->module_vmeOne_13[i] = vmeOne[13][i];
      target_event->module_vmeOne_15[i] = vmeOne[15][i];
      target_event->module_vmeOne_16[i] = vmeOne[16][i];
      target_event->module_vmeOne_17[i] = vmeOne[17][i];
      target_event->module_vmeOne_18[i] = vmeOne[18][i];
      target_event->module_vmeOne_19[i] = vmeOne[19][i];
    }

  for ( int i = 0; i < 32; i++ )
    {
      target_event->module_vmeUser_5[i] = vmeUser[5][i];
      target_event->module_vmeUser_6[i] = vmeUser[6][i];
      target_event->module_vmeUser_8[i] = vmeUser[8][i];
      target_event->module_vmeUser_9[i] = vmeUser[9][i];
      target_event->module_vmeUser_11[i] = vmeUser[11][i];
      target_event->module_vmeUser_13[i] = vmeUser[13][i];
      target_event->module_vmeUser_15[i] = vmeUser[15][i];
      target_event->module_vmeUser_16[i] = vmeUser[16][i];
      target_event->module_vmeUser_17[i] = vmeUser[17][i];
      target_event->module_vmeUser_18[i] = vmeUser[18][i];
      target_event->module_vmeUser_19[i] = vmeUser[19][i];
    }*/

}
//****************************************************************************

//**************************************************************************


//*************************************************************************
void TGalileoEventProcessor::postLoop()
{
    //    output_statistics_events();

}
//***
//************************************************************************
/** every event has its own number (stored inside the event structure)
    To estimate the ratio of analysed event the TAnalysis class wants to
    know this number quite often */
int * TGalileoEventProcessor::give_addres_of_mbs_event_number()
{
    return   &current_ev_nr;
}

//***********************************************************************
/** To have a chance to load the lookup table */
void TGalileoEventProcessor::preLoop()
{
    // this funcion is a kind of preloop, so here
    //   lookup.read_from_disk();

    counter_valid = 0;
    counter_put = 0;
    counter_try = 0;
    counter_total = 0;
    counter_nonsense = 0;

}
//*************************************************************************
//*************************************************************************
void TGalileoEventProcessor::perhaps_one_queue_is_too_big()
{
}
//************************************************************************
/** for debuging purposes */
void TGalileoEventProcessor::show_n_longwords ( uold_long *buf,int how_many )
{
    for ( int i = 0 ; i < how_many ; i++ )
    {
        std::cout << "nr " << i << ")   0x" << std::hex << buf[i]
                     << "\t\t dec = " << std::dec << buf[i] << std::endl;
    }
}
//***************************************************************************
//***************************************************************************
//*************************************************************************************
//ClassImp(TGalileoEventProcessor)
