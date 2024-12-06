#include "tdata_ranc1.h"


#include "tbuffer_of_data_block.h"
#include "string.h"
#include "TGalileoAnalysis.h"
#include "TNeedle.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Tdata_ranc1::Tdata_ranc1()
        : Tgeneric_data()
{
    frs_ptr =  GalileoAnalysis_ptr->give_gal_ptr();
}
//******************************************************************************
Tdata_ranc1::~Tdata_ranc1()
{
    array.clear();
}
//******************************************************************************
/*!
    \fn Tdata_ranc1::fill ( Tbuffer_of_data_block *buffer )
 */
int Tdata_ranc1::fill ( Tbuffer_of_data_block *buf, unsigned int timestamp )
{

  std::cout << "Tdata_ranc1::fill ( Tbuffer_of_data_block *buf )" << std::endl;


    int this_frame_length = 0;

#define get_item(x)     memcpy(&x, buf->gg(sizeof(x)), sizeof(x)); \
    this_frame_length+= sizeof(x) ; \
/*std::cout  << " " #x << ", \t\t " << std::dec << x << std::endl;*/

//     {
//         std::cerr << "F. data_psa_65000_1::fill_stucture ---  impossible , ingoring the object " << std::endl;
//         return 0 ;
//     }

    unsigned  int how_many_channels ;  // Oliver says it is long

    get_item ( how_many_channels );

    float a;   // oliver says it is float

//      std::cout << "\nsizeof a = " << sizeof(a)<< std::endl;

    int starting_channel = 3 * 32;
    int starting_channel_of_time_3  = 4 * 32;

    for ( int i = 0 ; i < how_many_channels ; i++ )
    {
        get_item ( a );
        array.push_back ( a );

        //  if (i >= starting_channel && i <  (starting_channel + 10) )
//       {
//         std::cout << " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
//         std::cout << "ancillary int nr " << i <<  " got value " << a << std::endl;
//       }



//        if (i >= starting_channel && i <  (starting_channel + 2*32)    &&  a < 4086)
//         {
//             if (i >= starting_channel_of_time_3 )
// 					std::cout << " TIME -------------------" ; //  << std::endl;
//             std::cout << "ancillary int nr " << i <<  " got value " << a << std::endl;
//         }
        
        if(i >= 120 && i < 120 + HOW_MANY_DANTE_PARAMETERS)
		  {
			parameter[i-120] = a; 
		  }
    }


    X1 = array[starting_channel++];
    Y1 = array[starting_channel++];
    X2 = array[starting_channel++];
    Y2 = array[starting_channel++];
    X3 = array[starting_channel++];
    Y3 = array[starting_channel++];


    charge1 = array[starting_channel++];
    charge2 = array[starting_channel++];
    charge3 = array[starting_channel++];




    timeA = array[starting_channel_of_time_3++];
    timeB = array[starting_channel_of_time_3++];
    timeC = array[starting_channel_of_time_3++];
    gammaOR = array[starting_channel_of_time_3++];
    dante_timestamp = timestamp;
parameter155 = array[155];

// std::cout << "parameter155 = " << parameter155 << std::endl;
//     display();
//   std::cout << "Tdata_ranc1::fill just read " <<  this_frame_length << "bytes" << std::endl;

    return this_frame_length;
}
//********************************************************************************************
/*!
    \fn Tdata_ranc1::distribute_data()
 */
void Tdata_ranc1::distribute_data()
{

//   std::cout << "Tdata_ranc1::distribute_data() " << std::endl;

//   std::cout
//   << "Dante   "
//   <<   " X1 = " << X1
//   <<   " Y1 = " << Y1
//
//   <<   "         X2 = " << X2
//   <<   " Y2 = " << Y2
//
//   <<   "       X3 = " << X3
//   <<   " Y3 = " << Y3
//
//   << "          charge1 = "  << charge1
//   << " charge2 = "  << charge2
//   << " charge3 = "  << charge3
//   << std::endl;



//   frs_ptr ->give_dante_ptr()->data_arrived(X1, Y1, X2, Y2, X3, Y3, charge1, charge2, charge3);
#ifdef  DANTE_PRESENT
    event_ptr->dante[0].x = X1;
    event_ptr->dante[0].y = Y1;
    event_ptr->dante[0].charge = charge1;
    event_ptr->dante[0].time = timeA;


    event_ptr->dante[1].x = X2;
    event_ptr->dante[1].y = Y2;
    event_ptr->dante[1].charge = charge2;
    event_ptr->dante[1].time = timeB;

    event_ptr->dante[2].x = X3;
    event_ptr->dante[2].y = Y3;
    event_ptr->dante[2].charge = charge3;
    event_ptr->dante[2].time = timeC;

    event_ptr->dante_gammaOR = gammaOR;
    event_ptr->dante_timestamp = dante_timestamp; 
    event_ptr->dante_fired = true;
   event_ptr->dante_parameter155 = parameter155;;
 
for(int i =  0 ; i < HOW_MANY_DANTE_PARAMETERS ; i++)	
{
  event_ptr->dante_parameter[i] = parameter[i];
}


// FAKE

//    event_ptr->dante[0].x = 1;
//   event_ptr->dante[0].y = 2;
//   event_ptr->dante[0].charge = 10;
//
//   event_ptr->dante[1].x = 3;
//   event_ptr->dante[1].y = 4;
//   event_ptr->dante[1].charge = 20;
//
//   event_ptr->dante[2].x = 5;
//   event_ptr->dante[2].y = 6;
//   event_ptr->dante[2].charge = 30;
#endif   // def  DANTE_PRESENT

//   std::cout << "DIstributed dante " << std::endl;
//   display();
    array.clear();

}
//*****************************************************************
void Tdata_ranc1::display()
{

    std::cout
    << "Dante   "
    <<   " X1 = " << X1
    <<   ",  Y1 = " << Y1

    <<   "         X2 = " << X2
    <<   ", Y2 = " << Y2

    <<   "       X3 = " << X3
    <<   ", Y3 = " << Y3

    << "          charge1 = "  << charge1
    << " charge2 = "  << charge2
    << " charge3 = "  << charge3

    << "          timeA = "  << timeA
    << " timeB = "  << timeB
    << " timeC = "  << timeC
    << std::endl;

}
