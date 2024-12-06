#ifndef _MFMBoxDiagFrame_
#define _MFMBoxDiagFrame_

/*
  MFMBoxDiagFrame.h

	Copyright Acquisition group, GANIL Caen, France

*/

#include "MFMNumExoFrame.h"

#define BOX_DIAG_NB_STATUS 2

#define BOX_DIAG_NB_CHANNELS 16

#include "MFMBlobFrame.h"

#pragma pack(push, 1) // force alignment


struct MFM_BoxDiag_data{
  unsigned CristalId : 16;
  unsigned Status1   : 16;
  unsigned Status2   : 16;
  unsigned TimeStop  : 16;
  unsigned Energy    : 16;
  unsigned Time      : 16;
  unsigned Checksum  : 16;
};


struct MFM_BoxDiag_frame{
	 MFM_common_header      Header;
	 MFM_numexo_eventInfo  EventInfo;
	 MFM_BoxDiag_data       Data;
};



//____________MFMBoxDiagFrame___________________________________________________________

class MFMBoxDiagFrame : public MFMNumExoFrame
{


public :


virtual void      SetStatus(int i, uint16_t status);
virtual uint16_t  GetStatus(int i);
virtual void      SetTimeStop(uint16_t timestop);
virtual uint16_t  GetTimeStop();
virtual void      SetEnergy(uint16_t energy);
virtual uint16_t  GetEnergy();
virtual void      SetTime(uint16_t time);
virtual uint16_t  GetTime();
virtual void      SetChecksum(uint16_t cksum);
virtual uint16_t  GetChecksum();
virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);



};
#pragma pack(pop) // free aligment
#endif
