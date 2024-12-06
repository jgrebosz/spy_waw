#ifndef _MFMXmlDataDescriptionFrame_
#define _MFMXmlDataDescriptionFrame_
/*
  MFMXmlDataDescriptionFrame.h
	Copyright Acquisition group, GANIL Caen, France
*/

#include "MFMBlobFrame.h"
#ifndef NO_MFMXML
#include <tinyxml.h>
#endif 
#include "XmlTags.h"

#pragma pack(push, 1) // force alignment

struct MFM_xml_Data {
};

struct MFM_XmlDataDescription_frame{
	 MFM_common_header  XmlBlobHeader;
	 MFM_xml_Data  XmlData;
};



//____________MFMXmlDataDescriptionFrame___________________________________________________________

class MFMXmlDataDescriptionFrame : public MFMBlobFrame
{
private:
uint64_t fTimeStamp;
int fEventNumber ;
#ifndef NO_MFMXML
TiXmlDocument * MyTinyDoc;
#endif 
public :

MFMXmlDataDescriptionFrame();
MFMXmlDataDescriptionFrame(int unitBlock_size, int dataSource,
	 		 int frameType, int revision, int frameSize,int headerSize);
virtual ~MFMXmlDataDescriptionFrame();
virtual void SetPointers(void * pt =NULL);

virtual void SetBufferSize(int size, bool ifinferior=true) ;
virtual void SetAttributs(void * pt =NULL);
virtual uint64_t GetTimeStamp();
virtual uint64_t GetTimeStampAttribut();
virtual uint32_t GetEventNumberAttribut();
virtual uint32_t GetEventNumber();
virtual char * GetText();
virtual void SetTimeStamp(uint64_t timestamp);
virtual void SetEventNumber(uint32_t eventnumber);

virtual string FillExampleOfText();
virtual void   FillEventRandomConst();
virtual int    InitXml(bool write_or_read, string commentaire);

void dump_to_stdout(const char* pFilename);
#ifndef NO_MFMXML 
void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 );
int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent);
#endif 
const char * getIndentAlt( unsigned int numIndents );
const char * getIndent( unsigned int numIndents );
};
#pragma pack(pop) // free aligment
#endif
