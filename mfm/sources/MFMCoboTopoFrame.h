#ifndef _MFMCoboTopoFrame_
#define _MFMCoboTopoFrame_
/*
  MFMCoboTopoFrame.h
	Copyright Acquisition group, GANIL Caen, France
*/

#include "MFMBlobFrame.h"
#include <tinyxml.h>

#pragma pack(push, 1) // force alignment

struct MFM_CoboTopo_Data {
	  unsigned CoboIdx  :  1;
	  unsigned AsadMask :  1;
	  unsigned TpMode   :  1;
	  unsigned Unused   :  1;

};

struct MFM_CoboTopo_header{
	 MFM_common_header  CoboTopoBlobHeader;
	 MFM_CoboTopo_Data  CoboTopoData;
};



//____________MFMCoboTopoFrame___________________________________________________________

class MFMCoboTopoFrame : public MFMBlobFrame
{
private:
uint64_t fTimeStamp;
int fEventNumber ;

TiXmlDocument * MyTinyDoc;

public :

MFMCoboTopoFrame();
MFMCoboTopoFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMCoboTopoFrame();
virtual void SetPointers(void * pt =NULL);

virtual void SetBufferSize(int size, bool ifinferior) ;
virtual void SetAttributs(void * pt =NULL);
virtual uint64_t GetTimeStamp();
virtual uint64_t GetTimeStampAttribut();
virtual uint32_t GetEventNumberAttribut();
virtual uint32_t GetEventNumber();

virtual void SetTimeStamp(uint64_t timestamp);
virtual void SetEventNumber(int eventnumber);
virtual uint16_t CoboGetIdx();
virtual void CoboSetIdx(uint16_t idx);
virtual uint16_t CoboGetAsadMask();
virtual void CoboSetAsadMask(uint16_t asadmask);
virtual uint16_t CoboGetTpMode ();
virtual void CoboSetTpMode (uint16_t mode);
};
#pragma pack(pop) // free aligment
#endif
