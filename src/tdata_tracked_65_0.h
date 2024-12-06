/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 *                                                                         *
 ***************************************************************************/
#ifndef TDATA_TRACKED_65_0_H
#define TDATA_TRACKED_65_0_H
/**
	@author Jerzy Grebosz <grebosz@dhcp-38-222>
*/
#include "tgeneric_data.h"
class Tbuffer_of_data_block;
class Ttracked_hit;
///////////////////////////////////////////////////////////////////////////
class Tdata_tracked_65_0 :public Tgeneric_data
{
    unsigned short NbGamma;

    Ttracked_hit * hit;
public:
    Tdata_tracked_65_0();

    ~Tdata_tracked_65_0();
    int fill ( Tbuffer_of_data_block *buffer );

    void distribute_data();
};

#endif
