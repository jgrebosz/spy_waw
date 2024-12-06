/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Tdata_neutron_wall_H
#define Tdata_neutron_wall_H

/**
For unpacking ancillary detectors

	@author Jerzy Grebosz <grebosz@dhcp-38-222>
*/
#include "tgeneric_data.h"
#include <vector>
#include <map>

#include "DataFrames.h"
#include "experiment_def.h"
#include "TNeutronWallDetector.h"
#include "TTACDetector.h"

class Tbuffer_of_data_block;
class TNeedle;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tdata_neutron_wall: public Tgeneric_data
{

//std::vector<neutron_wall_Data_t> sub_data_vector;
std::vector<std::pair<int, int> > sub_data_vector;

static std::map<int,TNeutronWallDetector*> lookup_nw; // not sure if it is this structure
static std::map<int,TTACDetector*> lookup_tac; // not sure if it is this structure

static bool flag_lookup_loaded;

TNeedle    *galileo_pointer ;

bool read_in_lookup_table() ;

private:
    unsigned long long ts_value;

public:
    Tdata_neutron_wall();

    ~Tdata_neutron_wall();
    int  fill (Tbuffer_of_data_block *buffer , unsigned long long ts,  int nr_of_data_bytes);
    void distribute_data();
    

};

#endif
