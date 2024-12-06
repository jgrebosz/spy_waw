#include "TDSSD_ring.h"
// #include "TGalileoAnalysis.h"



// #define COTO  cout << "File: " << __FILE__ << ", line: " << __LINE__ << ", "; // endl;
// #define COTO  cout << "line: " << __LINE__ << ", "; // endl;


//***********************************************************************
//  constructor
TDSSD_ring::TDSSD_ring(string _name, int nr_arg)
    : TNeedle_element(_name),
      // signal_ptr(nullptr),
      detector_nr(nr_arg)
{
    //   cout << "For "
    //   << name_of_this_element


    signal_name[0] = "timestamp";
    signal_name[1] = "energy";
    signal_name[2] = "cfd";


    cal_factors.resize(nr_of_channels);
    for(int i = 0 ; i < nr_of_channels ; i++)
        cal_factors[i].resize(2);

    named_pointer[name_of_this_element + "_rings_multiplicity"] =
            Tnamed_pointer(&rings_multpilicity, 0, this) ;
    named_pointer[name_of_this_element + "_sectors_multiplicity"]
            = Tnamed_pointer(&sectors_multpilicity, 0, this) ;

    named_pointer[name_of_this_element + "_sector_nr_when_OK"] =
            Tnamed_pointer(&active_sector_nr, &multiplicity1_validator, this) ;

    named_pointer[name_of_this_element + "_ring_nr_when_OK"] =
            Tnamed_pointer(&active_ring_nr, &multiplicity1_validator, this) ;


    // other incrementers are defined in f. create_my_spectra
    create_my_spectra();

    // preparing the polar to xy coordinates -----------

    for (int sector = 0 ; sector < SECTORS_NR ; ++sector)
    {
        double phi_deg = 90 - (sector * 11.25); // - 5.75;
        sectors_phi[sector] =  phi_deg;
        double phi_rad = phi_deg * (M_PI / 180);
        double r = 200;

        for (int ring = 0 ; ring < RINGS_NR ; ++ring)
        {

            double theta_deg = 15 + (15 - ring) * 2;  // it was: 125
            double theta_rad = theta_deg * (M_PI / 180);
            rings_theta[ring] = theta_deg;

            Tpara tmp;

            tmp.x = r * sin(theta_rad) * cos(phi_rad);
            tmp.y = r * sin(theta_rad) * sin(phi_rad);
            xy[sector][ring] = tmp;


            // double z = r * cos(theta_rad);
        }   // for rings
    }   // for sectors

}
//************************************************************

void TDSSD_ring::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor
    string folder = "silicons/" + name_of_this_element ;
    //-------------- for testing,  spectra of all ADC and TDCchannels

    // fan spectrum
    int zakres = 500;
    spec_hardware_channels_fan = new spectrum_1D(
                name_of_this_element + "_hardware_channels_fan",
                zakres, 0, zakres,
                folder, " [Rings starts from 0-150, Sectors 160-470 ]      nr * 10, then: \n 0- timestamp, 1- energy, 2- cfd",
                "X: No_such_incrementer");
    frs_spectra_ptr->push_back(spec_hardware_channels_fan) ;

    // Rings fan spectrum
    zakres = RINGS_NR * 10;
    spec_rings_fan = new spectrum_1D(
                name_of_this_element + "_rings_fan",
                zakres, 0, zakres,
                folder, "Rings nr * 10, then: \n 0- timestamp, 1- energy, 2- cfd", "X: No_such_incrementer");
    frs_spectra_ptr->push_back(spec_rings_fan) ;

    // Rings fan spectrum
    zakres = SECTORS_NR * 10;
    spec_sectors_fan = new spectrum_1D(
                name_of_this_element + "_sectors_fan",
                zakres, 0, zakres,
                folder, "Sector nr * 10, then: \n 0- timestamp, 1- energy, 2- cfd", "X: No_such_incrementer");
    frs_spectra_ptr->push_back(spec_sectors_fan) ;



    spec_sectors_multiplicity = new spectrum_1D(
                name_of_this_element + "_sectors_multiplicity",
                40, 0, 40,
                folder, "", "X: sectors_multiplicity");
    frs_spectra_ptr->push_back(spec_sectors_multiplicity) ;

    spec_rings_multiplicity = new spectrum_1D(
                name_of_this_element + "_rings_multiplicity",
                40, 0, 40,
                folder, "", "X: rings_multiplicity");
    frs_spectra_ptr->push_back(spec_rings_multiplicity) ;;


    string opis = "_ring_";
    int nr = 0 ;
    for(int i = 0 ; i < nr_of_channels ; i++)   // rings and sectora
    {

        if(i >= RINGS_NR)
        {
            nr = i - RINGS_NR;
            opis = "_sector_";
        }
        else nr = i;


        // incrementers
        ostringstream plik ;
        plik << name_of_this_element
                //<< "_"
             << opis

             << setfill('0')
             << setw(2)
             << nr << "_" << signal_name[1] ;

        string name = plik.str();

        //      cout << "Zrobgione name = " << name << endl;

        named_pointer[name+"_raw"] =
                Tnamed_pointer(&energy_raw[i], 0, this) ;


        // spectra


        //         cout << "creating spectrum for channel nr " << nr   << " with name: " << name
        //                 << " while nr_of_channels = " << nr_of_channels
        //             << endl;
        int range = 2*8192;
        spec_chan[i] = new spectrum_1D(name +  "_raw",
                                       range, 0, range,
                                       folder, "",
                                       "X: " + name + "_raw" );
        frs_spectra_ptr->push_back(spec_chan[i]) ;


        string name_cal = name  + "_cal";
        named_pointer[name_cal ] = Tnamed_pointer(&energy_cal[i], 0, this) ;

        spec_chan_cal[i] = new spectrum_1D(name_cal,
                                           200, 0, 100,
                                           folder, "",
                                           "X: " + name_cal);
        frs_spectra_ptr->push_back(spec_chan_cal[i]) ;

    } // channels--------------------------------------


    // Tworzenie widma XY

    string name = name_of_this_element + "_map_xy"  ;
    spec_xy = new spectrum_2D(name, name,
                              36,
                              0.0, 36,
                              20,
                              0.0, 20,
                              folder, "",
                              string("X: No_such_incrementer\n") +
                              "Y: No_such_incrementer");
    frs_spectra_ptr->push_back(spec_xy) ;

    string pinup_file = path.dir_my_binnings() + name  + ".mat.pinuptxt";
    ofstream f(pinup_file, ios::trunc);
    if(f)
    {
        f << "10.5 18.5 This is an inner edge of the ring " << endl;
    }
    else {

        cerr << " Warning: impossible to open (for writing) a pinup File : " << pinup_file << endl;
    }

    zakres = 150;
    int pixels = zakres/2; //  /2;
    name = name_of_this_element + "_map_polar"  ;
    spec_polar = new spectrum_2D(name, name,
                                 pixels,
                                 -zakres, zakres,
                                 pixels,
                                 -zakres, zakres,
                                 folder, "",
                                 string("X: No_such_incrementer\n") +
                                 "Y: No_such_incrementer");
    frs_spectra_ptr->push_back(spec_polar) ;

    //    // Tworzenie widma XZ

    //    name = name_of_this_element + "_map_xz"  ;
    //    spec_xz = new spectrum_2D(name, name,
    //                                  pixels,
    //                                  -zakres, zakres,
    //                                  pixels,
    //                                  100, 2500,
    //                                  folder, "",
    //                                  string("X: No_such_incrementer\n") +
    //                                  "Y: No_such_incrementer");
    //    frs_spectra_ptr->push_back(spec_xz) ;

}
//*************************************************************
void TDSSD_ring::make_preloop_action(ifstream &)
{
    cout << "Reading the calibration for " << name_of_this_element << endl ;
    auto lookup = owner->Get_DSSD_LookUpTable();


    // lookup.Read_DSSD_LookUpTable();
    std::map<int,TDSSDDetector*>::iterator it_ = lookup.begin(); // lookup.begin();
    //    int ile= 0 ;
    for (; it_ !=  lookup.end() ; ++it_)
    {
        if(it_->second){
            //            cout << "nazwa = " << it_->second->GetName() << endl;
            int ch = stoi(it_->second->GetName() );
            {


                vector<float> cal = (it_->second)->GetCalibrFactors();
                int i = ch;
                cal_factors[i].clear();

                for(size_t m = 0 ; m < cal.size() ; m++)
                {
                    double tmp = cal[m];
                    cal_factors[i].push_back( tmp);
                }
                //               // vector<float> cal =
                double t; // = (it_->second)->GetThetaDeg();
                double p; // = // (it_->second)->GetPosition().Phi();
                (it_->second)->get_theta_phi_deg(& t, &p);
                // theta_phi_degrees[sect]
                // if this is a ring - give his theta
                if(ch < RINGS_NR )
                {
                     rings_theta[ch] = t     ;
                }else
                {
                    sectors_phi[ch-RINGS_NR] = p;

                }
                // if this is a sector - give his phi
//                                cout << "for " <<ch << " channel " << "got:  phi =  "
//                                      << p << ", theta " << t << endl;
            }  // if getName
        }  // if second
    } // for po lookup table

    //    cout << "After reading DSSD callibration, cal_factors size  "
    //        << cal_factors.size()
    //        << endl;

//    for(size_t i = 0 ; i < cal_factors.size() ; i++)
//    {
//        cout << i << ") " << cal_factors[i][0] << ",  " << cal_factors[i][1] << endl;
//    }

//    cout << "DSSD calibration loaded successfully" << endl;

}
//***********************************************************************************************
bool TDSSD_ring::give_current_theta_phi(double *th, double *ph)
{
    *th = rings_theta[active_ring_nr];
    * ph = sectors_phi[active_sector_nr] ;

//    cout << "DSSD active ring nr =" << active_ring_nr
//         << ", active sector " << active_sector_nr << endl;

    return multiplicity1_validator;
}
//***********************************************************************************************
void TDSSD_ring::analyse_current_event()
{
    //         cout << "analyse_current_event()  for "
    //              <<  name_of_this_element
    //              << endl ;

    rings_multpilicity = 0 ;
    sectors_multpilicity = 0 ;

    multiplicity1_validator = false;
    active_ring_nr = -1;
    active_sector_nr = -1;

//    if(event_unpacked->dssd_fired.empty() == false)
    {

        int nr = 0;
        for(int ch = 0 ; ch < nr_of_channels ; ch++)
        {
            energy_cal[ch] = 0;

            timestamp_raw[ch] = event_unpacked->dssd_data[ch].timestamp;
            energy_raw[ch] = event_unpacked->dssd_data[ch].energy;
            cfd_raw[ch] = event_unpacked->dssd_data[ch].cfd;

            if(event_unpacked->dssd_fired[ch] == false)
            {
                continue;
            }


            if(ch < RINGS_NR) nr = ch;
            else nr = ch - RINGS_NR ;

            if((energy_raw[ch] != 0) )
            {
                spec_chan[ch]->increment(energy_raw[ch]);
                if(ch < RINGS_NR){
                    rings_multpilicity++;
                    active_ring_nr = ch;
                }
                else{
                    sectors_multpilicity++;
                    active_sector_nr = ch - RINGS_NR;
                }

                // fans --------------------------------
                if(timestamp_raw[ch])
                {
                    spec_hardware_channels_fan->increment( (ch*10) );
                    if(ch < RINGS_NR) spec_rings_fan->increment( (nr*10) );
                    else spec_sectors_fan->increment( (nr*10) );

                }

                if(energy_raw[ch])
                {
                    spec_hardware_channels_fan->increment( (ch*10) + 1 );
                    if(ch < RINGS_NR) spec_rings_fan->increment( (nr*10) +1);
                    else spec_sectors_fan->increment( (nr*10) +1);

                }
                if(cfd_raw[ch])
                {
                    spec_hardware_channels_fan->increment( (ch*10) +2);
                    if(ch < RINGS_NR) spec_rings_fan->increment( (nr*10)+2 );
                    else spec_sectors_fan->increment( (nr*10) +2);

                }
            }

            // calibrating -- only energy -----------------


            double result_cal = 0;
            //result_cal = 0;
            double tmp_val = energy_raw[ch] + randomizer::randomek() ;
            for(UInt_t i=0; i < cal_factors[ch].size();++i)
                result_cal += cal_factors[ch][i]*pow(tmp_val,(int)i);

            energy_cal[ch] = result_cal;

            spec_chan_cal[ch]->increment(energy_cal[ch]);

        } // end for ch (all strips)
        //----------------------------------------------------

        spec_sectors_multiplicity->increment(sectors_multpilicity);
        spec_rings_multiplicity->increment(rings_multpilicity);

        // Incrementing xy maps ---------------------------------
        if(sectors_multpilicity == 1 && rings_multpilicity == 1)
        {

            //spec_xy_raw->increment(last_sector_nr, 15 - last_ring_nr);
            spec_xy->increment(active_sector_nr, active_ring_nr);

            multiplicity1_validator = true;


            double x = xy[active_sector_nr][active_ring_nr].x;
            double y = xy[active_sector_nr][active_ring_nr].y;


            spec_polar->increment(x, y);
            // cout << "x = " << x << ", z = " <<z << endl;

        } // if multipl == 1

        return;
    }
    calculations_already_done = true ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#endif // _dssd_ring_H_


