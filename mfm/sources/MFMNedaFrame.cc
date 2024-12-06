/*
 MFMNedaFrame.cc
 Copyright Acquisition group, GANIL Caen, France
 */

#include <iostream>
#include <cmath>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <time.h>
using namespace std;

#include "MFMNedaFrame.h"

//_______________________________________________________________________________
MFMNedaFrame::MFMNedaFrame(int unitBlock_size, int dataSource, int frameType,
		int revision, int frameSize, int headerSize, int itemSize, int nItems) {
	/// Constructor of frame with a memory space\n
	/// fill header information : unitBlock_size,dataSource,....
	SetPointers();
	fCountNbEventCard = NULL;
	fEndFrame =0xF0F0F0F0;
	fNbofGoodF0F0 = 0;
    fNbofBadF0F0  = 0;

}

//_______________________________________________________________________________
MFMNedaFrame::MFMNedaFrame() {
	/// Constructor of a empty frame object
	fCountNbEventCard = NULL;
	fEndFrame =0xF0F0F0F0;
	fNbofGoodF0F0 = 0;
    fNbofBadF0F0  = 0;
}
//_______________________________________________________________________________
MFMNedaFrame::~MFMNedaFrame() {
	///Destructor
	if (fCountNbEventCard) {
		delete[] fCountNbEventCard;
		fCountNbEventCard = NULL;
	}

}
//_______________________________________________________________________________
void MFMNedaFrame::SetBufferSize(int size, bool ifinferior) {
	/// Do memory allocation or a reallacation for frame\n
	/// if ifinferior==true the allocaton is forced to size event if the acutal size is bigger\n
	MFMBasicFrame::SetBufferSize(size, ifinferior);
	MFMNedaFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMNedaFrame::SetPointers(void * pt) {
	/// Initialize pointers of frame\n
	/// if pt==NULL initialization is with current value of main pointer of frame (pData)\n
	/// else initialization is done with pData = pt\n
	/// pData must be the reference;
	MFMBasicFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pNedaFrame = (MFM_Neda_Frame*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMNedaFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMBasicFrame::SetAttributs(pt);
}
//_______________________________________________________________________________
string MFMNedaFrame::GetHeaderDisplay(char* infotext) {
	stringstream ss;
	string display("");
	display = ss.str();

	ss << MFMBasicFrame::GetHeaderDisplay(infotext);
	ss << "   EN = " << GetEventNumber();
	ss << "   TS = " << GetTimeStamp();
	ss << endl;
	ss << "   Board = " << GetBoardId() << " | Channel = " << GetChannelId()
			<< " | LeInter = " << (int) GetLeInterval();
	ss << " | ZcoInterval = " << (int) GetZcoInterval() << endl;
	ss << "   Tdc =  " << GetTdcValue() << " | Slow Integ = "
			<< GetSlowIntegral() << " | FastIntegral = ";
	ss << GetFastIntegral() << " | Bitfield = " << (int) GetBitfield()
			<< "| AbsMax = " << (int) GetAbsMax() << endl;
	ss << " Event is neutron = " << IsNeutron() << " | Valid CFD = " << IsCFDValid() << " | Parity = " << IsCFDParity() << endl; 

	display = ss.str();

	return display;
}
//_______________________________________________________________________________

uint64_t MFMNedaFrame::GetTimeStamp() {
	/// Compute time stamp and fill fTimeStamp attribut. return value of TimeStamp
	fTimeStamp = 0;
	uint64_t* timeStamp = &(fTimeStamp);

	if (GetRevision() == 0) {
		int byte_numexo[6] = { 1, 0, 5, 4, 3, 2 };
		for (int j = 0; j < 6; j++)
			memcpy(
					((char*) (&fTimeStamp)) + j,
					((char*) ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.EventTime)
							+ byte_numexo[j], 1);
	} else {
		memcpy(((char*) (&fTimeStamp)),
				((MFM_Neda_Header*) pHeader)->NedaEvtInfo.EventTime, 6);

	}
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt64((timeStamp), 6);
	return fTimeStamp;
}
/*
 //_______________________________________________________________________________

 uint64_t MFMNedaFrame::GetTimeStamp2() {
 /// Compute time stamp and fill fTimeStamp attribut. return value of TimeStamp
 fTimeStamp = 0;
 uint64_t* timeStamp = &(fTimeStamp);

 memcpy(((char*) (&fTimeStamp)),
 ((char*) ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.EventTime) + 2,
 4);

 memcpy(((char*) (&fTimeStamp)) + 4,
 ((char*) ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.EventTime), 2);

 if (fLocalIsBigEndian != fFrameIsBigEndian)
 SwapInt64((timeStamp), 6);
 return fTimeStamp;
 }
 */
//_______________________________________________________________________________
uint64_t MFMNedaFrame::GetTimeStampAttribut() {
	/// return time stamp without computing it
	return fTimeStamp;
}

//_______________________________________________________________________________

uint32_t MFMNedaFrame::GetEventNumber() {
	/// Compute and return envent number
	fEventNumber = 0;
	char * eventNumber = (char*) &(fEventNumber);

	fEventNumber = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.EventIdx;
	if (GetRevision() != 0) {
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32((uint32_t *) (eventNumber), 4);
		}
		

	return fEventNumber;
}
//_______________________________________________________________________________
uint32_t MFMNedaFrame::GetEventNumberAttibut() {
	/// Return event number without computing it
	return fEventNumber;
}

//_______________________________________________________________________________
void MFMNedaFrame::SetTimeStamp(uint64_t timestamp) {
	// Set frame timestamp
	char* pts = (char*) &timestamp;
	timestamp = timestamp & 0x0000ffffffffffff;
	memcpy(((MFM_Neda_Header*) pHeader)->NedaEvtInfo.EventTime, pts, 6);
}

//_______________________________________________________________________________
void MFMNedaFrame::SetEventNumber(uint32_t eventnumber) {
	/// set frame event number
	((MFM_Neda_Header*) pHeader)->NedaEvtInfo.EventIdx = eventnumber;
}

//_______________________________________________________________________________
void MFMNedaFrame::SetLocationId(uint16_t Id) {
	/// Set 16 bits of LocationId
	((MFM_Neda_Header*) pHeader)->NedaEvtInfo.LocationId = Id;
}
//_______________________________________________________________________________
void MFMNedaFrame::SetLocationId(uint16_t ChannelId, uint16_t BoardId) {
	uint16_t ui, ui2;
	ui2 = 0;
	//trig request form 0 to 4
	ui2 = ChannelId & NEDA_CHANNEL_ID_MASK;

	//id board from 5 to 15
	ui = BoardId & NEDA_BOARD_ID_MASK;
	ui = ui << 4;
	ui2 = ui2 | ui;
	SetLocationId(ui2);
}

//_______________________________________________________________________________

uint16_t MFMNedaFrame::GetLocationId() {
	uint16_t Id = 0;
	/// Compute and return the 2 bytes of LocationId()
	Id = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.LocationId;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16((&Id));
	return ((Id));
}
//_______________________________________________________________________________
uint16_t MFMNedaFrame::GetChannelId() {
	/// Compute and return extracted ChannelId
	return (GetLocationId() & NEDA_CHANNEL_ID_MASK);
}

//_______________________________________________________________________________
uint16_t MFMNedaFrame::GetBoardId() {
	/// Compute and return id value of Board
	return (((GetLocationId() >> 4) & NEDA_BOARD_ID_MASK));
}
//_______________________________________________________________________________

uint8_t MFMNedaFrame::GetLeInterval() {
	uint8_t id = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.LeInterval;
	return id;
}
//_______________________________________________________________________________
void MFMNedaFrame::SetLeInterval(uint8_t interval) {
	((MFM_Neda_Header*) pHeader)->NedaEvtInfo.LeInterval = interval;
}
//_______________________________________________________________________________
uint8_t MFMNedaFrame::GetZcoInterval() {
	return (uint8_t) ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.ZcoInterval;

}
//_______________________________________________________________________________
void MFMNedaFrame::SetZcoInterval(uint8_t interval) {
	((MFM_Neda_Header*) pHeader)->NedaEvtInfo.ZcoInterval = interval;
}
//_______________________________________________________________________________
uint16_t MFMNedaFrame::GetTdcValue() {
	uint16_t val = 0;
	val = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.TdcValue;
	if (GetRevision() > 0) {
		if (fLocalIsBigEndian != fFrameIsBigEndian) {
			SwapInt16(&val);
		}
	} else {
		val = val & 0xff;
	}
	return val;
}
//_______________________________________________________________________________
void MFMNedaFrame::SetTdcValue(uint16_t val) {
	((MFM_Neda_Header*) pHeader)->NedaEvtInfo.TdcValue = val;
}
//_______________________________________________________________________________
uint16_t MFMNedaFrame::GetSlowIntegral() {

	uint16_t integral = 0;
	char * pintegral = (char*) &(integral);

	integral = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.SlowIntegral;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		if (GetRevision() > 0)
			SwapInt16((uint16_t *) (pintegral));
	return integral;
}
//_______________________________________________________________________________
void MFMNedaFrame::SetSlowIntegral(uint16_t id) {
	((MFM_Neda_Header*) pHeader)->NedaEvtInfo.SlowIntegral = id;
}
//_______________________________________________________________________________
uint16_t MFMNedaFrame::GetFastIntegral() {
	uint16_t integral = 0;
	char * pintegral = (char*) &(integral);

	integral = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.FastIntegral;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		if (GetRevision() > 0)
			SwapInt16((uint16_t *) (pintegral));
	return integral;
}
//_______________________________________________________________________________
void MFMNedaFrame::SetFastIntegral(uint16_t id) {
	((MFM_Neda_Header*) pHeader)->NedaEvtInfo.FastIntegral = id;
}
//_______________________________________________________________________________
uint8_t MFMNedaFrame::GetBitfield() {
	uint8_t id = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.Bitfield;
	return id;
}
//_______________________________________________________________________________
bool MFMNedaFrame::GetBitfield(int id) {
	uint8_t bitfield = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.Bitfield;
	return ((bool) ((bitfield >> id) & 0x1));
}
//_______________________________________________________________________________
void MFMNedaFrame::SetBitfield(uint8_t bitfield) {
	((MFM_Neda_Header*) pHeader)->NedaEvtInfo.Bitfield = bitfield;
}
//_______________________________________________________________________________
void MFMNedaFrame::SetBitfield(int id, bool bit) {
	uint8_t bitfield = GetBitfield();
	uint8_t localbit = 0x1 << id;
	if (bit) {
		bitfield = localbit | bitfield;
	} else {
		bitfield = bitfield & (~localbit);
	}
	((MFM_Neda_Header*) pHeader)->NedaEvtInfo.Bitfield = bitfield;
}
//_______________________________________________________________________________
uint8_t MFMNedaFrame::GetAbsMax() {
	uint8_t id = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.AbsMax;
	return id;
}
//_______________________________________________________________________________
void MFMNedaFrame::SetAbsMax(uint8_t id) {
	((MFM_Neda_Header*) pHeader)->NedaEvtInfo.AbsMax = id;
}
//_______________________________________________________________________________
uint8_t MFMNedaFrame::GetInterpolCFD() {
	uint8_t id = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.InterpolCFD;
	return id;
}
//_______________________________________________________________________________
void MFMNedaFrame::SetInterpolCFD(uint8_t id) {
	((MFM_Neda_Header*) pHeader)->NedaEvtInfo.InterpolCFD = id;
}
//_______________________________________________________________________________
void MFMNedaFrame::NedaGetParameters(int i, uint16_t *value) {
	/// Compute and return the couple information of  value of the i-th item
	NedaGetParametersByItem((MFM_Neda_Item *) GetItem(i), value);
	int parity = (*value & NEDA_MASK_PARITY_ITEM) >> 14;
	*value = *value & NEDA_MASK_DATA_ITEM;
	if ((i % 2) == 0 && parity != 0) {
		std::cerr << "Problem decoding even sample "<<i<<" val  "<< *value << " parity " <<parity<<   std::endl;
		*value = 0;
	} else if ((i % 2) == 1 && parity != 1) {
		std::cerr << "Problem decoding odd sample "<<i<<" val  " << *value  << " parity " <<parity << std::endl;
		*value = 0;
	}
}
//_______________________________________________________________________________
void MFMNedaFrame::NedaGetParametersByItem(MFM_Neda_Item *item, uint16_t *value) {
	/// Compute and return the couple information of value of  item


	if (fLocalIsBigEndian != fFrameIsBigEndian) {
		uint16_t tmp = item->Value;
		if (GetRevision() > 0)
			SwapInt16(&tmp);
		*value = tmp;
	} else {
		*value = item->Value;
	}
	
}
//_______________________________________________________________________________
void MFMNedaFrame::NedaSetParameters(int i, uint16_t value) {
	/// set i-th item if frame with the couple information of value
	if ((i % 2) == 0)
		value = value | NEDA_MASK_DATA_ITEM;
	if ((i % 2) == 1)
		value = value | (NEDA_MASK_PARITY_ITEM | NEDA_MASK_DATA_ITEM);
	NedaSetParametersByItem((MFM_Neda_Item *) GetItem(i), value);

}
//_______________________________________________________________________________
void MFMNedaFrame::NedaSetParametersByItem(MFM_Neda_Item *item, uint16_t value) {
	/// set "item" with the couple information of value

	item->Value = value;
}
//_______________________________________________________________________________
void MFMNedaFrame::SetEndFrame(uint32_t end) {
	fEndFrame =end;
}
//_______________________________________________________________________________
bool MFMNedaFrame::TestEndOfFrame() {
	/// test last 4 butes of frame ,
		return (( pNedaFrame)->MFMNedaEOF.EndOfFrame == 0xf0f0f0f0);
}
//_______________________________________________________________________________
void MFMNedaFrame::FillEndOfFrame() {
	/// last 4 bytes to of frame,

static unsigned int seed = 6;
	if ((fEndFrame == 0xf0f0f0f0)|| (fEndFrame == 0)){
		((MFM_Neda_Frame*) pNedaFrame)->MFMNedaEOF.EndOfFrame = fEndFrame;

	}else{
		// get fEndFrame witch give the rate of error in per cent
		uint rando = seed;
		float tempo = rand_r(&seed);
		rando = (int )((float) (tempo *100/ (RAND_MAX + 1.0)));

		if (rando < fEndFrame){

			((MFM_Neda_Frame*) pNedaFrame)->MFMNedaEOF.EndOfFrame = 0xABBAABBA;

		}else{
			((MFM_Neda_Frame*) pNedaFrame)->MFMNedaEOF.EndOfFrame = 0xf0f0f0f0;
		}
	}

}
//_______________________________________________________________________________
void MFMNedaFrame::FillEventWithRamdomConst(uint64_t timestamp,
		uint32_t enventnumber) {
	/// Fill frame items  of sinus values with random perios,

	int size = GetNbItems();
	int nb_bits = 16;
	float nbofperiodes = 1;

	int i;
	float P;
	static unsigned int seed = 15;
	unsigned short uivalue;
	float rando, tempof;
	P = 1.570796327 * 2;
	rando = seed;

	int h;
	h = pow(2, nb_bits) - 1;

	if (size > 0)
		NedaSetParameters(0, 1);
	rando = (float) ((rand_r(&seed) / (RAND_MAX + 1.0)));
	seed++;

	for (i = 1; i < size; i++) {

		seed++;
		nbofperiodes = 1 + rando * 4;

		tempof = (float) i / (size / (nbofperiodes));
		uivalue
				= (unsigned short) (h / 2 + h / 2 * sin((float) 2 * P * tempof));

		NedaSetParameters(i, uivalue);
	}
	SetEventNumber(enventnumber);
	SetTimeStamp(timestamp);
	FillEndOfFrame();

}

//_______________________________________________________________________________

void MFMNedaFrame::GenerateANedaExample(int type, int eventnumber) {
	/// Generate a example of frame containing random value\n
	/// usable for tests.
	if (type != MFM_NEDA_FRAME_TYPE) {
		cout
				<< "Error in  MFMNedaFrame::GenerateANedaExample type not understood\n";
		return;
	}

	uint32_t unitBlock_size = 0;
	uint32_t itemsize = 0;
	uint32_t nbitem = 0;
	uint32_t framesize = 0;
	uint32_t revision = 1;
	uint32_t headersize = 0;
	unitBlock_size = NEDA_STD_UNIT_BLOCK_SIZE;
	itemsize = 2;
	nbitem = NEDA_NB_OF_ITEMS;

	headersize = NEDA_HEADERSIZE;

	framesize = headersize + nbitem * itemsize + sizeof(MFM_Neda_EOF);
	revision = 1;
	// generation of MFM header
	MFM_make_header(unitBlock_size, 0, type, revision, (int) (framesize
			/ unitBlock_size), (headersize / unitBlock_size), itemsize, nbitem);

	SetLocationId(1, 2);
	SetLeInterval(3);
	SetZcoInterval(4);
	SetTdcValue(5);
	SetSlowIntegral(6);
	SetFastIntegral(7);
	SetBitfield(8);
	SetAbsMax(9);
	FillEventWithRamdomConst(GetTimeStampUs(), eventnumber);
	FillEndOfFrame();
}
//____________________________________________________________________________

string MFMNedaFrame::DumpData(char mode, bool nozero) {
	// Dump  values of the current event.
	// if enter parameter is true (default value), all zero parameter of event aren't dumped
	// mode = 'd' for decimal, 'b' for binary, 'h' for hexa, 'o' for octal

	stringstream ss;
	string display("");

	int i, j, maxbin, presentation = 0, max_presentation = 5;
	char tempo[255];
	char Bin[255];
	char Bin2[255];

	int reste;
	char one = '1', zero = '0';
	int DecNumber = 0;

	if (mode == 'b')
		max_presentation = 3;
	if (GetEventNumber() == 0xFFFFFFFF) {
		ss << "No Event , so no event dump. Get a new event frame";
	} else {
		for (i = 0; i < GetNbItemsAttribut(); i++) {

			uint16_t value = 0;
			NedaGetParameters(i, &value);
			if ((value != 0xFFFF) || (nozero == false)) {
				switch (mode) {
				case 'd':// decimal display
					sprintf(tempo, "|%5d ", value);
					break;
				case 'o':// octal display
					sprintf(tempo, "|%5o ", value);
					break;
				case 'h':// hexa display
					sprintf(tempo, "|%5x x", value);
					break;
				case 'b': // binary display
					DecNumber = (int) value;
					Bin[0] = '\0';
					Bin[1] = zero;
					Bin2[1] = '\0';
					Bin2[0] = zero;
					j = 1;
					if (DecNumber != 0) {
						while (DecNumber >= 1) {
							reste = DecNumber % 2;
							DecNumber -= reste;
							DecNumber /= 2;
							if (reste)
								Bin[j] = one;
							else
								Bin[j] = zero;
							j++;
						}
						j--;
						maxbin = j;
						while (j >= 0) {
							Bin2[j] = Bin[maxbin - j];
							j--;
						}
					}
					sprintf(tempo, "|%16s ", Bin2);
					break;
				}
				ss << tempo;
				presentation++;
			}
			if (presentation == max_presentation) {
				ss << "|\n";
				presentation = 0;
			}
		}
		if (presentation != 0)
			ss << "|\n";
	}
	display = ss.str();
	return display;
}

//_______________________________________________________________________________
void MFMNedaFrame::InitStat() {
	MFMBasicFrame::InitStat();
	int i;
	fCountNbEventCard = new long long[65536];
	for (i = 0; i < 65536; i++) {
		fCountNbEventCard[i] = 0;
	}
}
//____________________________________________________________________
void MFMNedaFrame::FillStat() {
	MFMBasicFrame::FillStat();
	uint16_t id;
	id = GetLocationId();
	fCountNbEventCard[id]++;
	if (TestEndOfFrame())
		fNbofGoodF0F0++;
	else
		fNbofBadF0F0++;
	uint32_t eventnumber = GetEventNumber();
	static uint32_t eventnumberold = 0;
	if (eventnumber != 0) {
		if (eventnumber - eventnumberold < 0)
			IncrementNegativJump();
		if (eventnumber - eventnumberold != 1)
			IncrementNoContiJump();
		eventnumberold=eventnumber;
	}

}
//____________________________________________________________________
string MFMNedaFrame::GetStat(string info) {

	string display("");
	stringstream ss("");
	ss << MFMBasicFrame::GetStat(info);
	int i, j;
	int total = 0;
	for (i = 0; i < 65536; i++) {
		if (fCountNbEventCard[i] != 0) {
			j = i;
			ss << "Card " << ((j >> 4) & NEDA_BOARD_ID_MASK);
			j = i;
			ss << " Cristal  " << (j & NEDA_CHANNEL_ID_MASK);
			ss << " NbEvents = " << fCountNbEventCard[i] << "\n";
			total += fCountNbEventCard[i];
		}
	}
	ss << " Nb of Good F0F0F0F0          = "<<fNbofGoodF0F0  << "\n";
	ss << " Nb of Bad  F0F0F0F0          = "<<fNbofBadF0F0  << "\n";
	ss << " Total                        = " << total << "\n";
	display = ss.str();
	return display;
}
//____________________________________________________________________
bool MFMNedaFrame::IsNeutron()
{
  uint8_t id = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.Bitfield;
  return id>>7;
}
//____________________________________________________________________
bool MFMNedaFrame::IsCFDValid()
{
  uint8_t id = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.Bitfield;
  return (id>>5)&0x1;
}
//____________________________________________________________________
bool MFMNedaFrame::IsCFDParity()
{
  uint8_t id = ((MFM_Neda_Header*) pHeader)->NedaEvtInfo.Bitfield;
  return (id>>6)&0x1;
}
