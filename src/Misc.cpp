#include "Misc.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

void stringTrim(std::string &str)
{
  size_t ll = str.length();

  // get rid of empty  strings
  if(ll <= 0)
    return;

  // get rid of the CR character
  // (e.g. files written on Windows (\r\n) and read on Unix(\n) )
  if(str[ll-1] == '\r') {
    str = str.substr(0, ll-1);
    ll--;
  }

  // discard if now empty
  if(ll <= 0)
    return;

  size_t ii = str.find_first_not_of(" \t");

  // contains only white space
  if(ii == string::npos) {
    str.clear();
    return;
  }

  // remove white space at the beginning
  if(ii > 0) {
    str = str.substr(ii);
    ll = str.length();
  }

  ii = str.find_last_not_of(" \t");

  // remove white space at the end
//  if(ii >= 0 && ii < ll-1)
  if(ii < ll-1)
    str = str.substr(0, ii+1);
}

bool stringTrim(std::string& str, const std::string& cmt)
{
  stringTrim(str);

  size_t ll = str.length();
  if(ll <= 0)
    return false;

  // check for comment substring
  size_t nn = str.find_first_of(cmt);
  if(nn == string::npos)
    return false;

  // and remove it to the end of line
  str = str.substr(0, nn);

  // and remove possible white space at the end
  stringTrim(str);

  return false;
}

void  stringRemove(std::string &str, const std::string rem, bool by_char) {
  if(by_char) {
    size_t nn = str.find_first_of(rem);
    while(nn != string::npos) {
      str = str.substr(0, nn) + str.substr(nn+1);
      nn = str.find_first_of(rem);
    }
  }
  else {
    size_t nn = str.find(rem);
    while(nn != string::npos) {
      str = str.substr(0, nn) + str.substr(nn+rem.size());
      nn = str.find(rem);
    }
  }
}

bool stringFirst(std::string &line, std::string &keyw, const std::string sep)
{
  stringTrim(line, "#");
  if(line.empty())
    return false;
  size_t npos1 = line.find_first_not_of(sep, 0);
  size_t npos2 = line.find_first_of(sep, npos1);
  keyw = line.substr(npos1, npos2);
  line = line.substr(npos2+1);
  stringTrim(keyw);
  stringTrim(line);
  return true;
}

bool stringSplit(std::string &line, std::string &keyw, std::string &data, const std::string sep)
{
  stringTrim(line, "#");
  if(line.empty())
    return false;

  const char cc[2] = {'\'', '"'}; // if grouped as '' or ""
  for(int nn = 0; nn < 2; nn++) {
    if(line[0] == cc[nn]) {
      size_t npos2 = line.find_first_of(cc[nn], 1);
      if(npos2 != string::npos) {
        keyw = line.substr(1, npos2-1);
        data = line.substr(npos2+1);
      }
      else {
        keyw = line.substr(1);    // grab all ??
        data = "";
      }
      stringTrim(keyw);
      stringTrim(data);
      return true;
    }
  }

  size_t npos1 = line.find_first_not_of(sep, 0);
  size_t npos2 = line.find_first_of(sep, npos1);
  keyw = line.substr(npos1, npos2);
  if(npos2 != string::npos)
    data = line.substr(npos2+1);
  else
    data.clear();
  stringTrim(keyw);
  stringTrim(data);
  return true;
}

int stringCount(std::string &line, const std::string sep)
{
  stringTrim(line, "#");
  if(line.empty())
    return 0;

  int count = 1;
  size_t pos1 = 0;

  while(true) {
    size_t pos2 = line.find_first_of(sep, pos1);
    if(pos2 == string::npos)
      break;
    count++;
    pos1 = line.find_first_not_of(sep, pos2);
  }

  return count;
}

bool stringIncrement(std::string &str, int count)
{
  if(count == 0)
    return false;

  size_t lf = str.size();
  if(lf == string::npos || lf < 1)
    return false;

  if(count > 0) {
    // increment
    for(int nn = 0; nn < count; nn++) {
      for(size_t ll = lf-1; ll > 0; ll--) {
        if(str[ll] >= '0' && str[ll] < '9') {
          str[ll]++;
          break;
        }
        else if(str[ll] == '9') {
          str[ll] = '0';
        }
        else if(str[ll] >= 'a' && str[ll] < 'z') {
          str[ll]++;
          break;
        }
        else if(str[ll] == 'z') {
          str[ll] = 'a';
        }
        else if(str[ll] >= 'A' && str[ll] < 'Z') {
          str[ll]++;
          break;
        }
        else if(str[ll] == 'Z') {
          str[ll] = 'A';
        }
        else {
          return false;
        }
      }
    }
  }
  else {
    // decrement
    for(int nn = 0; nn < -count; nn++) {
      for(size_t ll = lf-1; ll > 0; ll--) {
        if(str[ll] >= '0' && str[ll] < '9') {
          str[ll]--;
          break;
        }
        else if(str[ll] == '0') {
          str[ll] = '9';
        }
        else if(str[ll] >= 'a' && str[ll] < 'z') {
          str[ll]++;
          break;
        }
        else if(str[ll] == 'a') {
          str[ll] = 'z';
        }
        else if(str[ll] >= 'A' && str[ll] < 'Z') {
          str[ll]++;
          break;
        }
        else if(str[ll] == 'A') {
          str[ll] = 'Z';
        }
        else
          return false;
      }
    }
  }
  return true;
}

bool stringIncrement(std::string &str, const std::string sep, int count)
{
  if(count == 0)
    return false;

  size_t lf = str.size();
  if(lf == string::npos || lf < 1)
    return false;

  size_t lff = (int)str.find_last_of(sep);
  if(lff == string::npos)
    return false;

  string fnn = str.substr(0, lff);
  if(stringIncrement(fnn, count)) {
    str = fnn + str.substr(lff);
    return true;
  }

  return false;
}

bool stringEq(const std::string &s1, const std::string &s2, bool ignorecase)
{
  if (!ignorecase)
    return s1 == s2;

  string sLow1(s1), sLow2(s2);
  transform(sLow1.begin(), sLow1.end(), sLow1.begin(), ::tolower);
  transform(sLow2.begin(), sLow2.end(), sLow2.begin(), ::tolower);
  return sLow1 == sLow2;
}

bool stringHas(const std::string &s1, const std::string &s2, bool ignorecase)
{
  if (!ignorecase)
    return s1.find(s2) != string::npos;

  string sLow1(s1), sLow2(s2);
  transform(sLow1.begin(), sLow1.end(), sLow1.begin(), ::tolower);
  transform(sLow2.begin(), sLow2.end(), sLow2.begin(), ::tolower);
  return sLow1.find(sLow2) != string::npos;
}

int stringWhere(const std::string &s1, const std::string &s2, bool ignorecase)
{
  size_t ind;
  if (!ignorecase) {
    ind = s1.find(s2);
  }
  else {
    string sLow1(s1), sLow2(s2);
    transform(sLow1.begin(), sLow1.end(), sLow1.begin(), ::tolower);
    transform(sLow2.begin(), sLow2.end(), sLow2.begin(), ::tolower);
    ind = sLow1.find(sLow2);
  }
  return (ind != string::npos) ? int(ind) : -1;
}

int stringLocate(const std::string &s1, const std::vector<std::string> &sv, bool ignorecase, bool partial)
{
  if(sv.empty())
    return -1;

  string line(s1), sLow1, rest;
  stringSplit(line, sLow1, rest);
  stringTrim(sLow1);
  if(ignorecase)
    transform(sLow1.begin(), sLow1.end(), sLow1.begin(), ::tolower);

  int count =  0;
  int which = -1;
  for(size_t nc = 0; nc < sv.size(); nc++) {
    string line(sv[nc]), sLow2, rest;
    stringSplit(line, sLow2, rest);
    stringTrim(sLow2);
    if(ignorecase)
      transform(sLow2.begin(), sLow2.end(), sLow2.begin(), ::tolower);
    size_t ind = sLow2.find(sLow1);
    if(ind == 0) {
      which = int(nc);
      if(sLow1==sLow2)
        return which;   // found a full match
      ++count;
    }
  }
  if(!partial)
    return -1;
  return (count > 1) ? -count : which;

}

int getKeyFromFile(const std::string filename, const std::string theKey, std::string &theValue, bool verbose)
{
  ifstream configfile( filename.c_str() );
  if(!configfile.good()) {
    cout << " Error opening " << filename << endl;
    return 2;   // error
  }
  if(verbose)
    cout << " getKeyFromFile() reading from " << filename << endl;

  string line, keyw, data;
  while(getline(configfile, line)) {
    if(!stringSplit(line, keyw, data))
      continue;           // empty or comment lines
    if(verbose)
      cout << line << endl;
    if(stringEq(keyw,theKey)) {
      theValue = data;    // data can be null
      return 0;           // found
    }
  }
  return 1;   // not found
}

void  forceTailSlash(std::string &dr) {
  if( dr.size()>0 && dr.at(dr.size()-1)!= '/' ) dr += '/';
}

bool  forceExtension(std::string &fn, const std::string &type)
{
  stringTrim(fn);
  string ext(type);
  if(ext.find(".") != 0)
    ext = "." + ext;
  size_t nn = fn.rfind(ext);
  if( (nn == string::npos) || (nn != fn.size()-ext.size()) ) {
    fn += ext;
    return true;
  }
  return false;
}

bool fileExists(const std::string &fname)
{
  FILE *fp = fopen(fname.c_str(), "r");
  if(fp) {
    fclose(fp);
    return true;
  }
  return false;
}

void printInfo(const std::string &s1) {
  cout << s1 << endl;
}

void printInfo(const std::string &s1, const std::string &s2) {
  const int w1 = 22;
	cout << left << setw(w1) << s1 << right << " ";
  cout << s2 << endl;
}

void printInfo(const std::string &s1, const std::string &s2, const std::string &s3) {
  const int w1 = 22;
  const int w2 = 22;
	cout << left << setw(w1) << s1 << right << " ";
	cout << left << setw(w2) << s2 << right << " ";
  cout << s3 << endl;
}

bool StrightLine(const float *data, int nchan, float &A, float&B, float ref)
{
  if(nchan < 2) {
    if(nchan < 1)
      return false;
    A = data[0];
    B = 0;
    return true;
  }

  double pos = ref;   // position where to pass through the channel
  double sx0 = 0, sx1 = 0, sx2 = 0;
  double yx0 = 0, yx1 = 0;
  for(int ii = 0; ii < nchan; ii++) {
    double x = ii + pos;
    double y = data[ii];
    sx0 += 1;
    sx1 += x;
    sx2 += x*x;
    yx0 += y;
    yx1 += y*x;
  }

  // the [x] components and the determinant are constant and could/should be precalculated
  //  N  =>  1   2    3    4    5    6    7     8     9    10
  //  x  =>  0   1    2    3    4    5    6     7     8     9
  // sx0 =>  1   2    3    4    5    6    7     8     9    10  ==  N
  // sx1 =>  0   1    3    6   10   15   21    28    36    45  ==  N*(N-1)/2
  // sx2 =>  0   1    5   14   30   55   91   140   204   285  == (2*N^3 - 3*N^2 + N)/6
  // det =>  0   1    6   20   50  105  196   336   540   825  == (N^4 - N^2)/12

  double deter = sx0*sx2-sx1*sx1;
  if(deter >= 0.) {
    A = float(( yx0*sx2 - yx1*sx1)/deter);
    B = float((-yx0*sx1 + yx1*sx0)/deter);
    return true;
  }
  else {
    A = B = 0;
    return false;
  }
}

bool Parabola(const float *data, int nchan, float &A, float &B, float &C, float ref)
{
  if(nchan < 3) {
    C = 0;
    return StrightLine(data, nchan, A, B, ref);
  }

  // the [x] components and the determinant are constant and could/should be precalculated
  //  N  =>  1   2    3    4    5    6     7     8      9     10
  //  x  =>  0   1    2    3    4    5     6     7      8      9
  //  x2 =>  0   1    4    9   16   25    36    49     64     81
  //  x3 =>  0   1    8   27   64  125   216   343    512    729
  //  x4 =>  0   1   16   81  256  625  1296  2401   4096   6561
  // sx0 =>  1   2    3    4    5    6     7     8      9     10  ==  N
  // sx1 =>  0   1    3    6   10   15    21    28     36     45  ==  N*(N-1)/2
  // sx2 =>  0   1    5   14   30   55    91   140    204    285  == (2*N^3 - 3*N^2 + N)/6
  // sx3 =>  0   1    9   36  100  225   441   784   1296   2025  ==  sx1^2
  // sx4 =>  0   1   17   98  354  979  2275  4676   8772  15333  == (6*N^5 - 15*N^4 + 10*N^3 - N)/30
  // det =>  0   0    4   80  700 3290 16464 56448 166320 435600  ==  ...

  double pos = ref;   // position where to pass through the channel
  double sx0 = 0, sx1 = 0, sx2 = 0, sx3 = 0, sx4 = 0;
  double yx0 = 0, yx1 = 0, yx2 = 0;
  sx0 = nchan;
  for(int ii = 0; ii < nchan; ii++) {
    double x = ii+pos;
    double t = x;
    double y = data[ii];
    sx1 += t;
    yx0 += y;
    yx1 += y*t;
    t   *= x;
    sx2 += t;
    yx2 += y*t;
    t   *= x;
    sx3 += t;
    sx4 += x*t;
  }

  double deter = sx0*sx2*sx4 + sx1*sx3*sx2 + sx2*sx1*sx3 - sx0*sx3*sx3 - sx1*sx1*sx4 - sx2*sx2*sx2;
  if(deter >= 0.) {
    A = float( ( yx0*(sx2*sx4-sx3*sx3) + yx1*(sx2*sx3-sx1*sx4) + yx2*(sx1*sx3-sx2*sx2) ) / deter );
    B = float( ( yx0*(sx3*sx2-sx1*sx4) + yx1*(sx0*sx4-sx2*sx2) + yx2*(sx2*sx1-sx0*sx3) ) / deter );
    C = float( ( yx0*(sx1*sx3-sx2*sx2) + yx1*(sx1*sx2-sx0*sx3) + yx2*(sx0*sx2-sx1*sx1) ) / deter );
    return true;
  }
  else {
    C = 0;
    return StrightLine(data, nchan, A, B, ref);
  }
}

bool InvertMatrix2(const double m[4], double invOut[4])
{
  double d00 = m[0];
  double d01 = m[1];
  double d10 = m[2];
  double d11 = m[3];

  double Det =  d00*d11 - d01*d10;
  if (Det == 0)
    return false;

  double D00 =  d11; double D01 = -d01;
  double D10 = -d10; double D11 =  d00;

  Det = 1/Det;

  invOut[0] = D00 * Det;
  invOut[1] = D01 * Det;
  invOut[2] = D10 * Det;
  invOut[3] = D11 * Det;

#if 0
#define indx(n1, n2) (n1*2+n2)
  double unit[4] = {0};    // memset(unit, 0, sizeof(unit));
  for(int i1 = 0; i1 < 2; i1++) {
    for(int i2 = 0; i2 < 2; i2++) {
      double accu = 0;
      for(int ii = 0; ii < 2; ii++) {
        accu += m[indx(i1,ii)]*invOut[indx(ii,i2)];
      }
      unit[indx(i1,i2)] = accu;
    }
  }
#undef indx
#endif

  return true;
}

bool InvertMatrix3(const double m[9], double invOut[9])
{
  double d00 = m[0];
  double d01 = m[1];
  double d02 = m[2];
  double d10 = m[3];
  double d11 = m[4];
  double d12 = m[5];
  double d20 = m[6];
  double d21 = m[7];
  double d22 = m[8];

  double Det =  d00*(d11*d22 - d12*d21) + d01*(d12*d20 - d10*d22) + d02*(d10*d21 - d11*d20);
  if (Det == 0)
    return false;

  double D00 =  d11*d22 - d12*d21; double D01 =  d12*d20 - d10*d22; double D02 =  d10*d21 - d11*d20;
  double D10 =  d21*d02 - d22*d01; double D11 =  d22*d00 - d20*d02; double D12 =  d20*d01 - d21*d00;
  double D20 =  d01*d12 - d02*d11; double D21 =  d02*d10 - d00*d12; double D22 =  d00*d11 - d01*d10;

  Det = 1/Det;

  invOut[0] = D00 * Det;
  invOut[1] = D01 * Det;
  invOut[2] = D02 * Det;
  invOut[3] = D10 * Det;
  invOut[4] = D11 * Det;
  invOut[5] = D12 * Det;
  invOut[6] = D20 * Det;
  invOut[7] = D21 * Det;
  invOut[8] = D22 * Det;

#if 0
#define indx(n1, n2) (n1*3+n2)
  double unit[9] = {0};    // memset(unit, 0, sizeof(unit));
  for(int i1 = 0; i1 < 3; i1++) {
    for(int i2 = 0; i2 < 3; i2++) {
      double accu = 0;
      for(int ii = 0; ii < 3; ii++) {
        accu += m[indx(i1,ii)]*invOut[indx(ii,i2)];
      }
      unit[indx(i1,i2)] = accu;
    }
  }
#undef indx
#endif

  return true;

}

// this is gluInvertMatrix from Mesa
bool InvertMatrix4(const double m[16], double invOut[16])
{
    double inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

#if 0
#define indx(n1, n2) (n1*4+n2)
    double unit[16] = {0};    // memset(unit, 0, sizeof(unit));
    for(int i1 = 0; i1 < 4; i1++) {
      for(int i2 = 0; i2 < 4; i2++) {
        double accu = 0;
        for(int ii = 0; ii < 4; ii++) {
          accu += m[indx(i1,ii)]*invOut[indx(ii,i2)];
        }
        unit[indx(i1,i2)] = accu;
      }
    }
#undef indx
#endif

    return true;
}

// taken from
// http://stackoverflow.com/questions/17432502/how-can-i-measure-cpu-time-and-wall-clock-time-on-both-linux-windows

//// should be done with boost e.g.
////#include <boost/timer/timer.hpp>
////
////// this is wallclock AND cpu time
////boost::timer::cpu_timer timer;
////
////... run some computation ...
////
////boost::timer::cpu_times elapsed = timer.elapsed();
////std::cout << " CPU TIME: " << (elapsed.user + elapsed.system) / 1e9 << " seconds"
////          << " WALLCLOCK TIME: " << elapsed.wall / 1e9 << " seconds"
////          << std::endl;
////

#if 0
#ifdef _WIN32
//  Windows
#include <Windows.h>
double get_wall_time(){
  LARGE_INTEGER time,freq;
  if (!QueryPerformanceFrequency(&freq)){
    return 0;    //  Handle error
  }
  if (!QueryPerformanceCounter(&time)){
    return 0;    //  Handle error
  }
  return (double)time.QuadPart / freq.QuadPart;
}
double get_cpu_time(){
  FILETIME a,b,c,d;
  if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
    //  Returns total user time.
    //  Can be tweaked to include kernel times as well.
    DWORD dwtt = d.dwLowDateTime | ((unsigned long long)d.dwHighDateTime << 32);
    return double(dwtt) * 0.0000001;
  }
  else{
    return 0;    //  Handle error
  }
}

#else
//  Posix/Linux
#include <sys/time.h>
double get_wall_time(){
  struct timeval time;
  if (gettimeofday(&time,nullptr)){
    return 0;   //  Handle error
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
double get_cpu_time(){
  return (double)clock() / CLOCKS_PER_SEC;
}

#endif
#endif
