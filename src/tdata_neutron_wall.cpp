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
#include "tdata_neutron_wall.h"
#include "tbuffer_of_data_block.h"
//#include "string.h"
#include <iostream>
#include <sstream>

#include "TGalileoEvent.h"
#include "TGalileoAnalysis.h"
extern TGalileoAnalysis *GalileoAnalysis_ptr;

// static
bool Tdata_neutron_wall::flag_lookup_loaded;
std::map<int,TNeutronWallDetector*> Tdata_neutron_wall::lookup_nw;
std::map<int,TTACDetector*> Tdata_neutron_wall::lookup_tac;
//////////////////////////////////////////////////////////////////////////////////
Tdata_neutron_wall::Tdata_neutron_wall()
{
//    if(event_ptr->neutronWall_fired.size()>0)
        event_ptr->neutronWall_fired.clear();
//    if(event_ptr->tac_fired.size()>0)
        event_ptr->tac_fired.clear();
    // sub_data_vector.resize ( 166 );
    if(!flag_lookup_loaded){
        galileo_pointer = GalileoAnalysis_ptr->give_gal_ptr()  ;
        lookup_nw = galileo_pointer->Get_NeutronWall_LookUpTable();
        lookup_tac = galileo_pointer->Get_TAC_LookUpTable();
        flag_lookup_loaded = true;
    }
}
//********************************************************************************
Tdata_neutron_wall::~Tdata_neutron_wall()
{
}
//********************************************************************************
int  Tdata_neutron_wall::fill ( Tbuffer_of_data_block *buf, unsigned long long ts, int nr_of_data_bytes )
{


//    std::cout << "F. Tdata_neutron_wall::" << __func__ << std::endl;

    //buf->gg(sizeof(x));
    int nr_of_data_words = nr_of_data_bytes / sizeof(int32_t);
    ts_value = ts;
    int32_t * ptr = ( int32_t* ) buf->get_reading_point ();
    // True unpacking-------------------------------------------

    int this_frame_length = 0;

    std::pair<int, int> d;
    ptr = ( int32_t* ) buf->get_reading_point ();
    for ( int i = 0 ;i < nr_of_data_words   ;i++, ptr++ )
    {
        d.first = (*ptr)  & 0xffff  ; // channel
        d.second =  (*ptr  >> 16); //  data
//        std::cout << d.first << "  " << d.second << std::endl;
        ++this_frame_length;
        sub_data_vector.push_back(d);
    }
    //std::cout << "Tdata_neutron_wall::fill just read " <<  this_frame_length << " Words" << std::endl;

    return this_frame_length *= sizeof(int32_t) ;
}
//*****************************************************************************
void Tdata_neutron_wall::distribute_data()
{
  //  std::cout << "sTdata_neutron_wall::distribute_data() " <<sub_data_vector.size()  << std::endl;
    event_ptr->neutronWall_fired.clear();


    std::map<int,TNeutronWallDetector*>::iterator it_nw = lookup_nw.begin();
    std::map<int,TTACDetector*>::iterator it_tac = lookup_tac.begin();
    for(int i = 0 ; i< sub_data_vector.size() ; ++i){
      //        std::cout << i << "  " << __FILE__ <<"  " << __LINE__ 
      //		  << " channel " << sub_data_vector[i].first << std::endl;
        // find in the lookup table
        bool nw_det_present=false;
        bool nw_tac_present=false;
        int chan_sig = sub_data_vector[i].first;
        int nw_det_nr = -10;
        int nw_det_signal =-10;
        if(chan_sig<176){
            for(it_nw=lookup_nw.begin();it_nw!=lookup_nw.end();++it_nw){
                int sig = it_nw->second->FindChannel(chan_sig);
                if(sig>=0){
                    nw_det_nr=it_nw->first;
                    nw_det_signal = sig;
                    nw_det_present=true;
                    break;
                }
            }
        }else{
            for(it_tac=lookup_tac.begin();it_tac!=lookup_tac.end();++it_tac){
                if(it_tac->second->GetChannel()==chan_sig){
                    nw_tac_present=true;
                    nw_det_nr=it_tac->first;
                    nw_det_signal=3;
                    break;
                }

            }
        }
        int data  = sub_data_vector[i].second; // data
//            std::cout    << "signal " << chan_sig
//                        << " --> " << nw_det_nr << ", "
//                         << nw_det_signal
//                         << " data = " << data << std::endl;


        if(nw_det_signal < 3 && nw_det_present==true){
       //     std::cout << "Neutron Wall data" << std::endl;
            event_ptr->neutronWall_fired[nw_det_nr] = true;
            event_ptr->neutron_wall_data[nw_det_nr].timestamp = ts_value;
            if(nw_det_signal == 0)
                event_ptr->neutron_wall_data[nw_det_nr].tof = data;
            else if(nw_det_signal == 1)
                event_ptr->neutron_wall_data[nw_det_nr].zco = data;
            else if(nw_det_signal == 2)
                event_ptr->neutron_wall_data[nw_det_nr].qvc = data;
            else{
                std::cout << "Alarm: signal nr for NW det is " << nw_det_signal << std::endl;
            }

        } else if (nw_det_signal ==3 && nw_tac_present==true){

          //  std::cout << "TAC data" << std::endl;
            event_ptr->tac_fired[nw_det_nr] = true;
            event_ptr->neutron_wall_tac[nw_det_nr].tac = data ;
        }
        else {
            std::cout << "Tdata_neutron_wall::distribute_data() too big detector_id = " << chan_sig << std::endl;
        }
    }
  //  std::cout << event_ptr->neutronWall_fired.size() << "  " << event_ptr->tac_fired.size() << std::endl;
    sub_data_vector.clear();
}
//**********************************************************************************
