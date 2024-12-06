/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 ***************************************************************************/
#ifndef TTRACKED_HIT_H
#define TTRACKED_HIT_H

/**
    @author Jerzy Grebosz <grebosz@dhcp-38-222>
*/

class Tbuffer_of_data_block;
class Tagata;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef double fl;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Ttracked_hit
{
        unsigned short Status;
        fl X;
        fl Y;
        fl Z;
        fl X1;
        fl Y1;
        fl Z1;
        fl E;
        fl T;

        fl DX;
        fl DY;
        fl DZ;
        fl DX1;
        fl DY1;
        fl DZ1;
        fl DE;
        fl DT;
        
        Tagata *aga ;
    public:
        Ttracked_hit();
        ~Ttracked_hit();
        int fill ( Tbuffer_of_data_block *buffer );
    void distribute_data();
    void display();

};

#endif
