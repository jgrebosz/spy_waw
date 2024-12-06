#ifndef TDATA_RANC1_H
#define TDATA_RANC1_H

#include <tgeneric_data.h>

#include <vector>
using  namespace std;

#include "experiment_def.h"

/**
Ancillary, but as floating point data

	@author Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/
class Tbuffer_of_data_block;
class TNeedle;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tdata_ranc1 : public Tgeneric_data
{
std::vector<float> array;

float X1, Y1;
float X2, Y2;
float X3, Y3;

float charge1;
float charge2;
float charge3;

float timeA;
float timeB;
float timeC;
float gammaOR;
unsigned int dante_timestamp;

float parameter155;

float parameter[HOW_MANY_DANTE_PARAMETERS];


TNeedle * frs_ptr;
public:
    Tdata_ranc1();

    ~Tdata_ranc1();
    int  fill ( Tbuffer_of_data_block *buffer, unsigned int timestamp  );
    void distribute_data();
void display();
};

#endif
