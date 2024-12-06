#ifndef _MFMVamosICFrame_
#define _MFMVamosICFrame_
/*
  MFMVamosICFrame.h

	Copyright Acquisition group, GANIL Caen, France

*/
#define VAMOSIC_FRAMESIZE 32
#define VAMOSIC_HEADERFRAMESIZE 18
#define VAMOSIC_TRIG_REQ_CRYS_ID_MASK 0x001f
#define VAMOSIC_BOARD_ID_MASK 0x07ff
#define VAMOSIC_NUMBER_CRISTAL_ID 2
#define VAMOSIC_NB_STATUS 2
#define VAMOSIC_STD_UNIT_BLOCK_SIZE 4

#include "MFMBlobFrame.h"

#pragma pack(push, 1) // force alignment
struct MFM_ICvamos_eventInfo {
  unsigned EventIdx  : 32;
  char     EventTime[6];
};


struct MFM_ICvamos_data{
  unsigned CristalId : 16;
  unsigned Status1   : 16;
  unsigned Status2   : 16;
  unsigned NotUsed1  : 16;
  unsigned Energy    : 16;
  unsigned NotUsed2  : 16;
  unsigned Checksum  : 16;
};


struct MFM_ICvamos_frame{
	 MFM_common_header      VamosICBlobcHeader;
	 MFM_ICvamos_eventInfo  VamosICEventInfo;
	 MFM_ICvamos_data       VamosICData;
};



//____________MFMVamosICFrame___________________________________________________________

class MFMVamosICFrame : public MFMBlobFrame
{

private:

long long * fCountNbEventCard;

uint64_t fTimeStamp;
int fEventNumber ;

public :

MFMVamosICFrame();
MFMVamosICFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMVamosICFrame();
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

void     SetCristalId(uint16_t cristalId) ;
void     SetCristalId(uint16_t tgRequest, uint16_t idBoard);
uint16_t GetCristalId();
uint16_t GetTGCristalId();
uint16_t GetBoardId();

virtual void      SetStatus(int i, uint16_t status);
virtual uint16_t  GetStatus(int i);

virtual void      SetEnergy(uint16_t energy);
virtual uint16_t  GetEnergy();
virtual void      SetChecksum(uint16_t cksum);
virtual uint16_t  GetChecksum();
virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);

virtual void 	  InitStat();
virtual void 	  FillStat();
virtual string    GetStat(string info);

};
#pragma pack(pop) // free aligment
#endif
