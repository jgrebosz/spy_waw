/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 ***************************************************************************/
#include "tdata_tracked_65_0.h"

#include "tbuffer_of_data_block.h"
#include "ttracked_hit.h"

#include <string.h>
#include <cstdlib>
extern bool flag_talk;
#ifdef AGATA_PRESENT
//***************************************************************************
Tdata_tracked_65_0::Tdata_tracked_65_0()
{
    hit = NULL;
}
//***************************************************************************
Tdata_tracked_65_0::~Tdata_tracked_65_0()
{
    if ( hit )
    {
        delete [] hit;
    }
}
//***************************************************************************
/*!
    \fn Tdata_tracked_65_0::fill ( Tbuffer_of_data_block *buffer )
 */
int Tdata_tracked_65_0::fill ( Tbuffer_of_data_block *buf )
{

//     int * ptr = ( int* ) buf->get_reading_point ();
//     for ( int i = 0 ; i < 5 ;i++, ptr++ )
//     {
//         std::cout << std::hex << ( int ) ptr << "    val " << * ptr << "    " << std::dec << * ptr << std::endl;
//     }

    if (flag_breakpoint)
        std::cout << "Tdata_tracked_65_0::fill ----------------------" << std::endl;

// #define get_item(x)     memcpy(&x, buf->ga(offset), sizeof(x)); \
// std::cout << "address " << std::hex<<  int(buf->ga(offset)) << " " #x << ", value " << std::dec << x << std::endl;\
// offset += sizeof(x);

// #define get_item(x)     memcpy(&x, buf->gg(sizeof(x)), sizeof(x)); \
// /*std::cout << "address " << std::hex<<  int(buf->inspect_address(offset)) << " " #x << ", value " << std::dec << x << std::endl;*/

//#define get_item(x)

    //if ( !buf->set_reading_point ( offset ) )

    int this_frame_length = 0;

#define get_item(x)     memcpy(&x, buf->gg(sizeof(x)), sizeof(x)); \
    this_frame_length+= sizeof(x) ; \
/* std::cout << " " << #x << ", \t\t " << std::dec << x << std::endl;*/

    get_item ( NbGamma );

    if (flag_breakpoint)
        std::cout << "After unpacking global, NbGamma = " << NbGamma
//     << ", this_frame_length = " << this_frame_length
             <<std::endl;


    if ( NbGamma > 50 )
    {
        std::cout << " Tdata_tracked_65_0::fill    After unpacking global, NbGamma = " << NbGamma
             <<std::endl;
        exit ( 221 );
    }
//     NbHit = 1;
    hit = new Ttracked_hit[NbGamma];

    for ( int i = 0 ; i < NbGamma ; i++ )
    {
        this_frame_length += hit[i].fill ( buf );
//         std::cout << "after hit nr " << i << " this_frame_length = " << this_frame_length << std::endl;
    }
    return this_frame_length;
}
//****************************************************************************
/*!
    \fn Tdata_tracked_65_0::distribute_data()
 */
void Tdata_tracked_65_0::distribute_data()
{
    // at first we distribute the core common data
//     std::cout << "Distributing Tdata_tracked_65_0 " << std::endl;

    for ( unsigned int i = 0 ; i < NbGamma ; i++ )
    {
//         std::cout << "Tdata_tracked_65_0::distribute_data()     hit nr " << i << " NbGamma = " << NbGamma  << std::endl;
        hit[i].distribute_data ();
    }

    delete [] hit;
    hit = NULL;
}
//***************************************************************************
#endif // AGATA_PRESENT