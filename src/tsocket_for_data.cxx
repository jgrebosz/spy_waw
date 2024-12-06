#include "tsocket_for_data.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "tbuffer_of_data_block.h"
#include "TGalileoAnalysis.h"
extern void unpack_one_event();
using namespace std;
//*************************************************************************
int Tsocket_for_data::exotic_open_socket (std::string hostname, std::string port_txt)
{

    // this function is executed from the constructor, so
    // we should not throw exceptions from here.

    current_hostname = hostname;
    current_port_txt = port_txt;

    struct hostent *he;
    struct sockaddr_in their_addr;
    struct sockaddr_in l_addr;
    //char *hostname;
    int port;
    int sock_opt = 1;
    //int nread;



    //hostname = host.c_str();
    port = atoi ( port_txt.c_str() );

    if ( ( sockfd = socket ( PF_INET, SOCK_STREAM, 0 ) ) == -1 )
    {
        perror ( "socket" );
        if(!flag_called_from_the_constructor)
            throw std::runtime_error("Socket opening problem");
        else  exit ( 1 );
    }

    l_addr.sin_family = PF_INET;
    l_addr.sin_port = htons ( 0 );
    l_addr.sin_addr.s_addr = htonl ( INADDR_ANY );
    memset ( & ( l_addr.sin_zero ), '\0', 8 );

    if ( setsockopt ( sockfd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof ( int ) ) == -1 ){
        perror ( "setsockopt" );
        if(!flag_called_from_the_constructor)
            throw std::runtime_error("setsockopt result is -1");
        exit ( 1 );
    }

    if ( bind(sockfd,(struct sockaddr *)&l_addr, sizeof(struct sockaddr)) ==-1 ){
        perror ( "bind" );
        if(!flag_called_from_the_constructor)
            throw std::runtime_error(" bind ( sockfd, is -1");
        std::exit ( 1 );
    }

    std::cout << " Before gethostbyname: "  << hostname << std::endl;

    if ( ( he=gethostbyname ( hostname.c_str() ) ) == nullptr)
    {
        // perror ( "perror:   gethostbyname" );
        if(!flag_called_from_the_constructor){
            throw std::runtime_error("gethostbyname the result  address is NULLPTR");
        }
        cout << "Error while connecting to server " << hostname << endl;

        exit ( 1 );
    }
    // std::cout << "success w gethost by name\n"  << std::endl;

    their_addr.sin_family = PF_INET;
    their_addr.sin_port = htons ( port );
    their_addr.sin_addr = * ( ( struct in_addr * ) he->h_addr );
    memset ( & ( their_addr.sin_zero ), '\0', 8 );

    if ( connect ( sockfd, ( struct sockaddr * ) &their_addr, sizeof ( struct sockaddr ) ) == -1 ){
        perror ( "after function connect - a result is -1 " );
        if(!flag_called_from_the_constructor)
            throw std::runtime_error( "after function connect - a result is -1 " );
        exit ( 1 );
    }
    bytes_already_used = 0 ;
    bytes_in_the_buffer = 0 ;
    cout << "socket succesfully opened" << endl;
    return 1;  // when succes
}
//********************************************************************************************************************************************************************
int Tsocket_for_data::get_next_buffer()
{
    //    std::cout << "F. " << __func__ << std::endl;
    bytes_already_used = 0 ;
    bytes_in_the_buffer = 0;

    uint64_t buf_sz = 0;
    uint64_t data_sz = 0;
    // sequence or reading
    //     for ( ;; )
    int nread =  0;
    if(flag_needle_server)
    {
        /* receive header */
        //        std::cout << "Reading the HEADER " << std::endl;

        if ( ( nread = reads ( sockfd, buf_header, sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME ) ) ) == ERROR )
        {
            std::cout << "Tsocket_for_data::get_next_buffer()  ---> Can't get the event data from the socket..." << std::endl;
            return nread;
        }

        //        std::cout <<  "receive header  ,  nread: " << nread << std::endl;

        I2O_SEND_TO_BU_MESSAGE_FRAME *i2omsg;
        //     int orgid;
        //     int *w1;

        i2omsg = ( I2O_SEND_TO_BU_MESSAGE_FRAME * ) buf_header;
        uint64_t buf_sz = i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.HighPart;

        //     std::cout <<  "A) buf_sz: = " <<  buf_sz << std::endl;

        buf_sz <<= 32 ;
        buf_sz +=   i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.LowPart ;
        //    std::cout <<  "B) buf_sz: = " <<  buf_sz << std::endl;
        buf_sz = buf_sz*4;

        //  std::cout <<  "C) buf_sz: = " <<  buf_sz
        //        << " sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME )= "
        //        << sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME )

        //          <<  " i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.HighPart = "
        //          << i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.HighPart
        //          << " , i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.LowPart = "
        //          << i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.LowPart
        //        << std::endl;



        data_sz = buf_sz - sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME );

        // data_sz = MAXDATASIZE - sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME );

    }
    /* receive data ----------------------------------------------------------------------------------*/
    else {
        data_sz = 1024;    // diamond
    }
    //    std::cout << "Reading the buffer of DATA , data_sz =" << data_sz <<  std::endl;
    if ( ( nread = reads ( sockfd, buf, data_sz ) ) == ERROR )
    {
        return nread;
    }

    //    std::cout << "'reads' DATA, nr of bytes read ="
    //              << nread << std::dec << std::endl ;
    if(nread >0)
    {

        //        unsigned long adr = reinterpret_cast<unsigned long> (buf);
        //        std::cout << "Success - afer 'reads' DATA nr of read ="
        //                              << nread << " into address "
        //                         << std::hex <<  adr << std::dec << std::endl ;
        //        cout << "sizeof(int) = " << sizeof(int)
        //             << ", sizeof(long) = " << sizeof(long)
        //             << endl;
        //        int ile = nread / 4 ;
        //        int * wsk = (int*) buf ;
        //        for(int i = 0 ; i < ile; ++i)
        //        {
        //            std::cout << "[" << i << "]   0x"
        //                                << hex << wsk[i]
        //                                   << dec << ",  dec= " << wsk[i] << std::endl;
        //        }
    }
    else
    {


        // simulation
        //#define SIMULATION_SOCKET
#ifdef SIMULATION_SOCKET
        uold_long *ab   uf = (uold_long *) buf;

        abuf[0] = 0x23;
        abuf[1] = 0x34;
        abuf[2] = 0x45;
        abuf[3] = 0x56;
        abuf[4] = 0xffffffff;
        abuf[5] = 0x5;
        abuf[6] = 0x6;
        abuf[7] = 0x7;
        abuf[8] = 0x8;
        abuf[9] = 0xffffffff;
        nread = bytes_in_the_buffer = 10 * sizeof(abuf[0]);

#endif

    }
    return ((bytes_in_the_buffer = nread));

}
//********************************************************************************************************************************************************************
int Tsocket_for_data::get_next_needle_buffer()
{
    std::cout << "F. " << __func__  << std::endl;
    bytes_already_used = 0 ;
    bytes_in_the_buffer = 0;
    // sequence or reading
    //     for ( ;; )
    int nread =  0;

    /* receive header */
    //        std::cout << "Reading the HEADER " << std::endl;

    if ( ( nread = reads ( sockfd, buf_header, sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME ) ) ) == ERROR )
    {
        std::cout << "Tsocket_for_data::get_next_buffer()  ---> Can't get the event data from the socket..." << std::endl;
        return nread;
    }

    //        std::cout <<  "receive header  ,  nread: " << nread << std::endl;

    I2O_SEND_TO_BU_MESSAGE_FRAME *i2omsg;
    //     int orgid;
    //     int *w1;

    i2omsg = ( I2O_SEND_TO_BU_MESSAGE_FRAME * ) buf_header;
    uint64_t buf_sz = i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.HighPart;

    //     std::cout <<  "A) buf_sz: = " <<  buf_sz << std::endl;

    buf_sz <<= 32 ;
    buf_sz +=   i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.LowPart ;

    //    std::cout <<  "B) buf_sz: = " <<  buf_sz << std::endl;
    buf_sz = buf_sz*4;

    //  std::cout <<  "C) buf_sz: = " <<  buf_sz
    //        << " sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME )= "
    //        << sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME )

    //          <<  " i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.HighPart = "
    //          << i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.HighPart
    //          << " , i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.LowPart = "
    //          << i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize.LowPart
    //        << std::endl;


    uint64_t data_sz = buf_sz - sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME );

    // data_sz = MAXDATASIZE - sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME );
    /* receive data ----------------------------------------------------------------------------------*/
    // data_sz = 128*128;
    //    std::cout << "Reading the buffer of DATA , data_sz =" << data_sz <<  std::endl;
    if ( ( nread = reads ( sockfd, buf, data_sz ) ) == ERROR )
    {

        return nread;
    }

    //    std::cout << "'reads' DATA, nr of bytes read ="
    //              << nread << std::dec << std::endl ;
    if(nread >0)
    {
        unsigned long adr = reinterpret_cast<unsigned long> (buf);
        //            std::cout << "Success - afer 'reads' DATA nr of read ="
        //                      << nread << " into address "
        //                 << std::hex <<  adr << std::dec << std::endl ;
        //            int * wsk = (int*) buf ;
        //            for(int i = 0 ; i < 120 ; ++i)
        //            {
        //                std::cout << "[" << i << "]   0x"<< std::hex << wsk[i] << std::dec << std::endl;
        //            }
    }
    else
    {


        // simulation
        //#define SIMULATION_SOCKET
#ifdef SIMULATION_SOCKET
        uold_long *ab   uf = (uold_long *) buf;

        abuf[0] = 0x23;
        abuf[1] = 0x34;
        abuf[2] = 0x45;
        abuf[3] = 0x56;
        abuf[4] = 0xffffffff;
        abuf[5] = 0x5;
        abuf[6] = 0x6;
        abuf[7] = 0x7;
        abuf[8] = 0x8;
        abuf[9] = 0xffffffff;
        nread = bytes_in_the_buffer = 10 * sizeof(abuf[0]);

#endif

    }
    return ((bytes_in_the_buffer = nread));

}
//*************************************************

//*************************************************
void Tsocket_for_data::close_exotic_socket()
{
    // closing the socket
    close ( sockfd );
    //   return 0;
}
//***************************************************************************
int Tsocket_for_data::reads ( int sd, char *buf, int maxb )
{
    //    cout << __PRETTY_FUNCTION__ << endl;
    int nleft;
    int nread;
    nleft = maxb;

    while ( nleft > 0 )
        //if ( nleft > 0 )
    {
        // cout << "Trying to be read: " << nleft << " bytes from socket "<< endl;
        nread = recv ( sd, buf, nleft, 0 );
        if ( nread < 0 )
        {
            perror ( " Cannot receive data " );
            std::cout << " perror ( Cannot receive data )" << std::endl;
            Tsocket_exception ex;
            ex.what = Tsocket_exception::cannot_read ;
            throw ex;
            return ERROR;
        }
        else if ( nread == 0 )
        {
            std::cout <<  " nread = 0 bytes, so Connection closed by client\n"  << std::endl;

            Tsocket_exception ex;
            ex.what = Tsocket_exception::connection_closed ;
            throw ex;

            return ERROR;
        }
        //        std::cout << "SUCCESS!!!! recv:  nread = " << nread << " bytes"
        //            << endl;
        nleft -= nread;
        buf += nread;
    }
    bytes_already_used = 0 ;
    bytes_in_the_buffer = ( maxb - nleft );
    //    std::cout << "end of function, return value =  " << (bytes_in_the_buffer)
    //              << "---------------------------------------------------" << std::endl;

    return bytes_in_the_buffer ; /* return >= 0 */    // how many bytes are read
}
//***************************************************************************************************************
uold_long*  Tsocket_for_data::give_next_event ( int *how_many_words )  // virtual
{

    //    std::cout << __PRETTY_FUNCTION__
    //              << " -> Getting the next event from the socket buffer and returning the pointer to the data"
    //         << std::endl;

    // reading something from the file into the buffer

    // reads()
    //    buf = new char[100];
    //    int ile = reads( sockfd, buf, 20);

#if 1
    // skipping file header 40/32 bytes was done while opening the file
    //   long *event_ptr = ( long * ) &buf[0];





    int skok = sizeof(uold_long);
    int nr_of_current_event = 0;
    int event_length = 0 ;
    //int nr_word = 0 ;
    for ( event_length = 0 ;  ; event_length++ )
    {
        // reading one long word?
        //        file.read ( ( char* ) ptr, skok );

        if(bytes_already_used >= bytes_in_the_buffer )
        {
//            std::cout << "bytes_already_used= " << bytes_already_used
//                      << ", (bytes_in_the_buffer/sizeof(uold_long) = " << (bytes_in_the_buffer/sizeof(uold_long))
//                      << std::endl;
            // buffer finished, we need the new delivery
            cout << "Trying to read data from socket" << endl;

            get_next_buffer();
            if(bytes_in_the_buffer ==0)
            {

                *how_many_words = 0 ;
                return NULL;
            }

            // what happens if the was an error during reading?


            //         if ( file.eof() )
            //         {
            //             *how_many_words = 0;
            //             TjurekEventEndException ex;
            //             throw ex;
            //         }
            //         if ( !file )
            //         {
            //             std::cout << "Error while reading event !!!!!!" << std::endl;
            //             exit ( 3 );
            //         }

        }
        uold_long * where_from =(uold_long*) &buf[bytes_already_used];

        event_buf[event_length] =  *where_from;

//        std::cout << "Event Data word = "
//        << std::hex << event_buf[event_length]
//           << std::dec
//           << ", event_length="<< event_length << std::endl;

        bytes_already_used+= skok;
        break;




    }
    // setting some values (for example length of valid data)



    // returning the pointer to the memory where the event is now
    return ( uold_long* ) buf;
#else  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


    int skok = sizeof(uold_long);
    int nr_of_current_event = 0;
    int event_length = 0 ;
    //int nr_word = 0 ;
    for ( event_length = 0 ;  ; event_length++ )
    {
        // reading one long word?
        //        file.read ( ( char* ) ptr, skok );

        if(bytes_already_used >= bytes_in_the_buffer )
        {
            std::cout << "AAA bytes_already_used= "
                      << bytes_already_used
                      << ", (bytes_in_the_buffer/sizeof(uold_long) = "
                      << (bytes_in_the_buffer/sizeof(uold_long))
                      << ", so we need to read more "
                      << std::endl;
            // buffer finished, we need the new delivery
            get_next_buffer();
            if(bytes_in_the_buffer ==0)
            {
                *how_many_words = 0 ;
                return NULL;
            }

            // what happens if the was an error during reading?


            //         if ( file.eof() )
            //         {
            //             *how_many_words = 0;
            //             TjurekEventEndException ex;
            //             throw ex;
            //         }
            //         if ( !file )
            //         {
            //             std::cout << "Error while reading event !!!!!!" << std::endl;
            //             exit ( 3 );
            //         }

        }
        uold_long * where_from =(uold_long*) &buf[bytes_already_used];

        event_buf[event_length] =  *where_from;  // copying data word into the event buffer
        std::cout << "Event Data word copied= " << std::hex << event_buf[event_length]
                     << std::dec << ", event_length="<< event_length << std::endl;

        bytes_already_used+= skok;
        if (event_buf[event_length] == 0xca010103 )
        {
            nr_of_current_event++; // which is marked by the code 0xca010103 above

            //if ( nr_of_current_event >= 10 ) // skipping the first 10, why ?
            {
                std::cout << __func__ << " line: " << __LINE__
                          << ")  End of event nr " << nr_of_current_event
                          << ", after " << event_length << " bytes" << std::endl;

                *how_many_words = nr_of_current_event ; // event_length / skok;
                break;
            }
        }
        else
        {
            //             if ( nr_of_current_event > 236000 )
            //             {
            //                 std::cout << event_length << ")   read longword "
            //                 << std::hex << event_buf[event_length]  << " --->ev nr 0x" << event_length << std::dec << std::endl;
            //             }
        }
    }
    // setting some values (for example length of valid data)

    // returning the pointer to the memory where the event is now
    //return ( uold_long* ) event_buf;

    // extra part ---------------------------
    if(flag_needle_server)
    {
        try
        {
            unpack_one_event(); // external function
        }
        catch (Texception_input_file x)
        {

            std::cerr << "caught the exception   Texception_input_file " << std::endl;
            if (x.error_nr == -1)  // eof
            {
                *how_many_words = 0;
                TjurekEventEndException ex;
                throw ex;
            }
            if (x.error_nr == -2)  // fail
            {
                std::cout << "Error while reading event !!!!!!" << std::endl;
                exit ( 3 );
            }
        } // end of catch

        nr_of_current_event++;
        //if ( nr_of_current_event > 236000 )
        {
            //                 std::cout << "End of event nr " << nr_of_current_event
            //                 << ", after " << event_length << " bytes" << std::endl;
        }

        *how_many_words = 77 ; //event_length / skok;

        // returning the pointer to the memory where the event is now
        return ( uold_long* ) event_buf;
    }
    else // this is unpacking DIAMANT server ==========================================
    {
        cout << "give_next_event for DIAMANT event " << endl;


        *how_many_words = 44 ;
        return  event_buf;
    }
#endif


}

//***************************************************************************************************************
uold_long*  Tsocket_for_data::give_next_needle_event ( int *how_many_words )  // virtual
{
#ifdef NIGDY
    /*     Hi Jurek,
   I send you a compressed file which contains about 50MB of raw data taken
   using 2 ADC Caen V785 and 1 Scaler. I have attached the manual of the
   V785 ADC, that may be useful to understand the format of the data coming
   from the ADCs (see page 41). The Scaler can be ignored.
   The file zipped in data4jurek01.tgz has this format:

   File Header 32 bytes
   first data Block:
   first ADC Block (see output buffer section in Caen V785 Manual,
   page 41):
   Header 32 bits - first is 1a002000
   16 Data Words 32 bits
   End Of Block 32 bits - first is 1c000000
   second ADC Block (as before)
   Header 32 bits - first is 22002000
   16 Data Words 32 bits each
   End Of Block 32 bits - first is 2c7c0000
   Scaler Block
   ignore this...
   Trailer 32 bits, always ffffffff
   second data Block:
   ...
   ...
   File Trailer 24 bytes

   You can see the content of the file using a tool like octaldump (od),
   and the values I wrote are read with the command
   od -tx4 data_pc1203_r1.i0 | more

   I Hope you can use this data easily. Please write me some comments soon...
   Thanks a lot!

   ciao

   pietro

   ps. we have used V785, not V785N !

   #######################################################################




   */
#endif // NIGDY

    //     std::cout << "Getting the next event from the socket buffer and returning the pointer to the data"
    //     << std::endl;

    // reading something from the file into the buffer


#if 0
    // skipping file header 40/32 bytes was done while opening the file
    //   long *event_ptr = ( long * ) &buf[0];





    int skok = sizeof(uold_long);
    int nr_of_current_event = 0;
    int event_length = 0 ;
    //int nr_word = 0 ;
    for ( event_length = 0 ;  ; event_length++ )
    {
        // reading one long word?
        //        file.read ( ( char* ) ptr, skok );

        if(bytes_already_used >= bytes_in_the_buffer )
        {/*           std::cout << "bytes_already_used= " << bytes_already_used
      << ", (bytes_in_the_buffer/sizeof(uold_long) = " << (bytes_in_the_buffer/sizeof(uold_long))
      << std::endl;*/
            // buffer finished, we need the new delivery
            get_next_buffer();
            if(bytes_in_the_buffer ==0)
            {

                *how_many_words = 0 ;
                return NULL;
            }

            // what happens if the was an error during reading?


            //         if ( file.eof() )
            //         {
            //             *how_many_words = 0;
            //             TjurekEventEndException ex;
            //             throw ex;
            //         }
            //         if ( !file )
            //         {
            //             std::cout << "Error while reading event !!!!!!" << std::endl;
            //             exit ( 3 );
            //         }

        }
        uold_long * where_from =(uold_long*) &buf[bytes_already_used];

        event_buf[event_length] =  *where_from;
        // std::cout << "Data word = " << std::hex << event_buf[event_length]
        //	   << std::dec << ", event_length="<< event_length << std::endl;
        bytes_already_used+= skok;
        if (event_buf[event_length] == 0xca010103 )
        {
            nr_of_current_event++;
            if ( nr_of_current_event >= 10 )
            {
                std::cout << __func__ << " line: " << __LINE__
                          << ")  End of event nr " << nr_of_current_event
                          << ", after " << event_length << " bytes" << std::endl;

                *how_many_words = nr_of_current_event ; // event_length / skok;
                break;
            }
        }
        else
        {
            //             if ( nr_of_current_event > 236000 )
            //             {
            //                 std::cout << event_length << ")   read longword "
            //                 << std::hex << event_buf[event_length]  << " --->ev nr 0x" << event_length << std::dec << std::endl;
            //             }
        }
    }
    // setting some values (for example length of valid data)

    // returning the pointer to the memory where the event is now
    return ( uold_long* ) event_buf;
#else  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


    int skok = sizeof(uold_long);
    int nr_of_current_event = 0;
    int event_length = 0 ;
    //int nr_word = 0 ;
    for ( event_length = 0 ;  ; event_length++ )
    {
        // reading one long word?
        //        file.read ( ( char* ) ptr, skok );

        if(bytes_already_used >= bytes_in_the_buffer )
        {
            std::cout << "AAA bytes_already_used= "
                      << bytes_already_used
                      << ", (bytes_in_the_buffer/sizeof(uold_long) = "
                      << (bytes_in_the_buffer/sizeof(uold_long))
                      << ", so we need to read more "
                      << std::endl;
            // buffer finished, we need the new delivery
            get_next_buffer();
            if(bytes_in_the_buffer ==0)
            {
                *how_many_words = 0 ;
                return NULL;
            }

            // what happens if the was an error during reading?


            //         if ( file.eof() )
            //         {
            //             *how_many_words = 0;
            //             TjurekEventEndException ex;
            //             throw ex;
            //         }
            //         if ( !file )
            //         {
            //             std::cout << "Error while reading event !!!!!!" << std::endl;
            //             exit ( 3 );
            //         }

        }
        uold_long * where_from =(uold_long*) &buf[bytes_already_used];

        event_buf[event_length] =  *where_from;  // copying data word into the event buffer
        std::cout << "Event Data word copied= " << std::hex << event_buf[event_length]
                     << std::dec << ", event_length="<< event_length << std::endl;

        bytes_already_used+= skok;
        if (event_buf[event_length] == 0xca010103 )
        {
            nr_of_current_event++; // which is marked by the code 0xca010103 above

            //if ( nr_of_current_event >= 10 ) // skipping the first 10, why ?
            {
                std::cout << __func__ << " line: " << __LINE__
                          << ")  End of event nr " << nr_of_current_event
                          << ", after " << event_length << " bytes" << std::endl;

                *how_many_words = nr_of_current_event ; // event_length / skok;
                break;
            }
        }
        else
        {
            //             if ( nr_of_current_event > 236000 )
            //             {
            //                 std::cout << event_length << ")   read longword "
            //                 << std::hex << event_buf[event_length]  << " --->ev nr 0x" << event_length << std::dec << std::endl;
            //             }
        }
    }
    // setting some values (for example length of valid data)

    // returning the pointer to the memory where the event is now
    //return ( uold_long* ) event_buf;

    // extra part ---------------------------
    if(flag_needle_server)
    {
        try
        {
            unpack_one_event(); // external function
        }
        catch (Texception_input_file x)
        {

            std::cerr << "caught the exception   Texception_input_file " << std::endl;
            if (x.error_nr == -1)  // eof
            {
                *how_many_words = 0;
                TjurekEventEndException ex;
                throw ex;
            }
            if (x.error_nr == -2)  // fail
            {
                std::cout << "Error while reading event !!!!!!" << std::endl;
                exit ( 3 );
            }
        } // end of catch

        nr_of_current_event++;
        //if ( nr_of_current_event > 236000 )
        {
            //                 std::cout << "End of event nr " << nr_of_current_event
            //                 << ", after " << event_length << " bytes" << std::endl;
        }

        *how_many_words = 77 ; //event_length / skok;

        // returning the pointer to the memory where the event is now
        return ( uold_long* ) event_buf;
    }
    else // this is unpacking DIAMANT server ==========================================
    {
        cout << "give_next_event for DIAMANT event " << endl;


        *how_many_words = 44 ;
        return  event_buf;
    }
#endif


}






