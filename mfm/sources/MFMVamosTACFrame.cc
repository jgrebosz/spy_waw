/*
 MFMVamosTACFrame.cc
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

#include "MFMVamosTACFrame.h"

//_______________________________________________________________________________
MFMVamosTACFrame::MFMVamosTACFrame(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, int headerSize) {
	/// Constructor for a Vamos Time frame . the header is filled with unitblock_size, data source , frame type , revision , frame, size and header size value
	SetPointers();
	fCountNbEventCard=NULL;
}

//_______________________________________________________________________________
MFMVamosTACFrame::MFMVamosTACFrame() {
	/// Constructor for a empty Vamos TAC frame
	fCountNbEventCard=NULL;

}
//_______________________________________________________________________________
MFMVamosTACFrame::~MFMVamosTACFrame() {
/// destructor of VamosTAC frame

	if (fCountNbEventCard){
		delete [] fCountNbEventCard;
		fCountNbEventCard=NULL;
	}

}
//_______________________________________________________________________________
void MFMVamosTACFrame::SetBufferSize(int size, bool ifinferior) {
	MFMBlobFrame::SetBufferSize(size, ifinferior);
	MFMVamosTACFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMVamosTACFrame::SetPointers(void * pt) {
	MFMBlobFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMVamosTACFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMBlobFrame::SetAttributs(pt);
	pUserData_char=(char*)&(((MFM_TACvamos_frame*) pHeader)->VamosTACData);
}
//_______________________________________________________________________________

uint64_t MFMVamosTACFrame::GetTimeStamp() {
	/// Computer, set attibut and return value of time stamp from  frame
	fTimeStamp = 0;
	uint64_t * timeStamp = &(fTimeStamp);
	memcpy(((char*) (&fTimeStamp)),
			((MFM_TACvamos_frame*) pHeader)->VamosTACEventInfo.EventTime, 6);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt64((timeStamp), 6);
	return fTimeStamp;
}
//_______________________________________________________________________________
uint64_t MFMVamosTACFrame::GetTimeStampAttribut() {
	/// Return attibut of time stamp
	return fTimeStamp;
}

//_______________________________________________________________________________

uint32_t MFMVamosTACFrame::GetEventNumber() {
	/// Computer, set attibut and return value of event number from  frame
	fEventNumber = 0;
	char * eventNumber = (char*) &(fEventNumber);

	fEventNumber = ((MFM_TACvamos_frame*) pHeader)->VamosTACEventInfo.EventIdx;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt32((uint32_t *) (eventNumber), 4);
	return fEventNumber;
}

//_______________________________________________________________________________
uint32_t MFMVamosTACFrame::GetEventNumberAttribut() {
	/// Return attibut of event number
	return fEventNumber;
}
//_______________________________________________________________________________
void MFMVamosTACFrame::SetTimeStamp(uint64_t timestamp) {
	/// Set value of Time Stamp in frame

	char* pts = (char*) &timestamp;
	timestamp = timestamp & 0x0000ffffffffffff;
	memcpy(((MFM_TACvamos_frame*) pHeader)->VamosTACEventInfo.EventTime, pts, 6);
}
//_______________________________________________________________________________
void MFMVamosTACFrame::SetEventNumber(uint32_t eventnumber) {
	/// Set Event Number of frame
	((MFM_TACvamos_frame*) pHeader)->VamosTACEventInfo.EventIdx = eventnumber;
}

//_______________________________________________________________________________
void MFMVamosTACFrame::SetCristalId(uint16_t cristalId) {
	/// Set 16 bits of CristalIdx
	((MFM_TACvamos_frame*) pHeader)->VamosTACData.CristalId = cristalId;
}
//_______________________________________________________________________________
void MFMVamosTACFrame::SetCristalId(uint16_t tgRequest, uint16_t idBoard) {

				// Methode to fill item in case of MFM_COBO_FRAME_TYPE frame
				uint16_t ui, ui2;
				ui2 = 0;

				//trig request form 0 to 4
				ui2 =tgRequest  & VAMOSTAC_TRIG_REQ_CRYS_ID_MASK;

				//id board from 5 to 15
				ui = idBoard & VAMOSTAC_BOARD_ID_MASK;
				ui = ui << 5;
				ui2 = ui2 | ui;
				SetCristalId(ui2);
}
//_______________________________________________________________________________

uint16_t MFMVamosTACFrame::GetCristalId() {
	uint16_t cristal = 0;
/// Compute and return the 2 bytes of CristalId()
	cristal = ((MFM_TACvamos_frame*) pHeader)->VamosTACData.CristalId;
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16((&cristal));
	return ((cristal));
}

//_______________________________________________________________________________
uint16_t MFMVamosTACFrame::GetTGCristalId() {
/// Compute and return extracted Trigger Request value of CristalId
	return (GetCristalId() & VAMOSTAC_TRIG_REQ_CRYS_ID_MASK);
}

//_______________________________________________________________________________
uint16_t MFMVamosTACFrame::GetBoardId() {
/// Compute and return id value of Board
	return ((GetCristalId()>>5) & VAMOSTAC_BOARD_ID_MASK);
}

//_______________________________________________________________________________

void MFMVamosTACFrame::SetStatus(int i, uint16_t status) {
	if (i < 0 and i > VAMOSTAC_NB_STATUS)
		cout << "MFMVamosTACFrame::VamosTACSetStatus Error of status index\n";
	if (i == 0)
		((MFM_TACvamos_frame*) pHeader)->VamosTACData.Status1 = status;
	if (i == 1)
		((MFM_TACvamos_frame*) pHeader)->VamosTACData.Status2 = status;
}
//_______________________________________________________________________________

uint16_t MFMVamosTACFrame::GetStatus(int i) {
	/// Set Status (0,1 or 2)
	uint16_t status;
	if (i < 0 and i > VAMOSTAC_NB_STATUS) {
		cout << "MFMVamosTACFrame::ExGetStatus Error of status index\n";
		return 0;
	} else {
		if (i == 0)
			status = (((MFM_TACvamos_frame*) pHeader)->VamosTACData.Status1);
		if (i == 1)
			status = (((MFM_TACvamos_frame*) pHeader)->VamosTACData.Status2);
	}
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&status);
	return status;
}

//_______________________________________________________________________________

void MFMVamosTACFrame::SetTime(uint16_t time) {
	/// Set Time
	((MFM_TACvamos_frame*) pHeader)->VamosTACData.Time= time;
}

//_______________________________________________________________________________

uint16_t MFMVamosTACFrame::GetTime() {
	/// Get Time
	uint16_t time;
	time = (((MFM_TACvamos_frame*) pHeader)->VamosTACData.Time);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&time);
	return time;
}
//_______________________________________________________________________________

void MFMVamosTACFrame::SetChecksum(uint16_t cksum) {
	/// Set Checksum data
	(((MFM_TACvamos_frame*) pHeader)->VamosTACData.Checksum) = cksum;
}
//_______________________________________________________________________________

uint16_t MFMVamosTACFrame::GetChecksum() {
	/// Compute and return value of Checksum
	uint16_t cksum;
	cksum = (((MFM_TACvamos_frame*) pHeader)->VamosTACData.Checksum);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&cksum);
	return cksum;
}

//_______________________________________________________________________________
void MFMVamosTACFrame::FillEventRandomConst(uint64_t timestamp,
		uint32_t enventnumber) {

	/// Fill all data of frame with random values to do test
	/// And report time stamp and event number
	int value = random();
	uint16_t uivalue = (uint16_t) (4294967296 * value / RAND_MAX);
	SetTime(uivalue);
	SetCristalId(8,112);
	SetEventNumber(enventnumber);
	SetTimeStamp(timestamp);
}
//_______________________________________________________________________________
string MFMVamosTACFrame::GetHeaderDisplay(char* infotext) {
	stringstream ss("");
	string display("");
	display = ss.str();
	if (infotext==NULL)
	ss << MFMCommonFrame::GetHeaderDisplay((char*)"--VamosTAC Frame--");
	else
	ss << MFMCommonFrame::GetHeaderDisplay(infotext);
	ss << "   BoardId ="<< GetBoardId();
	ss << "   Cristal Id ="<<GetTGCristalId();
	ss << "   EN = " << GetEventNumber();
	ss << "   TS = " << GetTimeStamp();

	display = ss.str();
	return display;
}
//_______________________________________________________________________________

void MFMVamosTACFrame::FillStat() {
	MFMCommonFrame::FillStat();
	uint16_t id ;
	id = GetCristalId();
	fCountNbEventCard[id]++;

}
//_______________________________________________________________________________
void MFMVamosTACFrame::InitStat() {
	MFMCommonFrame::InitStat();
	int i;
	fCountNbEventCard = new long long[65536];
	for ( i = 0;i<65536;i++){
		fCountNbEventCard[i]=0;
	}

}
//_______________________________________________________________________________
string  MFMVamosTACFrame::GetStat(string info){

	string display("");
	stringstream ss("");
	ss << MFMCommonFrame::GetStat( info);
    int i, j; int total =0;

	for ( i=0;i<65536;i++ ){

		if (fCountNbEventCard[i]!=0){
			j =i;
			ss << "Card "<< ((j>>5) & VAMOSTAC_BOARD_ID_MASK);
			j =i;
			ss << " Cristal  "<< (j& VAMOSTAC_TRIG_REQ_CRYS_ID_MASK );
			ss << " NbEvents = "<< fCountNbEventCard[i] <<"\n";
			total += fCountNbEventCard[i];
		}
	}
	ss<<"Total MFMVamosTACFrame       = "<< total<<"\n";
	display = ss.str();
	return display;
}
//_______________________________________________________________________________


