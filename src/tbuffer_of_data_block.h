/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 ***************************************************************************/
#ifndef TBUFFER_OF_DATA_BLOCK_H
#define TBUFFER_OF_DATA_BLOCK_H
/**
    @author Jerzy Grebosz <grebosz>
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

 enum error_nr_enum  { myEOF = -1,  myFAIL = -2 };
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class Texception_input_file
{
public:
    error_nr_enum     error_nr;    // -1 EOF, -2 FAIL
    Texception_input_file(error_nr_enum er) : error_nr(er)
    {}
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
class Tbuffer_of_data_block
{
    std::string current_file;
public:
    Tbuffer_of_data_block();

    ~Tbuffer_of_data_block();
    int  read_in_next_block();
    int  read_in_next_block_from_socket();
    int  read_in_next_block_from_file();
    bool open_file ( std::string name );
     int socket_needs_to_be_reopen();

    std::vector<std::string> list_of_names;
    bool flag_list;
    ulong nr_of_file_already_analysed;
    bool open_next_file_from_the_list();

//        char * inspect_address ( int byte );
    char * inspect_address ( );

//         char * ga ( int byte, int size )   // to have it shorter
//         {
//             byte_read+= size;
//             if ( byte_read > length_of_data_block )
//             {
//                 std::cout << " F. ga ( int byte, int size ) --- byte_read > length_of_data_block  !!!!!!!!"<< std::endl;
//             }
//             return inspect_address ( byte );
//         }

    //---------------------------------------------------------------------------
    char * gg ( int size )  // to have it shorter   (get new block of data)
    {
        if ( byte_read >= length_of_data_block )
        {
            // std::cout << " F. ga ( int byte, int size ) --- byte_read > length_of_data_block  !!!!!!!!"<< std::endl;
            read_in_next_block();
        }
        char *ptr = &buffer[ byte_read ];
        byte_read+= size;
        return ptr;
    }
    //-------------------------
    bool set_reading_point ( int offs )
    {
        if ( offs+byte_read >= length_of_data_block )
        {
            offs -=  (length_of_data_block - byte_read);
            std::cout << " F. ga ( int byte, int size ) --- byte_read > length_of_data_block  !!!!!!!!"<< std::endl;
            read_in_next_block();
        }
        byte_read = offs;
        return true;
    }
    //--------------------------------------------------------
    char * get_reading_point ()
    {
//             return &buffer[byte_read];
        if ( byte_read >= length_of_data_block )
        {
            std::cout << " F. ga ( int byte, int size ) --- byte_read > length_of_data_block  !!!!!!!!"<< std::endl;
            read_in_next_block();
        }
        return   &buffer[ byte_read ];
    }
    //----------------------------------------------------------
    char * get_key_data ()
    {
//             return &buffer[byte_read];

        top:
        if ( byte_read >= (length_of_data_block-20)  || ( byte_read < 0) )
        {
//            std::cout << " F. get_key_data () --- byte_read > length_of_data_block  ( "
//                      << byte_read << ", >  len= " << length_of_data_block -20 <<
//                         "      !!!!!!!!"<< std::endl;
            read_in_next_block();
        }
        char *ptr = &buffer[ byte_read ];
        byte_read += 20; // size of the key
        // std::cout << " byte_read = " << std::dec << byte_read << " out of " << length_of_data_block << std::endl ;


        if(byte_read >= length_of_data_block )
        {
         std::cout << "F. get_key_data ():   Alarm - nonsense memory begins here - so we read nex block"   << std::endl;
         goto top;
        }
        return  ptr;
    }
    //******************************************
    int jump_bytes_read ( int how_many )
    {
        if ( byte_read + how_many >= length_of_data_block )
        {
            how_many -= ( length_of_data_block - byte_read );
            read_in_next_block();
        }
        else byte_read += how_many;

        return byte_read;
    }
    //*******************************************
    int give_bytes_used()
    {
        return byte_read;
    }
    //*******************************************
    void jump_over_header()
    {
        // a header is 20 bytes
        jump_bytes_read ( 20 );   // sizeof(Tkey)
    }
    //*******************************************
    void info();
    //*******************************************
    std::string give_current_filename()
    {
        return current_file ;
    }
    //*******************************************
protected:
    char * buffer;
    int length_of_data_block;   // in bytes

    int byte_read;
    int block_number;
    long long file_size ;
//         bool flag_all_read;
public:
     std::ifstream plik;
     std::ifstream * give_file_ptr() { return &plik;}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
