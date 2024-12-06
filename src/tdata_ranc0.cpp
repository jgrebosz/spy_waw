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
#include "Tdata_ranc0_oryginal.h"
#include "tbuffer_of_data_block.h"
//#include "string.h"
#include <iostream>


#include "TGalileoEvent.h"
//////////////////////////////////////////////////////////////////////////////////
Tdata_ranc0_oryginal::Tdata_ranc0_oryginal()
{
   // sub_data_vector.resize ( 166 );
}
//********************************************************************************
Tdata_ranc0_oryginal::~Tdata_ranc0_oryginal()
{
}
//********************************************************************************
int  Tdata_ranc0_oryginal::fill ( Tbuffer_of_data_block *buf, int nr_of_data_bytes )
{

    std::cout << "F. Tdata_ranc0_oryginal::" << __func__ << std::endl;

    //buf->gg(sizeof(x));
    int nr_of_data_words = nr_of_data_bytes / sizeof(int32_t);

    int32_t * ptr = ( int32_t* ) buf->get_reading_point ();

    std::cout << "After key for Neutron Wall, nr_of_data_words = " << nr_of_data_words  << std::endl;

    for ( int i = 0 ;i < nr_of_data_words   ;i++, ptr++ )
    {
        std::cout << i << "] adr = "  << std::hex << ( long ) ptr
             << "    val " << std::hex << " 0x" << std::hex << (int) *(ptr) << "   "
             << std::dec
             //<< * ptr << "    "
                // << std::dec << * ptr
             //<< std::endl;

        // std::cout
             << "    Energy = " <<  (*ptr  >> 16)
             << ", Channel = " << ( (*ptr)  & 0xffff )   //<< std::endl;
       // std::cout

             << std::endl;
      //  if((*ptr  >> 16) == 0) break;

    }


    int this_frame_length = 0;

    std::pair<int, int> d;
    ptr = ( int32_t* ) buf->get_reading_point ();
    for ( int i = 0 ;i < nr_of_data_words   ;i++, ptr++ )
    {
          d.first = (*ptr)  & 0xffff  ; // channel
          d.second =  (*ptr  >> 16); //  data
          ++this_frame_length;
          sub_data_vector.push_back(d);
      }
    std::cout << "FAKE !!!! Tdata_ranc0_oryginal::fill just read " <<  this_frame_length << " Words" << std::endl;

    return this_frame_length *= sizeof(int32_t) ;
}
//*****************************************************************************
void Tdata_ranc0_oryginal::distribute_data()
{
    std::cout << "so far empty F. Tdata_ranc0_oryginal::distribute_data() " << std::endl;


    for(int i = 0 ; sub_data_vector.size() ; ++i)
    {
        // find in the lookup table
     sub_data_vector[i].first; // channel;
     sub_data_vector[i].second; // data

    }

//############################
    // at first we distribute the core common data
    //     std::cout << "data_psa_65000_1 ---> " ;
    //
    event_ptr->neutron_wall_fired.clear();

#if 0
    // detector_id :   0-63  Germanium 64 -----
    float first_time = sub_data_vector[0].trTime ;
    try {
        int place_in_array = -INT_MAX;
        TString detector_type = "";
        if(lookup.find(detector_id) != lookup.end()){
            place_in_array = lookup[detector_id].first;
            TString det = lookup[detector_id].second;
            TObjArray* toks=det.Tokenize("_");
            detector_type = ((TObjString*)toks->At(0))->GetString();
        }
        //    std::cout << "Detector "<< detector_type << " Place in array " << place_in_array
        //	      << " and detector id " << detector_id << std::endl;
        if (place_in_array>=0 && detector_type.Length()>0){
            if(detector_type.CompareTo("germanium")==0) {
                //std::cout << "Germ det nr " << place_in_array << " fired " << std::endl;
                int siz = sub_data_vector.size();
                for (int i = 0 ; i < fmin(siz , max_subdata) ; i++) {
                    // UNPACKING INTO    EVENT.H
                    // this below is a base class pointer to the output event
                    event_ptr->eagle_fired[place_in_array]=true;
                    //std::cout << "Germanium "<< place_in_array << "eAmpli " << sub_data_vector[i].eAmpli  << std::endl;;
                    event_ptr->germanium_data[place_in_array][i].eAmpli = sub_data_vector[i].eAmpli;
                    event_ptr->germanium_data[place_in_array][i].eEnergy = sub_data_vector[i].eEnergy ;

                    event_ptr->germanium_data[place_in_array][i].wBase = sub_data_vector[i].wBase;
                    event_ptr->germanium_data[place_in_array][i].wAmpli = sub_data_vector[i].wAmpli ;

                    event_ptr->germanium_data[place_in_array][i].wEnergy = sub_data_vector[i].wEnergy;
                    event_ptr->germanium_data[place_in_array][i].trTime = sub_data_vector[i].trTime ;

                } // end for
            }  // end if detector type
            else if (detector_type.CompareTo("euclides")==0){
                //std::cout << "Silicon det nr " << place_in_array << " fired " << std::endl;
                //std::cout << "" << std::endl;
                int siz = sub_data_vector.size();
                for (int i = 0 ; i < fmin(siz , max_subdata) ; i++) {
                    // UNPACKING INTO    EVENT.H
                    // this below is a base class pointer to the output event
                    event_ptr->euclides_fired[place_in_array]=true;

                    event_ptr->euclides_data[place_in_array][i].eAmpli = sub_data_vector[i].eAmpli;
                    event_ptr->euclides_data[place_in_array][i].eEnergy = sub_data_vector[i].eEnergy ;

                    event_ptr->euclides_data[place_in_array][i].wBase = sub_data_vector[i].wBase;
                    event_ptr->euclides_data[place_in_array][i].wAmpli = sub_data_vector[i].wAmpli ;

                    event_ptr->euclides_data[place_in_array][i].wEnergy = sub_data_vector[i].wEnergy;
                    event_ptr->euclides_data[place_in_array][i].trTime = sub_data_vector[i].trTime ;
                } // end for
            } // end if detector type
        } // end if place in array
    } // end try
    catch (std::out_of_range) {
        std::cout << "data_psa_65000_1::distribute_data()      Alarm, detector_id = " << detector_id << std::endl;
    }
#endif // 0
    sub_data_vector.clear();


    //#############################



    sub_data_vector.clear();

}
