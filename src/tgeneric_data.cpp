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
#include "tgeneric_data.h"

#include <iostream>
#include "TGalileoEvent.h"

//static 
TGalileoEvent * Tgeneric_data::event_ptr;
//************************************************************
Tgeneric_data::Tgeneric_data()
{
}
//************************************************************************
Tgeneric_data::~Tgeneric_data()
{
}
//************************************************************************
/*!
    \fn Tgeneric_data::display_data()
 */
void Tgeneric_data::display_data()
{
    std::cout << "Generic version of function responsible for displaying data Tgeneric_data::display_data()" 
    << std::endl;
}
//************************************************************************

/*!
    \fn Tgeneric_data::distribute_data()
 */
void Tgeneric_data::distribute_data()
{
    std::cout << "Empty Generic version of function responsible for displaying data Tgeneric_data::distribute_data()" 
    << std::endl;
}
