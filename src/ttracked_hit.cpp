/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 *                                                                         *
 ***************************************************************************/
#include "ttracked_hit.h"

#include "tbuffer_of_data_block.h"
#include <string.h>

#include "TGalileoAnalysis.h"
// #include "Tagata.h"

#ifdef  AGATA_PRESENT
extern TGalileoAnalysis *GalileoAnalysis_ptr;

extern bool flag_breakpoint;
//*****************************************************************************
Ttracked_hit::Ttracked_hit()
{
aga =  GalileoAnalysis_ptr->give_aga_ptr();
}
//****************************************************************************
Ttracked_hit::~Ttracked_hit()
{
}
//****************************************************************************
/*!
    \fn Ttracked_hit::fill ( Tbuffer_of_data_block *buffer )
 */
int Ttracked_hit::fill ( Tbuffer_of_data_block *buf )
{
  int bytes = 0 ;

 if(flag_breakpoint) std::cout << "Ttracked_hit::fill" << std::endl;

#define get_item(x)   \
/* if(flag_breakpoint)  {std::cout << std::hex <<  (int)  buf->get_reading_point () << std::dec << "    ";}*/ \
 memcpy(&x, buf->gg(sizeof(x)), sizeof(x));  bytes += sizeof(x);\
/*  if(flag_breakpoint)   std::cout << "----> got hit " #x " value= " << x << std::endl;*/ 

  // unpacking the data here, into the object
  get_item ( Status );
  get_item ( X );
  get_item ( Y );
  get_item ( Z );
  get_item ( X1 );
  get_item ( Y1 );
  get_item ( Z1 );

  get_item ( E );
  get_item ( T );

  get_item ( DX );
  get_item ( DY );
  get_item ( DZ );
  get_item ( DX1 );
  get_item ( DY1 );
  get_item ( DZ1 );

  get_item ( DE );
  get_item ( DT );

#undef get_item

   if(flag_breakpoint) 
   {
   std::cout << "------- in hits, bytes= " << bytes << std::endl;
    display();
  }
  return bytes;
}
//**************************************************************************
/*!
    \fn Ttracked_hit::distribute_data()
 */
void Ttracked_hit::distribute_data()
{
  Tgamma_data_tracked t;
  t.X = X;
  t.Y = Y;
  t.Z = Z;
  t.X1= X1;
  t.Y1 = Y1;
  t.Z1 = Z1;
  t.E = E;
  t.T = T;

  t.DX = DX;
  t.DY = DY;
  t.DZ = DZ;
  t.DX1 =  DX1;
  t.DY1 =  DY1;
  t.DZ1 =  DZ1;
  t.DE =  DE;
  t.DT =  DT;
  t.validator = true;
  
//  if(flag_breakpoint)    display();
#ifdef  AGATA_PRESENT
aga->put_into_unpacked_event(&t);  // putting into   TEvent.h
#endif // def  AGATA_PRESENT
}
//*****************************************************************************
void Ttracked_hit::display()
{
  std::cout << "Ttracked_hit::display()  "
  << "Gamma quantum "
  << " X= " << X
  << " Y= " <<  Y
  << " Z= " << Z
  << "\t X1= " << X1
  << " Y1= " << Y1
  << " Z1= " << Z1
  << "\t E= " << E
  << " T= " << T

//   << "\t DX= " << DX
//   << " DY= " << DY
//   << " DZ= " << DZ
//   << "\t DX1= " << DX1
//   << " DY1= " << DY1
//   << " DZ1= " << DZ1
//   << "\t DE= " << DE
//   << " DT= " << DT 
  << std::endl;
}
//****************************************************************************
#endif // def  AGATA_PRESENT
