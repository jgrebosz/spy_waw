// -rem neda-daq02 10000
// coulexBU_i1898_3001_0000.adf - RING

// eagleBU_i1983_0000_0000.adf

//  coulexBU_i1793_4000_0000.adf

//---------------------------------------------------------------
//        Go4 Framework release package V.1.0000
//                      24-May-2002
//---------------------------------------------------------------
//   The GSI Online Offline Object Oriented (Go4) Project
//   Experiment Data Processing at DVEE department, GSI
//---------------------------------------------------------------
//
//Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI
//                    Planckstr. 1, 64291 Darmstadt, Germany
//Contact:            http://go4.gsi.de
//----------------------------------------------------------------
//This software can be used under the license agreements as stated
//in Go4License.txt file which is part of the distribution.
//----------------------------------------------------------------
#include <iostream>
#include <string.h>
#include <stdlib.h>
//#include "TROOT.h"
//#include "TRint.h"
//#include "TApplication.h"
//#include "TH1.h"
//#include "TBrowser.h"
//#include "TCanvas.h"
//#include "TBenchmark.h"
#include "TGalileoAnalysis.h"
//#include "Go4AnalysisClient/TGo4AnalysisClient.h"

#include "Tgrebware_exception.h"
#include "lockspy_2.h"

void usage(); // print announcement

using namespace std;

#ifdef NOGO4
TROOT go4application ( "The GO4","This is the Go4" );
#endif


#define kRemote 2
#define kBatch  1

#define WHAT  std::cout << "-----> File " << __FILE__ << ", Line= " << __LINE__ << " function " << __func__ << std::endl ;


// global variable
extern bool go4_gui_mode ;  // Take does not want to save *.spc files in Go4 mode
unsigned int starting_event; // global to be set in main and accessed from constructor of TGo4Analysis.cxx

int argcounter = 0;
string string_arguments[10];

//************************************************************************
//================== GALILEO analysis main program =======================
//************************************************************************
int main ( int argc, char **argv )
{
    // keep users from running multiple spies in one directory



    // argv[0] program
    // argv[1] "-gui" when started by GUI.
    // argv[2] analysis name as specified in GUI
    // argv[3] hostname of GUI
    // argv[4] connector port of GUI
    //

    // If not "-gui", following args are free to application
    std::cout << "This version of SPY was fully recompiled "
              << __DATE__
              << "  "
              << __TIME__
              << std::endl;

    argcounter = argc;

    for ( int i=0 ; i < argc ; i++ )
    {
        printf ( "%d %s\n",i,argv[i] );
        string_arguments[i] = argv[i] ;


    }

    if ( argc < 2 )
    {
        usage(); // no argument: print usage

        exit ( 8 );
    }
    int runningMode = kRemote;   // runningMode of analysis, kRemote or kBatch
    go4_gui_mode = true;
    if ( strcmp ( argv[1],"-gui" ) != 0 )
    {
        runningMode = kBatch; // no GUI
        go4_gui_mode = false ;
    }

    // Now we know the running mode

    int maxevents = -1; // number of events (batch mode)
    int len;
    unsigned int iport;
    std::string myfile;

    std::string tmp("Current");
    len = tmp.size() ;
    myfile  = tmp;

    // 1
    
    bool flag_remote_event_server = false ;

    std::string kawalek = std::string ( argv[1] ).substr ( 0,4 ) ;
    if    (kawalek == "-rem"
         )
    {
        flag_remote_event_server = true ;
    }
    else
    {
        //================= Begin  EB analysis ==================================
        // Prepare the EB analysis
        // In batch mode the arguments are
        // argv[1] filename
        // argv[2] optional number of events or -1

        if ( runningMode == kRemote )   // GUI analysis
        {
            len = strlen ( argv[2] );
            myfile=argv[2]; // build file names from analysis name
            iport = atoi ( argv[4] );
        }
        else
        {
            // This is the normal analysis of the Needle FILE -------------------------

            // Perhaps there is a file name and somebody put *.lmd there?
            std::string name = argv[1];
            std::string::size_type pos = name.find ( ".lmd" );
            if ( pos != std::string::npos )
            {
                name.erase ( pos );
            }
            
            //WH  AT;
            len = name.size();
            myfile = name.c_str() ;
            if ( argc > 2 )
                maxevents=atoi ( argv[2] );

            if ( argc > 3 )
                starting_event=atoi ( argv[3] );
        }
    }



    if ( maxevents == -1 ) maxevents = 999999999;
    //========================================================
    std::string mylmd = myfile; // input file with full path
    std::string myasf;
    std::string myraw;
    std::string mycal;
    std::string myusr;

    // strip any path information from output file name:
    std::string   lastname = mylmd;
    std::string   outname = lastname;

    // removing the path before
    std::cout << "lastname = " << lastname << std::endl;
    std::string::size_type pos = lastname.rfind ( "/" );
    if ( pos != std::string::npos )
    {
        outname = lastname.substr ( pos+1, std::string::npos );
    }
    // if this is a command file with extension, we change the point into '_'
    pos = lastname.rfind ( "." );
    if ( pos != std::string::npos )
    {
        lastname[pos] = '_';
    }

    myasf = outname;
    myraw = outname;
    mycal = outname;
    myusr = outname;

    // Now setup the Rising analysis itself

    TGalileoAnalysis*  myanalysis = nullptr ;
    
    // 3       return 6;
    if ( flag_remote_event_server == false )
    {
        //       mylmd += ".lmd"; // input GSI listmode
        mylmd = argv[1];
        myasf += "_ASF_spectra"; // root autosave file
        myraw += "_RAW_tree"; // root raw events
        mycal += "_CAL_tree"; // root calibrated events
        myusr += "_USR_never_used";  // root usr
        
        std::cout << "mylmd = " << mylmd << std::endl;


        myanalysis = new TGalileoAnalysis ( mylmd,
                                            myraw,
                                            mycal,
                                            myusr );

        // we do not want read in the old polygons etc
        std::string to_delete = myasf ;
        to_delete += ".root" ;
        unlink ( to_delete.c_str() );

        myanalysis->SetAutoSaveFile ( myasf );
    }
    else   // flag_remote_event_server == TRUE
    {
        std::cout << "avgv[1] = " << argv[1] << std::endl;
        std::cout << "avgv[2] = " << argv[2]<< std::endl;
        if(argc < 4) {
            cout << "Socket port number is missing?" << endl;
            exit(3);
        }
        std::cout << "avgv[3] = " << argv[3]<< std::endl;

        myanalysis = new TGalileoAnalysis ( std::string ( argv[1] ),
                std::string ( argv[2] ),
                std::string ( argv[3] ),
                "" );
        unlink ( "Current_ASF_spectra.root" );
        myanalysis->SetAutoSaveFile ( "Current_ASF_spectra" );
        //      myanalysis->SetAutoSaveFile("Current_ASF");
        // with the autosave file name "6003" was an error !!!!!!!!!!!!
    }

    myanalysis->SetAutoSaveInterval ( 200000 );
    usage();

    if ( runningMode == kBatch )
    {
        std::cout << "*  starting analysis in batch mode ...  " << std::endl;
        if ( myanalysis->InitEventClasses() )
        {
            myanalysis->run_jurek_ImplicitLoop ( maxevents );
            delete myanalysis;
            std::cout << "\t Done (in a natural way) !"<<std::endl;
        }
        else
        {
            std::cout << "Init event classes failed, aborting!"<<std::endl;
            exit ( 7 );
        }
    }
    return 0;
}
//***********************************************************************************************
void usage()
{
    std::cout << std::endl;
    std::cout << "*****************************************" << std::endl;
    std::cout << "*    SPY                                *" << std::endl;
    std::cout << "* the analysis program for Galileo/Needle*" << std::endl;
    std::cout << "* experiments                           *" << std::endl;
    std::cout << "*                                       *" << std::endl;
    std::cout << "*                                       *" << std::endl;
    std::cout << "* usage:                                *" <<std::endl;
    std::cout << "* 1. for ONline analysis                *" << std::endl;
    std::cout << "*                                       *" << std::endl;
    std::cout << "*   spy -rem   node_name   port_nr      *" <<std::endl;
    std::cout << "*                                       *" << std::endl;
    std::cout << "* 2. for OFFline analysis               *" << std::endl;
    std::cout << "*                                       *" << std::endl;
    std::cout << "*   spy filename [how many events]  [event to start with]    *" <<std::endl;
    std::cout << "*****************************************" << std::endl;
}



