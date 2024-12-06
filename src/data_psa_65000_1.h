/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 *                                                                         *
 ***************************************************************************/
#ifndef DATA_PSA_65000_1_H
#define DATA_PSA_65000_1_H

#include <iostream>
#include <vector>
#include <map>
#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TNeedle.h"
#include "spectrum.h"
#include "tgeneric_data.h"
#include <psa_hit.h>
#include <climits>
#include "DataFrames.h"
#include "experiment_def.h"
#include "TNEDADetector.h"
#include "TEAGLEDetector.h"
#include "TDSSDDetector.h"
//#include "TGalileoDetector.h"
//#include "TSpiderDetector.h"
//#include "TLaBrDetector.h"
//#include "TTraceDetector.h"

 #define CORR_ADC_PROFILE // Dino says that this data has no profile

typedef float   typ_oliver;
// typedef double   typ_oliver;
/**
 @author Jerzy Grebosz <grebosz@dhcp-38-222>
 */
class Tbuffer_of_data_block;
class Tagata;
class TNeedle;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class data_psa_65000_1 : public Tgeneric_data
{
  
  TNeedle    *galileo_pointer ;
  int max_subdata;   // in the event there is data for energy and AC
  subDataNEDA_t      sub_data_neda;
  subDataEAGLE_t     sub_data_eagle;
  subDataDSSD_t     sub_data_dssd;
  int                detector_domain;
  unsigned long long tstamp;
  unsigned int       frameKey  ;
  bool neda_pres;
  static std::map<int,TNEDADetector* > lookup_neda;

  bool eagle_pres;
  static std::map<int,TEAGLEDetector*> lookup_eagle;

  bool dssd_pres;
  static std::map<int,TDSSDDetector*> lookup_dssd;

  static bool flag_lookup_loaded;
  bool neutronwall_pres;
  
  Tagata *aga ;
public:
  data_psa_65000_1() ;
  virtual ~data_psa_65000_1() {} // if no body of constructor - the error is: undefined reference to `vtable for data_psa_65000_1'
  int  fill_neda_structure(Tbuffer_of_data_block *buf, Tkey *key);
  int  fill_eagle_structure(Tbuffer_of_data_block *buf, Tkey *key);
  int  fill_dssd_structure(Tbuffer_of_data_block *buf, Tkey *key);
  //---------------------------------------------------
  friend
  std::ostream    &operator << (std::ostream &s, data_psa_65000_1 &obj) {
    
#define print_item(x)   s << " " << #x << ", \t\t " <<  d.x << "\n"
    subDataEAGLE_t d = obj.sub_data_eagle;
    s << "Domain ----------" << std::endl;
    print_item(energy);     // raw energy from energy filter
    print_item(cfd);        // calibrated energy from eAmpli
    s << std::endl;
   
    return s;
  }
  //------------------------------------------------------------------
  void distribute_data();
};
#endif
