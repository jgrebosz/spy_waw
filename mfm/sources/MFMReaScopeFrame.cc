/*
 MFMReaScopeFrame.cc
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

#include "MFMReaScopeFrame.h"

//_______________________________________________________________________________
MFMReaScopeFrame::MFMReaScopeFrame(int unitBlock_size, int dataSource, int frameType,
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
MFMReaScopeFrame::MFMReaScopeFrame() {
	/// Constructor of a empty frame object
	fCountNbEventCard = NULL;
	fEndFrame =0xF0F0F0F0;
	fNbofGoodF0F0 = 0;
        fNbofBadF0F0  = 0;
}
//_______________________________________________________________________________
MFMReaScopeFrame::~MFMReaScopeFrame() {
	///Destructor
	if (fCountNbEventCard) {
		delete[] fCountNbEventCard;
		fCountNbEventCard = NULL;
	}
}
//_______________________________________________________________________________
void MFMReaScopeFrame::SetBufferSize(int size, bool ifinferior) {
	/// Do memory allocation or a reallacation for frame\n
	/// if ifinferior==true the allocaton is forced to size event if the acutal size is bigger\n
	MFMBasicFrame::SetBufferSize(size, ifinferior);
	MFMReaScopeFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMReaScopeFrame::SetPointers(void * pt) {
	/// Initialize pointers of frame\n
	/// if pt==NULL initialization is with current value of main pointer of frame (pData)\n
	/// else initialization is done with pData = pt\n
	/// pData must be the reference;
	MFMBasicFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pReaScopeFrame = (MFM_ReaScope_Frame*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMReaScopeFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMBasicFrame::SetAttributs(pt);
}
//_______________________________________________________________________________
string MFMReaScopeFrame::GetHeaderDisplay(char* infotext) {
	stringstream ss;
	string display("");
	display = ss.str();

	ss << MFMBasicFrame::GetHeaderDisplay(infotext);
	ss << "   EN = " << GetEventNumber();
	ss << "   TS = " << GetTimeStamp();
	ss << endl;
	ss << "   Board = " << GetBoardId() << " | Channel = " << GetChannelId()
	   << " | SetupScope = "<< GetSetupScope()  << endl; 

	display = ss.str();

	return display;
}
//_______________________________________________________________________________

uint64_t MFMReaScopeFrame::GetTimeStamp() {
	/// Compute time stamp and fill fTimeStamp attribut. return value of TimeStamp
	fTimeStamp = 0;
	uint64_t* timeStamp = &(fTimeStamp);

	if (GetRevision() == 0) {
		int byte_numexo[6] = { 1, 0, 5, 4, 3, 2 };
		for (int j = 0; j < 6; j++)
			memcpy(
					((char*) (&fTimeStamp)) + j,
					((char*) ((MFM_ReaScope_Header*) pHeader)->ReaScopeEvtInfo.EventTime)
							+ byte_numexo[j], 1);
	} else {
		memcpy(((char*) (&fTimeStamp)),
				((MFM_ReaScope_Header*) pHeader)->ReaScopeEvtInfo.EventTime, 6);

	}
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt64((timeStamp), 6);
	return fTimeStamp;
}

//_______________________________________________________________________________
uint64_t MFMReaScopeFrame::GetTimeStampAttribut() {
	/// return time stamp without computing it
	return fTimeStamp;
}

//_______________________________________________________________________________
uint32_t MFMReaScopeFrame::GetEventNumber() {
	/// Compute and return envent number
	fEventNumber = 0;
	char * eventNumber = (char*) &(fEventNumber);

	fEventNumber = ((MFM_ReaScope_Header*) pHeader)->ReaScopeEvtInfo.EventIdx;
	if (GetRevision() != 0) {
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32((uint32_t *) (eventNumber), 4);
		}
	return fEventNumber;
}
//_______________________________________________________________________________
uint32_t MFMReaScopeFrame::GetEventNumberAttibut() {
	/// Return event number without computing it
	return fEventNumber;
}

//_______________________________________________________________________________
void MFMReaScopeFrame::SetTimeStamp(uint64_t timestamp) {
	// Set frame timestamp
	char* pts = (char*) &timestamp;
	timestamp = timestamp & 0x0000ffffffffffff;
	memcpy(((MFM_ReaScope_Header*) pHeader)->ReaScopeEvtInfo.EventTime, pts, 6);
}

//_______________________________________________________________________________
void MFMReaScopeFrame::SetEventNumber(uint32_t eventnumber) {
	/// set frame event number
	((MFM_ReaScope_Header*) pHeader)->ReaScopeEvtInfo.EventIdx = eventnumber;
}

//_______________________________________________________________________________
void MFMReaScopeFrame::SetLocationId(uint16_t Id) {
	/// Set 16 bits of LocationId
	((MFM_ReaScope_Header*) pHeader)->ReaScopeEvtInfo.LocationId = Id;
}
//_______________________________________________________________________________
void MFMReaScopeFrame::SetLocationId(uint16_t ChannelId, uint16_t BoardId) {
	uint16_t ui, ui2;
	ui2 = 0;
	//trig request form 0 to 4
	ui2 = ChannelId & REA_SCOPE_CHANNEL_ID_MASK;

	//id board from 5 to 15
	ui = BoardId & REA_SCOPE_BOARD_ID_MASK;
	ui = ui << 4;
	ui2 = ui2 | ui;
	SetLocationId(ui2);
}

//_______________________________________________________________________________

uint16_t MFMReaScopeFrame::GetLocationId() {
	uint16_t Id = 0;
	/// Compute and return the 2 bytes of LocationId()
	Id = ((MFM_ReaScope_Header*) pHeader)->ReaScopeEvtInfo.LocationId;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16((&Id));
	return ((Id));
}
//_______________________________________________________________________________
uint16_t MFMReaScopeFrame::GetChannelId() {
	/// Compute and return extracted ChannelId
	return (GetLocationId() & REA_SCOPE_CHANNEL_ID_MASK);
}

//_______________________________________________________________________________
uint16_t MFMReaScopeFrame::GetBoardId() {
	/// Compute and return id value of Board
	return (((GetLocationId() >> 4) & REA_SCOPE_BOARD_ID_MASK));
}
//_______________________________________________________________________________
uint16_t MFMReaScopeFrame::GetSetupScope() {

	uint16_t integral = 0;
	char * pintegral = (char*) &(integral);

	integral = ((MFM_ReaScope_Header*) pHeader)->ReaScopeEvtInfo.SetupScope;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		if (GetRevision() > 0)
			SwapInt16((uint16_t *) (pintegral));
	return integral;
}
//_______________________________________________________________________________
void MFMReaScopeFrame::SetSetupScope(uint16_t setup) {
	((MFM_ReaScope_Header*) pHeader)->ReaScopeEvtInfo.SetupScope = setup;
}
//_______________________________________________________________________________
void MFMReaScopeFrame::ReaScopeGetParameters(int i, uint16_t *value) {
	/// Compute and return the couple information of  value of the i-th item
	ReaScopeGetParametersByItem((MFM_ReaScope_Item *) GetItem(i), value);
}
//_______________________________________________________________________________
void MFMReaScopeFrame::ReaScopeGetParametersByItem(MFM_ReaScope_Item *item, uint16_t *value) {
	/// Compute and return the couple information of value of  item

	if (fLocalIsBigEndian != fFrameIsBigEndian) {
		uint16_t tmp = item->Value;
		SwapInt16(&tmp);
		*value = tmp;
	} else {
		*value = item->Value;
	}
	
}
//_______________________________________________________________________________
void MFMReaScopeFrame::ReaScopeSetParameters(int i, uint16_t value) {
	/// set i-th item if frame with the couple information of value
	ReaScopeSetParametersByItem((MFM_ReaScope_Item *) GetItem(i), value);

}
//_______________________________________________________________________________
void MFMReaScopeFrame::ReaScopeSetParametersByItem(MFM_ReaScope_Item *item, uint16_t value) {
	/// set "item" with the couple information of value
	static int toto;
	item->Value = value;
}
//_______________________________________________________________________________
void MFMReaScopeFrame::SetCheckSum(uint16_t checksum) {
	((MFM_ReaScope_Frame*) pHeader)->CheckSum.CheckSum = checksum;
}
//_______________________________________________________________________________
uint16_t MFMReaScopeFrame::GetCheckSum() {

	uint16_t checksum = 0;
	char * pchecksum = (char*) &(checksum);

	checksum = ((MFM_ReaScope_Frame*) pHeader)->CheckSum.CheckSum;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		if (GetRevision() > 0)
			SwapInt16((uint16_t *) (pchecksum));
	return checksum;
}
//_______________________________________________________________________________
void MFMReaScopeFrame::FillEventWithRamdomConst(uint64_t timestamp,
		uint32_t enventnumber) {
	/// Fill frame items  of sinus values with random perios,

	int size = GetNbItems();
	int nb_bits = 16;
	float nbofperiodes = 1;
        int16_t board=112;
	int i;
	static int16_t channel = 0;
	float P;
	static unsigned int seed = 15;
	unsigned short uivalue;
	float rando, tempof;
	P = 1.570796327 * 2;
	rando = seed;
        channel ++;
        channel = channel % REA_SCOPE_NB_CHANNELS;
	int h;
	h = pow(2, nb_bits) - 1;
        SetLocationId(channel,board);
	if (size > 0)
		ReaScopeSetParameters(0, 1);
	rando = (float) ((rand_r(&seed) / (RAND_MAX + 1.0)));
	seed++;
	
        SetSetupScope(1966);
	for (i = 1; i < size; i++) {
		seed++;
		nbofperiodes = 1 + rando * 4;
		tempof = (float) i / (size / (nbofperiodes));
		uivalue = (unsigned short) (h / 2 + h / 2 * sin((float) 2 * P * tempof));
		ReaScopeSetParameters(i, uivalue);
	}
	SetEventNumber(enventnumber);
	SetTimeStamp(timestamp);
	SetCheckSum(66);

}

//_______________________________________________________________________________
void MFMReaScopeFrame::GenerateAReaScopeExample(int type, int eventnumber) {
	/// Generate a example of frame containing random value\n
	/// usable for tests.
	if (type != MFM_REA_SCOPE_FRAME_TYPE) {
		cout
				<< "Error in  MFMReaScopeFrame::GenerateAReaScopeExample type not understood\n";
		return;
	}

	uint32_t unitBlock_size = 0;
	uint32_t itemsize = 0;
	uint32_t nbitem = 0;
	uint32_t framesize = 0;
	uint32_t revision = 1;
	uint32_t headersize = 0;
	unitBlock_size = REA_SCOPE_STD_UNIT_BLOCK_SIZE;
	itemsize = sizeof (MFM_ReaScope_Item);
	nbitem = REA_SCOPE_NB_OF_ITEMS;

	headersize = REA_SCOPE_HEADERSIZE;

	framesize = headersize + nbitem * itemsize + sizeof(MFM_ReaScopeCheckSum);
	revision = 1;
	// generation of MFM header
	MFM_make_header(unitBlock_size, 0, type, revision, (int) (framesize
			/ unitBlock_size), (headersize / unitBlock_size), itemsize, nbitem);

	SetLocationId(1, 2);
	SetSetupScope(6);
	FillEventWithRamdomConst(GetTimeStampUs(), eventnumber);
	SetCheckSum(66);
}
//_______________________________________________________________________________
string MFMReaScopeFrame::DumpData(char mode, bool nozero) {
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
			ReaScopeGetParameters(i, &value);
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
void MFMReaScopeFrame::InitStat() {
	MFMBasicFrame::InitStat();
	int i;
	fCountNbEventCard = new long long[65536];
	for (i = 0; i < 65536; i++) {
		fCountNbEventCard[i] = 0;
	}
}
//_______________________________________________________________________________
void MFMReaScopeFrame::FillStat() {
	MFMBasicFrame::FillStat();
	uint16_t id;
	id = GetLocationId();
	fCountNbEventCard[id]++;
	
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
//_______________________________________________________________________________
string MFMReaScopeFrame::GetStat(string info) {

	string display("");
	stringstream ss("");
	ss << MFMBasicFrame::GetStat(info);
	int i, j;
	int total = 0;
	for (i = 0; i < 65536; i++) {
		if (fCountNbEventCard[i] != 0) {
			j = i;
			ss << "Card " << ((j >> 4) & REA_SCOPE_BOARD_ID_MASK);
			j = i;
			ss << " Cristal  " << (j & REA_SCOPE_CHANNEL_ID_MASK);
			ss << " NbEvents = " << fCountNbEventCard[i] << "\n";
			total += fCountNbEventCard[i];
		}
	}
	
	ss << " Total                        = " << total << "\n";
	display = ss.str();
	return display;
}
//_______________________________________________________________________________
