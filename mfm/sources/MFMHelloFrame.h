#ifndef _MFMHelloFrame_
#define _MFMHelloFrame_
/*
  MFMHelloFrame.h
	Copyright Acquisition group, GANIL Caen, France
*/

#define HEL_HEADERFRAMESIZE 18
#define HEL_FRAMESIZE  18

#include "MFMBlobFrame.h"

#pragma pack(push, 1) // force alignment
struct MFM_hel_eventInfo {
  char EventTime[6];
  unsigned EventIdx  : 32;
};

struct MFM_hel_data{
};

struct MFM_hel_header{
	 MFM_common_header  HelBlobcHeader;
	 MFM_hel_eventInfo  HelEventInfo;
	 MFM_hel_data       HelData;
};

//____________MFMHelloFrame___________________________________________________________

class MFMHelloFrame : public MFMBlobFrame
{
private:
uint64_t fTimeStamp;
int32_t fEventNumber ;

public :

MFMHelloFrame();
MFMHelloFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMHelloFrame();
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
// Hello
virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);
virtual string    DumpData(char mode ='d', bool nozero=0);
};
#pragma pack(pop) // free aligment
#endif
