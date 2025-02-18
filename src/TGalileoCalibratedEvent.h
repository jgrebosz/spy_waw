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

#ifndef TEBCALIBRATEDEVENT_H
#define TEBCALIBRATEDEVENT_H

#include "tjurekabstracteventelement.h"

#include "experiment_def.h"
#include <map>
#include <vector>

#include <string>

// NOTE:  how to make the new item here
// 1. it has to be and incrementer. If it is not, make it at first
// 2. Make here in this file the entry for this new variiable
//     for example:    double    my_data;
// 3. Gto to the funciton
//  void TRisingAnalysis::put_data_to_ntuple()
// and somwhere on its end, type the folowing instruction
/*
   ntuple_entry_vector.push_back(
      ntuple_entry_data("name of the chosen incremeter"
                        &fxCaliEvent->my_data,     // name of the item in this tree
                        true or false    // true - if we do not want to put the value if the incremeter is valid (so the ZERO will be here in ntuple)
                                              //   false - if we want to copy even if it is not valid, - so for example contains value -999999
                                              //             sometimes this last approach is better. It is better to have -9999 than 0
                        ));
*/

class TGalileoCalibrateProcessor ;

///////////////////////////////////////////////////////////////////////////////
/**
 * Class containing calibrated event structure of EB example analysis.
 * @author j. Adamczewski
 * @since 23-feb-2001
 */


/////////////////////////////////////////////////////////////////////////////////////////////////////

class TGalileoCalibratedEvent : public TjurekAbstractEventElement
{
    public:

        TGalileoCalibratedEvent() ;
        virtual ~TGalileoCalibratedEvent() ;
        /**
         * Method called by the event owner (analysis step) to fill the
         * event element from the set event source. Event source can
         * be the source of the analysis step (if this is a raw event)
         or the event processor (if this is a reduced event).
         * Fill method has to specify how the event source writes to the
         * members of this event structure. Either by calling methods of
         * the source (like myevent.a=fxEventSource->GetA(); etc ), or by
         * direct copy to the member (like fxEventSource->FillMemoryAt(&myevent.a);)
         */
        virtual int Fill();

        /**
         * Method called by the event owner (analysis step) to clear the
         * event element.
         */
        virtual void Clear ( std::string t="" );

        /** for my testing purposes */
        void set_value ( double v );


        // members of the Real Rising experiment ==================================
        int begin_for_zeroing ;   //! dummy member to recognize begin for memset function
        int end_for_zeroing ;   //! dummy member to recognize begin for memset function


//  ClassDef(TRisingCalibratedEvent,1)
};

#endif //TEBCALIBRATEDEVENT_H
