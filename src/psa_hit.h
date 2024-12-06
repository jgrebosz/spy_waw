/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 *                                                                         *
 ***************************************************************************/
#ifndef PSA_HIT_H
#define PSA_HIT_H

#include <iostream>

class Tbuffer_of_data_block;

//  typedef float fl;
typedef double fl;
/**
    @author Jerzy Grebosz <grebosz@dhcp-38-222>
*/
class Tagata;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class psa_hit
{
        unsigned short Status;
        unsigned short ID;
        fl X;
        fl Y;
        fl Z;
        fl E;
        fl T;
        fl DX;
        fl DY;
        fl DZ;
        fl DE;
        fl DT;
    public:
        psa_hit();
	//Tagata *aga;

        ~psa_hit();
        int fill ( Tbuffer_of_data_block *buffer );

        friend
        std::ostream &   operator << ( std::ostream & s, psa_hit & obj )
        {
            s
            << "  Status= " << obj.Status
            << "  ID= " << obj.ID
            << " X =  " << obj.X
            << " Y = " << obj.Y
            << " Z = " << obj.Z
            << "    E = " << obj.E
            << " T = " << obj.T

            << "    DX = " << obj.DX
            << " DY = " << obj.DY
            << " DZ = " << obj.DZ
            << " DE = " << obj.DE
            << " DT = " << obj.DT   ;
            s << std::endl;
            return s;
        }
        
        void display()
        {
	 std::cout << *this << std::endl; 
	}
    void distribute_data(int crystal_id);
};

#endif
