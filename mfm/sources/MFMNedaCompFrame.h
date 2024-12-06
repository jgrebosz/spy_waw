#ifndef _MFMNedaCompFrame_
#define _MFMNedaCompFrame_
/*
  MFMNedaCompFrame.h

	Copyright Acquisition group, GANIL Caen, France

*/

#include "MFMBlobFrame.h"


#define NEDACOMP_HEADERSIZE 20
#define NEDACOMP_FRAMESIZE  42
#define NEDACOMP_STD_UNIT_BLOCK_SIZE 2
#define NEDACOMP_BOARD_ID_MASK 0x0fff
#define NEDACOMP_CHANNEL_ID_MASK 0x000f
#pragma pack(push, 1) // force alignment


struct  MFM_NedaComp_EventInfo {
	  unsigned EventIdx     : 32;
	  char     EventTime[6];
	  unsigned LocationId     : 16;
	};

// NedaComp data
struct MFM_NedaComp_Data {
	      unsigned Energy         : 16;
	  	  unsigned Time           : 16;
	  	  unsigned TdcCorValue    : 16;
	  	  unsigned SlowIntegral : 32;
	  	  unsigned FastIntegral : 32;
	  	  signed   IntRaiseTime : 32;
	  	  unsigned NeuralNetWork: 16;
	  	  unsigned NbZero       : 8;
	  	  unsigned NeutronFlag  : 8;
	  	};


struct MFM_NedaComp_Header{
	 MFM_common_header      NedaCompHeader;
	 MFM_NedaComp_EventInfo 	NedaCompEvtInfo;
};
struct MFM_NedaComp_Frame{
	 MFM_NedaComp_Header       NedaCompHeader;
	 MFM_NedaComp_Data         NedaCompData;
};

//____________MFMNedaCompFrame___________________________________________________________

class MFMNedaCompFrame : public MFMBlobFrame
{

private:
 MFM_NedaComp_Frame* pNedaCompFrame;
long long * fCountNbEventCard;
uint64_t fTimeStamp;
uint32_t fEventNumber;

public :

MFMNedaCompFrame();
MFMNedaCompFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize,
		       int itemSize, int nItems);
virtual ~MFMNedaCompFrame();
virtual void SetPointers(void * pt =NULL);


//virtual void SetHeaderBasic(MFM_basic_header* header) ;
virtual void SetBufferSize(int size, bool ifinferior) ;
virtual void SetAttributs(void * pt =NULL);
virtual uint64_t GetTimeStampAttribut();
virtual uint64_t GetTimeStamp();
virtual uint32_t GetEventNumber();
virtual uint32_t GetEventNumberAttibut();
virtual void SetTimeStamp(uint64_t timestamp);
virtual void SetEventNumber(uint32_t eventnumber);

virtual uint16_t GetBoardId();
virtual uint16_t GetChannelId();
virtual void SetLocationId(uint16_t Id);
virtual void SetLocationId(uint16_t ChannelId, uint16_t BoardId);
virtual uint16_t GetLocationId();
virtual uint16_t GetEnergy() ;
virtual void SetEnergy(uint16_t energy) ;
virtual uint16_t GetTime() ;
virtual void SetTime(uint16_t time) ;
virtual uint16_t GetTdcCorValue() ;
virtual void SetTdcCorValue(uint16_t val);
virtual uint32_t GetSlowIntegral() ;
virtual void SetSlowIntegral(uint32_t integral);
virtual uint32_t GetFastIntegral() ;
virtual void SetFastIntegral(uint32_t integral) ;
virtual int32_t GetIntRaiseTime() ;
virtual void SetIntRaiseTime(int32_t time) ;
virtual uint16_t GetNeuralNetWork() ;
virtual void SetNeuralNetWork(uint16_t neural) ;
virtual uint8_t GetNbZero() ;
virtual void SetNbZero(uint8_t nb) ;
virtual bool GetNeutronFlag() ;
virtual void SetNeutronFlag(bool neutron);

virtual void FillEventWithRamdomConst(uint64_t timestamp=0,uint32_t enventnumber=0);
virtual void GenerateANedaExample(int type,int32_t eventnumber);
virtual string GetHeaderDisplay(char* infotext) ;
virtual void InitStat() ;
virtual void FillStat();
virtual string  GetStat(string info);
};
#pragma pack(pop) // free alignment
#endif
