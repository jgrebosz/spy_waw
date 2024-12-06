#ifndef _MFMS3eGUNFrame_
#define _MFMS3eGUNFrame_
/*
  MFMS3eGUNFrame.h

	Copyright Acquisition group, GANIL Caen, France
	Class eGUN frame based on Numexo2 card frame.
    Author : Luc Legeard
*/

#include "MFMNumExoFrame.h"


#pragma pack(push, 1) // force alignment


//____________MFMS3eGUNFrame___________________________________________________________

class MFMS3eGUNFrame:public MFMNumExoFrame
{
public:

MFMS3eGUNFrame();
MFMS3eGUNFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMS3eGUNFrame();

virtual void      SetSetup(uint16_t i,uint16_t setup);
virtual uint16_t  GetSetup(uint16_t i);
virtual void      SetCup(uint16_t cup);
virtual uint16_t  GetCup();
virtual void      SetGrid(uint16_t grid);
virtual uint16_t  GetGrid();
virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);

};
#pragma pack(pop) // free aligment
#endif
