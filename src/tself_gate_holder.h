/*
All detectors which have selfgate will inherit this class. It is to be able
to store the address of such object in an abstract pointer for the beta_analog purposes
*/

#ifndef TSELF_GATE_HOLDER_H
#define TSELF_GATE_HOLDER_H

#include "Tself_gate_abstract_descr.h"

class Tself_gate_holderxxx
{
public:
    virtual    bool check_selfgate(Tself_gate_abstract_descr *desc) = 0;
};

#endif // TSELF_GATE_HOLDER_H
