#ifndef _MFMReaGenericFrame_
#define _MFMReaGenericFrame_
/*
  MFMReaGenericFrame.h

	Copyright Acquisition group, GANIL Caen, France

*/

#include "MFMNumExoFrame.h"

#define REA_GENERIC_NB_STATUS 2
#define REA_GENERIC_STD_UNIT_BLOCK_SIZE 4





#pragma pack(push, 1) // force alignment


struct MFM_ReaGeneric_data{
  unsigned CristalId : 16;
  unsigned Status1   : 16;
  unsigned Status2   : 16;
  unsigned Type_Tns  : 16;
  unsigned Energy    : 16;
  unsigned Time      : 16;
  unsigned Checksum  : 16;
};


struct MFM_ReaGeneric_frame{
	 MFM_common_header         Header;
	 MFM_numexo_eventInfo      EventInfo;
	 MFM_ReaGeneric_data       Data;
};
enum ReaTnsType {REA_GENERIC_ENERGY_TYPE =0x00,REA_GENERIC_ENERGY_TIME_TYPE=0x01,REA_GENERIC_TIME_TYPE=0x02,REA_GENERIC_CHARGE_TYPE=0x04};


//____________MFMReaGenericFrame___________________________________________________________

class MFMReaGenericFrame : public MFMNumExoFrame
{



public :


virtual void      SetStatus(int i, uint16_t status);
virtual uint16_t  GetStatus(int i);
virtual void      SetTypeTns(enum ReaTnsType type);
virtual enum ReaTnsType  GetTypeTns();
virtual void      SetEnergy(uint16_t energie);
virtual uint16_t  GetEnergy();
virtual void      SetTime(uint16_t time);
virtual uint16_t  GetTime();
virtual void      FillEventRandomConst(uint64_t timestamp=0,uint32_t enventnumber=0);


};
#pragma pack(pop) // free aligment
#endif
