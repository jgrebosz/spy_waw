//---------------------------------------------------------------
//        Go4 Framework release package V.1.0000
//                      24-May-2002
//---------------------------------------------------------------
//   The GSI Online Offline Object Oriented (Go4) Project
//   Experiment Data Processing at DVEE department, GSI
//---------------------------------------------------------------
//
//Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI
//                    Planckstr. 1, 64291 Darmstadt, Germany
//Contact:            http://go4.gsi.de
//----------------------------------------------------------------
//This software can be used under the license agreements as stated
//in Go4License.txt file which is part of the distribution.
//----------------------------------------------------------------
/* Generated by Together */

#include "TGalileoCalibratedEvent.h"

#include "TGalileoCalibrateProcessor.h"
// #include "Go4EventServer/TGo4TreeSource.h"
// #include "Go4EventServer/TGo4FileSource.h"


//*************************************************************************
void TGalileoCalibratedEvent::Clear ( std::string  t )
{
//     TRACE((11,"TGalileoCalibratedEvent::Clear()",__LINE__, __FILE__));

    // zeroing new members


    char *ptr = ( char * ) & begin_for_zeroing ;
    char *kon_ptr = ( char * ) & end_for_zeroing ;

    while ( ptr < kon_ptr )
        * ( ptr++ ) = 0 ;

//     sci21_position = -50;
//     sci41horiz_position = -60 ;
//     sci41vertic_position = -70;

}
//*************************************************************************
int TGalileoCalibratedEvent::Fill()
{
    //TRACE((11,"TGalileoCalibratedEvent::Fill()",__LINE__, __FILE__));
    //TGo4TraceSingleton* tr=TGo4TraceSingleton::Instance();
    // check for different source types
    int rev=-1;

        std::cout << "Jestem TRisingCalibratedEvent::Fill() " << std::endl ;
#ifdef NOGO4
    
    TGalileoCalibrateProcessor* proc;
    proc = ( TGalileoCalibrateProcessor* ) GetEventSource();
    Clear();
    proc->BuildCalibratedEvent ( this );
    rev=0;

 
#endif



    //     std::cout << "Jestem TRisingCalibratedEvent::Fill() line " << __LINE__ <<  " " << __FILE__
    //     << " rev = " << rev << std::endl ;
    return rev;


}
//***************************************************************************************
TGalileoCalibratedEvent::~TGalileoCalibratedEvent()
{
//     TRACE ( ( 12,"TGalileoCalibratedEvent::~TRisingCalibratedEvent()",__LINE__, __FILE__ ) );

}
//*********************************************************************************
TGalileoCalibratedEvent::TGalileoCalibratedEvent()
        :TjurekAbstractEventElement ( "Calibrated" )
{
//     TRACE ( ( 12,"TGalileoCalibratedEvent::TRisingCalibratedEvent()",__LINE__, __FILE__ ) );
    TGalileoCalibratedEvent::Clear();

}
/** No descriptions ***********************************************************/
// DUMMY FUNCTION
void TGalileoCalibratedEvent::set_value ( double v )
{
    /*
       cate_dE = v ;
       cate_E = v + 5;
    */
    std::cout << "Empty function TRisingCalibratedEvent::set_value(double v)" << std::endl;
}

//*********************************************************************************

