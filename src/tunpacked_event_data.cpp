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
#include "tunpacked_event_data.h"

#include <iostream>

//******************************************************************

Tunpacked_event_data::Tunpacked_event_data()
{
}
//******************************************************************
Tunpacked_event_data::~Tunpacked_event_data()
{
}
//******************************************************************
/*!
    \fn Tunpacked_event_data::collect_one_event()
 */
void Tunpacked_event_data::collect_one_event()
{
    /// @todo implement me
}

//******************************************************************
/*!
    \fn Tunpacked_event_data::distribute_event_data()
 */
void Tunpacked_event_data::distribute_event_data()
{
//  std::cout << "Distributing........................................................................................................................." << std::endl;
    for ( unsigned int i = 0 ; i < event_contributor.size() ; i++ )
    {       
        event_contributor[i]->distribute_data();
    }

//     std::cout << "They are not  needed anymore" << std::endl;
    reset();
}
//***********************************************************************
/*!
    \fn Tunpacked_event_data::add_the_contributor(Tgeneric_data *ptr)
 */
void Tunpacked_event_data::add_the_contributor ( Tgeneric_data *ptr )
{
    event_contributor.push_back ( ptr );
}
//*************************************************************************
/*!
    \fn Tunpacked_event_data::display_data()
 */
void Tunpacked_event_data::display_data()
{
    std::cout << "Displayin data from all unpacked frames...." << std::endl;
    for ( unsigned int i = 0 ; i < event_contributor.size() ; i++ )
    {
        event_contributor[i]->display_data();
    }
}
//***************************************************************
/*!
    \fn Tunpacked_event_data::distribute_data()
 */
void Tunpacked_event_data::distribute_data()
{
    /// @todo implement me
}


/*!
    \fn Tunpacked_event_data::reset()
 */
void Tunpacked_event_data::reset()
{
  for ( unsigned int i = 0 ; i < event_contributor.size() ; i++ )
    {
        delete event_contributor[i];
    }
     event_contributor.clear();
}
