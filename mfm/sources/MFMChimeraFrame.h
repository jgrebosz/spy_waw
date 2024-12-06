#ifndef _MFMChimeraFrame_
#define _MFMChimeraFrame_
/*
  MFMChimeraFrame.h
	Copyright Acquisition group, GANIL Caen, France
	e.d.f.  2016  (test class for chimera daq) 
*/

#define CHI_HEADERFRAMESIZE 20
#define CHI_FRAMESIZE  20

#include "MFMBlobFrame.h"

#pragma pack(push, 1) // force alignment
struct MFM_CHI_eventInfo {
  char EventTime[6];
  unsigned EventIdx  : 32;
  unsigned reserved : 16; 
};

struct MFM_CHI_data{
};

struct MFM_CHI_header{
	 MFM_common_header  CHIBlobcHeader;
	 MFM_CHI_eventInfo  CHIEventInfo;
 	 MFM_CHI_data       CHIData;
};

//____________MFMChimeraFrame___________________________________________________________

class MFMChimeraFrame : public MFMBlobFrame
{
private:
uint64_t fTimeStamp;
int32_t fEventNumber ;

public :

MFMChimeraFrame();
MFMChimeraFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMChimeraFrame();
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
