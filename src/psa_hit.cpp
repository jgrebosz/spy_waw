/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 *                                                                         *

 ***************************************************************************/
#include "psa_hit.h"
#include "TGalileoEvent.h"
// #include "Tagata.h"       // aga
#include <iostream>
#include <cstring>
#include "TGalileoAnalysis.h"


#include "tbuffer_of_data_block.h"

//***************************************************************************
psa_hit::psa_hit()
{
  //aga =  GalileoAnalysis_ptr->give_aga_ptr();
}
//***************************************************************************
psa_hit::~psa_hit()
{
}
//***************************************************************************
/*!
    \fn psa_hit::fill( char *buffer , char *begin_buffer, int length)
 */
int psa_hit::fill ( Tbuffer_of_data_block *buf )
{
//     std::cout << " psa_hit::fill    HIT     "
    // "starting address to unpack hits "
    //= " << std::hex << int ( buf->inspect_address ( offset ) ) << std::dec 
//     << std::endl;
    // int offset = 0;

//   if ( !buf->set_reading_point ( offset ) )
//     {
//         std::cerr << "F. data_psa_65000_1::fill_stucture ---  impossible , ingoring the object " << std::endl;
//         return 0 ;
//     }

    int bytes = 0 ;

#define get_item(x)     memcpy(&x, buf->gg(sizeof(x)), sizeof(x));  bytes += sizeof(x);\
/*std::cout << "hit  from address " << std::hex << (int)(buf->ga(offset)) << std::dec << "----> got hit " #x " value= " << x << std::endl;*/ \
/*offset += sizeof(x);*/

//     std::cout << "\nsizeof X = " << sizeof ( X ) << std::endl;
//
//     for ( int i = 0 ; i < 30 ; i+=1 )
//     {
//         double *ptr = ( double* ) (begin_buffer+offset+i);
//         float *ptrf = ( float* ) (begin_buffer+offset+i);
//         short *ptri = ( short* ) (begin_buffer+offset+i);
//
//         std::cout << "hit  address ---> " << std::hex << ptr << std::dec  << "  "<< i  << ")  "<<  *ptr << "    \t as float= " << *ptrf<< "    \t as short= " << *ptri<< std::endl;
//     }
//     std::cout << std::endl;

    get_item ( Status );
    get_item ( ID );
    get_item ( X );
    get_item ( Y );
    get_item ( Z );
    get_item ( E );
    get_item ( T );

    get_item ( DX );
    get_item ( DY );
    get_item ( DZ );
    get_item ( DE );
    get_item ( DT );

#undef get_item
//     std::cout << "in hits, bytes= " << bytes << std::endl;
    return bytes;
}
//***************************************************************************
/*!
    \fn psa_hit::distribute_data()
 */
void psa_hit::distribute_data(int crystal_id)
{
    // it seams that we do not need to take this data -
//     std::cout << "So far empty psa_hit::distribute_data() " << std::endl;
//     std::cout << (*this)<< std::endl;
    
  psa_hit t;
  t.X = X;
  t.Y = Y;
  t.Z = Z;
 
  t.E = E;
  t.T = T;

  t.DX = DX;
  t.DY = DY;
  t.DZ = DZ;
 
  t.DE =  DE;
  t.DT =  DT;
 // t.validator = true;
  
//  if(flag_breakpoint)    display();
#ifdef  AGATA_PRESENT
aga->put_into_unpacked_event(&t, crystal_id);  // putting into   TEvent.h
#endif // def  AGATA_PRESENT
}
