#ifndef TSOCKET_FOR_DATA_H
#define TSOCKET_FOR_DATA_H

/**
The class to obtain the data coming online from the experiment

@author Jerzy Grebosz
*/

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "i2o.h"
#include "experiment_def.h"

#if DIAMANT_PRESENT
//========= Diamant - Begin ----------
#include "MFMAllFrames.h"
#include "DataParameters.h"
#include "DataScalers.h"
#include "MError.h"
#include "Cobo.h"
#include <cstring>
#include <cstdlib>
#include <time.h>
#include "MFMlib.in.h"
//#include "diamant_data.h"
////========= Diamant - End ----------
#endif


#define SUCCESS 0
#define ERROR   1

#define MAXDATASIZE 262144
#define MAXEVENTWORDS 1000


#include <string>
typedef unsigned int uold_long;

struct Tsocket_exception
{

    enum situation { cannot_read,   // cannot receive data
                     connection_closed,   // connection closed by client
                     one_byte };  // probably not revelant
    situation what;

};

/////////////////////////////////////////////////////////////////////////
class Tsocket_for_data
{
    int sockfd;     // file_descriptor
    char *buf;          // [MAXDATASIZE];
    char * buf_header;          // [sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME ) ];
    int bytes_already_used;
    int bytes_in_the_buffer;
    uold_long *event_buf ;
    int nr_of_current_event = 0 ;

    std::string current_hostname;
    std::string current_port_txt;
    bool flag_called_from_the_constructor;
    bool flag_needle_server = true;




public:
    Tsocket_for_data(std::string host, std::string port_txt )
    {
        flag_called_from_the_constructor = true;
        buf = new char[MAXDATASIZE];
        buf_header  =new char [sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME ) ];
        event_buf = new uold_long[MAXEVENTWORDS];
        exotic_open_socket ( host, port_txt );
        bytes_already_used = 0 ;
        bytes_in_the_buffer = 0 ;
        int nr_of_current_event = 0 ;
        flag_called_from_the_constructor = false;
        if(host == "neda-daq07"){
            flag_needle_server = false;

        }
        // int ile = reads(sockfd, buf, 10);
    }
    //-----------------------------------------------
    ~Tsocket_for_data()
    {
        close_exotic_socket();
        delete []  event_buf;
        delete  [] buf_header;
        delete [] buf;
    }
    //*************************************************************************
    int exotic_open_socket ( std::string hostname, std::string port_txt );
    int reopen_last_socket()
    {
        close_exotic_socket();
        return exotic_open_socket(current_hostname, current_port_txt);
    }
    int get_next_buffer();
    int get_next_needle_buffer();
    void close_exotic_socket();
    int reads ( int sd, char *buf, int maxb );
    uold_long*  give_next_event ( int *how_many_words ) ;
    uold_long*  give_next_needle_event ( int *how_many_words ) ;
    //int give_bytes_in_the_buffer() {return bytes_in_the_buffer;}
    char * give_buff_addr() { return buf; }
};

#endif
