#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#include <string>
#include <vector>
#include <chrono>

inline double get_wall_time() {
  std::chrono::duration<double> td = std::chrono::system_clock::now().time_since_epoch();
  return td.count();
}

//double get_wall_time();
//double get_cpu_time();

void  stringTrim(std::string &str);
bool  stringTrim(std::string &str, const std::string& cmt);
bool  stringFirst(std::string &line, std::string &keyw, const std::string sep = " \t");
bool  stringSplit(std::string &line, std::string &keyw, std::string &data, const std::string sep = " \t");
int   stringCount(std::string &line, const std::string sep = " \t");
bool  stringIncrement(std::string &str, int count = 1);
bool  stringIncrement(std::string &str, const std::string sep, int count = 1);
bool  stringEq    (const std::string &s1, const std::string &s2, bool ignorecase = true);
bool  stringHas   (const std::string &s1, const std::string &s2, bool ignorecase = true);
int   stringWhere (const std::string &s1, const std::string &s2, bool ignorecase = true);
int   stringLocate(const std::string &s1, const std::vector<std::string> &s2, bool ignorecase = true, bool partial = true);
void  stringRemove(std::string &str, const std::string rem, bool by_char = true);

int   getKeyFromFile(const std::string filename, const std::string theKey, std::string &theValue, bool verbose = false);
void  forceTailSlash(std::string &dr);
bool  forceExtension(std::string &fn, const std::string &type);
bool  fileExists(const std::string &fname);

void  printInfo(const std::string &s1);
void  printInfo(const std::string &s1, const std::string &s2);
void  printInfo(const std::string &s1, const std::string &s2, const std::string &s3);

bool  StrightLine(const float *data, int nchan, float &A, float &B, float ref = 0.5f);
bool  Parabola   (const float *data, int nchan, float &A, float &B, float &C, float ref = 0.5f);

bool  InvertMatrix2 (const double m[ 4], double invOut[ 4]);
bool  InvertMatrix3 (const double m[ 9], double invOut[ 9]);
bool  InvertMatrix4 (const double m[16], double invOut[16]);

template <typename T>
inline T AverageValue(const T * data, int nchan)
{
  if(nchan < 1)
    return 0;
  T vv = 0;
  for(int nn = 0; nn < nchan; nn++)
    vv += data[nn];
  return vv/nchan;
}

template <typename T>
inline T AverageVariance(const T * data, int nchan, T meanval = 0)
{
  if(nchan < 1)
    return 0;
  T vv = 0;
  for(int nn = 0; nn < nchan; nn++) {
    float vt  = data[nn]-meanval;
    vv += vt*vt;
  }
  return vv/nchan;
}

//template<class T>
//class median
//{
//public:
//  median() : a1(0), a2(0), an(0) {}
//  void reset(const T &a3) {a1=a3; a2=a3; an=a3;}
//  T exec(const T &a3) {
//    if(a1 < a2) {
//      if     (a3 > a2) an = a2;
//      else if(a3 < a1) an = a1;
//      else             an = a3;
//    }
//    else {
//      if     (a3 > a1) an = a1;
//      else if(a3 < a2) an = a2;
//      else             an = a3;
//    }
//    a1 = a2;
//    a2 = a3;
//    return an;
//  }
//  int myID;
//  T   a1;
//  T   a2;
//  T   an;
//};

#endif  // MISC_H_INCLUDED
