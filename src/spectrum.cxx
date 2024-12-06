// spectra  hierarchy

#include "spectrum.h"
#include "TGalileoAnalysis.h"

// #define COTO  std::cout<<"plik "<<__FILE__<<" linia "<<__LINE__<<std::endl ;


// static pointer to the descritor of the spectra
Cspectra_descriptor * spectrum_abstr::spectra_descriptor;

//*****************************************************************************

spectrum_1D::spectrum_1D ( std::string name_root, std::string title, int nr_of_bins,
                           double first_channel,
                           double last_channel, std::string folder, std::string note, const std::string list_incrementers )

{
    //  std::cout << "Constructor for Spectrum: " << name_root << std::endl ;
    //last_channel_paper = 66 ; //ast_channel;
    max_bin = 0 ;
    //   std::cout << "name_root=" << name_root << ", title= " << title
    //   << ", nr_of_bins="  << nr_of_bins
    //   << ", first_channel=" << first_channel
    //   << ", last_channel=" << last_channel
    //   << ", folder=" << folder
    //   << ", note=" << note
    //   << ", list_incrementers=" << list_incrementers << std::endl;

    create ( name_root, title, nr_of_bins,
             first_channel,
             last_channel,
             folder,
             note,
             list_incrementers ) ;
}
//*****************************************************************************

spectrum_1D::spectrum_1D ( std::string name_root, int nr_of_bins,
                           double first_channel,
                           double last_channel, std::string folder, std::string note, const std::string list_incrementers )

{
    //  std::cout << "Constructor for Spectrum: " << name_root << std::endl ;
    //last_channel_paper = 66 ; //ast_channel;
    max_bin = 0 ;
    //   std::cout << "name_root=" << name_root << ", title= " << title
    //   << ", nr_of_bins="  << nr_of_bins
    //   << ", first_channel=" << first_channel
    //   << ", last_channel=" << last_channel
    //   << ", folder=" << folder
    //   << ", note=" << note
    //   << ", list_incrementers=" << list_incrementers << std::endl;

    create ( name_root, name_root, nr_of_bins,
             first_channel,
             last_channel,
             folder,
             note,
             list_incrementers ) ;
}
//*******************************************************************************
spectrum_1D::~spectrum_1D()
{
    //     std::cout << "Spectrum destructor running " << std::endl ;
    //save_to_disk() ;

    // if we use the following instruction, the spectrum will not be saved
    // into xxxASF.root file (auto save file)

    // TGo4Analysis::Instance() ->  RemoveHistogram(spectrum_name);

    //std::cout << "Spectrum: " << give_name() << " incremented "
    //     << give_statistic() << " times " << std::endl ;


    // the root part of spectrum was registered, so we do not have to delete it ?????????????
    // As there is no root here - we must delete it  !!!!!
    //delete ptr_root_spectrum;
}
//**************************************************************************
/** No descriptions */
spectrum_1D::spectrum_1D()
{}
//**************************************************************************
/** No descriptions */
//void spectrum_1D::create(char * name_root, char * title, int nr_of_bins,
//                         double first_channel,
//                         double last_channel, const char * folder, const char * note)
void spectrum_1D::create ( std::string name_root, std::string title, int nr_of_bins,
                           double first_channel,
                           double last_channel, std::string folder, std::string note, std::string list_of_incrementers )
{
    // std::cout << "  Constructor for Spectrum: " << name_root ; //<< std::endl ;

    // perhaps such a spectrum already exist in the memory (made by the previous
    // session) ?

    //     std::cout << "Does it already exist ? " << std::endl ;
    TH1 * wskaznik = GalileoAnalysis_ptr -> GetHistogram ( name_root.c_str() ) ;

    if ( wskaznik != nullptr )
    {
        // such a root spectrum already exist in root memory,
        // so we have to delete it

        //         std::cout << "Yes, removing it ." << std::endl ;
        GalileoAnalysis_ptr -> RemoveHistogram ( name_root ) ;

        //         std::cout << "Spectrum already existed, so deleting it "    << std::endl ;
    }

    //     COTO;
    // here we could check if there are special wishes about the binning
    {
        // local
        std::string fname = std::string ( "./my_binnings/" ) + name_root + ".spc.binning" ;
        std::ifstream plik ( fname.c_str() );
        if ( plik )
        {
            plik >> zjedz >> nr_of_bins
                    >> zjedz >> first_channel
                    >> zjedz >> last_channel ;
        }
        plik.close();
    } // local

    // creating the spectrum
    //     std::cout << "Spectrum new TH1D..." << std::endl ;

    // S is Short, D is Double  I (long int)
    // ptr_root_spectrum  = new TH1S (name_root, title, nr_of_bins, first_channel,  last_channel);
    ptr_root_spectrum  = new TH1I ( name_root.c_str(), title.c_str(),
                                    nr_of_bins, first_channel,  last_channel );
    //         std::cout << "Spectrum created brand-new " << std::endl ;

    reset_incrementers() ;

    statistics_of_increments = 0 ;
    spectrum_length = nr_of_bins ;
    min_bin = first_channel;

    ptr_condition_flag_is_true = nullptr ;
    // COTO;
    GalileoAnalysis_ptr ->  AddHistogram ( ptr_root_spectrum, folder.c_str() );
    //--------------------------- for 1D spectra
    //  notice = note ;
    //   COTO;
    //   std::cout << "before adding entry" << std::endl;
    spectra_descriptor->add_entry ( name_root + ".spc",
                                    nr_of_bins, first_channel,  last_channel,
                                    note.c_str() , list_of_incrementers );

    // COTO;
    // if somebody wants to analyse the specra

    int continuation = 1 ;
    if ( continuation )
    {
        // look at the disk if if does not exist
        std::cout << ":" ; //<< std::flush ;
        read_from_disk() ;
    }
    else
    {
        save_to_disk() ;
        std::cout << "." ; // << std::flush ;
    }
    // COTO;
    max_bin = last_channel;
    // std::cout << " end of create 1D spectrum " ;

}
//********************************************************* ********************
void spectrum_1D::increment_yourself()
{

    //std::cout << "Function incerment yourself, (how many incrementers="
    //     << how_many_incrementers << std::endl ;

    if ( can_we_increment() == false )
    {
        //std::cout << "INcrement not possible" << std::endl ;
        return ;
    }


    // look at the right data
    for ( unsigned int i = 0 ; i < int_incrementers_vector.size() ; i++ )
    {
        //std::cout << "Checking the candidate nr " << i << std::endl ;

        int value = * ( int_incrementers_vector[i] );
        //std::cout << "it has a value  " << value << std::endl ;

        //can be nagative !!!
        {
            //std::cout << "just before incrementation " << value << std::endl ;
            ptr_root_spectrum->Fill ( value );
            //statistics();

            // std::cout << "                       YES, incremented " << value << std::endl ;
        }

    }


    for ( unsigned int i = 0 ; i < double_incrementers_vector.size() ; i++ )
    {
        //std::cout << "Checking the candidate nr " << i << std::endl ;

        double value = * ( double_incrementers_vector[i] );
        //std::cout << "it has a value  " << value << std::endl ;

        //can be nagative !!!
        {
            //std::cout << "just before incrementation " << value << std::endl ;
            ptr_root_spectrum->Fill ( value );
            //statistics();

            // std::cout << "                       YES, incremented " << value << std::endl ;
        }
    }
}
//*****************************************************************************
void spectrum_1D::int_incrementer_X ( int * candidate )
{
    //std::cout << "F. : incrementer_X" << std::endl;
    int_incrementers_vector.push_back ( candidate );
}
//*****************************************************************************
void spectrum_1D::double_incrementer_X ( double * candidate )
{

    //std::cout << "F. : incrementer_X" << std::endl;
    double_incrementers_vector.push_back ( candidate );

}
//**************************************************************************
void spectrum_1D::int_incrementer_Y ( int * candidate )
{
    /* empty */
    std::cout << "Incrementer Y has no sense for 1D spectrum, any key... " << std::endl ;
    int liczba ;
    std::cin >> liczba ;
}
//**************************************************************************
void spectrum_1D::double_incrementer_Y ( double * candidate )
{
    /* empty */
    std::cout << "Incrementer Y has no sense for 1D spectrum, any key... " << std::endl ;
    int liczba ;
    std::cin >> liczba ;
}
//***********************************************************************
// char * spectrum_1D::give_name()
// {
//   return ptr_root_spectrum->GetName(); ;
// }
//************************************************************************
void spectrum_1D::save_to_disk()
{


    //#define  SPECTRA_IN_ASCII
#ifdef SPECTRA_IN_ASCII
    // this is 1D spectrum, so we save it in ASCII
    std::string name_s = std::string ( "spectra/" ) + ptr_root_spectrum->GetName();
    name_s += ".asc";

    //std::cout << "Saving 1D spectrum " << name_s << std::endl ;
    std::ofstream plik ( name_s.c_str(), std::ios::trunc );

    if ( !plik )
    {
        std::cout << "Error while saving spectrum "
           << name_s
           << std::endl ;
    }

    // we get value from the bin, not from "channel" so we do not
    // care about range of channels, for ex. -200 +200.

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow
    for ( int i = 1 ; i <= spectrum_length ; i ++ ) // see warning above
    {
        // int value =    (int)  ptr_root_spectrum->GetBinContent(i);
        // double bin_position = ptr_root_spectrum->GetBinLowEdge(i) ;

        plik
                << ptr_root_spectrum->GetBinLowEdge ( i )       // bin_position
                << "\t\t"
          << ( ( int )  ptr_root_spectrum->GetBinContent ( i ) ) // value
          << "\n" ;
    }
    plik << std::flush ;
    plik.close();
    //std::cout << "Succes " << std::endl;
#else

    // we save it in binary
    std::string name_s = std::string ( "spectra/" ) + ptr_root_spectrum->GetName();
    name_s += ".spc";

    //     std::cout << "Saving 1D spectrum " << name_s << std::endl ;
    std::ofstream plik ( name_s.c_str(), std::ios::trunc | std::ios::binary );

    if ( !plik )
    {
        std::cout << "Error while saving spectrum - cant open file "
           << name_s << " for writing"
           << std::endl ;
    }

    // we get value from the bin, not from "channel" so we do not
    // care about range of channels, for ex. -200 +200.

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow


    // the system which I am using now is
    // 1. double word - bins
    // 2. double word - left ede of the first bin
    // 3. double word - right ede of the last bin

    //  double left_edge = ptr_root_spectrum->GetBinLowEdge(1);
    //  double waga = ptr_root_spectrum->GetBinLowEdge(2) -
    //                    ptr_root_spectrum->GetBinLowEdge(1);;
    //
    //
    //  plik.write((char*)&left_edge, sizeof( left_edge));
    //  plik.write((char*)&waga, sizeof( waga));


    double bins = ptr_root_spectrum->GetNbinsX() ;
    // ouble left_edge = ptr_root_spectrum->GetXaxis()->GetBinLowEdge ( 1 );
    // ERROR OF ALAIN? double right_edge = ptr_root_spectrum->GetXaxis()->GetBinUpEdge ( ( int ) bins + 1 ) -1 ;
    double left_edge = ptr_root_spectrum->GetXaxis()->GetXmin();
    double right_edge = ptr_root_spectrum->GetXaxis()->GetXmax();
    plik.write ( ( char* ) &bins, sizeof ( bins ) );
    plik.write ( ( char* ) &left_edge, sizeof ( left_edge ) );
    plik.write ( ( char* ) &right_edge, sizeof ( right_edge ) );

#if 0

    std::cout
            << "F. spectrum_1D::save_to_disk()  "
      << "spectrum " << name_s
      << ", bins = " << bins
      << ", left edge = " << left_edge
      << ", right edge = " << right_edge
      << std::endl;
#endif

    for ( int i = 1 ; i <= spectrum_length ; i ++ ) // see warning above
    {

        //double bin_position = ptr_root_spectrum->GetBinLowEdge(i) ;
        //plik.write((char*)&bin_position, sizeof(bin_position));

        int value = ( int )  ptr_root_spectrum->GetBinContent ( i );
        //        if(i < 15)
        //         std::cout << "For channel "  << i << "  saving value = " << value << std::endl;
        plik.write ( ( char* ) &value, sizeof ( value ) );
    }
    //plik << std::flush ;
    plik.close();
    //std::cout << "Succes " << std::endl;


#endif
}
//************************************************************************
void spectrum_1D::inc_by ( long int chan, int value )
{
    ptr_root_spectrum->Fill ( chan, value );
    //statistics(value);

}
//***********************************************************************
/** No descriptions */
double spectrum_1D::give_max_chan_paper()
{
    return  max_bin;
}
//************************************************************************
//**************************   2D   *************************************/
/************************************************************************/
spectrum_2D::spectrum_2D ( std::string  name_root, std::string  title,
                           int nr_of_bins_x, double first_channel_x, double last_channel_x,
                           int nr_of_bins_y, double first_channel_y, double last_channel_y,
                           const std::string  folder, const std::string  note, const std::string  list_of_incrementers )
{

    //     std::cout << "Constructor for 2D Spectrum: " << name_root << std::endl ;

    // perhaps such a spectrum already exist in the memory (made by the previous
    // session) ?

//    std::cout << "Does " << name_root << " already exist ? " << std::endl ;
    TH1 * wskaznik = GalileoAnalysis_ptr -> GetHistogram ( name_root.c_str() ) ;

    if ( wskaznik != nullptr )
    {
        // such a root spectrum already exist in root memory,
        // so we have to delete it

        std::cout << "Yes, removing it ." << std::endl ;
        GalileoAnalysis_ptr -> RemoveHistogram ( name_root.c_str() ) ;

        //         std::cout << "Spectrum already existed, so deleting it "    << std::endl ;
    }
    //     if(flag_talking_histograms)
    //     {
    //         std::cout <<  "breakpoint" << std::endl;
    //     }
    std::string fname = std::string ( "./my_binnings/" ) + name_root + ".mat.binning" ;
    std::ifstream plik ( fname.c_str() );
    if ( plik )
    {
        plik >> zjedz >> nr_of_bins_x
                >> zjedz >> first_channel_x
                >> zjedz >> last_channel_x ;
        plik >> zjedz >> nr_of_bins_y
                >> zjedz >> first_channel_y
                >> zjedz >> last_channel_y ;
    }
    plik.close();


    // creating the spectrum
    //std::cout << "Spectrum new TH2I..." << std::endl ;
    ptr_root_spectrum  = new TH2I ( name_root.c_str(), title.c_str(),
                                    nr_of_bins_x,  first_channel_x,  last_channel_x,
                                    nr_of_bins_y,  first_channel_y,  last_channel_y
                                    );
    //std::cout << "Spectrum created brand-new " << std::endl ;

//    double def_tab[6] ;

//    //------- X

//    def_tab[0] = ptr_root_spectrum->GetNbinsX() ;
//    def_tab[1] = ptr_root_spectrum->GetXaxis()->GetBinLowEdge ( 1 ); // this is the first, real bin, because the bin 0 is reserved for underflow
//    // the last real bin underflow bin + nr of bins
//    def_tab[2] = ptr_root_spectrum->GetXaxis()->GetBinUpEdge ( ( int ) ( def_tab[0] ) );

//    // ------ Y
//    def_tab[3] = ptr_root_spectrum->GetNbinsY() ;
//    def_tab[4] = ptr_root_spectrum->GetYaxis()->GetBinLowEdge ( 1 );
//    def_tab[5] = ptr_root_spectrum->GetYaxis()->GetBinUpEdge ( ( int ) ( def_tab[3] ) );


    //----------------

    statistics_of_increments = 0 ;
    x_spectrum_length = nr_of_bins_x ;
    x_min_bin = first_channel_x;

    y_spectrum_length = nr_of_bins_y ;
    y_min_bin = first_channel_y;


    GalileoAnalysis_ptr ->  AddHistogram ( ptr_root_spectrum, folder.c_str() );

    //---------------------------  for 2D spectra

    spectra_descriptor-> add_entry ( name_root + ".mat",
                                     nr_of_bins_x,  first_channel_x,  last_channel_x,
                                     nr_of_bins_y,  first_channel_y,  last_channel_y,
                                     note.c_str(), list_of_incrementers.c_str() );


    // look at the disk if if does not exist
    int continuation = 1 ;
    if ( continuation )
    {
        //         std::cout << "Readinfg from disk - for cxontitunation" << std::endl;
        std::cout << "[#]" ; // << std::flush ;
        read_from_disk();

    }
    else
    {
        save_to_disk() ;
        std::cout << "[.]" << std::flush ;
    }

}
//*******************************************************************************

spectrum_2D::~spectrum_2D()
{
    // perhaps the analysis class will save it earlier ?
    //std::cout << "Spectrum destructor running " << std::endl ;

    // ptr_root_spectrum ->Write();     -< error, no file open

    //save_to_disk() ;

    // if we use the following instruction, the spectrum will not be saved
    // into xxxASF.root file (auto save file)

    // TGo4Analysis::Instance() ->  RemoveHistogram(spectrum_name);


    //  std::cout << "Spectrum: " << give_name() << " incremented "
    //       << give_statistic() << " times " << std::endl ;



    //     the root part of spectrum was registered, so we do not have to delete it ?????????????
}
//**********************************************************************************

//************************************************************************
void spectrum_2D::save_to_disk()
{
    // this is 2D spectrum, so we save it in binary
    std::string  name_s = "spectra/" ;
    name_s += ptr_root_spectrum->GetName();
    name_s += ".mat";
    //      std::cout << "Saving 2D spectrum " << name_s << std::endl ;
    std::ofstream plik ( name_s.c_str(), std::ios::binary );

    if ( !plik )
    {
        std::cout << "Error while saving spectrum "
           << name_s
           << std::endl ;
    }


    //  std::cout << "is saving "
    //       <<  (2*y_spectrum_length * x_spectrum_length )/1024
    //       << " KB  " << std::endl ;


    // we get value from the bin, not from "channel" so we do not
    // care about range of channels, for ex. -200 +200.

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow




    // NOTE: TRICK   we check if the matrix has less than 32000 std::couts in every cell.
    // If yes, we can save this matrix in 16 bit formats
    // If NO, we save it in 32 bit format, and we signalise this by negative value
    // in the  tab[0]

    bool flag_must_be_32_bit = false;
    for ( int y = 1 ; y <= y_spectrum_length && !flag_must_be_32_bit ; y ++ ) // see note above
    {
        for ( int x = 1 ; x <= x_spectrum_length ; x ++ ) // see note above
        {
            if ( ptr_root_spectrum->GetBinContent ( x, y ) > 32000 )
            {
                flag_must_be_32_bit = true;
                //std::cout << "Saving 32 bit spectrum " << name_s << std::endl;
                break;
            }
        }
    }


    // the system which I am using now is
    // 0. double word - left ede of the first bin X
    // 1. double word - waga X
    // 2. double word - bins X

    // 3. double word - left ede of the first bin Y
    // 4. double word - waga Y
    // 5. double word - bins Y

    double tab[6] ;


    //ptr_root_spectrum)->GetXaxis();

    //------- X
    tab[0] = ptr_root_spectrum->GetNbinsX() ;
    tab[1] = // ptr_root_spectrum->GetXaxis()->GetBinLowEdge ( 1 );
            ptr_root_spectrum->GetXaxis()->GetXmin();
    tab[2] = // ptr_root_spectrum->GetXaxis()->GetBinUpEdge ( ( int ) tab[0] + 1 ); // here was a bug, because we had to skip the underflow bin !!! (+1)
            ptr_root_spectrum->GetXaxis()->GetXmax();


    //     std::cout << "  NBins X = "  << tab[0]
    //          << "  Xmin =" << tab[1]
    //          << "  Xmax " <<  tab[2] << std::endl;

    // ------ Y
    tab[3] = ptr_root_spectrum->GetNbinsY() ;
    //tab[4] = ptr_root_spectrum->GetYaxis()->GetBinLowEdge ( 1 );
    //tab[5]  = ptr_root_spectrum->GetYaxis()->GetBinUpEdge ( ( int ) tab[3] + 1 ); // here was a bug, because we had to skip the underflow bin !!! (+1)
    tab[4]  = ptr_root_spectrum->GetYaxis()->GetXmin();
    tab[5]  = ptr_root_spectrum->GetYaxis()->GetXmax();
    //
    //  std::cout << "  NBins Y = "  << tab[3]
    //          << "  BinLowEdge[1] =" << tab[4]
    //          << "  BinUpEdge[" << tab[3]+1 << "] =  " << tab[5] << std::endl;

    tab[0] *= ( flag_must_be_32_bit? -1 : 1 ); // moment of puttng a negative sign (TRICK above)

    //    std::cout << "This matrix has xbins = "
    //          << ptr_root_spectrum->GetNbinsX()
    //         << " ybins = "
    //          << ptr_root_spectrum->GetNbinsY() << std::endl;


    plik.write ( ( char* ) tab, sizeof ( tab ) );


    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow

    if ( flag_must_be_32_bit == false ) // +++++++++++++++++++++++++++++++
    {
        typedef  short int  slowo;
        slowo linijka[x_spectrum_length]  ;

        for ( int y = 1 ; y <= y_spectrum_length ; y ++ ) // see note above
        {
            for ( int x = 1 ; x <= x_spectrum_length ; x ++ ) // see note above
            {
                linijka[x - 1] = ( slowo )  ptr_root_spectrum->GetBinContent ( x, y );
            }
            plik.write ( ( char* ) linijka, sizeof ( linijka ) ) ;
        }

    }
    else    // so flag_must_be_32_bit !!!   //++++++++++++++++++++++++++
    {

        typedef  int  slowo;
        slowo linijka[x_spectrum_length]  ;
        for ( int y = 1 ; y <= y_spectrum_length ; y ++ ) // see note above
        {
            for ( int x = 1 ; x <= x_spectrum_length ; x ++ ) // see note above
            {
                linijka[x - 1] = ( slowo )  ptr_root_spectrum->GetBinContent ( x, y );
            }
            plik.write ( ( char* ) linijka, sizeof ( linijka ) ) ;
        }
    }
    plik.close();

    //   std::cout << "Succes " << std::endl;

}
//*****************************************************************************
void spectrum_2D::int_incrementer_X ( int * candidate )
{

    //std::cout << "F. : incrementer_X" << std::endl;
    x_int_incrementers_vector.push_back ( candidate );

}

//*****************************************************************************
void spectrum_2D::double_incrementer_X ( double * candidate )
{

    //std::cout << "F. : incrementer_X" << std::endl;
    x_double_incrementers_vector.push_back ( candidate );
}
//**************************************************************************
void spectrum_2D::int_incrementer_Y ( int * candidate )
{
    //std::cout << "F. : incrementer_Y" << std::endl;
    y_int_incrementers_vector.push_back ( candidate );
}
//**************************************************************************
void spectrum_2D::double_incrementer_Y ( double * candidate )
{

    //std::cout << "F. : incrementer_Y" << std::endl;
    y_double_incrementers_vector.push_back ( candidate );

}
//***********************************************************************
void spectrum_2D::increment_yourself()
{
    //std::cout << "Function incerment yourself, (how many incrementers="
    //     << how_many_incrementers << std::endl ;


    if ( can_we_increment() == false )
        return ;

    // look at the right integer data

    // all Y from intgeger list -------------
    for ( unsigned int y = 0 ; y < y_int_incrementers_vector.size()  ;  y++ )
    {

        // x integer list first ----------
        for ( unsigned int x = 0 ; x < x_int_incrementers_vector.size() ; x++ )
        {
            //std::cout << "Checking the candidate nr " << i << std::endl ;

            // test if this not the same data
            if ( x_int_incrementers_vector[x] == y_int_incrementers_vector[y] )
                continue ;


            int x_value = * ( x_int_incrementers_vector[x] );
            int y_value = * ( y_int_incrementers_vector[y] );
            //std::cout << "it has a value  " << x_value << std::endl ;

            //can be nagative !!!
            {
                //std::cout << "just before incrementation " << value << std::endl ;
                ptr_root_spectrum->Fill ( x_value, y_value );
                statistics();

                // std::cout << "                       YES, incremented " << value << std::endl ;
            }

        }

        // x double list now ----------
        for ( unsigned int x = 0 ; x < x_double_incrementers_vector.size() ; x++ )
        {
            //std::cout << "Checking the candidate nr " << i << std::endl ;

            // test if this not the same data
            //  if(x_incrementers_list[x] == y_incrementers_list[y] )continue ;


            double x_value = * ( x_double_incrementers_vector[x] );
            int y_value = * ( y_int_incrementers_vector[y] );
            //std::cout << "it has a value  " << x_value << std::endl ;

            //can be nagative !!!
            {
                //std::cout << "just before incrementation " << value << std::endl ;
                ptr_root_spectrum->Fill ( x_value, y_value );
                statistics();

                // std::cout << "                       YES, incremented " << value << std::endl ;
            }

        }



    }

    // now the same for Y double ###############################
    for ( unsigned int y = 0 ; y < y_double_incrementers_vector.size(); y++ )
    {

        // x integer list first ----------
        for ( unsigned int x = 0 ; x < x_int_incrementers_vector.size() ; x++ )
        {
            //std::cout << "Checking the candidate nr " << i << std::endl ;

            // test if this not the same data
            //if(x_int_incrementers_list[x] == y_double_incrementers_list[y] )continue ;


            int x_value = * ( x_int_incrementers_vector[x] );
            double y_value = * ( y_double_incrementers_vector[y] );
            //std::cout << "it has a value  " << x_value << std::endl ;

            //can be nagative !!!
            {
                //std::cout << "just before incrementation " << value << std::endl ;
                ptr_root_spectrum->Fill ( x_value, y_value );
                statistics();

                // std::cout << "                       YES, incremented " << value << std::endl ;
            }

        }

        // x double list now ----------
        for ( unsigned int x = 0 ; x < x_double_incrementers_vector.size() ; x++ )
        {
            //std::cout << "Checking the candidate nr " << i << std::endl ;

            // test if this not the same data
            //  if(x_incrementers_list[x] == y_incrementers_list[y] )continue ;


            double x_value = * ( x_double_incrementers_vector[x] );
            double y_value = * ( y_double_incrementers_vector[y] );
            //std::cout << "it has a value  " << x_value << std::endl ;

            //can be nagative !!!
            {
                //std::cout << "just before incrementation " << value << std::endl ;
                ptr_root_spectrum->Fill ( x_value, y_value );
                statistics();

                // std::cout << "                       YES, incremented " << value << std::endl ;
            }

        }



    }


}
//*****************************************************************************
void spectrum_1D::increment ( int x_value, int y_value )
{
    std::cout << "just before MATIX incrementation " << x_value << std::endl ;
    ptr_root_spectrum->Fill ( x_value, y_value );
    statistics();

    std::cout << "            YES, incremented x " << x_value << " y " << y_value << std::endl ;

}
//****************************************************************************
/** this function is meant only for scalers "spectra". They are scrolled like
pen writing on the paper band */
void spectrum_1D::scroll_left_by_n_bins ( int bins_to_scroll )
{

    //   std::cout << "Scrolling spectrum " << give_name() << std::endl;

    for ( int i = 0 ; i < spectrum_length ; i ++ )
    {

        if ( i < ( spectrum_length - bins_to_scroll ) )
        {

            //    below Stat_t is a root typedef (meaning: int ?)
            Stat_t ttt = ptr_root_spectrum->GetBinContent ( i + bins_to_scroll )  ;

            ptr_root_spectrum->SetBinContent ( i,    // channel nr
                                               ttt );

            //          ptr_root_spectrum->Fill(i, value);
            //      ptr_root_spectrum->SetBinContent(
            //          i,  // channel nr
            //          ptr_root_spectrum->GetBinContent(i+bins_to_scroll)  // value
            //          );
        }
        else
        {
            // put zero
            ptr_root_spectrum->SetBinContent ( i, 0 ) ;
            ptr_root_spectrum->Fill ( i, 0 );
        }

    } // end of for

}
//*****************************************************************************
/** for continuation option */
void spectrum_1D::read_from_disk()
{

    // this is 1D spectrum, so we save it in ASCII
    std::string name_s = std::string ( "spectra/" ) + ptr_root_spectrum->GetName();
    name_s += ".spc";

    //   std::cout << "spectrum_1D::read_from_disk()    Restoring 1D spectrum " << name_s << std::endl ;
    std::ifstream plik ( name_s.c_str(), std::ios::binary );

    if ( !plik )
    {
        std::cout << "Can't open file " << name_s << " to restore spectrum"
           << "\nmost probably spectrum was not stored on the disk before"
           << std::endl ;
        save_to_disk() ;
        return;
    }

    // we get value from the bin, not from "channel" so we do not
    // care about range of channels, for ex. -200 +200.

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow


    // the system which I am using now is
    // 1. double word - left ede of the first bin
    // 2. double word - waga

    //  double left_edge = ptr_root_spectrum->GetBinLowEdge(1);
    //  double waga = ptr_root_spectrum->GetBinLowEdge(2) -
    //                    ptr_root_spectrum->GetBinLowEdge(1);;
    //
    //
    //  plik.read((char*)&left_edge, sizeof( left_edge));
    //  plik.read((char*)&waga, sizeof( waga));


    double bins, left_edge, right_edge ;

    plik.read ( ( char* ) &bins, sizeof ( bins ) );
    plik.read ( ( char* ) &left_edge, sizeof ( left_edge ) );
    plik.read ( ( char* ) &right_edge, sizeof ( right_edge ) );

    double waga = ( right_edge - left_edge ) / bins ;

#ifdef NIGDY

    bool flag_test = false ;
    std::string nnn = ptr_root_spectrum->GetName();
    if ( nnn == "test_frame_signal_raw" )
    {
        flag_test = true;
        std::cout << "\nread bins = " << bins
                  << ", left_edge = " << left_edge
                  << ", right_edge = " << right_edge << std::endl;
    }
#endif // NIGDY


    int value ;
    for ( int i = 0 /*1*/ ; i < ptr_root_spectrum->GetNbinsX() && plik  ; i ++ ) // see warning above
    {
        double chan = ( i * waga ) + left_edge + ( waga / 2.0 ); // - 1;  // (without -1 the spectrum was shifted +1 channel during reading

        plik.read ( ( char* ) &value, sizeof ( value ) );
        if ( plik.eof() )
            break ;
        ptr_root_spectrum->Fill ( chan, value );

#ifdef NIGDY

        if ( flag_test && i < 20 )
        {
            std::cout << "read value = " << value
                      << ", is put to the channel  = " << chan  << std::endl;
        }
#endif // NIGDY

        //std::cout << "Chan " << chan << " value " << value << std::endl;
    }
    plik.close();



}
//***********************************************************************
//*****************************************************************************
/** for continuation option */
void spectrum_2D::read_from_disk()
{
    double ttt = 9 ;

    // this is 1D spectrum, so we save it in ASCII
    std::string name_s = std::string ( "spectra/" ) + ptr_root_spectrum->GetName();
    name_s += ".mat";

//    std::cout << "Restoring 2D spectrum " << name_s << std::endl ;
//    if(name_s.find("polar") != string::npos )
//    {
//        cout << "JEST " << name_s << endl;
//        ttt = sin(M_PI);
//        ttt = 6;
//        cout << "JEST " << ttt << endl;
//        // exit(33);

//    }
    std::ifstream plik ( name_s.c_str(), std::ios::binary );

    if ( !plik )
    {
        std::cout << "Can't open file " << name_s << " to restore spectrum"
           << "\nmost probably spectrum was not stored on the disk before"
           << std::endl ;
        save_to_disk();
        return;
    }


    // we get value from the bin, not from "channel" so we do not
    // care about range of channels, for ex. -200 +200.

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow


    // the system which I am using now is
    // 0. double word - bins X
    // 1. double word - left edge of the first bin X
    // 2. double word - right edge of the first bin X

    // 3. double word - bins Y
    // 4. double word - left edge of the first bin Y
    // 5. double word - right edge of the first bin y

    double def_tab[6] ;

    //------- X

    def_tab[0] = ptr_root_spectrum->GetNbinsX() ;
    def_tab[1] = ptr_root_spectrum->GetXaxis()->GetBinLowEdge ( 1 ); // this is the first, real bin, because the bin 0 is reserved for underflow

    // the last real bin underflow bin + nr of bins
//    def_tab[2] = ptr_root_spectrum->GetXaxis()->GetBinUpEdge ( ( int ) ( 1 + def_tab[0] ) );
    def_tab[2] = ptr_root_spectrum->GetXaxis()->GetBinUpEdge ( ( int ) (def_tab[0] ) );

    // ------ Y
    def_tab[3] = ptr_root_spectrum->GetNbinsY() ;
    def_tab[4] = ptr_root_spectrum->GetYaxis()->GetBinLowEdge ( 1 );
//    def_tab[5] = ptr_root_spectrum->GetYaxis()->GetBinUpEdge ( ( int ) ( 1 + def_tab[3] ) );
    def_tab[5] = ptr_root_spectrum->GetYaxis()->GetBinUpEdge ( ( int ) (def_tab[3] ) );


//    std::cout << "This matrix has xbins = "
//          << ptr_root_spectrum->GetNbinsX()
//          << " ybins = "
//          << ptr_root_spectrum->GetNbinsY() << std::endl;


    double retab[6]  = { 0,0,0,0,0,0};
    plik.read ( ( char* ) retab, sizeof ( retab ) );

    // NOTE: TRICK If the first word (tab[0] is negative, this means that the coding is 32 bit word(int)
    // otherwise it is standard 16 bit word (short int)
    bool flag_must_be_32_bit = false;
    if ( retab[0] < 0 )
    {
        flag_must_be_32_bit = true;
        retab[0] *= -1;  // we change sign into positive
    }


    for ( int i = 0 ; i < 6 ; i++ )
    {
//        cout << i << ") defTab[i] " << def_tab[i] << " but read  retab[i] " <<  retab[i] << endl;
        if ( def_tab[i] != retab[i] )
        {

            std::cout << " matrix has  different binning - so we do not read, matrix should be brand new " << std::endl;
            return ; // <different binning - we do not read, matrix should be zero
        }
    }

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow

    //     bool gadaj = false;
    //
    //     if (name_s == "spectra/user_psa_front.mat")
    //     {
    //         gadaj = 1;
    //         std::cout << name_s << std::endl;
    //     }

    //typedef short int slowo;
    if ( flag_must_be_32_bit == false )
    {
        typedef short int slowo;
        slowo *linijka = new slowo[ ( int ) retab[0]] ; // bins x
        for ( int y = 1 ; y <= retab[3] ; y ++ ) // bins y
        {
            plik.read ( ( char* ) linijka, sizeof ( slowo ) * ( ( int ) retab[0] ) ) ;
            for ( int x = 1 ; x <= retab[0] ; x ++ ) // see note above
            {
                ptr_root_spectrum->SetBinContent ( x, y, linijka[x - 1] );
            }
        }
        delete [] linijka ;
    }
    else      // flag_must_be_32_bit == false
    {
        typedef int slowo;
        slowo *linijka = new slowo[ ( int ) retab[0]] ; // bins x
        for ( int y = 1 ; y <= retab[3] ; y ++ ) // bins y
        {
            plik.read ( ( char* ) linijka, sizeof ( slowo ) * ( ( int ) retab[0] ) ) ;
            for ( int x = 1 ; x <= retab[0] ; x ++ ) // see note above
            {
                ptr_root_spectrum->SetBinContent ( x, y, linijka[x - 1] );
            }
        }
        delete [] linijka ;

    }
    plik.close();

    //     for (int i = -100 ; i < 100 ; i++)
    //     {
    //         increment (i,-40 );
    //     }
    //             save_to_disk() ;
    //         if (gadaj)
    //  {
    //   exit(999);
    //  }


    //      std::cout << "Succes " << std::endl;
}

//***********************************************************************
/** No descriptions */
void spectrum_2D::increment_by ( int x, int y, int value )
{
    std::cout << "Empty function" << std::endl;
}

