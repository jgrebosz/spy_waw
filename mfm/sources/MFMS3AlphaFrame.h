#ifndef _MFMS3AlphaFrame_
#define _MFMS3AlphaFrame_
/*
  MFMS3AlphaFrame.h

	Copyright Acquisition group, GANIL Caen, France
	Author : Luc Legeard
	Class Alpha frame based on Numexo2 card frame.
*/

#include "MFMNumExoFrame.h"


#pragma pack(push, 1) // force alignment


//____________MFMS3AlphaFrame___________________________________________________________

class MFMS3AlphaFrame:public MFMNumExoFrame
{
public:

MFMS3AlphaFrame();
MFMS3AlphaFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMS3AlphaFrame();

virtual void      SetSetup(uint16_t i,uint16_t setup);
virtual uint16_t  GetSetup(uint16_t i);
virtual void      SetStatus(uint16_t status);
virtual uint16_t  GetStatus();
virtual void      SetEnergy(uint16_t energy);
virtual uint16_t  GetEnergy();
virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);

};
#pragma pack(pop) // free aligment
#endif
