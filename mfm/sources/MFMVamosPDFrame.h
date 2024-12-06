#ifndef _MFMVamosPDFrame_
#define _MFMVamosPDFrame_
/*
  MFMVamosPDFrame.h

	Copyright Acquisition group, GANIL Caen, France

*/


#define VAMOSPD_FRAMESIZE 280
#define VAMOSPD_HEADERFRAMESIZE 18
#define VAMOSPD_TRIG_REQ_CRYS_ID_MASK 0x001f
#define VAMOSPD_BOARD_ID_MASK 0x07ff
#define VAMOSPD_NUMBER_CRISTAL_ID 2
#define VAMOSPD_NB_VALUE 64
#define VAMOSPD_STD_UNIT_BLOCK_SIZE 4
#define VAMOSPD_NB_CHANNEL 2048

#include "MFMBlobFrame.h"

#pragma pack(push, 1) // force alignment
struct MFM_vamosPD_eventInfo {
  unsigned EventIdx  : 32;
  char     EventTime[6];
};
struct MFM_LabelValue {
  unsigned Lab : 16;
  unsigned Val : 16;
};

struct MFM_vamosPD_data{
  unsigned CristalId : 16;
  MFM_LabelValue   LabelValue[64];
  unsigned LocalCount: 16;
  unsigned Checksum  : 16;
};


struct MFM_vamosPD_frame{
	 MFM_common_header      VamosPDBlobcHeader;
	 MFM_vamosPD_eventInfo  VamosPDEventInfo;
	 MFM_vamosPD_data       VamosPDData;
};



//____________MFMVamosPDFrame___________________________________________________________

class MFMVamosPDFrame : public MFMBlobFrame
{
private:
uint64_t fTimeStamp;
int fEventNumber ;
long long * fCountNbEventCard;
int * fLabelIndice; // vector to make statistic
int * fIndiceLabel; // vector to make statistic
int * fNbLabels;// vector to make statistic
uint16_t fNbPara;
int MaxTrace;

public :

MFMVamosPDFrame();
MFMVamosPDFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMVamosPDFrame();
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

virtual void      SetLabel(int i, uint16_t Label);
virtual uint16_t  GetLabel(int i);
virtual void      SetEnergy(int i, uint16_t energy);
virtual uint16_t  GetEnergy(int i);
virtual void      GetParameters(int i, uint16_t *label, uint16_t* value);
virtual void      SetLocalCount(uint16_t count);
virtual uint16_t  GetLocalCount();
virtual void      SetChecksum(uint16_t cksum);
virtual uint16_t  GetChecksum();
virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);

virtual string DumpData(char mode, bool nozero);
virtual void   PrintStat(string info);
virtual string GetStat(string info);
virtual void   FillStat();
virtual void   InitStat();

};
#pragma pack(pop) // free aligment
#endif
