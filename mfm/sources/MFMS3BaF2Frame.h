#ifndef _MFMS3BaF2Frame_
#define _MFMS3BaF2Frame_
/*
  MFMS3BaF2Frame.h

	Copyright Acquisition group, GANIL Caen, France
	Class S3BaF2 frame based on Numexo2 card frame.
    Author : Luc Legeard
*/

#include "MFMNumExoFrame.h"


#pragma pack(push, 1) // force alignment


//____________MFMS3BaF2Frame___________________________________________________________

class MFMS3BaF2Frame:public MFMNumExoFrame
{
public:

MFMS3BaF2Frame();
MFMS3BaF2Frame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMS3BaF2Frame();

virtual void      SetSetup(uint16_t setup);
virtual uint16_t  GetSetup();
virtual void      SetStatus(uint16_t status);
virtual uint16_t  GetStatus();
virtual void      SetEnergy(uint16_t energy);
virtual uint16_t  GetEnergy();
virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);

};
#pragma pack(pop) // free aligment
#endif
