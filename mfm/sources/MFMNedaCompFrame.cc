/*
 MFMNedaCompFrame.cc
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

#include "MFMNedaCompFrame.h"

//_______________________________________________________________________________
MFMNedaCompFrame::MFMNedaCompFrame(int unitBlock_size, int dataSource, int frameType,
		int revision, int frameSize, int headerSize, int itemSize, int nItems) {
	/// Constructor of frame with a memory space\n
	/// fill header information : unitBlock_size,dataSource,....
	SetPointers();
	fCountNbEventCard =NULL;

}

//_______________________________________________________________________________
MFMNedaCompFrame::MFMNedaCompFrame() {
	/// Constructor of a empty frame object
	fCountNbEventCard =NULL;
}
//_______________________________________________________________________________
MFMNedaCompFrame::~MFMNedaCompFrame() {
	///Destructor
	if (fCountNbEventCard){
	delete [] fCountNbEventCard;
	fCountNbEventCard=NULL;
	}

}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetBufferSize(int size, bool ifinferior) {
	/// Do memory allocation or a reallacation for frame\n
	/// if ifinferior==true the allocaton is forced to size event if the acutal size is bigger\n
	MFMCommonFrame::SetBufferSize(size, ifinferior);
	MFMNedaCompFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetPointers(void * pt) {
	/// Initialize pointers of frame\n
	/// if pt==NULL initialization is with current value of main pointer of frame (pData)\n
	/// else initialization is done with pData = pt\n
	/// pData must be the reference;
	MFMCommonFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pNedaCompFrame = (MFM_NedaComp_Frame*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMCommonFrame::SetAttributs(pt);
}
//_______________________________________________________________________________
string MFMNedaCompFrame::GetHeaderDisplay(char* infotext) {
	stringstream ss;
	string display("");
	display = ss.str();


	ss << MFMCommonFrame::GetHeaderDisplay(infotext);
	ss << "   EN = " << GetEventNumber();
	ss << "   TS = " << GetTimeStamp();
	ss << endl;
	ss << "   Board = " << GetBoardId() << " | Channel = " << GetChannelId()
			<< " | TdcCorValue = " << (int) GetTdcCorValue()<< " | Time = " << (int) GetTime()<< " | IntRaiseTime =  " << GetIntRaiseTime() << endl;
	ss << "   Slow Integ = "
			<< GetSlowIntegral() << " | FastIntegral = ";
	ss << GetFastIntegral() << " | NeuralNetWork = " << (int) GetNeuralNetWork()
			<< " | NbZero = " << (int) GetNbZero() << " | NeutronFlag = "<<GetNeutronFlag()<< endl;

	display = ss.str();

	return display;
}
//_______________________________________________________________________________

uint64_t MFMNedaCompFrame::GetTimeStamp() {
	/// Compute time stamp and fill fTimeStamp attribut. return value of TimeStamp
	fTimeStamp = 0;
	uint64_t* timeStamp = &(fTimeStamp);

	memcpy(((char*) (&fTimeStamp)),
			((MFM_NedaComp_Header*) pHeader)->NedaCompEvtInfo.EventTime, 6);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt64((timeStamp), 6);

	return fTimeStamp;
}
//_______________________________________________________________________________
uint64_t MFMNedaCompFrame::GetTimeStampAttribut() {
	/// return time stamp without computing it
	return fTimeStamp;
}

//_______________________________________________________________________________

uint32_t MFMNedaCompFrame::GetEventNumber() {
	/// Compute and return envent number
	fEventNumber = 0;
	char * eventNumber = (char*) &(fEventNumber);

	fEventNumber = ((MFM_NedaComp_Header*) pHeader)->NedaCompEvtInfo.EventIdx;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32((uint32_t *) (eventNumber), 4);

	return fEventNumber;
}
//_______________________________________________________________________________
uint32_t MFMNedaCompFrame::GetEventNumberAttibut() {
	/// Return event number without computing it
	return fEventNumber;
}

//_______________________________________________________________________________
void MFMNedaCompFrame::SetTimeStamp(uint64_t timestamp) {
	// Set frame timestamp
	char* pts = (char*) &timestamp;
	timestamp = timestamp & 0x0000ffffffffffff;
	memcpy(((MFM_NedaComp_Header*) pHeader)->NedaCompEvtInfo.EventTime, pts, 6);
}

//_______________________________________________________________________________
void MFMNedaCompFrame::SetEventNumber(uint32_t eventnumber) {
	/// set frame event number
	((MFM_NedaComp_Header*) pHeader)->NedaCompEvtInfo.EventIdx = eventnumber;
}

//_______________________________________________________________________________
void MFMNedaCompFrame::SetLocationId(uint16_t Id) {
	/// Set 16 bits of LocationId
	((MFM_NedaComp_Header*) pHeader)->NedaCompEvtInfo.LocationId = Id;
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetLocationId(uint16_t ChannelId, uint16_t BoardId) {
	uint16_t ui, ui2;
	ui2 = 0;
	//trig request form 0 to 4
	ui2 = ChannelId & NEDACOMP_CHANNEL_ID_MASK;

	//id board from 5 to 15
	ui = BoardId & NEDACOMP_BOARD_ID_MASK;
	ui = ui << 4;
	ui2 = ui2 | ui;
	SetLocationId(ui2);
}

//_______________________________________________________________________________

uint16_t MFMNedaCompFrame::GetLocationId() {
	uint16_t Id = 0;
	/// Compute and return the 2 bytes of LocationId()
	Id = ((MFM_NedaComp_Header*) pHeader)->NedaCompEvtInfo.LocationId;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16((&Id));
	return ((Id));
}

//_______________________________________________________________________________
uint16_t MFMNedaCompFrame::GetChannelId() {
	/// Compute and return extracted ChannelId
	return (GetLocationId() & NEDACOMP_CHANNEL_ID_MASK);
}

//_______________________________________________________________________________
uint16_t MFMNedaCompFrame::GetBoardId() {
	/// Compute and return id value of Board
	return (((GetLocationId() >> 4) & NEDACOMP_BOARD_ID_MASK));
}

//_______________________________________________________________________________
void MFMNedaCompFrame::SetEnergy(uint16_t energy) {
	/// Set 16 bits of LocationId
	((MFM_NedaComp_Frame*) pHeader)->NedaCompData.Energy = energy;
}

//_______________________________________________________________________________

uint16_t MFMNedaCompFrame::GetEnergy() {
	uint16_t energy = 0;
	/// Compute and return the 2 bytes of LocationId()
	energy = ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.Energy;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16((&energy));
	return (energy);
}


//_______________________________________________________________________________

uint16_t MFMNedaCompFrame::GetTime() {
	uint16_t time = ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.Time;
	return time;
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetTime(uint16_t time) {
	((MFM_NedaComp_Frame*) pHeader)->NedaCompData.Time = time;
}

//_______________________________________________________________________________
uint16_t MFMNedaCompFrame::GetTdcCorValue() {
	uint16_t val = 0;
	val = ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.TdcCorValue;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&val);
	return val;
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetTdcCorValue(uint16_t val){
	((MFM_NedaComp_Frame*) pHeader)->NedaCompData.TdcCorValue = val;
}
//_______________________________________________________________________________
uint32_t MFMNedaCompFrame::GetSlowIntegral() {
	uint32_t integral = 0;
	char * pintegral = (char*) &(integral);
	integral = ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.SlowIntegral;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32((uint32_t *) (pintegral), 4);
	return integral;
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetSlowIntegral(uint32_t integral){
	((MFM_NedaComp_Frame*) pHeader)->NedaCompData.SlowIntegral = integral;
}
//_______________________________________________________________________________
uint32_t MFMNedaCompFrame::GetFastIntegral() {
	uint32_t integral = 0;
	char * pintegral = (char*) &(integral);
	integral = ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.FastIntegral;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32((uint32_t *) (pintegral), 4);
	return integral;
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetFastIntegral(uint32_t integral) {
	((MFM_NedaComp_Frame*) pHeader)->NedaCompData.FastIntegral = integral;
}

//_______________________________________________________________________________
int32_t MFMNedaCompFrame::GetIntRaiseTime() {
	int32_t time = 0;
char * ptime = (char*) &(time);
time = ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.IntRaiseTime;
if (fLocalIsBigEndian != fFrameIsBigEndian)
	SwapInt32((uint32_t *) (ptime), 4);
return time;
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetIntRaiseTime(int32_t time) {
	((MFM_NedaComp_Frame*) pHeader)->NedaCompData.IntRaiseTime = time;
}
//_______________________________________________________________________________
uint16_t MFMNedaCompFrame::GetNeuralNetWork() {
	uint16_t neural = 0;
	neural = ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.NeuralNetWork;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&neural);
	return neural;
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetNeuralNetWork(uint16_t neural) {
	((MFM_NedaComp_Frame*) pHeader)->NedaCompData.NeuralNetWork = neural;
}
//_______________________________________________________________________________
uint8_t MFMNedaCompFrame::GetNbZero() {
	uint8_t id = ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.NbZero;
	return id;
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetNbZero(uint8_t nb) {
	((MFM_NedaComp_Frame*) pHeader)->NedaCompData.NbZero = nb;
}
//_______________________________________________________________________________
/*bool MFMNedaCompFrame::GetNeutronFlag() {
	uint8_t val = ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.NbZero;
	return (val>0);
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetNeutronFlag(bool neutron) {
	if (neutron )((MFM_NedaComp_Frame*) pHeader)->NedaCompData.NbZero = 1;
	else ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.NbZero = 0;

}*/
//_______________________________________________________________________________
bool MFMNedaCompFrame::GetNeutronFlag() {
        uint8_t val = ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.NeutronFlag;
	return (val>0);
}
//_______________________________________________________________________________
void MFMNedaCompFrame::SetNeutronFlag(bool neutron) {
        if (neutron )((MFM_NedaComp_Frame*) pHeader)->NedaCompData.NeutronFlag = 1;
        else ((MFM_NedaComp_Frame*) pHeader)->NedaCompData.NeutronFlag = 0;

}
//_______________________________________________________________________________
void MFMNedaCompFrame::FillEventWithRamdomConst(uint64_t timestamp,
		uint32_t enventnumber) {
	/// Fill frame items  of sinus values with random perios,

	static unsigned int seed =15;

	float rando;
	rando = seed;
	rando = (float) ((rand_r(&seed) / (RAND_MAX + 1.0)));
	seed++;
	SetEventNumber(enventnumber);
	SetTimeStamp(timestamp);
	SetEnergy((uint32_t)rando);
	SetTime(1) ;
	SetTdcCorValue(2);
	SetSlowIntegral(3);
	SetFastIntegral(4) ;
    SetIntRaiseTime(5) ;
    SetNeuralNetWork(6) ;
    SetNbZero(7) ;
    SetNeutronFlag(true);
}

//_______________________________________________________________________________

void MFMNedaCompFrame::GenerateANedaExample(int type, int eventnumber) {
/// Generate a example of frame containing random value\n
/// usable for tests.
if (type != MFM_NEDACOMP_FRAME_TYPE) {
	cout
			<< "Error in  MFMNedaCompFrame::GenerateANedaExample type not understood\n";
	return;
}

uint32_t unitBlock_size = 0;

uint32_t framesize = 0;
uint32_t revision = 1;
uint32_t headersize = 0;
unitBlock_size = NEDACOMP_STD_UNIT_BLOCK_SIZE;
headersize = NEDACOMP_HEADERSIZE;

framesize = NEDACOMP_FRAMESIZE;
revision = 1;
// generation of MFM header
MFM_make_header(unitBlock_size, 0, type, revision, (int) (framesize
		/ unitBlock_size), (headersize / unitBlock_size));

SetLocationId(1, 2);

}


//_______________________________________________________________________________
void MFMNedaCompFrame::InitStat() {
	MFMCommonFrame::InitStat();
	int i;
	fCountNbEventCard = new long long[65536];
	for ( i = 0;i<65536;i++){
		fCountNbEventCard[i]=0;
	}
}
//____________________________________________________________________
void MFMNedaCompFrame::FillStat() {
	MFMCommonFrame::FillStat();
	uint16_t id ;
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
//____________________________________________________________________
string  MFMNedaCompFrame::GetStat(string info){

	string display("");
	stringstream ss("");
	ss << MFMCommonFrame::GetStat( info);
    int i, j; int total =0;
	for ( i=0;i<65536;i++ ){
		if (fCountNbEventCard[i]!=0){
			j =i;
			ss << "Card "<< ((j>>4) & NEDACOMP_BOARD_ID_MASK);
			j =i;
			ss << " Cristal  "<< (j& NEDACOMP_CHANNEL_ID_MASK);
			ss << " NbEvents = "<< fCountNbEventCard[i] <<"\n";
			total += fCountNbEventCard[i];
		}
	}
	ss<<"Total                        = "<< total<<"\n";

	display = ss.str();
	return display;
}
//____________________________________________________________________
