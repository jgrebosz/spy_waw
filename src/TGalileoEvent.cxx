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

#include "TGalileoEvent.h"
#include "TGalileoEventProcessor.h"
// #include "Go4EventServer/TGo4TreeSource.h"
// #include "Go4EventServer/TGo4FileSource.h"
#include <cstring>
#include <cstdlib>




//int TGalileoEvent::fgiZeroField[41]={0};

//******************************************************************************
TGalileoEvent::TGalileoEvent()
        :TjurekAbstractEventElement ( "Raw" )
{
// TRACE((12,"TGalileoEvent::TGalileoEvent()",__LINE__, __FILE__));

//     std::cout << "constructor  TGalileoEvent()!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" <<std::endl;
    TGalileoEvent::Clear();

    
//     how_many_tracked_gamma = 0;
//     std::cout << "end of constructor" <<std::endl;
}
//******************************************************************************
TGalileoEvent::~TGalileoEvent()
{
//     std::cout <<"TGalileoEvent::~TGalileoEvent() line--------------> " << __LINE__ << "  file " << __FILE__ << std::endl;

//fCateArray->Clear();
//delete fCateArray;

}
//******************************************************************************
int TGalileoEvent::Fill()
{
//    TRACE((11,"TGalileoEvent::Fill()",__LINE__, __FILE__));

    //TGo4TraceSingleton* tr=TGo4TraceSingleton::Instance();
    // check for different source types
    int rev=-1;

#ifdef NOGO4
d f g
    TGalileoEventProcessor* proc;
    if ( CheckEventSource ( "TGalileoEventProcessor" ) )
    {
        proc = ( TGalileoEventProcessor* ) GetEventSource();
        proc->BuildEbEvent ( this );
        rev=0;
    }
    else
    {
        rev=1;
    }
#endif

    return rev;
}
//******************************************************************************
void  TGalileoEvent::Clear ( )
{

//     TRACE ( ( 11,"TGalileoEvent::Clear()",__LINE__, __FILE__ ) );
//     std::cout << "Zeroing the TGalileoEvent -------------------------------------------------" << std::endl;

    neda_fired.clear();
    eagle_fired.clear();

//    dssd_fired.clear(); // no more as map

    char *ptr = ( char * ) & begin_for_zeroing ;
    char *kon_ptr = ( char * ) & end_for_zeroing ;
    int ile = kon_ptr-ptr ;
    
// std::cout << "Zeroing ile = " << ile   << " for this = " << std::hex << this << std::dec << std::endl;

//   std::cout << "Zeroing word from address = 0x" << std::hex << (int)ptr
//          << " till = 0x" << int(& end_for_zeroing)
//          << "    so bytes = " << std::dec << (kon_ptr-ptr)
//          << " while size of is = " << sizeof(*this)  << std::dec << std::endl;

  memset(ptr, 0, ile);
    
//     memset(ptr, 0, sizeof(*ptr) *( kon_ptr-ptr) );
   //   how_many_tracked_gamma = 0;
       
//   std::cout << "After zeroing ==========   -&how_many_tracked_gamma = 0x" << std::hex<< int(&how_many_tracked_gamma) << std::dec // << std::endl;
//        << ", contents = " << how_many_tracked_gamma << std::endl;

    return;

}




//***********************************************************************************



