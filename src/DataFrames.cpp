#include "DataFrames.h"
#include "Misc.h"


// case insensitive comparisons
uint32_t OutDataKey(const std::string& keyDataOut) {
  if (stringEq(keyDataOut, "EAGLE")) {
    return keyDataEAGLE;
  }
  else if (stringEq(keyDataOut, "NEDA")) {
    return keyDataNEDA;
  }
  else {
    return 0; // ??
  }
}

int DetTypeIndex(const std::string& strDetType)
{
  if (stringEq(strDetType, "EAGLE")) {
    return kDetEAGLE;
  }
  else if (stringEq(strDetType, "NEDA")) {
    return kDetNEDA;
  }
  else {
    return kDetNone;
  }
}

std::string OutDetectorKey(uint32_t dataKey)
{
  std::string returnVal;
  switch(dataKey){
    /////////////////////
    //  composite keys //
  case keyDataEAGLE:
    returnVal="data:eagle";
    break;
  case keyDataNEDA:
    returnVal="data:neda";
    break;
    //  end of evt data   //
    ////////////////////////
  default:
    returnVal="data:unknown";  
    
  }
  return returnVal;

}
