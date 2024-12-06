/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 ***************************************************************************/
#include "tbuffer_of_data_block.h"
#include <string.h>
#include <filesystem>
#include "TGalileoAnalysis.h"
extern TGalileoAnalysis *GalileoAnalysis_ptr;
//**************************************************************************
Tbuffer_of_data_block::Tbuffer_of_data_block()
{
    buffer = 0 ;
    //     flag_all_read = false;
    block_number = 0 ;
    nr_of_file_already_analysed = 0 ;
    file_size = 1;
}
//**************************************************************************
Tbuffer_of_data_block::~Tbuffer_of_data_block()
{
    if ( buffer ) delete [] buffer;
}
//**************************************************************************
/*!
    \fn Tbuffer_of_data_block::read_in_next_block()
 */
int  Tbuffer_of_data_block::read_in_next_block()
{   
    if(GalileoAnalysis_ptr->flag_sorting_from_file)
        return read_in_next_block_from_file();
    else
        return read_in_next_block_from_socket();

}
//***************************************************************************
int Tbuffer_of_data_block::read_in_next_block_from_socket()
{
   // std::cout << "read_in_next_block_from_socket   nr " << block_number << std::endl;




    block_number++;
    if ( buffer ) {
        delete  []  buffer;
        buffer = nullptr;
    }
    
    // true reading
    TGo4EventSourceParameter* src = GalileoAnalysis_ptr->give_source_of_events() ;

    int length_of_block_in_bytes = length_of_data_block = src->give_next_buffer();

    if(length_of_data_block <= 1)
    {   byte_read = 0 ;
        return   length_of_data_block;
    }
    int32_t * block = nullptr;
    block = (int32_t *) src->give_buff_addr();

    //std::cout << "addr of block = 0x"    << std::hex << reinterpret_cast<unsigned long> (block) << std::dec << std::endl;


    //int * wsk = (int*) src->give_buff_addr();
    //std::cout << "koniec2  read into address "
    //     << std::hex
    //     <<  (unsigned long) wsk
    //      << std::dec << std::endl ;

    //for(int i = 0 ; i < 12 ; ++i)
    //{
    //    std::cout << "[" << i << "]   0x"<< std::hex << wsk[i] << std::dec << std::endl;
    //}

    // reserving the bufer of the current length as a place for the following data


    //std::cout << "buffer address = " << std::hex << ( int ) ( buffer ) << std::dec << std::endl;

    //memset ( buffer, 0x0, sizeof ( char ) *  length_of_data_block );

    // memcpy ( buffer, &length_of_data_block, sizeof ( length_of_data_block ) );
    //    memcpy ( buffer+4, block,    length_of_data_block-4 );

    // removing some zeros which are in front
    uint32_t * bl = (uint32_t*) block;
    int offset_bytes = 0 ;

    for(int i = 0 ; bl[i] == 0 ; ++i)
    {
        offset_bytes+= 4;
    }
    length_of_block_in_bytes-= offset_bytes;

    buffer = new char[length_of_block_in_bytes+50];
    memcpy ( buffer, block + (offset_bytes/4),    length_of_block_in_bytes);

#ifdef NIGDY
    uint32_t * bu = (uint32_t*) buffer;

    for ( int i = 0  ; i < 15/*length_of_data_block*/ ; i++ )
    {
        std::cout << "#[" << i << "]  buffer 0x"
                  << std::hex << bu[i]
                     << ",  \tsource 0x"
                     <<  bl[i] << "    " << std::dec <<  bl[i]
                         << std::endl   ;
        //    if ( ! ( ( i+1 ) % 10 ) ) std::cout << std::endl/
    }
#endif
    byte_read = 0 ;
    return   length_of_data_block;

}
//***************************************************************************
int  Tbuffer_of_data_block::read_in_next_block_from_file()
{
    //std::cout << "Tbuffer_of_data_block::read_in_next_block_from_file()"  << std::endl;

    if (buffer != 0 && length_of_data_block >0)
    {
        // zerowanie bufora po poprzednim bloku
        memset ( buffer, 0x0, sizeof ( char ) *  length_of_data_block );
    }
top:
    length_of_data_block = 0;
    int when_to_stop = 999999999; //2000 ; //9999999 ;
    long long where = 0 ;
    do
    {
        // Reading only the length value
        where = plik.tellg();
        plik.read ( ( char* ) &length_of_data_block,   sizeof ( int ) );
        //        std::cout << "next block length will be = " << length_of_data_block
        //                  << "   hex = " << std::hex << length_of_data_block << std::dec
        //                  << std::endl;


        if (plik.eof() || block_number > when_to_stop)
        {
            std::cout << "1 End of event file  while reading the block nr " << block_number << ", byte nr "
                         //<< where
                      << std::endl;
            std::cout << "1 Read byte nr " << where << " what is " << (100.0 *where)/file_size  << "%  of " << file_size<< std::endl;
            open_next_file_from_the_list();
            goto top;
        }
        if ( plik.fail() )
        {
            std::cout << "Error with the file reading " << std::endl;
            {
                Texception_input_file x(myFAIL);
                throw x;
            }
        } // end if
    } while (length_of_data_block <= 0   || length_of_data_block > 8000);
    //  the while loop avove is for situation of the lost synchronistaion of blocks
    // in the begining should be a length of the block, which is not a huge number. If we find
    // some 'realistic' number - we read the block, and then the other routines will search the
    // tokens of the block headers
    
    //     if (length_of_data_block > 4000)
    //     {
    //         std::cout << "nONSENSE value of length_of_data_block = " << length_of_data_block << std::endl;
    //         length_of_data_block = 40;
    //     }

    where = plik.tellg();

    //     if (((double)where)/file_size  > 0.30)
    //     {
    //         std::cout << "E Read byte nr " << where << " what is " << (100.0 *where)/file_size  << "%  of " << file_size<< std::endl;
    //     }


    block_number++;

//    std::cout << "Reading next frame, length_of_data_block = " << std::dec << length_of_data_block
//              <<  ", hex= " << std::hex << ( length_of_data_block ) << std::dec
//               << std::endl;

    //     length_of_data_block = 1000;   //FAKE !!!!!!!!!!!!!!!!!!

    if ( buffer )
    {
        delete  []  buffer;
    }

    // reserving the bufer of the current length as a place for the following data
    buffer = new char[length_of_data_block];
    //   std::cout << "buffer address = " << std::hex << ( int ) ( buffer ) << std::dec << std::endl;

    memset ( buffer, 0x0, sizeof ( char ) *  length_of_data_block );
#ifdef NIGDY
    for ( int i = 0  ; i < length_of_data_block ; i++ )
    {
        std::cout << std::dec<< i << ")   --> " << std::hex << ( unsigned int ) buffer[i] << "<--,    " ;
        if ( ! ( ( i+1 ) % 10 ) ) std::cout << std::endl;
    }
    std::cout << std::dec ;
#endif

    memcpy ( buffer, &length_of_data_block, sizeof ( length_of_data_block ) );

    /*    std::cout << "After copying in " << std::endl;*/

    //         for ( int i = 0  ; i< length_of_data_block  ; i++ )
    //         {
    //             std::cout << std::dec << i << ")   --> " << std::hex << ( unsigned short int ) buffer[i] << "<--,    " ;
    //             if(!((i+1) % 10)) std::cout << std::endl;
    //         }
    //     std::cout << "Cont reading " << std::endl;
    where = plik.tellg();
    plik.read ( buffer+4, length_of_data_block-4 );



    if (plik.eof() || block_number > when_to_stop)
    {
        if (block_number > when_to_stop)
        {
            std::cout << "Stopping after desired numbers of blocks " << block_number << std::endl;
        }

        std::cout << "After reading the consents of block - 2 End of event file  while reading the block nr " << block_number << ", byte nr " << where
                  << ", when_to_stop = " << when_to_stop
                  << ", length_of_data_block = " << length_of_data_block
                  << std::endl;

        std::cout << "2 Read byte nr " << where << " what is " << (100.0 *where)/file_size  << "%  of " << file_size<< std::endl;

        open_next_file_from_the_list();
    }




    if ( !plik )
    {
        std::cout << "Tbuffer_of_data_block::read_in_next_block()  ---> Error durig reading " << std::endl;
        Texception_input_file x(myFAIL);
        throw x  ;   // return -1;
    }
    //     std::cout << "Start buf =" << std::hex<< int ( buffer )
    //     << "End buf =" << std::hex<< int ( &buffer[length_of_data_block-1] )
    //     << std::endl;

    //    std::cout << "after reading the whole block " << std::endl;


    //    for ( int i = 0  ; i < 16 /*length_of_data_block*/ ; ++i /*i+=4*/ )
    //    {
    //        unsigned int *ptr = ( unsigned int * ) buffer;
    //        std::cout << "<" << i << ">L    0x"
    //             << std::hex << ( long)  ptr
    //             << " value = 0x" << ptr[i] << ",    "
    //             << std::dec  << ptr[i] << std::endl;

    //        //             if ( ! ( ( i+1 ) % 10 ) ) std::cout << std::endl;
    //    }

    //
    //     std::cout << "& buffer = " << std::hex << ( int ) buffer << std::dec << std::endl;

    byte_read = 0 ;
    return   length_of_data_block;
}
//***************************************************************************
/*!
    \fn Tbuffer_of_data_block::open_file(std::string name)
 */
bool Tbuffer_of_data_block::open_file ( std::string filename )
{

    // if '@' is in front, or the extension is *.txt
    if (filename[0] == '@' || (filename.substr(filename.size() - 4, 4) == ".txt"))   // if this is a list of files to analyse
    {
        flag_list = true;
        std::cout << "This file should contain a list of data files. " << std::endl;

        std::string name_without_at = filename; // for extension txt
        
        if(filename[0] == '@')  name_without_at = filename.substr(1); // for @ - removing it
        
        std::ifstream plik(name_without_at.c_str() );
        if (!plik)
        {
            std::cout << "Can't open the file " << name_without_at << " (which should contain a list of files to analyse) " << std::endl;
            exit(10);
            //return false;
        }
        std::cout << "File " << name_without_at << "<--- with the list of files "<< std::endl;
        // Readint the contents of the file with the list and creating a vector with these names.
        std::string single_name;
        for ( ; plik.good() ; )
        {
            plik >> single_name ;
            std::cout << "file on the list is: " << single_name << std::endl;

            if (!plik) break;
            list_of_names.push_back(single_name);
        }
        filename = list_of_names[0];
    }
    else  // if this is a name of the single file to analyse
    {
        flag_list = false;
        list_of_names.push_back(filename);  // put this name on the list
    }

    //  plik.close();

    if(std::filesystem::exists(filename.c_str()) == false)
    {
        std::cerr << "The file " << filename << " DOES NOT EXISTS"
        << std::endl;

        Texception_input_file x(myFAIL);
        throw x;

    }

    plik.open ( filename.c_str(), std::ios::binary );

    if ( !plik )
    {
        std::cerr << "---> Can't open file " << filename << std::endl;
    }

    plik.seekg(0, std::ios::end);
    file_size = plik.tellg();
    plik.seekg(0, std::ios::beg);
    long pocz = plik.tellg();
    file_size -= pocz;

    std::cout << "File " << filename  << " successfully open for analysis "<< std::endl;
    current_file = filename;
    return 0;
}
//***************************************************************
int Tbuffer_of_data_block::socket_needs_to_be_reopen(){
    TGo4EventSourceParameter* src = GalileoAnalysis_ptr->give_source_of_events() ;
    return src->reopen_socket();  // 1 means was succesful
}
//***************************************************************
bool Tbuffer_of_data_block::open_next_file_from_the_list()
{
    if (!flag_list)
    {
        std::cout << "The whole list file is analysed" << std::endl;
        Texception_input_file x( myEOF);
        throw x;
    }
    nr_of_file_already_analysed++;
    if (nr_of_file_already_analysed >= list_of_names.size())
    {
        std::cout << "All files are already analysed" << std::endl;
        Texception_input_file x( myEOF);
        throw x;
    }
    plik.close();
    std::cout << "File nr " << nr_of_file_already_analysed << std::endl;
    if (nr_of_file_already_analysed >= list_of_names.size())
    {
        std::cout << "Alarm - bigger than the vector?" << std::endl;
    }

    std::cout
            << "Current file to analysis  has a name "
            << list_of_names[nr_of_file_already_analysed]
               << std::endl;

    // opening the data file to analyse
    plik.open ( list_of_names[nr_of_file_already_analysed].c_str(),  std::ios::binary );
    if ( !plik )
    {
        std::cerr << "Can't open file with events " << list_of_names[nr_of_file_already_analysed] << std::endl;
        Texception_input_file x( myFAIL);
        throw x;
    }
    std::cout << "File " << list_of_names[nr_of_file_already_analysed]
                 << " successfully open for analysis "<< std::endl;

    plik.seekg(0, std::ios::end);
    file_size = plik.tellg();
    plik.seekg(0, std::ios::beg);
    long pocz = plik.tellg();
    file_size -= pocz;

    current_file = list_of_names[nr_of_file_already_analysed];
    read_in_next_block();  // skiping info block?
    read_in_next_block();    // reading the real data block
    // if successful,
    return true;
}
//*************************************************************************
/*!
    \fn Tbuffer_of_data_block::give_address(int byte)
 */
// char * Tbuffer_of_data_block::inspect_address ( int byte )
// {
//     if ( byte_read >= length_of_data_block )
//     {
//         std::cerr << "Buffer is over, I need to read the next one" << std::endl;
//         read_in_next_block();
//         return   &buffer[0];
//     }
//     else
//         return &buffer[byte];
// }
//*******************************************************************
char * Tbuffer_of_data_block::inspect_address ( )
{
    std::cout << "This function seems to be never used " << std::endl;
    if ( byte_read >= length_of_data_block )
    {
        std::cerr << "Buffer is over, I need to read the next one" << std::endl;
        read_in_next_block();
        return   &buffer[0];
    }
    else
        return &buffer[byte_read];
}
//**************************************************************************
/*!
    \fn Tbuffer_of_data_block::info()
 */
void Tbuffer_of_data_block::info()
{
    // std::cout
    // << " Buffer info:    length_of_data_block= " << length_of_data_block
    // << " now  byte_read = " << byte_read << std::endl;
}
//**************************************************************************
