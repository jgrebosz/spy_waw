#ifndef TGO4EVENTSOURCEPARAMETER_H
#define TGO4EVENTSOURCEPARAMETER_H

#include "experiment_def.h"

#include <iostream>
#include "TGalileoEventProcessor.h"

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

#include "tsocket_for_data.h"

#include "tbuffer_of_data_block.h"
extern Tbuffer_of_data_block bufAgata;
int unpack_frame ( );
class TjurekEventSourceException
{ }
;

class TjurekEventEndException : public TjurekEventSourceException
{

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TGo4EventSourceParameter
{
protected:
    int nr_of_current_event;
public:
    TGo4EventSourceParameter() :nr_of_current_event ( 0 )
    {}

    virtual ~TGo4EventSourceParameter() {} // <--- to be able correctly delete objects of derived classes

    virtual cuold_long* give_next_event ( int *how_many_words ) = 0;
    virtual int give_next_buffer()
    { std::cout << "emptyx version" << std::endl; return 0; }
    virtual char *give_buff_addr()
    { std::cout << "emptyxc version" << std::endl; return nullptr; }

    int give_event_nr()
    {
        return nr_of_current_event;
    }
    virtual std::string give_name_of_source()
    {
        return "abstract ???" ;
    }
    virtual int reopen_socket() {return 0;} ;
    virtual void unpack_one_event() { };

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TGo4RevServParameter : public TGo4EventSourceParameter
{
    std::string server_name;
    Tsocket_for_data *exotic_socket;


public:
    TGo4RevServParameter ( std::string host, std::string port ) // online name of the event server
    {
        // here will be calling of opening the socket
        std::cout << "TGo4RevServParameter : constructor, host = "
                  << host << ", port=" << port << std::endl;

        exotic_socket = new Tsocket_for_data (host, port);


    }
    ~TGo4RevServParameter ()
    {
        delete exotic_socket ;
    }

    // getting the events _________________________________
    //virtual
    cuold_long*  give_next_event ( int *how_many_words )
    {
        cuold_long *ptr= exotic_socket->give_next_event(how_many_words);
        if (*how_many_words) nr_of_current_event++;
        return ptr;
    }
    //---------------------------------------------------------
    int give_next_buffer()
    {

        int nr_bytes = exotic_socket->get_next_buffer();

        //                char * buff = exotic_socket->give_buff_addr();
        //        unsigned long adr = reinterpret_cast<unsigned long> (buff);

        //        std::cout << "posrednio  read into address "
        //             << std::hex <<  adr << std::dec << std::endl ;
        //        int * wsk = (int*) buff ;
        //        for(int i = 0 ; i < 12 ; ++i)
        //        {
        //            std::cout << "[" << i << "]   0x"<< std::hex << wsk[i] << std::dec << std::endl;
        //        }

        return nr_bytes;
    }
    //-------------------------
    char* give_buff_addr() {

        //        int * wsk = (int*) exotic_socket->give_buff_addr();
        //        std::cout << "posrednio4  read into address "
        //             << std::hex
        //             <<  (unsigned long) wsk
        //              << std::dec << std::endl ;

        //        for(int i = 0 ; i < 12 ; ++i)
        //        {
        //            std::cout << "[" << i << "]   0x"<< std::hex << wsk[i] << std::dec << std::endl;
        //        }
        return exotic_socket->give_buff_addr(); }

    //--------------------------------------------------------
    std::string  give_name_of_source()
    {
        return server_name;
    }
    int reopen_socket()
    {
        return exotic_socket->reopen_last_socket();
    }

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if DIAMANT_PRESENT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TDiamantRevServParameter : public TGo4EventSourceParameter
{
    std::string server_name;
    Tsocket_for_data *exotic_socket;

    MFMCommonFrame *fFrame ;
    MFMDiamantFrame *fDiamantframe;



//    int fLun {0};
//    unsigned long long fPtInFile =0;

//    // static TTree *fTree = nullptr;

    static const int MaxDIAMANT = 96; //6 boards * 16 channels
    static const int MaxChannel = 16;
    static const int MaxDIAMANTboards = 6;
    int DIAMANTBoardIdOffset = 100;   // default, actual value
//    //may be included in the command line
    unsigned long long fCount_elseframe = 0;
//    //unsigned
//    //char  data[1000000];

//    int minsizeheader = 8; //MFM_BLOB_HEADER_SIZE; // =8
//    int cont = 0;
//    unsigned long long iframe = 0;
//    char* vector;
//    char ** pvector;
//    int vectorsize;;
//    int framesize;
    double energy;
    double top;
    double pid;
    uold_long detector_data[10];


public:
    TDiamantRevServParameter ( std::string host, std::string port ) // online name of the event server
    {
        // here will be calling of opening the socket
        std::cout << "TDiamantRevServParameter : constructor, host = "
                  << host << ", port=" << port << std::endl;

        exotic_socket = new Tsocket_for_data (host, port);

        fFrame = new MFMCommonFrame();

    }
    ~TDiamantRevServParameter ()
    {
        delete exotic_socket ;
    }

    // getting the events _________________________________
    //virtual
    cuold_long*  give_next_event ( int *how_many_words );
    //---------------------------------------------------------
    int give_next_buffer()
    {

        int nr_bytes = exotic_socket->get_next_buffer();

        //                char * buff = exotic_socket->give_buff_addr();
        //        unsigned long adr = reinterpret_cast<unsigned long> (buff);

        //        std::cout << "posrednio  read into address "
        //             << std::hex <<  adr << std::dec << std::endl ;
        //        int * wsk = (int*) buff ;
        //        for(int i = 0 ; i < 12 ; ++i)
        //        {
        //            std::cout << "[" << i << "]   0x"<< std::hex << wsk[i] << std::dec << std::endl;
        //        }

        return nr_bytes;
    }
    //-------------------------
    char* give_buff_addr() {

        //        int * wsk = (int*) exotic_socket->give_buff_addr();
        //        std::cout << "posrednio4  read into address "
        //             << std::hex
        //             <<  (unsigned long) wsk
        //              << std::dec << std::endl ;

        //        for(int i = 0 ; i < 12 ; ++i)
        //        {
        //            std::cout << "[" << i << "]   0x"<< std::hex << wsk[i] << std::dec << std::endl;
        //        }
        return exotic_socket->give_buff_addr(); }

    //--------------------------------------------------------
    std::string  give_name_of_source()
    {
        return server_name;
    }
    int reopen_socket()
    {
        return exotic_socket->reopen_last_socket();
    }
    void ReadDiamantFrame(MFMCommonFrame* commonframe);

};
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TGo4MbsFileParameter  : public TGo4EventSourceParameter
{
    std::string filename;
    int starting_event_nr;
    std::ifstream file;
    int event_length;


    //unsigned
    char  data[1000000];
public:
    TGo4MbsFileParameter ( std::string nam ) : filename ( nam ), starting_event_nr ( 0 )
    {
        std::cout << " Opening file called " << nam << std::endl;

        //       std::cout << "  bufAgata.open_file ( nam.c_str() );-------------" << std::endl;
        bufAgata.open_file ( nam.c_str() );

        int length_of_buffer = bufAgata.read_in_next_block();

        //std::cout << "First frame LENGTH = " << std::dec << length_of_buffer <<  std::endl;

        // in GAlileo not a case unpack_frame ( );   // unpacking the configuration frame
        //std::cout << " skipping this CONFIGURATION  frame, because it is not interesting for us" << std::endl;

        //##################################################################
        //     std::cout << "Second frame ---------------------------------------------------------------------------" << std::endl;
        // length_of_buffer = bufAgata.read_in_next_block();
        //       std::cout << "LENGTH = " << std::dec << length_of_buffer <<  std::endl;
    }

    //------------------------------------------------------------------------------
    void SetStartEvent ( int ev )
    {
        // positioning the
        starting_event_nr = ev; //
        nr_of_current_event = starting_event_nr;
        if(ev) std::cout << "NOT IMPLEMENTED    skipping " << ev <<  " events" << std::endl;
    }

    // Getting the events  from the file_________________________________
    cuold_long*  give_next_event ( int *how_many_words );  // virtual

    std::string  give_name_of_source()
    {
        // return filename;
        // std::cout << " give_name_of_source()" << std::endl;
        return  bufAgata.give_current_filename();
    }

};
//**************************************************
#if DIAMANT_PRESENT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TDiamantFileParameter  : public TGo4EventSourceParameter
{
    std::string filename;
    int starting_event_nr;

    std::ifstream * file_ptr;
    int event_length;

    MFMCommonFrame * fFrame;
    MFMDiamantFrame * fDiamantframe;
    int fLun {0};
    unsigned long long fPtInFile =0;

    // static TTree *fTree = nullptr;

    static const int MaxDIAMANT = 96; //6 boards * 16 channels
    static const int MaxChannel = 16;
    static const int MaxDIAMANTboards = 6;
    int DIAMANTBoardIdOffset = 100;   // default, actual value
    //may be included in the command line
    unsigned long long fCount_elseframe = 0;
    //unsigned
    //char  data[1000000];

    int minsizeheader = 8; //MFM_BLOB_HEADER_SIZE; // =8
    int cont = 0;
    unsigned long long iframe = 0;
    char* vector;
    char ** pvector;
    int vectorsize;;
    int framesize;
    double energy;
    double top;
    double pid;
    uold_long detector_data[10];;
public:
    double give_energy() { return energy;}
    double give_top() { return top;}
    double give_pid() { return pid;}
    TDiamantFileParameter ( std::string nam );

    //------------------------------------------------------------------------------
    void SetStartEvent ( int ev )
    {
        // positioning the
        starting_event_nr = ev; //
        nr_of_current_event = starting_event_nr;
        if(ev) std::cout << "NOT IMPLEMENTED    skipping " << ev <<  " events" << std::endl;
    }

    // Getting the events  from the file_________________________________
    cuold_long*  give_next_event ( int *how_many_words );  // virtual

    std::string  give_name_of_source()
    {
        // return filename;
        // std::cout << " give_name_of_source()" << std::endl;
        return  filename;
    }
    void ReadDiamantFrame(MFMCommonFrame* commonframe) ;
    void unpack_one_event();
    //int give_next_buffer();

};
//**************************************************
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//**************************************************


#endif
////////////////
#endif // TGO4EVENTSOURCEPARAMETER_H
