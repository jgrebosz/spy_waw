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
#ifndef TUNPACKED_EVENT_DATA_H
#define TUNPACKED_EVENT_DATA_H

/**
All the data from ADF read about one event

	@author Jerzy Grebosz <grebosz@dhcp-38-222>
*/

#include <vector>

#include "tgeneric_data.h"

////////////////////////////////////////////////////////////////////////////////
class Tunpacked_event_data{
public:
    Tunpacked_event_data();

    ~Tunpacked_event_data();
    void distribute_event_data();
    void collect_one_event();
    void add_the_contributor(Tgeneric_data *ptr);
    void display_data();
    void distribute_data();
    void reset();
  
protected:
    std::vector<Tgeneric_data*> event_contributor;
};

#endif
