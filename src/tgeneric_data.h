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
#ifndef TGENERIC_DATA_H
#define TGENERIC_DATA_H

/**
    @author Jerzy Grebosz <grebosz@dhcp-38-222>
*/

class TGalileoEvent;    // we need it for the pointer to the event where the unpacked information goes.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tgeneric_data
  {
  public:
    Tgeneric_data();

    virtual  ~Tgeneric_data();
    virtual void display_data();
    virtual void distribute_data();

    static TGalileoEvent *event_ptr;
  };

#endif
