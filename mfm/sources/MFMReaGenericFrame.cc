/*
 MFMReaGenericFrame.cc
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

#include "MFMReaGenericFrame.h"

//_______________________________________________________________________________

void MFMReaGenericFrame::SetStatus(int i, uint16_t status) {
	if (i < 0 and i > REA_GENERIC_NB_STATUS)
		cout << "MFMReaGenericFrame::ReaGenericSetStatus Error of status index\n";
	if (i == 0)
		((MFM_ReaGeneric_frame*) pHeader)->Data.Status1 = status;
	if (i == 1)
		((MFM_ReaGeneric_frame*) pHeader)->Data.Status2 = status;
}
//_______________________________________________________________________________

uint16_t MFMReaGenericFrame::GetStatus(int i) {
	/// Set Status (0,1 or 2)
	uint16_t status;
	if (i < 0 and i > REA_GENERIC_NB_STATUS) {
		cout << "MFMReaGenericFrame::ExGetStatus Error of status index\n";
		return 0;
	} else {
		if (i == 0)
			status = (((MFM_ReaGeneric_frame*) pHeader)->Data.Status1);
		if (i == 1)
			status = (((MFM_ReaGeneric_frame*) pHeader)->Data.Status2);
	}
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&status);
	return status;
}
//_______________________________________________________________________________

void MFMReaGenericFrame::SetTypeTns(enum ReaTnsType type) {
	/// Set TypeTns
	((MFM_ReaGeneric_frame*) pHeader)->Data.Type_Tns= type;
}
//_______________________________________________________________________________

enum ReaTnsType MFMReaGenericFrame::GetTypeTns() {
	/// Get TypeTns
	uint16_t  type;
	type = (((MFM_ReaGeneric_frame*) pHeader)->Data.Type_Tns);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&type);
	return (enum ReaTnsType)type;
}
//_______________________________________________________________________________

void MFMReaGenericFrame::SetEnergy(uint16_t energy) {
	/// Set Energy
	((MFM_ReaGeneric_frame*) pHeader)->Data.Energy = energy;
}
//_______________________________________________________________________________

uint16_t MFMReaGenericFrame::GetEnergy() {
	/// Get Energy
	uint16_t energy;
	energy = (((MFM_ReaGeneric_frame*) pHeader)->Data.Energy);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&energy);
	return energy;
}
//_______________________________________________________________________________

void MFMReaGenericFrame::SetTime(uint16_t time) {
	/// Set Time
	((MFM_ReaGeneric_frame*) pHeader)->Data.Time= time;
}
//_______________________________________________________________________________

uint16_t MFMReaGenericFrame::GetTime() {
	/// Get Time
	uint16_t time;
	time = (((MFM_ReaGeneric_frame*) pHeader)->Data.Time);
	if (fLocalIsBigEndian != fFrameIsBigEndian)
		SwapInt16(&time);
	return time;
}

//_______________________________________________________________________________
void MFMReaGenericFrame::FillEventRandomConst(uint64_t timestamp,
		uint32_t eventnumber) {

	/// Fill all data of frame with random values to do test
	/// And report time stamp and event number
	MFMNumExoFrame::FillEventRandomConst(timestamp,eventnumber);
	int value = random();
	uint16_t uivalue = (uint16_t) (4294967296 * value / RAND_MAX);
	SetTypeTns(REA_GENERIC_TIME_TYPE);
	SetEnergy(uivalue);
	SetTime(uivalue+1);
}
//_______________________________________________________________________________
