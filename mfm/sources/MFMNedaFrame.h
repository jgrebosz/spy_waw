#ifndef _MFMNedaFrame_
#define _MFMNedaFrame_
/*
  MFMNedaFrame.h

  Copyright Acquisition group, GANIL Caen, France

*/

#include "MFMBasicFrame.h"
#include<map>
#include <vector>
#define NEDA_HEADERSIZE 44
#define NEDA_STD_UNIT_BLOCK_SIZE 2
#define NEDA_FRAMESIZE 512
#define NEDA_NB_OF_ITEMS 232
#define NEDA_BOARD_ID_MASK 0x0fff
#define NEDA_CHANNEL_ID_MASK 0x000f
#define NEDA_MASK_DATA_ITEM 0x3fff
#define NEDA_MASK_PARITY_ITEM 0x4000
#pragma pack(push, 1) // force alignment


struct MFM_Neda_EventInfo {
  unsigned LocationId   : 16;
  unsigned EventIdx     : 32;
  char     EventTime[6];
  unsigned LeInterval   : 8;
  unsigned ZcoInterval  : 8;
  unsigned TdcValue     : 16;
  unsigned Free0        : 16;
  unsigned SlowIntegral : 16;
  unsigned Free1        : 16;
  unsigned FastIntegral : 16;
  unsigned Bitfield     : 8;
  unsigned AbsMax       : 8;
  unsigned InterpolCFD  : 8;
  unsigned Free2        : 8;
};

// Neda
struct MFM_Neda_Item {
  unsigned Value :  16;
};
struct MFM_Neda_EOF {
  unsigned EndOfFrame :  32;
};

struct MFM_Neda_Header{
  MFM_basic_header       NedaBasicHeader;
  MFM_Neda_EventInfo 	NedaEvtInfo;
};
struct MFM_Neda_Frame{
  MFM_Neda_Header       NedaNedaHeader;
  MFM_Neda_Item         MFMNedaItem[NEDA_NB_OF_ITEMS];
  MFM_Neda_EOF 		   MFMNedaEOF;
};

//____________MFMNedaFrame___________________________________________________________

class MFMNedaFrame : public MFMBasicFrame
{

 private:
  MFM_Neda_Frame* pNedaFrame;
  long long * fCountNbEventCard;
  long long	int fNbofGoodF0F0;
  long long	int fNbofBadF0F0;

  uint32_t fEndFrame;
 public :

  MFMNedaFrame();
  MFMNedaFrame(int unitBlock_size, int dataSource,
	       int frameType, int revision, int frameSize,int headerSize,
	       int itemSize, int nItems);
  virtual ~MFMNedaFrame();
  virtual void SetPointers(void * pt =NULL);


  //virtual void SetHeaderBasic(MFM_basic_header* header) ;
  virtual void SetBufferSize(int size, bool ifinferior) ;
  virtual void SetAttributs(void * pt =NULL);
  virtual uint64_t GetTimeStampAttribut();
  virtual uint64_t GetTimeStamp();
  //virtual uint64_t GetTimeStamp2();
  virtual uint32_t GetEventNumber();
  virtual uint32_t GetEventNumberAttibut();
  virtual void SetTimeStamp(uint64_t timestamp);
  virtual void SetEventNumber(uint32_t eventnumber);

  virtual uint16_t GetBoardId();
  virtual uint16_t GetChannelId();
  virtual void SetLocationId(uint16_t Id);
  virtual void SetLocationId(uint16_t ChannelId, uint16_t BoardId);
  virtual uint16_t GetLocationId();
  virtual uint8_t GetLeInterval();
  virtual void SetLeInterval(uint8_t id);
  virtual uint8_t GetZcoInterval();
  virtual void SetZcoInterval(uint8_t interval);
  virtual uint16_t GetTdcValue();
  virtual void SetTdcValue(uint16_t value);
  virtual uint16_t GetSlowIntegral();
  virtual void SetSlowIntegral(uint16_t integral);
  virtual uint16_t GetFastIntegral();
  virtual void SetFastIntegral(uint16_t integral);
  virtual uint8_t GetBitfield();
  virtual bool GetBitfield(int id);
  virtual void SetBitfield(uint8_t id);
  virtual void SetBitfield(int  id,bool bit);
  virtual uint8_t GetAbsMax();
  virtual void SetInterpolCFD(uint8_t id);
  virtual uint8_t GetInterpolCFD();
  virtual void SetAbsMax(uint8_t id);
    virtual void NedaGetParametersByItem(MFM_Neda_Item *item,uint16_t *value);
  virtual void NedaSetParametersByItem(MFM_Neda_Item *item,uint16_t value);
  virtual void NedaGetParameters(int i, uint16_t *value);
  virtual void NedaSetParameters(int i, uint16_t value);
  virtual void FillEventWithRamdomConst(uint64_t timestamp=0,uint32_t enventnumber=0);
  virtual void GenerateANedaExample(int type,int32_t eventnumber);
  virtual string GetHeaderDisplay(char* infotext) ;
  virtual string DumpData(char mode='d', bool nozero=false);
  virtual void InitStat() ;
  virtual void FillStat();
  virtual string  GetStat(string info);
  virtual void SetEndFrame(uint32_t end);
  virtual void FillEndOfFrame();
  virtual bool TestEndOfFrame();
  // Added by Alain to decode the bitfield
  virtual bool IsNeutron();
  virtual bool IsCFDValid();
  virtual bool IsCFDParity();

};
#pragma pack(pop) // free alignment
#endif
