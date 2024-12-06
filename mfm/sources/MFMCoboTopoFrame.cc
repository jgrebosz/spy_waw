/*
 MFMCoboTopoFrame.cc
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

#include "MFMCoboTopoFrame.h"
#include "XmlTags.h"
//_______________________________________________________________________________
MFMCoboTopoFrame::MFMCoboTopoFrame(int unitBlock_size, int dataSource,
		int frameType, int revision, int frameSize, int headerSize) {
	/// Constructor for a exogam frame . the header is filled with unitblock_size, data source , frame type , revision , frame, size and header size value
	SetPointers();

}

//_______________________________________________________________________________
MFMCoboTopoFrame::MFMCoboTopoFrame() {
	/// Constructor for a empty exogam frame


}
//_______________________________________________________________________________
MFMCoboTopoFrame::~MFMCoboTopoFrame() {
/// destructor of Exogam frame
}
//_______________________________________________________________________________
void MFMCoboTopoFrame::SetBufferSize(int size, bool ifinferior) {
	MFMBlobFrame::SetBufferSize(size, ifinferior);
	MFMCoboTopoFrame::SetPointers();
}
//_______________________________________________________________________________
void MFMCoboTopoFrame::SetPointers(void * pt) {
	MFMBlobFrame::SetPointers(pt);
	pHeader = (MFM_topcommon_header*) pData;
	pData_char = (char*) pData;
}
//_______________________________________________________________________________
void MFMCoboTopoFrame::SetAttributs(void * pt) {
	SetPointers(pt);
	MFMBlobFrame::SetAttributs(pt);
}
//_______________________________________________________________________________

uint64_t MFMCoboTopoFrame::GetTimeStamp() {
	/// do nothing and return 0;

	return 0;
}
//_______________________________________________________________________________
uint64_t MFMCoboTopoFrame::GetTimeStampAttribut() {
	/// Return attibut of time stamp
	return 0;
}
//_______________________________________________________________________________
uint32_t MFMCoboTopoFrame::GetEventNumber() {
	/// dp nothing and return 0
	return 0;
}
//_______________________________________________________________________________
uint32_t MFMCoboTopoFrame::GetEventNumberAttribut() {
	/// Return attibut of event number
	return 0;
}
//_______________________________________________________________________________
void MFMCoboTopoFrame::SetTimeStamp(uint64_t timestamp) {
	/// Set value of Time Stamp in frame
}
//_______________________________________________________________________________
void MFMCoboTopoFrame::SetEventNumber(int eventnumber) {
	/// Set Event Number of frame
}
//_______________________________________________________________________________
uint16_t MFMCoboTopoFrame::CoboGetIdx() {
	return ((int16_t) (((MFM_CoboTopo_header*) pHeader)->CoboTopoData.CoboIdx));
}
//_______________________________________________________________________________
void MFMCoboTopoFrame::CoboSetIdx(uint16_t idx){
	((MFM_CoboTopo_header*) pHeader)->CoboTopoData.CoboIdx = idx;
}
//_______________________________________________________________________________
uint16_t MFMCoboTopoFrame::CoboGetAsadMask() {
	return ((int16_t) (((MFM_CoboTopo_header*) pHeader)->CoboTopoData.AsadMask));
}
//_______________________________________________________________________________
void MFMCoboTopoFrame::CoboSetAsadMask(uint16_t asadmask){
	((MFM_CoboTopo_header*) pHeader)->CoboTopoData.AsadMask = asadmask;
}
//_______________________________________________________________________________
uint16_t MFMCoboTopoFrame::CoboGetTpMode() {
	return ((int16_t) (((MFM_CoboTopo_header*) pHeader)->CoboTopoData.AsadMask));
}
//_______________________________________________________________________________
void MFMCoboTopoFrame::CoboSetTpMode(uint16_t mode){
	((MFM_CoboTopo_header*) pHeader)->CoboTopoData.AsadMask = mode;
}
