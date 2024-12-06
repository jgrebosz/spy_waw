#ifndef _MFMNumExoFrame_
#define _MFMNumExoFrame_
/*
  MFMNumExoFrame.h

	Copyright Acquisition group, GANIL Caen, France
	Class for  frames based on Numexo2 cards.
    Author : Luc Legeard
*/
#include "MFMBlobFrame.h"


#define NUMEXO_FRAMESIZE 32
#define NUMEXO_HEADERFRAMESIZE 18
#define NUMEXO_CRYS_MASK 0x001f
#define NUMEXO_BOARD_ID_MASK 0x07ff
#define NUMEXO_NUMBER_CRISTAL_ID 2
#define NUMEXO_STD_UNIT_BLOCK_SIZE 4
#define NUMEXO_NB_CHANNELS 16


#pragma pack(push, 1) // force alignment

struct MFM_numexo_eventInfo {
  unsigned EventIdx  : 32;
  char EventTime[6];
};

struct MFM_numexo_data{
	 unsigned CristalId : 16;
	 unsigned Data1 : 16;
	 unsigned Data2 : 16;
	 unsigned Data3 : 16;
	 unsigned Data4 : 16;
	 unsigned Data5 : 16;
	 unsigned Checksum : 16;
};

struct MFM_numexo_frame{
	 MFM_common_header     Header;
	 MFM_numexo_eventInfo  EventInfo;
	 MFM_numexo_data       Data;
};



//____________MFMNumExoFrame___________________________________________________________

class MFMNumExoFrame : public MFMBlobFrame
{
private:
uint64_t fTimeStamp;
int      fEventNumber ;

long long * fCountNbEventCard;


public :

MFMNumExoFrame();
MFMNumExoFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMNumExoFrame();

virtual void SetPointers(void * pt =NULL);

virtual void SetBufferSize(int size, bool ifinferior) ;
virtual void SetAttributs(void * pt =NULL);
virtual uint64_t GetTimeStamp();
virtual uint64_t GetTimeStampAttribut();
virtual uint32_t GetEventNumberAttribut();
virtual uint32_t GetEventNumber();

virtual void SetTimeStamp(uint64_t timestamp);
virtual void SetEventNumber(uint32_t eventnumber);
virtual string GetHeaderDisplay(char* infotext=NULL);


virtual void      SetCristalId(uint16_t cristalId);
virtual void      SetCristalId(uint16_t tgRequest, uint16_t idBoard);
virtual uint16_t  GetCristalId();
virtual uint16_t  GetTGCristalId();
virtual uint16_t  GetBoardId();
virtual void      SetChecksum(uint16_t cristalId);;
virtual uint16_t  GetChecksum();
virtual uint16_t  ComputeChecksum();
virtual bool      VerifyChecksum();
virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);
virtual void 	  InitStat();
virtual void 	  FillStat();
virtual string    GetStat(string info);
};
#pragma pack(pop) // free aligment
#endif
