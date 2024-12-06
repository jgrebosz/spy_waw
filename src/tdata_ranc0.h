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
#ifndef Tdata_ranc0_oryginal_H
#define Tdata_ranc0_oryginal_H

/**
For unpacking ancillary detectors

	@author Jerzy Grebosz <grebosz@dhcp-38-222>
*/
#include "tgeneric_data.h"
#include <vector>
#include "DataFrames.h"
#include "experiment_def.h"

class Tbuffer_of_data_block;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tdata_ranc0_oryginal: public Tgeneric_data
{

//std::vector<neutron_wall_Data_t> sub_data_vector;
std::vector<std::pair<int, int> > sub_data_vector;

public:
    Tdata_ranc0_oryginal();

    ~Tdata_ranc0_oryginal();
    int  fill (Tbuffer_of_data_block *buffer , int nr_of_data_bytes);
    void distribute_data();
    

};

#endif
