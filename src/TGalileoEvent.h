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

#ifndef TEBEVENT_H
#define TEBEVENT_H

#include "tjurekabstracteventelement.h"

#include "experiment_def.h"
#include <vector>
#include <map>

static constexpr  int fixed_size_of_vector = 35;  /// max nr of  tracked gammas (multiplicity)
constexpr int  how_many_agata_crystals = 9;

#include  "Tincrementer_donnor.h"
#include  "DataFrames.h"

struct  Tdata_for_dante_plate {
    float x;
    float y;
    float charge;
    float time;

};
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Class containing unpacked event structure of EB example analysis.
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class TGalileoEvent  : public TjurekAbstractEventElement
{
public:
    TGalileoEvent() ;
    virtual ~TGalileoEvent() ;
    /**
     * Method called by the event owner (analysis step) to fill the
     * event element from the set event source. Event source can
     * be the source of the analysis step (if this is a raw event) or the event processor (if this is a reduced event).
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
    virtual void Clear();
    virtual void SetValid(bool t) { } ;

    // =============================================================
    int begin_for_zeroing ;   //!

    int     frs_synchro_data;   // what for ?



    // NEDA
    subDataNEDA_t neda_data[HOW_MANY_NEDA];
    std::map<int,bool>  neda_fired;   // WARNING <-- must be cleared every event

     // EAGLE
    subDataEAGLE_t eagle_data [HOW_MANY_EAGLE];
    std::map<int,bool> eagle_fired;   // WARNING <-- must be cleared every event

    // DSSD
   subDataDSSD_t dssd_data[HOW_MANY_DSSD];
    // std::map<int,bool> dssd_fired;   // WARNING <-- must be cleared every event
    bool dssd_fired[HOW_MANY_DSSD] ;

    unsigned long long LTRIG_data;
    unsigned long long  seconds;
    unsigned long long timestamp;

    bool diamant_fired[HOW_MANY_DIAMANT];
    int diamant_data[HOW_MANY_DIAMANT][2];



#ifdef AC_PRESENT
    int Cluster_AC_energy[HOW_MANY_GE_CLUSTERS];
#endif

    //int dgf_synchro_data ;

    //#################################################################
    /*
    #if ((CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN) ||  \
     (CURRENT_EXPERIMENT_TYPE == GALILEO_PROTOTYPE) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ))
    */
    //#################################################################
    int trigger;  // info word send in the go4 structure as parameter trigger
    //#endif



    int end_for_zeroing ;  //! dummy member to recognize end for memset function




public:
//    ClassDef(TGalileoEvent,1)
};


#endif //TEBEVENT_H


