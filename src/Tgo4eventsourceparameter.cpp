#include "Tgo4eventsourceparameter.h"

#include <fcntl.h>
#include <iomanip>

extern void unpack_one_event();
//***************************************************************************
// Getting the events  from the file_________________________________
cuold_long*  TGo4MbsFileParameter::give_next_event ( int *how_many_words )  // virtual
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

    //     std::cout << "Getting the next event from the FILE and returning the pointer to the data"
    //     << std::endl;

    // reading something from the file into the buffer


    bool flag_unpacking_was_successful = true; // fake

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
    return ( cuold_long* ) data;
}
//******************************************************************************************************

#if DIAMANT_PRESENT
TDiamantFileParameter::TDiamantFileParameter(string nam) : filename ( nam ), starting_event_nr ( 0 )
{

    //    std::cout << " Opening file called " << nam << std::endl;
    bufAgata.open_file ( nam.c_str() );
    file_ptr = bufAgata.give_file_ptr();


    //    fLun =
    //            open(filename.c_str(), (O_RDONLY));
    //    if (fLun <= 0) {

    //        cout << "Error opening input file: " << filename
    //             << ", fLUn= " << fLun << endl;

    //    }


    //    file.open(filename, ios::binary);
    if (file_ptr == nullptr ) {

        cout << "Error opening Diamant input file: " << filename
             << endl;
        exit(-99);

    }


    cout << "Success opening input file: "
         << filename
            //         << ", fLUn= " << fLun
         << endl;


    fFrame = new MFMCommonFrame();
    fDiamantframe = new MFMDiamantFrame();

    fDiamantframe->InitStat();

    minsizeheader = 8; //MFM_BLOB_HEADER_SIZE; // =8
    cont = 0;
    iframe = 0;
    vectorsize = minsizeheader;
    vector = (char*) (malloc(vectorsize));
    pvector = &vector;
    framesize = 0;

    //int length_of_buffer = read_in_next_block();

    //std::cout << "First frame LENGTH = " << std::dec << length_of_buffer <<  std::endl;

    // in GAlileo not a case unpack_frame ( );   // unpacking the configuration frame

    //##################################################################
    //     std::cout << "Second frame ---------------------------------------------------------------------------" << std::endl;
    // length_of_buffer = bufAgata.read_in_next_block();
    //       std::cout << "LENGTH = " << std::dec << length_of_buffer <<  std::endl;
}
//***************************************************************************************************************
// Getting the events  from the file_________________________________
cuold_long*   TDiamantFileParameter::give_next_event ( int *how_many_words )  // virtual
{
    //         cout << "Getting the next event from the Diamond FILE "
    //         << endl;

    // for result
    memset(detector_data, 0, sizeof(detector_data));

    // reading something from the file into the buffer

label_top:
    // Read a block
    // argumets:
    // int file_descriptor
    // char buf[20] buffer
    // int nbytes = sizeof buf
    // Return value bytes_read
    framesize = fFrame->jurek_ReadInFile(*file_ptr, pvector, &vectorsize);

    //    cout << "1. framesize =  " << framesize << endl;

    fPtInFile+=framesize;

    *how_many_words = framesize;

    if (framesize <= 0){
        cout <<"END of Diamond data file" << endl;
        Texception_input_file ex(myEOF);
        throw ex;

    }
    //    cout << "Before reading USER Frame ----" << endl;


    // unpack it -  recoginise it and... unpack it


    // ReadUserFrame(fFrame);
    MFMCommonFrame* commonframe = fFrame;
    commonframe->SetAttributs();
    int type = commonframe->GetFrameTypeAttribut();
    int headersize = commonframe->GetHeaderSizeAttribut();

    // part of use of frame
    // example

    switch (type) {
    case MFM_DIAMANT_FRAME_TYPE: {
        ReadDiamantFrame(commonframe);
        break;
    }
    default: {
        fCount_elseframe++;
        //		ReadDefaultFrame(commonframe);
        //        return detector_data;
        //        break;
        goto label_top;
    }

    }// end of switch



    //    if (iframe++ >= nframes) {
    //        break;
    //    }
    //    fFrame->SetAttributs();


    //    bool flag_unpacking_was_successful = true; // fake


    nr_of_current_event++;

    //  *how_many_words = 77 ; //event_length / skok;


    ReadDiamantFrame(commonframe);
    // returning the pointer to the memory where the event is now
    return detector_data;
}
//**************************************************************************
void TDiamantFileParameter::unpack_one_event()
{
    ReadDiamantFrame(fFrame);
}
//**************************************************************************
//int TDiamantFileParameter::give_next_buffer(){ detector_data; }
//**************************************************************************
void TDiamantFileParameter::ReadDiamantFrame(MFMCommonFrame* commonframe)
{
    fDiamantframe->SetAttributs(commonframe->GetPointHeader());
    int framesize = fDiamantframe->GetFrameSize();
    fDiamantframe->FillStat();
    //    cout <<  "Board: "   <<  fDiamantframe->GetBoardId()  << endl;
    //    cout <<  "Channel: " <<  fDiamantframe->GetTGCristalId()  << endl;

    //    cout <<  "Energy: "  <<  fDiamantframe->GetEnergy()  << endl;
    //    cout <<  "Top: "     <<  fDiamantframe->GetTop()  << endl;
    //    cout <<  "TS: "      <<  fDiamantframe->GetTimeStamp();

    detector_data[0] = fDiamantframe->GetBoardId();
    // DIAMANTBoardId = board;
    detector_data[1] = fDiamantframe->GetTGCristalId() ;
    // DIAMANTChannelId  = channel;


    // After Lookup Table


    //    target_event->diamant_fired[1] = true;
    //    target_event->diamant_data[1][0] = 77;
    //    target_event->diamant_data[1][1] = 88;



    //    if ((DIAMANTChannelId >= 0) &&
    //            (DIAMANTChannelId <16) &&
    //            (board>=100) &&
    //            (board <200) )
    //    {
    //        DIAMANTId  = diamant_id_lookup[board-100][channel];
    //    }
    //    else {
    //        cout << "Illegal board or channel id: " << board <<  "  "<< DIAMANTChannelId << endl;
    //    }


    //    if (DIAMANTId < 0  || DIAMANTId >= MaxDIAMANT) {
    //        printf(" Wrong board or channel id:  %5d, %5d\n", board, channel);
    //    }
    //    DIAMANT
    detector_data[2]= fDiamantframe->GetEnergy()/512.;
    //    DIAMANT
    detector_data[3] = fDiamantframe->GetTop()/64.;
    //    EventTS = fDiamantframe->GetTimeStamp();

    //    if (DIAMANTTop > 0 && DIAMANTEnergy > 0) {
    //        DIAMANT
    //            pid = 1. - top * 1./ energy;

    //        cout << "DIAMANTPID = " << DIAMANTPID << endl;
    //    }

    // WPISANIE
    // fTree->Fill();

}
//////////////////////////////////////////////////////////////////////

//*************************************************************************
cuold_long *TDiamantRevServParameter::give_next_event(int *how_many_words)
{
    cout << "This function " << __func__
         << " never worked !!!!!!!!!!" << endl;

    static bool flag_first_time = true;
    if(flag_first_time)
    {
//        fFrame = new MFMCommonFrame();
//        fDiamantframe = new MFMDiamantFrame();
//        fDiamantframe->InitStat();
        flag_first_time = false;
    }


//   exotic_socket-> reads ( int sd, char *buf, int maxb )

   cuold_long *ptr = exotic_socket->give_next_event(how_many_words);
   uint8_t *sptr = (uint8_t *) ptr;


  // MFMCommonFrame *ptr_common = (MFMCommonFrame * )ptr;
   MFM_dia_frame * ptr_frame = (MFM_dia_frame * )ptr;




//    for(int i = 0 ; i < 30 ; ++i)
//    {
//        cout
        // << i << ") ptr[i] = "
//                    << hex << ptr[i] << " "
//               << " , dec = "         << dec << ptr[i]
//            << endl;
//    }
    for(int i = 0 ; i < 120 ; ++i)
    {
        int x = sptr[i];
        cout
//                << i << ") sptr[i] = "
            << hex
            << setw(2) << x << " " ;
//               << ", \tdec = "
//            << dec << sptr[i];

       if((i % 32) == 0) cout << endl;
    }

label_top:
    fFrame->ReadInMem(( char**)&ptr);

    // fPtInFile+=framesize;
    ptr += ptr_frame->Header.frameSize;
    //fFrame = reinterpret_cast<MFMCommonFrame*>(ptr);

    // framesize = fFrame->jurek_ReadInFile(*file_ptr, pvector, &vectorsize);

    // as it is already in memory, here we can try
    // to unpack it

    // unpack it -  recoginise it and... unpack it---------------------
    cout << "HERE SHOULD BE UNPACKING..... MFM" << endl;

    // ReadUserFrame(fFrame);



    MFMCommonFrame* commonframe = fFrame;

    commonframe->SetAttributs();

    int type = commonframe->GetFrameTypeAttribut();
    int headersize = commonframe->GetHeaderSizeAttribut();

    cout << "type of frame = " << type << ", headersize = " << headersize << endl;

    // part of use of frame
    // example
    switch (type) {
    case MFM_DIAMANT_FRAME_TYPE:    // 0x16
    {
        //ReadDiamantFrame(commonframe);
        cout << "ROZPOZNANY blok DIAMANT  !!!!!!!!!!!!!!!! " << endl;
        fDiamantframe->SetAttributs(commonframe->GetPointHeader());
        int framesize = fDiamantframe->GetFrameSize();
        fDiamantframe->FillStat();
        break;
    }
    default: {
        fCount_elseframe++;
        //		ReadDefaultFrame(commonframe);
        //        return detector_data;
        //        break;
        ptr += fFrame->GetFrameSize();
        goto label_top;
    }

    }// end of switch

#if 1



    //    if (iframe++ >= nframes) {
    //        break;
    //    }
    //    fFrame->SetAttributs();


    //    bool flag_unpacking_was_successful = true; // fake


    nr_of_current_event++;

    //  *how_many_words = 77 ; //event_length / skok;


    ReadDiamantFrame(commonframe);
    // returning the pointer to the memory where the event is now
#endif
    detector_data[0]= 0;
    detector_data[1]= 0;
    detector_data[2]= 0;
    detector_data[3]= 0;


    return detector_data;




    if (*how_many_words) nr_of_current_event++;
    return ptr;
}
/////////////////////////////////////////////////////////////////////////

//**************************************************************************
void TDiamantRevServParameter::ReadDiamantFrame(MFMCommonFrame* commonframe)
{
    fDiamantframe->SetAttributs(commonframe->GetPointHeader());
    int framesize = fDiamantframe->GetFrameSize();
    fDiamantframe->FillStat();
    //    cout <<  "Board: "   <<  fDiamantframe->GetBoardId()  << endl;
    //    cout <<  "Channel: " <<  fDiamantframe->GetTGCristalId()  << endl;

    //    cout <<  "Energy: "  <<  fDiamantframe->GetEnergy()  << endl;
    //    cout <<  "Top: "     <<  fDiamantframe->GetTop()  << endl;
    //    cout <<  "TS: "      <<  fDiamantframe->GetTimeStamp();

    detector_data[0] = fDiamantframe->GetBoardId();
    // DIAMANTBoardId = board;
    detector_data[1] = fDiamantframe->GetTGCristalId() ;
    // DIAMANTChannelId  = channel;


    // After Lookup Table


    //    target_event->diamant_fired[1] = true;
    //    target_event->diamant_data[1][0] = 77;
    //    target_event->diamant_data[1][1] = 88;



    //    if ((DIAMANTChannelId >= 0) &&
    //            (DIAMANTChannelId <16) &&
    //            (board>=100) &&
    //            (board <200) )
    //    {
    //        DIAMANTId  = diamant_id_lookup[board-100][channel];
    //    }
    //    else {
    //        cout << "Illegal board or channel id: " << board <<  "  "<< DIAMANTChannelId << endl;
    //    }


    //    if (DIAMANTId < 0  || DIAMANTId >= MaxDIAMANT) {
    //        printf(" Wrong board or channel id:  %5d, %5d\n", board, channel);
    //    }
    //    DIAMANT
    detector_data[2]= fDiamantframe->GetEnergy()/512.;
    //    DIAMANT
    detector_data[3] = fDiamantframe->GetTop()/64.;
    //    EventTS = fDiamantframe->GetTimeStamp();

    //    if (DIAMANTTop > 0 && DIAMANTEnergy > 0) {
    //        DIAMANT
    //            pid = 1. - top * 1./ energy;

    //        cout << "DIAMANTPID = " << DIAMANTPID << endl;
    //    }

    // WPISANIE
    // fTree->Fill();

}
//////////////////////////////////////////////////////////////////////

#endif // Diamant Presemt
