#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <string.h>


#include <data_psa_65000_1.h>
#include "tsocket_for_data.h" // for exception only
#include <tbuffer_of_data_block.h>

#include "tgeneric_data.h"
#include "tunpacked_event_data.h"

#include "tdata_tracked_65_0.h"

//********************************************************************
//   /onlineNEDA_00/xData/currentRun/needleLF_i2_0002_0028.adf
//  /onlineNEDA_01/xData/hil099/run_0011_28-02-2023_12h10m51s/needleBU_i11_0011_0011.adf
//
//         -rem neda-daq02 10000

//../needleBU_i171_0191_0000.adf

//../run_0057.dat.28-04-18_21h41m37s

//a)  -fil_diamant  ../run_0057.dat.28-04-18_21h41m37s
// -fil_dia ../run_0122.dat.02-07-23_13h03m05s
// -fil_dia ../run_0145.dat.04-07-23_07h54m06s
// -fil_dia ../rundiamant.dat  // <--- old lookup table
// -rem_dia neda-daq07 10216

//****************************************************************

// bool read_next_frame(char *buf);
//int read_next_frame ( std::ifstream &plik, char **buf );
//int test1();

/*
 *  New Keywords
 *  Merger   frame: 0xca010103
 *  Galileo  frame: 0xca010101
 *  Euclides frame: 0xca010102
 *  Ancillary using AGAVA:  0xfa0201A0
 *  ...         --------> and others are defined in DataFrame.h
 */

bool flag_talk = false; // true; // false;
bool flag_need_reconnecting = false;


//************************************************************
Tbuffer_of_data_block bufAgata;
Tunpacked_event_data event;
int unpack_frame();
bool flag_unpacking_was_successful = true; // fake
//*****************************************************************************************
bool unpacking_oliver_event_was_successful()
{
    return flag_unpacking_was_successful;  // always true?
}
//*****************************************************************************************
int open_input_file()
{
    //     int byte_since_beg = 0;
    std::cout << "Hello, world! from " << std::endl;
    //     //system("pwd");

    //     bufAgata.open_file ( "WEEK49-AFC_merger.adf" );


    exit(55); // <<<<<<<<<--------------------------------------------
    bufAgata.open_file("AFC_0000.adf");
    int length_of_buffer = bufAgata.read_in_next_block();

    std::cout << "First frame LENGTH = " << std::dec << length_of_buffer <<  std::endl;

    int offset = 0 ;

    //-------------

    //  unpack_frame ( );
    //   std::cout << " skipping this CONF frame, bec it is not interesting for us" << std::endl;

    //##################################################################
    std::cout << "Second frame ---------------------------------------------------------------------------" << std::endl;
    length_of_buffer = bufAgata.read_in_next_block();
    std::cout << "LENGTH = " << std::dec << length_of_buffer <<  std::endl;

    offset = 0 ;
}
//*************************************************************************
void unpack_one_event()
{

    if(flag_need_reconnecting)
    {
        if(bufAgata.socket_needs_to_be_reopen() )  // 1 - means succesful
        {
            flag_need_reconnecting = false;
        }
    }
    try {
        if (flag_talk)
            std::cout << "================== NEXT EVENT ===========================" << std::endl;

        //int new_offset =
        unpack_frame();
        //         event.display_data();

        if (flag_talk)
            std::cout << "===Distribute ===============" << std::endl;

        event.distribute_event_data();
    }
    catch (Tsocket_exception ex)
    {
        if(ex.what == Tsocket_exception::connection_closed)
        {
            flag_need_reconnecting = true;
        }
        else if(ex.what == Tsocket_exception::cannot_read )
        {
            std::cout << "No data comming now from the socket..." << std::endl;
        }

    }

    catch (...) {
        event.reset();
        // std::cout << " re-throw " << __func__ << ", " << __FILE__ << ", " << __LINE__ << std::endl;
        throw;
    }
    event.reset();

}
//***************************************************************************
// -rem neda-daq02 10000
//***************************************************************************
int unpack_frame()
{
    static int nesting = 0 ;
    try {
        static int frame_nr;
        frame_nr++;


        nesting++;
        //        std::string ident(nesting * 3, '>');
        //        ident += " " ;
        std::string pattern(">>> ");
        std::string ident;
        for(int i = 0 ; i < nesting ; ++i) ident += pattern;

        if (flag_talk){
            std::cout << ident << "Frame nr " << frame_nr << std::endl;

            std::cout << "Juz przeczytanych bajtow  "  <<
                         bufAgata.give_bytes_used()<< std::endl;
        }
        Tkey key(bufAgata.get_key_data());
        key.recognise();

        if (flag_talk)
            std::cout << ident << "in F. unpack_frame nesting " << nesting
                      << "   "  << std::hex << key.give_message_type() << "  "
                                 << key.give_name()
                                 << std::dec << std::endl;
        int frame_size = key.give_length();
        int how_many_bytes_read_by_this_frame = 20 ;
        // sometimes it has only the key, and nothing more
        if (frame_size > 20) {
            switch (key.recognise()) {
            case conf :
                // ignore
                // write ascii characters on the screen and done
                std::cout << ident << "unpacking Conf. Text" << (bufAgata.get_reading_point()) << "<<<END" << std::endl;
                //offset += jump;
                break;

            case composite_frame : { // this kind of block has another frames inside
                //                if (flag_breakpoint)
                //                    std::cout << ident
                //                              << "unpacking the 'composite_frame' frame with total length bytes "
                //                              << frame_size << std::endl;

                for (int i = 0 ; how_many_bytes_read_by_this_frame < frame_size ; i++) {
                    //                    if(flag_breakpoint && i > 0)
                    //                        std::cout << "Another frame in a composite sequence = " << i << std::endl;

                    int how_many = unpack_frame();    // next frame starts after the key ?
                    //                    if (flag_breakpoint)
                    //                        std::cout << ident << "this frame had bytes " << how_many << std::endl;

                    how_many_bytes_read_by_this_frame += how_many ;

                    //                    if (flag_breakpoint) {
                    //                        std::cout << ident << "Length of the current frame " << frame_size
                    //                                  << ", so far nested frames used "
                    //                                  << how_many_bytes_read_by_this_frame << std::endl;}
                }

                int skip = frame_size - how_many_bytes_read_by_this_frame ;
                if (skip) {
                    printf("Jumping some bytes ...( %i)\n",skip);
                    bufAgata.jump_bytes_read(skip);
                }
            }

                break;

            case event_data_psa:
            {
                if (flag_talk)
                    std::cout << ident
                              << "unpacking the 'event_data_psa' frame with total length bytes "
                              << frame_size << std::endl;
                for (int i = 0 ;  frame_size > how_many_bytes_read_by_this_frame ; i++) {
                    int ile = unpack_frame();    // nest frame starts after the key
                    how_many_bytes_read_by_this_frame += ile ;
                }

                int skip = frame_size - how_many_bytes_read_by_this_frame ;
                if (skip) {
                    printf("Jumping some bytes ...( %i)\n",skip);
                    bufAgata.jump_bytes_read(skip);
                }
                break;
            }


            case data_neda:
            {
                if (flag_talk) std::cout << ident
                                         << "unpacking the 'data_neda' frame with total length bytes "
                                         << frame_size << std::endl;

                data_psa_65000_1 *ptr  = new data_psa_65000_1;
                how_many_bytes_read_by_this_frame += ptr->fill_neda_structure(&bufAgata,&key);
                event.add_the_contributor(ptr);

                // jump - offsetB
                //offset += jump;
                int skip = frame_size - how_many_bytes_read_by_this_frame;
                if (skip > 0) {
                    printf("Jumping some bytes ...( %i)\n",skip);
                    bufAgata.jump_bytes_read(skip);
                }
                how_many_bytes_read_by_this_frame += skip;
                break;
            }
            case data_eagle:
            {
                if (flag_talk) std::cout << ident
                                         << "unpacking the 'data_eagle' frame with total length bytes "
                                         << frame_size << std::endl;

                data_psa_65000_1 *ptr  = new data_psa_65000_1;
                how_many_bytes_read_by_this_frame += ptr->fill_eagle_structure(&bufAgata,&key);
                event.add_the_contributor(ptr);

                // jump - offsetB
                //offset += jump;
                int skip = frame_size - how_many_bytes_read_by_this_frame;
                if (skip > 0) {
                    printf("Jumping some bytes ...( %i)\n",skip);
                    bufAgata.jump_bytes_read(skip);
                }
                how_many_bytes_read_by_this_frame += skip;
                break;
            }
            case data_dssd:
            {
                if (flag_talk) std::cout << ident
                                         << "unpacking the 'data_dssd' frame with total length bytes "
                                         << frame_size << std::endl;

                data_psa_65000_1 *ptr  = new data_psa_65000_1;
                how_many_bytes_read_by_this_frame += ptr->fill_dssd_structure(&bufAgata,&key);
                event.add_the_contributor(ptr);

                // jump - offsetB
                //offset += jump;
                int skip = frame_size - how_many_bytes_read_by_this_frame;
                if (skip > 0) {
                    printf("Jumping some bytes ...( %i)\n",skip);
                    bufAgata.jump_bytes_read(skip);
                }
                how_many_bytes_read_by_this_frame += skip;
                break;
            }
            default:
                std::cout << ""
""
""
"Unrecognised frame nr   "
                          << frame_nr  << "  -  "
                          << std::hex
                          << key.give_message_type()
                          << std::dec << std::endl;


                // 		std::cout << " Nearest bytes are   " << std::endl;
                bufAgata.info();

                // searching the  proper agata word  0xfa000000
                int skip = 1;
                bufAgata.jump_bytes_read(skip);
                how_many_bytes_read_by_this_frame += skip;
                break;

            } // end of switch
        } // end of if  > 20


        nesting--;
        return how_many_bytes_read_by_this_frame ;

    } // end of try
    catch (Tsocket_exception ex)
    {
        nesting = 0 ;
        if(ex.what == Tsocket_exception::connection_closed)
        {
            flag_need_reconnecting = true;
        }
    }
    catch (Texception_input_file ex)
    {

        //        cout << "typ error = " << ex.error_nr << endl;
//        if(ex.error_nr == -1) cout << "End of Fle" << endl;
//        if(ex.error_nr == -2) cout << "Fail" << endl;
        throw;
    }

    catch (...) {
        std::cout << " re-throw " << __func__ << ", " << __FILE__ << ", " << __LINE__ << std::endl;
        throw ;
    }

    return 0;   // JG
}







