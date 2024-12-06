/*
 MFMVamosICFrame.cc
 Copyright Acquisition group, GANIL Caen, France
 */

#include <iostream>
#include <cmath>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
using namespace std;

#include "MFMVamosICFrame.h"

//_______________________________________________________________________________
MFMVamosICFrame::MFMVamosICFrame(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, int headerSize) {
	/// Constructor for a Vamos Ionization Chamber frame . the header is filled with unitblock_size, data source , frame type , revision , frame, size and header size value
	SetPointers();
	fCountNbEventCard=NULL;
}

//_______________________________________________________________________________
MFMVamosICFrame::MFMVamosICFrame() {
	/// Constructor for a empty Vamos IC frame
	fCountNbEventCard=NULL;

}
//_______________________________________________________________________________
MFMVamosICFrame::~MFMVamosICFrame() {
/// destructor of VamosIC frame

	if (fCountNbEventCard){
		delete [] fCountNbEventCard;
		fCountNbEventCard=NULL;
	}

}
//_______________________________________________________________________________
void MFMVamosICFrame::SetBufferSize(int size, bool ifinferior) {
	MFMBlobFrame::SetBufferSize(size, ifinferior);
	MFMVamosICFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMVamosICFrame::SetPointers(void * pt) {
	MFMBlobFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMVamosICFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMBlobFrame::SetAttributs(pt);
	pUserData_char=(char*)&(((MFM_ICvamos_frame*) pHeader)->VamosICData);
}
//_______________________________________________________________________________

uint64_t MFMVamosICFrame::GetTimeStamp() {
	/// Computer, set attibut and return value of time stamp from  frame
	fTimeStamp = 0;
	uint64_t * timeStamp = &(fTimeStamp);
	memcpy(((char*) (&fTimeStamp)),
			((MFM_ICvamos_frame*) pHeader)->VamosICEventInfo.EventTime, 6);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt64((timeStamp), 6);
	return fTimeStamp;
}
//_______________________________________________________________________________
uint64_t MFMVamosICFrame::GetTimeStampAttribut() {
	/// Return attibut of time stamp
	return fTimeStamp;
}

//_______________________________________________________________________________

uint32_t MFMVamosICFrame::GetEventNumber() {
	/// Computer, set attibut and return value of event number from  frame
	fEventNumber = 0;
	char * eventNumber = (char*) &(fEventNumber);

	fEventNumber = ((MFM_ICvamos_frame*) pHeader)->VamosICEventInfo.EventIdx;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32((uint32_t *) (eventNumber), 4);
	return fEventNumber;
}

//_______________________________________________________________________________
uint32_t MFMVamosICFrame::GetEventNumberAttribut() {
	/// Return attibut of event number
	return fEventNumber;
}
//_______________________________________________________________________________
void MFMVamosICFrame::SetTimeStamp(uint64_t timestamp) {
	/// Set value of Time Stamp in frame

	char* pts = (char*) &timestamp;
	timestamp = timestamp & 0x0000ffffffffffff;
	memcpy(((MFM_ICvamos_frame*) pHeader)->VamosICEventInfo.EventTime, pts, 6);
}
//_______________________________________________________________________________
void MFMVamosICFrame::SetEventNumber(uint32_t eventnumber) {
	/// Set Event Number of frame
	((MFM_ICvamos_frame*) pHeader)->VamosICEventInfo.EventIdx = eventnumber;
}

//_______________________________________________________________________________
void MFMVamosICFrame::SetCristalId(uint16_t cristalId) {
	/// Set 16 bits of CristalIdx
	((MFM_ICvamos_frame*) pHeader)->VamosICData.CristalId = cristalId;
}
//_______________________________________________________________________________
void MFMVamosICFrame::SetCristalId(uint16_t tgRequest, uint16_t idBoard) {

				// Methode to fill item in case of MFM_COBO_FRAME_TYPE frame
				uint16_t ui, ui2;
				ui2 = 0;

				//trig request form 0 to 4
				ui2 =tgRequest  & VAMOSIC_TRIG_REQ_CRYS_ID_MASK;

				//id board from 5 to 15
				ui = idBoard & VAMOSIC_BOARD_ID_MASK;
				ui = ui << 5;
				ui2 = ui2 | ui;
				SetCristalId(ui2);
}
//_______________________________________________________________________________

uint16_t MFMVamosICFrame::GetCristalId() {
	uint16_t cristal = 0;
/// Compute and return the 2 bytes of CristalId()
	cristal = ((MFM_ICvamos_frame*) pHeader)->VamosICData.CristalId;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16((&cristal));
	return ((cristal));
}

//_______________________________________________________________________________
uint16_t MFMVamosICFrame::GetTGCristalId() {
/// Compute and return extracted Trigger Request value of CristalId
	return (GetCristalId() & VAMOSIC_TRIG_REQ_CRYS_ID_MASK);
}

//_______________________________________________________________________________
uint16_t MFMVamosICFrame::GetBoardId() {
/// Compute and return id value of Board
	return ((GetCristalId()>>5) & VAMOSIC_BOARD_ID_MASK);
}

//_______________________________________________________________________________

void MFMVamosICFrame::SetStatus(int i, uint16_t status) {
	if (i < 0 and i > VAMOSIC_NB_STATUS)
		cout << "MFMVamosICFrame::VamosICSetStatus Error of status index\n";
	if (i == 0)
		((MFM_ICvamos_frame*) pHeader)->VamosICData.Status1 = status;
	if (i == 1)
		((MFM_ICvamos_frame*) pHeader)->VamosICData.Status2 = status;
}
//_______________________________________________________________________________

uint16_t MFMVamosICFrame::GetStatus(int i) {
	/// Set Status (0,1 or 2)
	uint16_t status;
	if (i < 0 and i > VAMOSIC_NB_STATUS) {
		cout << "MFMVamosICFrame::ExGetStatus Error of status index\n";
		return 0;
	} else {
		if (i == 0)
			status = (((MFM_ICvamos_frame*) pHeader)->VamosICData.Status1);
		if (i == 1)
			status = (((MFM_ICvamos_frame*) pHeader)->VamosICData.Status2);
	}
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&status);
	return status;
}

//_______________________________________________________________________________

void MFMVamosICFrame::SetEnergy(uint16_t energy) {
	/// Set Energy
	((MFM_ICvamos_frame*) pHeader)->VamosICData.Energy = energy;
}

//_______________________________________________________________________________

uint16_t MFMVamosICFrame::GetEnergy() {
	/// Get Energy
	uint16_t energy;
	energy = (((MFM_ICvamos_frame*) pHeader)->VamosICData.Energy);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&energy);
	return energy;
}
//_______________________________________________________________________________

void MFMVamosICFrame::SetChecksum(uint16_t cksum) {
	/// Set Checksum data
	(((MFM_ICvamos_frame*) pHeader)->VamosICData.Checksum) = cksum;
}
//_______________________________________________________________________________

uint16_t MFMVamosICFrame::GetChecksum() {
	/// Compute and return value of Checksum
	uint16_t cksum;
	cksum = (((MFM_ICvamos_frame*) pHeader)->VamosICData.Checksum);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&cksum);
	return cksum;
}

//_______________________________________________________________________________
void MFMVamosICFrame::FillEventRandomConst(uint64_t timestamp,
		uint32_t enventnumber) {

	/// Fill all data of frame with random values to do test
	/// And report time stamp and event number
	int value = random();
	uint16_t uivalue = (uint16_t) (4294967296 * value / RAND_MAX);
	SetEnergy(uivalue);
	SetCristalId(8,112);
	SetEventNumber(enventnumber);
	SetTimeStamp(timestamp);
}
//_______________________________________________________________________________
string MFMVamosICFrame::GetHeaderDisplay(char* infotext) {
	stringstream ss("");
	string display("");
	display = ss.str();
	if (infotext==NULL)
	ss << MFMCommonFrame::GetHeaderDisplay((char*)"--VamosIC Frame--");
	else
	ss << MFMCommonFrame::GetHeaderDisplay(infotext);
	ss << "   BoardId ="<< GetBoardId();
	ss << "   Cristal Id ="<<GetTGCristalId();
	ss << "   EN = " << GetEventNumber();
	ss << "   TS = " << GetTimeStamp();

	display = ss.str();
	return display;
}

//____________________________________________________________________
void MFMVamosICFrame::FillStat() {
	MFMCommonFrame::FillStat();
	uint16_t id ;
	id = GetCristalId();
	fCountNbEventCard[id]++;

}
//_______________________________________________________________________________
void MFMVamosICFrame::InitStat() {
	MFMCommonFrame::InitStat();
	int i;
	fCountNbEventCard = new long long[65536];
	for ( i = 0;i<65536;i++){
		fCountNbEventCard[i]=0;
	}

}
//____________________________________________________________________
string  MFMVamosICFrame::GetStat(string info){

	string display("");
	stringstream ss("");
	ss << MFMCommonFrame::GetStat( info);
    int i, j; int total =0;

	for ( i=0;i<65536;i++ ){

		if (fCountNbEventCard[i]!=0){
			j =i;
			ss << "Card "<< ((j>>5) & VAMOSIC_BOARD_ID_MASK);
			j =i;
			ss << " Cristal  "<< (j& VAMOSIC_TRIG_REQ_CRYS_ID_MASK );
			ss << " NbEvents = "<< fCountNbEventCard[i] <<"\n";
			total += fCountNbEventCard[i];
		}
	}
	ss<<"Total MFMVamosICFrame       = "<< total<<"\n";
	display = ss.str();
	return display;
}

