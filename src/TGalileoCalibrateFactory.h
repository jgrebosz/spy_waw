//---------------------------------------------------------------
//        Go4 Framework release package V.1.0000 
//                      24-May-2002
//---------------------------------------------------------------
//	 The GSI Online Offline Object Oriented (Go4) Project
//	 Experiment Data Processing at DVEE department, GSI
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

#ifndef TEBCALIBRATEFACTORY_H
#define TEBCALIBRATEFACTORY_H


#include "TGalileoEvent.h"
#include "TGalileoCalibratedEvent.h"
#include "TGalileoCalibrateProcessor.h"


class TGalileoCalibrateFactory   : public TGo4EventServerFactory 
{
public:
    TGalileoCalibrateFactory() ;
    TGalileoCalibrateFactory(std::string  name);
    virtual ~TGalileoCalibrateFactory() ;
};
#endif //TEBCALIBRATEFACTORY_H
