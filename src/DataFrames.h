#ifndef DATAFRAMES_H_INCLUDED
#define DATAFRAMES_H_INCLUDED

#include <cstdint>
#include <cstring>
#include <cfloat>
#include <math.h>
#include <string>

const uint16_t inpMagicMask   = 0xFFF0;       // in AGATA the low nibble identifies the mezzanine
const uint16_t inpMagicData   = 0xDA00;
const uint16_t inpMagicWave   = 0xDA50;
const uint16_t inpMagicSpec   = 0xDA60;
const uint16_t inpIdleMask    = 0x0008;
const uint16_t inpPileMask    = 0x0001;

const uint32_t keyDataEAGLE     = 0xFA020102;     // adf::data:eagle
const uint32_t keyDataNEDA      = 0xFA020101;     // adf::data:neda
const uint32_t keyDataDSSD      = 0xFA0201A2;     // adf::data:dssd

const uint32_t keyEventGlobal   = 0xCA020100;   // out of GlobalFilter


const uint32_t keyEventMask  = 0xCA000000;    // mask to check EventBuilder/EventMerger frames
const uint32_t keyIdleMask   = 0x80000000;    // mask applied to agataKey.evnum for keyData?? stuff (to be seen)
const uint32_t keyBoardMask  = 0x0000FF00;    // mask applied to agataKey.evnum to get the domain number
const uint32_t keyChanMask   = 0x000000FF;    // mask applied to agataKey.evnum to get the domain number
const uint32_t keyNbSubMask  = 0x3F000000;    // mask applied to agataKey.evnum to get the number of submain

const int rangeADC = 16384;                   // 

const int      kDetTypes = 3;                 // total number of sub arrays
enum EDetType {kDetNone = -1,
               kDetEAGLE=0, kDetNEDA};

//struct kDetNames {
//  const char* name[kDetTypes] = {
//    "Galileo", "Euclides", "VME",   "NWall",
//    "LaBr",    "Spider",   "Trace", "V1730",
//    "RFD"
//  };
//};
//////////////////////////////////////////////////////////////////////////////
struct agataKey {
  // POD with 5 uint32_t
  uint32_t size = 0;    // in bytes
  uint32_t key = 0;     //
  uint32_t evnum = 0;   // AGATA: defined but never used === GALILEO: domain_number/crystal_id info for the EventBuilder
                        // storing the number of subdomains in &0x3F000000
  uint32_t ts_0 = 0;    //
  uint32_t ts_1 = 0;    //

  uint32_t GetBytes  () const { return size; }
  uint32_t GetEvnum  () const { return  evnum; }
  uint64_t GetTstamp () const { return (uint64_t(ts_1 )<<32) | (uint64_t(ts_0));}
  uint32_t GetBoard  () const { return (evnum & keyBoardMask)>>8; }
  uint32_t GetChannel() const { return (evnum & keyChanMask);} 
  uint32_t GetNbSub  () const { return (evnum & keyNbSubMask) >> 24; }
  bool     IsIdle    () const { return (evnum & keyIdleMask) != 0; }
  //agataKey() { memset(this, 0, sizeof(agataKey)); }
};
//////////////////////////////////////////////////////////////////////////////
struct subData_t
{
  uint64_t timestamp = 0;
  float eAmpli        = 0;    // 0 raw energy from energy filter
  float eEnergy       = 0;    // 1 calibrated energy from eAmpli
  float wBase         = 0;    // 2 baseline value from the trace
  float wAmpli        = 0;    // 3 raw energy derived from the traces
  float wEnergy       = 0;    // 4 calibrated energy from wAmpli
  float trTime        = 0;    // 5 inside trace
  float xAmpli        = 0;    // 6 raw energy corrected for adc-non linearities
  float xEnergy       = 0;    // 7 calibrated energy from xAmpli

  float GetEnergy() const {return eEnergy;}
  float GetTime  () const {return trTime;}
  void  SetEnergy(float e) {eEnergy = e;}
  void  SetTime  (float t) {trTime = t;}
  //subData_t() { memset(this, 0, sizeof(subData_t)); }
};
//////////////////////////////////////////////////////////////////////////////
struct calData_t
{
  int32_t cmode = 1;        // select calibration formula; default is stright-line
  float   ecalv[6];         // p0...p5 or offs, gain, xexp, re-gain
  float   pvmin = -FLT_MAX; // minimum accepted energy for specific paramenters
  float   pvmax =  FLT_MAX; // maximum accepted energy for specific paramenters
  float   scal0 = 0.f;      // offset calibration of traces (keV) (applied only to spectra)
  float   scal1 = 1.f;      // amplitude calibration of traces (keV/chan).
  int32_t tsadd = 0;        // latency correction for timestamp (samples)
  float   tmove = 0;        // T0 shift calibration of traces (samples)
  float   emink = 10.f;     // minimum accepted energy (keV)
  //float tfast = 35.f;     // preamplifier response (ns)
  //int   tfall = 4600;     // main pole of the exponential (samples)
  //int   trise = 800;      // risetime of the trapezoid (samples)
  calData_t() {
    memset(ecalv, 0, sizeof(ecalv));
    ecalv[1] = 1.f;
    cmode   = 1;
  }
  float Ecal(float ampli) const {
    switch (cmode) {
    case 0:
      return (ecalv[1]*ampli + ecalv[0]*(1.f - expf(-ecalv[2]*ampli)))*ecalv[3];
    case 1:
      return ecalv[0] +  ecalv[1]*ampli;
    case 2:
      return ecalv[0] + (ecalv[1] +  ecalv[2]*ampli)*ampli;
    case 3:
      return ecalv[0] + (ecalv[1] + (ecalv[2] +  ecalv[3]*ampli)*ampli)*ampli;
    case 4:
      return ecalv[0] + (ecalv[1] + (ecalv[2] + (ecalv[3] +  ecalv[4]*ampli)*ampli)*ampli)*ampli;
    case 5:
      return ecalv[0] + (ecalv[1] + (ecalv[2] + (ecalv[3] + (ecalv[4] +  ecalv[5]*ampli)*ampli)*ampli)*ampli)*ampli;
    default:
      return ampli;
    }
  }
  float Scal(float ampli) const { return scal0 + scal1*ampli; }
};
//////////////////////////////////////////////////////////////////////////////
class MWD;
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
struct agava_t {
  // POD with 14 uint32_t
  uint32_t header = 0;
  uint32_t status = 0;
  uint32_t local_trigger_counter = 0;
  uint32_t validation_counter = 0;
  uint32_t rejection_counter = 0;
  uint32_t timeout_counter = 0;
  uint32_t local_trigger_tag_hi = 0;
  uint32_t local_trigger_tag_lo = 0;
  uint32_t validation_tag_hi = 0;
  uint32_t validation_tag_lo = 0;
  uint32_t event_number = 0;
  uint32_t rejection_tag_hi = 0;
  uint32_t rejection_tag_lo = 0;
  uint32_t cblt_trailer = 0;

  //agava_t() { memset(this, 0, sizeof(agava_t)); }
};
//////////////////////////////////////////////////////////////////////////////
struct caenPSDData_t
{
  uint32_t length     ;
  uint32_t keyword    ;
  uint32_t channel    ;
  uint32_t ttag0      ;
  uint32_t ttag1      ;
  float    time       ;
  int16_t  ChargeShort;
  int16_t  ChargeLong ;
};
//////////////////////////////////////////////////////////////////////////////
struct subDataNEDA_t
{
    uint64_t timestamp = 0 ;
  float    qshort    = 0.;
  float    qlong     = 0.;
  float    qfast     = 0.;
  float    qslow     = 0.;
  float    light     = 0.;
  float    cfd       = 0.;
  float    intrise   = 0.;
  uint16_t nbzero    = 0 ;
  uint16_t flags     = 0 ;
};
//////////////////////////////////////////////////////////////////////////////
struct subDataEAGLE_t
{
  uint64_t timestamp = 0 ;  // JG
  uint16_t energy    = 0 ;
  uint16_t cfd       = 0 ;  // in a future here will be a time information
};
//////////////////////////////////////////////////////////////////////////////
struct subDataDSSD_t
{
  uint64_t timestamp = 0 ;  // JG
  uint16_t energy    = 0 ;
  uint16_t cfd       = 0 ;  // in a future here will be a time information
};
//////////////////////////////////////////////////////////////////////////////
struct syncData_t // simple structure to be send to gal-13 for the sync of the CAEN digi
{
  uint32_t ts_lo  =0;
  uint32_t ts_hi  =0;
  float    eEner  =0.;
};
//////////////////////////////////////////////////////////////////////////////
int      DetTypeIndex(const std::string& strDetType);
uint32_t OutDataKey  (const std::string& keyDataOut);

//------------------------------------------------------------------------------------------
struct neutronWall_Data_t
{
  //    int channel;
  //    int data;
  int tof    = 0;     // 0 - TOF
  int zco    = 0;     // 1 - ZCO
  int qvc    = 0;     // 2 - QVC
  unsigned long long timestamp = 0;
};
//////////////////////////////////////////////////////////////////////////////
struct neutronWall_TAC_t
{
  int tac = 0;
  double tac_calibrated =0.;
};
//////////////////////////////////////////////////////////////////////////////
struct neutronWall_SC_t
{
  int energy = 0;
};
//////////////////////////////////////////////////////////////////////////////
enum types_of_blocks {
  agata = 10, conf,
  data, data_crystal, data_ccrystal, data_neda, data_eagle,
    data_neutron_wall,
    data_ranc1,
    data_ranc2,
    data_dssd,
//    data_ranc3,
//    data_ranc4,
//    data_ranc5,
//    data_ranc6,
//    data_ranc7,
//    data_ranc8,
  composite_frame, event_data_psa,
  raw_frame,
  unrecognised = 300
};

//////////////////////////////////////////////////////////////////////////////
class Tkey
{
private:
  unsigned int length;
  unsigned int message_type;
  unsigned int event_number;
  unsigned int timestamp0;
  unsigned int timestamp1;
  std::string name;

  types_of_blocks block_type;
public:
  Tkey(char *ptr) {
    unsigned int *p ;
    p = (unsigned int *) ptr ;

    length = * (p++);
    message_type = * (p++);
    event_number = * (p++);
    timestamp0 = * (p++);
    timestamp1 = * (p++);
  }

  std::string give_name() { return name;}
  //------------------------
  types_of_blocks recognise() {
    switch(message_type){
      

    case keyEventGlobal:          // Event merger container
      block_type = composite_frame;
      name = "composite_frame keyEventGlobal";
      break;

    case keyDataNEDA:
      block_type = data_neda;
      name = "keyDataNEDA";
      break;
    case keyDataEAGLE:
      block_type = data_eagle;
      name = "keyDataEAGLE";
      break;
    case keyDataDSSD:
        block_type = data_dssd;
        name = "keyDataDSSD";
        break;
    default:
      block_type = unrecognised;
      name = "unrecognised";
    }
    return block_type;
  }



  int give_length() {
    return length ;
  }

  unsigned int give_evt_number(){
    return event_number;
  }
  unsigned long long give_timestamp() {
    return (uint64_t (timestamp1) << 32) |
      (0x00000000FFFFFFFFULL&(uint64_t)(timestamp0));
  }
  
  unsigned int give_ts_low(){
    return timestamp0;
  }

  unsigned int give_ts_high(){
    return timestamp1;
  }
  

  unsigned int give_id_of_detector() {
    return ((event_number & keyBoardMask)>>8)*16+event_number & keyChanMask;
  }
  unsigned int give_message_type(){
    return message_type;
  }
  bool     IsIdle() {
    return (event_number & keyIdleMask) != 0;
  }
  ////////

};
//////////////////////////////////////////////////////////////////////////////

#endif // DATAFRAMES_H_INCLUDED

