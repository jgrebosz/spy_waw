#ifndef _MFMS3RuthFrame_
#define _MFMS3RuthFrame_
/*
  MFMS3RuthFrame.h

	Copyright Acquisition group, GANIL Caen, France
	Class Rutherford frame based on Numexo2 card frame.
    Author : Luc Legeard
*/

#include "MFMS3AlphaFrame.h"


#pragma pack(push, 1) // force alignment


//____________MFMS3RuthFrame___________________________________________________________

class MFMS3RuthFrame:public MFMS3AlphaFrame
{
public:

MFMS3RuthFrame();
MFMS3RuthFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMS3RuthFrame();


virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);

};
#pragma pack(pop) // free aligment
#endif
