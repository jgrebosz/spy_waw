#ifndef _MFMS3SynchroFrame_
#define _MFMS3SynchroFrame_
/*
  MFMS3SynchroFrame.h

	Copyright Acquisition group, GANIL Caen, France
	Class Synchro frame based on Numexo2 card frame.
    Author : Luc Legeard
*/

#include "MFMNumExoFrame.h"


#pragma pack(push, 1) // force alignment



struct MFM_syncho_data{
  uint16_t CristalId ;
  uint16_t Nothing1  ;
  uint16_t Nothing2  ;
  char     Period[6];
  uint16_t Checksum ;
};

struct MFM_syncho_frame{
	 MFM_common_header  Header;
	 MFM_numexo_eventInfo  EventInfo;
	 MFM_syncho_data       Data;
};

//____________MFMS3SynchroFrame___________________________________________________________

class MFMS3SynchroFrame:public MFMNumExoFrame
{
public:

MFMS3SynchroFrame();
MFMS3SynchroFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMS3SynchroFrame();

virtual void      SetPeriod(uint64_t period);
virtual uint64_t  GetPeriod();

virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);

};
#pragma pack(pop) // free aligment
#endif

