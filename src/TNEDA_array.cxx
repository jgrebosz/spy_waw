#include "TNEDA_array.h"
#include <dirent.h> // for scandir
//#include <malloc.h> // for scandir
#include <stdlib.h>

#include <sstream>


#include "TGalileoAnalysis.h"
#include "Tnamed_pointer.h"
TGalileoCalibratedEvent *TNEDA_array::tmp_event_calibrated;
TGalileoEvent *TNEDA_array::tmp_event_unpacked;

int TNEDA_array::multiplicity_raw;
int TNEDA_array::multiplicity_good;
std::map<int,TNEDADetector*> TNEDA_array::lookup;

using namespace std;

//extern int local_mult_tmp ;
// const std::string TNEDA_array::cluster_characters =
//     (HOW_MANY_GE_CRYSTALS == 8) ? ("ABCDGJKL")
//     : ("ABCDEFGJKLMNPQR") ;

// UWAGA: Aby wejść do edycji "blokowej" w Qtcreatorze
// wystarczy po wcisnięciu myszki wcisnąć też klawisz ALT
// i wtedy ciągnąć myszką zaznaczenie bloku.


//*******************************************************
//*******************************************************
//         Neda detectors array class
//*******************************************************
//*******************************************************
TNEDA_array::TNEDA_array(std::string name) : TNeedle_element(name),
    spectra_vector(*GalileoAnalysis_ptr->give_gal_ptr()->address_spectra_frs())
{
    //std::cout << "\n\n\n\nConstructor of the Rising Class =========================== " << std::endl ;
    //Tcrystal::rising_pointer = this ; // for crystals

    TNEDA_crystal::array_nw_pointer = this ; // for crystals

    //Tcluster::rising_pointer = this ; // for cluster

    //     dynamic_cast<TRisingAnalysis*> (TGo4Analysis::Instance());

    galileo_pointer = GalileoAnalysis_ptr->give_gal_ptr()  ;  // we will need if to ask traget for theta angles
    //spectra_vector = gal_pointer->address_spectra_frs();
    //gal_pointer->address_spectra_frs();


    //---------------------------------------------
    // constructing the current setup of Galileo
    //---------------------------------------------
    detector_neda.clear();
    TNEDA_array::lookup = galileo_pointer->Get_NEDA_LookUpTable();
    std::map<int,TNEDADetector*>::iterator it_ = TNEDA_array::lookup.begin();
    for (; it_!=TNEDA_array::lookup.end() ; ++it_) {
        if(it_->second){

            std::string name = it_->second->GetName().c_str();
            // if the name taken from the lookup table starts with #, this is a speciall type of
            // data, not connected to the Neda, so the Neda prefix will be skipped
            if(name[0] != '#')
            {

                detector_neda[it_->first] = new TNEDA_crystal(
                            // Form("NEDA_%.3i_%s",it_->first,it_->second->GetName().c_str()), this, it_->first);
                            "NEDA_" + name  ,  this, it_->first);
            }else{
                // another type of detector (not true Neda)
                name = name.substr(1);
                detector_neda[it_->first] = new TNEDA_crystal(
                            name, this, it_->first);
            }
        }
    }

    create_my_spectra();

    // default coordinates of gates for recognising neutrons
    vector<double> coord_1 = {

        0.252882, 29.8159,
        0.261073, 57.2165,
        0.418082, 60.429,
        0.471329, 60.0511,
        0.646087, 59.1062,
        0.86863, 57.9724,
        0.861803, 31.7056,
        0.82221, 25.2807,
        0.738927, 20.7454,
        0.465868, 7.5175,
        0.342991, 7.32853,
        0.258343, 9.02925,
        0.220114, 11.1079,
        0.22421, 16.588,
        0.252882, 29.8159};

    reading_neutron_polygon(name_of_this_element + "_total_rtime_vs_psdlf_polygon_neutron_a.poly", &neutron_cclf_cut_polygon_ptr, coord_1);

    vector<double> coord_2 = {
        0.267254, 58,
        0.871773, 58,
        0.85067, -0.001,
        0.26, -0.001,
        0.243669, 13.3169,
        0.267254, 58};
    reading_neutron_polygon(name_of_this_element + "_total_rtime_vs_psdlf_polygon_neutron_b.poly", &neutron_second_t2_cut_polygon_ptr, coord_2);

    // -----------
    vector<double> triangel = {
        209.5, 12.5,
        1072.1,  59.6,
        1066.5, -56.45,
        209.5, 12.5
    };
    reading_neutron_polygon(name_of_this_element + "_dt_vs_dr_polygon_triangel.poly", &triangel_polygon_ptr, triangel);

    //-------------------------------------------------------
    named_pointer["Neda_multiplicity_raw"]
            = Tnamed_pointer(&multiplicity_raw, nullptr, this) ;

    named_pointer["Neda_multiplicity_of_good"]
            = Tnamed_pointer(&multiplicity_good, nullptr, this) ;

    named_pointer[name_of_this_element + "_nr_of_clean_neutrons"]
                = Tnamed_pointer(&nr_of_clean_neutrons, nullptr, this) ;

    named_pointer[name_of_this_element + "_dr"]
                = Tnamed_pointer(&dr, &dr_valid, this) ;
    named_pointer[name_of_this_element + "_dt"]
                = Tnamed_pointer(&dt, &dt_valid, this) ;

}
//*************************************************************************
void TNEDA_array::create_my_spectra()
{

    std::string folder = "clusters/" ;

    //=============== calibrated spectra (for Gold lines) =====
    // energy cal ------------------------------------

    std::string name;
    //int galileo_ring[6]={152,129,119,90,61,51};
    

    // FAN spectrum of all crystals----------------------------------------
    name = name_of_this_element + "_fan"  ;
    spec_hitpattern = new spectrum_1D(
                name,
                name,
                HOW_MANY_NEDA, 0, HOW_MANY_NEDA,
                folder,
                "Statistics of Neda detectors which fired"
                );
    spectra_vector.push_back(spec_hitpattern) ;

    name = name_of_this_element + "_light_cal_vs_detID";
    spec_light_cal_id = new spectrum_2D(
                name,
                name,
                64,-.5,63.5,
                2048,0,65535,
                folder,
                "",  // note
                "INFO: no incrementer available"
                "\rY:\r ALL_Neda_light_cal_when_good"
                );
    spectra_vector.push_back(spec_light_cal_id);

    name = name_of_this_element + "_qshort_vs_detID";
    spec_qshort_id = new spectrum_2D(
                name,
                name,
                64,-.5,63.5,
                2048,0,65535,
                folder,
                "",  // note
                "INFO: no incrementer available"
                "\rY:\r ALL_Neda_qshort_when_good"
                );
    spectra_vector.push_back(spec_qshort_id);

    name = name_of_this_element + "_qlong_vs_detID";
    spec_qlong_id = new spectrum_2D(
                name,
                name,
                64,-.5,63.5,
                2048,0,65535,
                folder,
                "",  // note
                "INFO: no incrementer available"
                "\rY:\r ALL_Neda_qlong_when_good");
    spectra_vector.push_back(spec_qlong_id);

    name = name_of_this_element + "_qfast_vs_detID";
    spec_qfast_id = new spectrum_2D(
                name,
                name,
                64,-.5,63.5,
                2048,0,65535,
                folder,
                "",  // note
                "INFO: no incrementer available"
                "\rY:\r ALL_Neda_qfast_when_good"
                );
    spectra_vector.push_back(spec_qfast_id);

    name = name_of_this_element + "_qslow_vs_detID";
    spec_qslow_id = new spectrum_2D(
                name,
                name,
                64,-.5,63.5,
                2048,0,65535,
                folder,
                "",  // note
                "INFO: no incrementer available"
                "\rY:\r ALL_Neda_qslow_when_good"
                );
    spectra_vector.push_back(spec_qslow_id);

    name = name_of_this_element + "_psdcaen_vs_detID";
    spec_psdcaen_vs_id = new spectrum_2D(
                name,
                name,
                64,-.5,63.5,
                1024,-0.5,1.5,
                folder,
                "",  // note
                "INFO: no incrementer available"
                "\rY:\rALL_Neda_psdcaen_shifted_when_good"
                );
    spectra_vector.push_back(spec_psdcaen_vs_id);

    name = name_of_this_element + "_psdlf_vs_detID";
    spec_psdlf_vs_id = new spectrum_2D(
                name,
                name,
                64,-.5,63.5,
                1024,-0.5,1.5,
                folder,
                "",  // note
                "INFO: no incremeter for available"
                "\rY:\r ALL_Neda_psdlf_shifted_when_good"
                );
    spectra_vector.push_back(spec_psdlf_vs_id);

    // ============== Spectrum of PSD vs QLong integrated by FPGA
    name = name_of_this_element + "_psdcaen_vs_qlong";
    spec_PSDcaen_vs_id = new spectrum_2D(
                name,
                name,
                2048, 0, 65535,
                512,0,1,
                folder,
                "",  // note
                "X:\r ALL_Neda_qlong_when_good"
                "\rY:\r ALL_Neda_psdcaen_shifted_when_good");
    
    
    spectra_vector.push_back(spec_PSDcaen_vs_id);

    //--------------

    name = name_of_this_element + "_total_rtime_vs_psdlf";
    spec_rtime_vs_psdlf = new spectrum_2D(
                name,
                name,
                500, 0, 1,
                310,-10, 300,
                folder,
                "",  // note
                "X:\r ALL_Neda_qlong_when_good"
                "\rY:\r ALL_Neda_psdcaen_shifted_when_good");


    spectra_vector.push_back(spec_rtime_vs_psdlf);
    //----------------

    name = name_of_this_element + "_total_rtime";
    spec_rtime = new spectrum_1D(
                name,
                name,

                550,-10,1000,
                folder,
                "",  // note
                "X:\r ALL_Neda_rtime_when_good");
    spectra_vector.push_back(spec_rtime);

#if 1
    //--------------
    name = name_of_this_element + "_total_psdlf";
    spec_psdlf = new spectrum_1D(
                name,
                name,

                500, 0, 1,
                folder,
                "",  // note
                "X:\r ALL_Neda_psdlf_shifted_when_good")
                ;
    spectra_vector.push_back(spec_psdlf);
#endif //


    // ============== Spectrum of PSD vs Light integrated by LF
    name = name_of_this_element + "_psdlf_vs_light_raw";
    spec_PSDlf_vs_light_raw = new spectrum_2D(
                name,
                name,
                2048, 0, 65535,
                512,0,1,
                folder,
                "",  // note
                "X:\r ALL_Neda_light_raw_when_good"
                "\rY:\r ALL_Neda_psdlf_shifted_when_good");


    spectra_vector.push_back(spec_PSDlf_vs_light_raw);

    // ============== Spectrum of PSD vs Light integrated by LF   CALIBRATED
    name = name_of_this_element + "_psdlf_vs_light_cal";
    spec_PSDlf_vs_light_cal = new spectrum_2D(
                name,
                name,
                2048, 0, 65535,
                512,0,1,
                folder,
                "",  // note
                "X:\r ALL_Neda_light_cal_when_good"
                "\rY:\r ALL_Neda_psdlf_shifted_when_good");


    spectra_vector.push_back(spec_PSDlf_vs_light_cal);




    name = name_of_this_element + "_multiplicity_raw"  ;
    spec_multiplicity_raw = new spectrum_1D(
                name,
                name,
                52, 0, 52,
                folder, "multiplicy (raw) of NEDA ",
                "Neda_multiplicity_raw");
    spectra_vector.push_back(spec_multiplicity_raw) ;


    name = name_of_this_element + "_multiplicity_of_good"  ;
    spec_multiplicity_of_good = new spectrum_1D(
                name,
                name,
                52, 0, 52,
                folder, "multiplicy (raw) of NEDA ",
                "Neda_multiplicity_raw");
    spectra_vector.push_back(spec_multiplicity_of_good) ;

    name = name_of_this_element + "_total_psd_lf_shifted";
    spectra_vector.push_back(
                spec_total_psd_lf_shifted = new spectrum_1D(
                name, name,
                1500, 0, 1.5,
                folder,
                "PSD calculated in LF (shifted)",
                "ALL_Neda_psdlf_shifted_when_good")

            );




    name = name_of_this_element + "_total_psd_caen_shifted";
    spectra_vector.push_back(
                spec_total_psd_caen_shifted = new spectrum_1D(
                name,name,
                1500, 0, 1.5,
                folder,
                "PSD calculated in FPGA (shifted)",
                "ALL_Neda_psdcaen_shifted_when_good")
            );



    name = name_of_this_element + "_dt_vs_dr";
    spec_hNe_dt_vs_dr = new spectrum_2D(
                name,
                name,
                600, 0, 1200,
                300,-80, 80,
                folder,
                "",  // note
                "INFO: no incrementer available"
                "\rY:\r x x x x"
                );
    spectra_vector.push_back(spec_hNe_dt_vs_dr);


    //-------------------- MONITOR -------------------------
    int ile = 52;
    int margines = 2;
    int margines_gorny = 3;

    int how_long_row = 10;

    name = name_of_this_element + "_monitor";
    spec_geometry_monitor = new spectrum_2D(name,
                                            name,
                                            2*(how_long_row+ 2*margines), -margines, how_long_row + margines ,
                                            (10 + margines + margines_gorny), -10 - margines, margines_gorny,
                                            folder,
                                            "Monitoring",  // note
                                            "INFO: No incrementers avaliable here"
                                            );

    spectra_vector.push_back(spec_geometry_monitor);

    picture_width = how_long_row+margines + margines;
    // opis mapy monitorowej
    // otwarcie pliku "nazwa macierzy" + ".pinuptxt" jako nowego (trunc)

    string pinup_file = path.dir_my_binnings() + name  + ".mat.pinuptxt";
    ofstream f(pinup_file, ios::trunc);
    if(f)
    {
        // cout << "Otwarcie OK" << endl;
        f << "2.5 2.5 The Spy resets this graph every " << (HOW_MANY_SECONDS_TO_RESET / 60.0 )<< " min "  << endl;
        f << "-1 0.5 NEDA "  << endl;                // x  y  etykieta_Ge_00
        //f << "-1 -2.5 ACS " << endl;
        for(int i = 0 ; i < ile ;++i)
        {
            int x = i % 10 ;
            int row = (i / 10) * -2;

            f << x << " " << (row - 0.2)  << " " << i  << endl;                // x  y  etykieta_Ge_00
            //f << x << " -3.2 " << i  << endl;                // x  y  etykieta_Ge_00
        }

    }
    else {

        cerr << " Warning: impossible to open (for writing) a pinup File : " << pinup_file << endl;
    }


}
//*************************************************************************
bool TNEDA_array::reading_neutron_polygon(string name, TjurekPolyCond **polygon, std::vector<double> vec)
{

    if(*polygon == nullptr){
        // create this polygon object
        *polygon = new TjurekPolyCond(name);
    }
    // try to read it from a disk

    if (!read_banana(name, polygon))
    {
        std::cout << "During Reading-in the condition named "
                  << name
                  << "\n  Impossible to read polygon gate: " << name
                  << "\nMost probably it does not exist (yet)"
                  << std::endl;
        // should I repair the stream?
        //FH::repair_the_stream(plik);

        // if there is nothing on a disk, we have to create it

        vector<double> iksy  ;
        vector<double> igreki ;
        for(uint i = 0 ; i < vec.size(); i += 2)
        {
            iksy.push_back(vec[i]);
            igreki.push_back(vec[i+1]);
        }

        (*polygon)->SetValues(&vec[0], &igreki[0], int(iksy.size()) ) ;
        (*polygon)->Enable() ;
        GalileoAnalysis_ptr->AddAnalysisCondition(*polygon);

        // and store it on a disk --------------------------------


        string pathed_name = std::string("./system/polygons/") + name;

        ofstream plik(pathed_name.c_str());

        if(!plik)
        {
            cout << "Polygon file " << pathed_name  << " can not be opened " << endl ;
            return false ;
        }

        for(uint i = 0 ; i < iksy.size() ; ++i)

        {
            plik <<  iksy[i]  << "     " <<  igreki[i] << "\n";

        }

#if 0
#endif


    }
    return true;
}
//*************************************************************************
void TNEDA_array::make_preloop_action([[maybe_unused]] std::ifstream & file)
{
    tmp_event_unpacked   = event_unpacked ;
    tmp_event_calibrated = TGalileoAnalysis::fxCaliEvent;
    //    std::cout << "TNEDA_array::make_preloop_action "  << std::endl;

    //simulate_event();

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // very important:
    // calibration factors and gates are read everytime we start/stop the analysis

    read_good_crystal_specification();
    read_calibration_from_lookuptable();
    read_neda_xyz();


    // zeroing monitoring statistics for the watchdog
}
//**********************************************************************
void TNEDA_array::read_calibration_from_lookuptable()
{

    filename_psd_lf = path.dir_experiment_setup()  + "neda_psdlf_positions.txt";
    filename_psd_caen = path.dir_experiment_setup()  + "neda_psdcaen_positions.txt";
    filename_neda_time_alignment= path.dir_experiment_setup()  + "neda_time_positions.txt";

    file_psd_lf = ifstream(filename_psd_lf);
    file_psd_caen = ifstream(filename_psd_caen);
    file_time_alignment = ifstream(filename_neda_time_alignment);

    std::map<int,TNEDADetector*>::iterator it_ = TNEDA_array::lookup.begin();
    for (; it_!=TNEDA_array::lookup.end() ; ++it_) {
        if(it_->second){
            detector_neda[it_->first]->make_preloop_action(it_->second);
        }
    }

    file_psd_lf.close();
    file_psd_caen.close() ;
    file_time_alignment.close();
    std::cout << "Neda  calibration loaded successfully" << std::endl;
}
//********************************************************
//**********************************************************************
void TNEDA_array::read_neda_xyz()
{

    neda_xyz.clear();

    string filename_xyz = path.dir_experiment_setup()  + "neda_xyz.txt";

    auto file = ifstream(filename_xyz);
    if(!file){
        cout << "Can't open file " << filename_xyz << endl;
        exit(234);
    }
    // return;
    try{
        //    FH::spot_in_file(file, "00");
        double nr, x, y, z;
        //    file >> zjedz ;
        //    >> x >> y >> z ;
        //    neda_xyz.push_back(TVector3(x, y, z));
        do {

            file >> zjedz >> nr ; ;
            if(!file) break;
            file >> x >> y >> z ;
            if(!file) break;
            neda_xyz.push_back(TVector3(x, y, z));
//            cout << "wczytania linia dla nr = " << nr << endl;

            // perhaps would be nice to have this information also in the particular
            // neda detectors
            // detector_neda[nr+1]->set_xyz(TVector3(x, y, z));


        } while (file);

    }catch(Tno_keyword_exception e)
    {
        cout << "Tno_keyword_exception..." << e.message << endl;
    }
    catch(Treading_value_exception e)
    {
        cout << "Treading_value_exception..." << e.message << endl;
    }


    //cout << "Neda xyz information is read in. lines =  " << neda_xyz.size() << endl;


}
//********************************************************
void TNEDA_array::analyse_current_event()
{
    //   cout << __PRETTY_FUNCTION__ << endl;

    multiplicity_raw = 0 ;
    multiplicity_good = 0 ;
    nr_of_clean_neutrons = 0;
    dr_valid = false;
    dt_valid = false;
    dr = 0;
    dt = 0;

    tsdiff=0.;
    timediff=0.;
    times_of_crystals.clear();

    for(auto  wsk : detector_neda)
    {
        // cout << "FFF" << wsk.first << endl;
        wsk.second->zeroing_good_flags();// to zero flags in detectors which do not fired
    }

    // Loop over particular detectors
    if(tmp_event_unpacked->neda_fired.size()>0){
        std::map<int,bool>::iterator it_ = tmp_event_unpacked->neda_fired.begin();
        for(;it_ != tmp_event_unpacked->neda_fired.end();++it_)
        {
            //      std::cout << "it_first, it_second" << it_->first << " , "  << it_->second<< std::endl;
            if (it_->second) {    // if really fired
                int indeks = it_->first;
                detector_neda[indeks]->analyse_current_event();
                // filling global histograms
                int idDet = detector_neda[indeks]->give_det_name_nr();

                //                std::cout << " po analizie detektor Neda (name) = "
                //                          << detector_neda[indeks]->give_name()
                //                          <<  " indeks = " << indeks
                //                           << "  it_first = "
                //                           << it_->first
                //                           << " it_second (bool) =" << it_->second
                //                           << " , idDet = "  << idDet << std::endl;

                spec_light_cal_id->increment(idDet,detector_neda[indeks]->give_light_cal());
                spec_qshort_id->increment(idDet,detector_neda[indeks]->give_qshort());
                spec_qlong_id->increment(idDet,detector_neda[indeks]->give_qlong());
                spec_qfast_id->increment(idDet,detector_neda[indeks]->give_qfast());
                spec_qslow_id->increment(idDet,detector_neda[indeks]->give_qslow());
                spec_psdcaen_vs_id->increment(idDet,detector_neda[indeks]->give_psdcaen());
                spec_psdlf_vs_id->increment(idDet,detector_neda[indeks]->give_psdlf());

                spec_PSDlf_vs_light_raw->increment(detector_neda[indeks]->give_light_raw(),
                                               detector_neda[indeks]->give_psdlf());

                spec_PSDlf_vs_light_cal->increment(detector_neda[indeks]->give_light_cal(),
                                               detector_neda[indeks]->give_psdlf());
                if(it_->first < 52)
                {
                    spec_PSDcaen_vs_id->increment(detector_neda[indeks]->give_qlong(),
                                                  detector_neda[indeks]->give_psdcaen());
                }

                if(detector_neda[indeks]->give_flag_this_is_true_neda())
                {
                    //                    cout << "True neda - zapakowanie do times_of_crystals czasu real_time = "
                    //                         << detector_neda[indeks]->give_real_time()
                    //                         << " z detektora  o indeksie ["
                    //                         << it_->first
                    //                         << "] i jego adresu  "
                    //                         << detector_neda[indeks] << endl;

                    times_of_crystals.push_back(
                                dublet( detector_neda[indeks]->give_real_time(), indeks)
                                )
                            ;
                    //                    cout << "Bieżaca Tresc times_of_crystals" << endl;
                    //                    for(auto t : times_of_crystals)
                    //                    {
                    //                        cout << "time = " << cout.precision(20) << t.timest
                    //                             << " in det[" << t.key<< "], "
                    //                             << endl;
                    //                    }

                    spec_hitpattern->increment(idDet);

                    int x = idDet % 10 ;
                    int y = idDet / 10;
                    y *= -2;


                    // kind of legend ...
                    //                    for(int n = 0 ; n < 15 ; ++n)
                    //                        spec_geometry_monitor->increment(n, 1);

                    // true plot
                    spec_geometry_monitor->increment(x, y);
                    //cout << "inkrementacja monitor id = " << idDet << " x = " << x << ", y = " << y << endl;

                }
            }
        }

        //        cout << "Tresc times_of_crystals" << endl;
        //        for(auto t : times_of_crystals)
        //        {
        //            cout << "time = " << cout.precision(20) << t.timest
        //                 << " in det[" << t.key << "], "
        //                 << endl;
        //        }

        process_neda();

    }

    spec_multiplicity_raw->increment(multiplicity_raw);
    spec_multiplicity_of_good->increment(multiplicity_good);

    // Erasing monitor spectrum every 2 minutes------------------------

    auto elapsed = time(nullptr)- last_time_of_erasing_monitor_spectra;
    static double ostatni_dodany_kafelek_paska = 0;
    if(elapsed > HOW_MANY_SECONDS_TO_RESET )
    {
        // cout << "Reset of monitor spectrum" << endl;
        spec_geometry_monitor->give_root_spectrum_pointer()->Reset();
        last_time_of_erasing_monitor_spectra = time(nullptr);
        ostatni_dodany_kafelek_paska = 0;
    }else {

        double suwak = elapsed *picture_width / HOW_MANY_SECONDS_TO_RESET ;

        if(suwak > ostatni_dodany_kafelek_paska)
        {
            spec_geometry_monitor->increment(suwak-2, 2);
            //spec_geometry_monitor->increment(suwak+0.5, -2);

            ostatni_dodany_kafelek_paska += 0.5;
            //            cout << "\nElapsed = " << elapsed << ",  Quarter = " << suwak
            //                 << ", ostatni dodany = " << ostatni_dodany_kafelek_paska << endl;
        }
    }
    //-----------------------------------------------------------------


    //std::cout << "ANALIZING GE Trising::analyse_current_event() GE"<<std::endl;

    // here, for testing purposes, we want pack artificially to the event
    //simulate_event();

    //  std::cout << "============ llNext event ================" << std::endl;
    //  std::cout << tmp_event_unpacked->eagle_fired.size() << std::endl;

    calculations_already_done = true ;
}
//***********************************************************************
void  TNEDA_array::make_postloop_action()
{
    // std::cout << "F.Trising::make_postloop_action() " << std::endl ;

    // save all spectra made in frs objects and in frs elements
    // (mw, sci, etc.)
    //     std::cout <<  "Saving Rising " << spectra_rising->size() << " spectra... " << std::endl;
    //     for (unsigned int i = 0 ; i < spectra_rising->size() ; i++) {
    //         spectra_rising[i]->save_to_disk() ;
    //     }
    std::cout <<  "Saving Rising spectra - done " << std::endl;
}
//***********************************************************************
void TNEDA_array::simulate_event()
{
    //    for(unsigned int i = 0 ; i < detector.size() ; i++)
    //    {
    //        //std::cout << "Loop i =  " << i << std::endl;
    //        for(int cri = 0 ; cri < 7 ; cri++)
    //        {
    //            tmp_event_unpacked->Cluster_data[i][cri][0] =
    //                (100*i) +(10*cri) +0 ; // energy
    //            tmp_event_unpacked->Cluster_data[i][cri][1] =
    //                (100*i) +(10*cri) +1;  // time
    //        }
    //        // remember the source of data
    //    }
}
//**********************************************************************
/** No descriptions */
bool TNEDA_array::save_selected_spectrum(std::string name)
{
    for (unsigned int i = 0 ; i < spectra_vector.size() ; i++) {
        if (name == spectra_vector[i]->     give_name()) {
            spectra_vector[i]->save_to_disk() ;
            return true ;
        }
    }
    return false ;  // was not here, perhaps in frs or hector?
}
//***********************************************************************
/** Depending on the status of hardware we can use different algorithms */
void TNEDA_array::read_doppler_corr_options()
{}
//*******************************************************************
/** Rereading the definiton of the condiotinal (gated) spectra */
void TNEDA_array::read_def_conditional_spectra()   // called form preloop
{
    // here should be a function for retrieving the names.
    std::vector<std::string> names =
            Tfile_helper::find_files_in_directory(path.dir_user_def_spectra(),
                                                  ".user_definition");

    // loop which is looking into the directory for a specified definions *.
    for (unsigned nr = 0 ; nr < names.size() ; nr++) {
        std::cout << "User defined conditional file :" << names[nr] << std::endl;
        create_cond_spectrum(names[nr]);
    }
}
//*******************************************************************************
/** Create one conditional spectrum */
void TNEDA_array::create_cond_spectrum([[maybe_unused]] std::string name)
{
#ifdef NIGDY
#endif

}
//********************************************************************************
/** where the user spectra can be incremented */
void TNEDA_array::user_spectra_loop()
{
#ifdef NIGDY
#endif

}
//**************************************************************

//**************************************************************
void TNEDA_array::read_good_crystal_specification()
{
    // reading from the file
    std::string pname =  path.dir_options() + "good_galileo_germanium.txt" ;
    std::ifstream plik(pname.c_str());
    if (!plik) {
        std::cout << "Can't open file " << pname << std::endl;
        // so  we set the default values
        //------- En 4

        TNEDA_crystal::flag_good_energy_requires_threshold = true ;
        TNEDA_crystal::energy_threshold_lower = 20 ;
        TNEDA_crystal::energy_threshold_upper = 8*8192 ;
        // everything else is zero (because  it is  static)
        return ;
    }
    try {
        // reading

        TNEDA_crystal::flag_increment_energy_cal_with_zero   = false;
        //             (bool) Tfile_helper::find_in_file(plik, "increment_energy_cal_with_zero") ;

        TNEDA_crystal::flag_increment_time_with_zero = false;
        //             (bool) Tfile_helper::find_in_file(plik, "increment_time_with_zero") ;


        //------- En 4 ---------------------
        TNEDA_crystal::flag_good_energy_requires_threshold =
                (bool) Tfile_helper::find_in_file<bool>(plik, "good_energy_requires_threshold") ;

        TNEDA_crystal::energy_threshold_lower =
                Tfile_helper::find_in_file<double>(plik, "energy_threshold_lower") ;

        TNEDA_crystal::energy_threshold_upper =
                Tfile_helper::find_in_file<double>(plik, "energy_threshold_upper") ;
        // time -------
        TNEDA_crystal::flag_good_time_requires_threshold =
                (bool) Tfile_helper::find_in_file<bool>(plik, "good_time_requires_threshold") ;


        // The new version has time threshold for LOW and High, so actually this is not a threshold
        // but just a time gate
        try {  // the new style
            TNEDA_crystal::good_time_threshold_lower =
                    Tfile_helper::find_in_file<double>(plik, "good_time_threshold_lower") ;

            TNEDA_crystal::good_time_threshold_upper =
                    Tfile_helper::find_in_file<double>(plik, "good_time_threshold_upper") ;


        } catch (...) { // perhaps it is an old style
            // at first repair failed stream
            std::cout << "You are using the old style definiotion of 'good germanium'" << std::endl;
            // repair the stream
            plik.clear(plik.rdstate() & ~(std::ios::eofbit | std::ios::failbit));

            // now try to read
            TNEDA_crystal::good_time_threshold_lower =
                    Tfile_helper::find_in_file<double>(plik, "good_time_threshold") ;

            TNEDA_crystal::good_time_threshold_upper = 99999;
        }

    } catch (Tfile_helper_exception k) {
        std::cout << "Error while reading " << pname << "\n" << k.message << std::endl;
        exit(1);
    }
}
//*************************************************************************************************************
void TNEDA_array::process_neda()
{

    if(times_of_crystals.size() <1 ) return;

    //    cout << __PRETTY_FUNCTION__ << " ============================================" << endl;


    //times_of_crystals.clear();
    //times_of_crystals.push_back(dublet(100, 1));
    //times_of_crystals.push_back(dublet(200, 2));
    //times_of_crystals.push_back(dublet(300, 3));


    // times_of_crystals.push_back(55);
    //    cout << "Przed szukaniem minimal czasu: w tablicy jest pozycji " << times_of_crystals.size()
    //         << endl;
    //    for(auto t : times_of_crystals)
    //    {
    //        cout << "time = "
    //                //<< cout.precision(20)
    //             << t.timest
    //             << " in det[" << t.key << "], "
    //             << endl;
    //    }
    //cout << "Po ich wylistowaniu" << endl;
    //  sort all NEDA data in order of neda.time
    ulong keymin = 999999;
    ulong indmin = 999999;
    if(times_of_crystals.size() > 0 )
    {
        //        sort(   &times_of_crystals[0].timest,
        //                &times_of_crystals[times_of_crystals.size()].timest);
        auto minimal = times_of_crystals[0].timest;
        keymin = times_of_crystals[0].key;
        indmin = 0;
        // instead we find the minimum
        for(ulong i = 0 ;  i < times_of_crystals.size() ; ++i)
        {
            if(times_of_crystals[i].timest < minimal)
            {
                minimal = times_of_crystals[i].timest;
                keymin = times_of_crystals[i].key;
                indmin = i;
            }
        }


        //        cout << "\nPo znalezieniu minimum \n" ;

        //       cout << "timest = " << minimal
        //             << " det[" << keymin << "], " << endl;

        //        cout << endl;
    }


    // reference time is the smallest
    double time_ref =  times_of_crystals[indmin].timest; //use time of first neda as a time ref. for the event.
    int  time_ref_type = 0;


    // check if it is a gamma ---------------
    if (  detector_neda[keymin]-> give_psdlf() < 0.3) {
        time_ref_type = 1;
    }
    else {
        time_ref_type = 2;
    }



    for(uint nr = 1 ; nr < times_of_crystals.size(); ++nr)
    {
        //        cout << "trying to access detector_neda nr (key) = "
        //             << times_of_crystals[nr].key
        //             << ", timest = " << times_of_crystals[nr].timest
        //             << endl;
        //                cout << "After step2,  rtime = "
        //                     <<
        //                        detector_neda[ times_of_crystals[nr].key]->give_real_time()
        //                     << endl;
        detector_neda[times_of_crystals[nr].key] ->step2_make_rtime(time_ref);
        //event.neda[i_list].rtime = event.neda[i_list].time - time_ref;


        spec_rtime_vs_psdlf->increment(
                    detector_neda[ times_of_crystals[nr].key] ->give_psdlf(),
                detector_neda[ times_of_crystals[nr].key]->give_real_time());

        spec_rtime->increment(
                    detector_neda[ times_of_crystals[nr].key]->give_real_time());

        spec_psdlf->increment(
                    detector_neda[ times_of_crystals[nr].key] ->give_psdlf() );

    }
    // It seams, that we will need TWO polygons set on the TOTAL matrix:
    // rtime vs cclf
    //

    //------------ NEUTRONS ------------------------------

    ulong nneda = times_of_crystals.size();
    ulong k_list= 0;
    vector<NEUTRON_DATA> neutron;   //[MAX_NEDA];
    neutron.resize(nneda);
    //memset(neutron,0,sizeof(neutron));


    for (uint i_list=0; i_list < nneda; i_list++) {
        TNEDA_crystal * detptr = detector_neda[times_of_crystals[i_list].key];

        if (time_ref_type==1) {   // is a gamma ?

            // if it is inside of some polygon???

            if (neutron_cclf_cut_polygon_ptr &&
                    neutron_cclf_cut_polygon_ptr->Test(detptr->give_psdlf(),       // x
                                                       detptr->give_real_time()) == true)
            {


                neutron[k_list].id = detptr->give_det_name_nr();
                neutron[k_list].i_list = i_list;
                neutron[k_list].rtime = detptr->give_real_time();
                neutron[k_list].cclf   =  detptr->give_psdlf();
                neutron[k_list].ccfpga =  detptr->give_psdcaen();
                neutron[k_list].light =  detptr->give_light_cal();
                neutron[k_list].ban_result = 1;

                k_list++;
            }
        }
        else if (time_ref_type==2)
        {
            if (neutron_second_t2_cut_polygon_ptr &&      // the first neutron is in this cut as well
                    neutron_second_t2_cut_polygon_ptr->
                    Test(detptr->give_psdlf(),       // x
                         detptr->give_real_time())     // y
                    )
            {

                neutron[k_list].id = detptr->give_det_name_nr();
                neutron[k_list].i_list = i_list;
                neutron[k_list].rtime = detptr->give_real_time();
                neutron[k_list].cclf   =  detptr->give_psdlf();
                neutron[k_list].ccfpga =  detptr->give_psdcaen();
                neutron[k_list].light =  detptr->give_light_cal();
                neutron[k_list].ban_result = 2;
                k_list++;
            }
        }
    }
    int nr_neutrons=k_list;

    int nr_scatters = 0;
    int nr_separated = 0;

    for (long i_list=0; i_list < nr_neutrons-1; i_list++) {
        int id1 = neutron[i_list].id;
        double z1 = neutron[i_list].light;
        double t1 = neutron[i_list].rtime;

        for (ulong  j_list=i_list+1; j_list < (ulong) nr_neutrons; j_list++) {
            int   id2 = neutron[j_list].id;
            double z2 = neutron[j_list].light;
            double t2 = neutron[j_list].rtime;
            //dt;
            if (z1>=z2) {
                dt  = t2 - t1;
            }
            else {
                dt =  t1 - t2;
            }
            dt_valid = true;
            // this is a matrix dr - distance between two hits on neda detectors
            // versus time dt between them. It is to eliminate
            // events where the same neutron was
            TVector3  dv = neda_xyz[id1] - neda_xyz[id2];
            dr = (double)dv.Mag();
            dr_valid = true;

            spec_hNe_dt_vs_dr->increment(dr,dt);


            // checking if it is in a triangled polygon gate

            if(triangel_polygon_ptr && triangel_polygon_ptr->Test(dr,dt))
            {
                nr_separated++;
            }
            else {
                nr_scatters++;
            }

        }
    }
     nr_of_clean_neutrons = nr_neutrons - nr_scatters;
    if (nr_neutrons>0 && nr_of_clean_neutrons < 1) {
        nr_of_clean_neutrons = 1;
    }
//    if(nr_neutrons > 0)
//    cout << "n_neutrons_clean " << nr_of_clean_neutrons
//         << ", because n_neutrons= " << nr_neutrons
//         << "  , scatters =" << nr_scatters
//         << endl;


}
//####################
#if 0
void func()   // To jest cytat z maila Marcina Palacza. Wg tego miałem zrobić procedurę
// i chyba zrobiłem, ale co dalej?
// wygląda na to, że realizuje to funkcja  align_neda_times_to_neda_nr_0
{
#define NEDA_REF_ID 0     //align NEDA times with the NEDA_REF_ID one
    //use both TS and CFD to calculate NEDA times
    for (Int_t i_list=0; i_list<event.nneda; i_list++) // Petla po wszystkich nedach
    {
        Short_t id = event.neda[i_list].id;   // nr nedy 0-51

        // Obliczenie real_time - z randomizacja

        event.neda[i_list].time_raw = event.neda[i_list].TS *10
                + (event.neda[i_list].cfd + gRandom->Rndm()*10-5)/1000.0 ;   // in ns, cfd is given in ps

        // korekcja wartoscia z pliku
        Float_t correction = -neda_time_positions[id]  + neda_time_positions[NEDA_REF_ID];

        event.neda[i_list].time =  event.neda[i_list].time_raw + correctio
                event.neda[i_list].time =   event.neda[i_list].time - card_shifts[id/16+2];

    }
}


==================

void LFSelector::ProcessNeda() {

    if (!event.nneda) return;

#define NEDA_REF_ID 0     //align NEDA times with the NEDA_REF_ID one
    //use both TS and CFD to calculate NEDA times
    for (Int_t i_list=0; i_list<event.nneda; i_list++) {
        Short_t id = event.neda[i_list].id;
        event.neda[i_list].time_raw = event.neda[i_list].TS *10
                + (event.neda[i_list].cfd + gRandom->Rndm()*10-5)/1000.0 ;   // in ns, cfd is given in ps
        Float_t correction = -neda_time_positions[id]  +  neda_time_positions[NEDA_REF_ID];
        event.neda[i_list].time =  event.neda[i_list].time_raw + correction ;
        event.neda[i_list].time =   event.neda[i_list].time - card_shifts[id/16+2];
    }
    -------------------------------------------------------------

#define REQ_PSDLF_POS    0.15
#define REQ_PSDCAEN_POS  0.15
    for (Int_t i_list=0; i_list<event.nneda; i_list++) {
        Short_t id = event.neda[i_list].id;
        if (event.neda[i_list].qshort>0) {
            event.neda[i_list].ccfpga =  (event.neda[i_list].qlong-
                                          event.neda[i_list].qshort)/event.neda[i_list].qshort -
                    neda_psdcaen_positions[id] + REQ_PSDCAEN_POS;
        }
        if (event.neda[i_list].qslow >0) {
            event.neda[i_list].cclf  =   event.neda[i_list].qslow/event.neda[i_list].qfast -
                    neda_psdlf_positions[id] + REQ_PSDLF_POS;
        }
    }
    -------------------------------------------------
    //  sort all NEDA data in order of neda.time
    sort(event.neda,event.neda+event.nneda,CompareNEDATimes);

    event.time_ref =  event.neda[0].time; //use time of first neda as a time ref. for the event.
    if (event.neda[0].cclf < 0.3) {   // check if it is a gamma
        event.time_ref_type = 1;
    }
    else {
        event.time_ref_type = 2;
    }

    for (Int_t i_list=1; i_list<event.nneda; i_list++) {
        event.neda[i_list].rtime = event.neda[i_list].time -  event.time_ref;
    }
    -------------------------------
    Int_t k_list= 0;
    NEUTRON_DATA neutron[MAX_NEDA];
    memset(neutron,0,sizeof(neutron));
    for (Int_t i_list=0; i_list<event.nneda; i_list++) {
        if (event.time_ref_type==1) {
            if (neutron_cclf_cut &&
                    neutron_cclf_cut->
                    IsInside(event.neda[i_list].cclf,event.neda[i_list].rtime)) {
                neutron[k_list].id = event.neda[i_list].id;
                neutron[k_list].i_list = i_list;
                neutron[k_list].rtime = event.neda[i_list].rtime;
                neutron[k_list].cclf   =  event.neda[i_list].cclf;
                neutron[k_list].ccfpga =  event.neda[i_list].ccfpga;
                neutron[k_list].light =  event.neda[i_list].light;
                neutron[k_list].ban_result = 1;
                k_list++;
            }
        }
        else if (event.time_ref_type==2) {
            if (neutron_second_t2_cut &&      // the first neutron is in this cut as well
                    neutron_second_t2_cut->
                    IsInside(event.neda[i_list].cclf,event.neda[i_list].rtime)) {
                neutron[k_list].id = event.neda[i_list].id;
                neutron[k_list].i_list = i_list;
                neutron[k_list].rtime = event.neda[i_list].rtime;
                neutron[k_list].cclf   =  event.neda[i_list].cclf;
                neutron[k_list].ccfpga =  event.neda[i_list].ccfpga;
                neutron[k_list].light =  event.neda[i_list].light;
                neutron[k_list].ban_result = 2;
                k_list++;
            }
        }
    }
    event.n_neutrons=k_list;

    Int_t n_scatters = 0; Int_t n_separated = 0;
    for (Int_t i_list=0; i_list<event.n_neutrons-1; i_list++) {
        Int_t id1 = neutron[i_list].id;
        Float_t z1 = neutron[i_list].light;
        Float_t t1 = neutron[i_list].rtime;
        for (Int_t j_list=i_list+1; j_list<event.n_neutrons; j_list++) {
            Int_t   id2 = neutron[j_list].id;
            Float_t z2 = neutron[j_list].light;
            Float_t t2 = neutron[j_list].rtime;
            Float_t dt;
            if (z1>=z2) {
                dt  = t2 - t1;
            }
            else {
                dt =  t1 - t2;
            }

            TVector3  dv = neda_xyz[id1] - neda_xyz[id2];
            Float_t dr = (Float_t)dv.Mag();
            hNedrdt->Fill(dr,dt);


            if(drdt_cut && drdt_cut->IsInside(dr,dt)) {
                n_separated++;
            }
            else {
                n_scatters++;
            }
        }
    }
    event.n_neutrons_clean = event.n_neutrons - n_scatters;
    if (event.n_neutrons>0 && event.n_neutrons_clean<1) {
        event.n_neutrons_clean = 1;
    }

}
============================================= Plik nagłówkowy ===============
        typedef struct {
        unsigned short id;
        unsigned short TS;
        Bool_t         pur;
        Bool_t         pll;
        Float_t        time_raw;  // best possible time (ns),
        //to be calculated with TS and fine CFD time
        Float_t        time;    // the same but alligned
        Float_t        rtime;    // time vs time ref of the event;
        Float_t        qshort;
        Float_t        qlong;
        Float_t        qfast;
        Float_t        qslow;
        Float_t        light;
        Float_t        cfd;
        Float_t        intrise;
        unsigned short nbzero;
        Float_t        cclf;
        Float_t        ccfpga;
        } NEDA_DATA;

typedef struct {
    unsigned short id;
    unsigned short i_list;   // position on the NEDA data list
    unsigned short ban_result;
    Bool_t         pur;
    Bool_t         pll;
    Float_t        rtime;    // time vs time ref of the event;
    Float_t        qshort;
    Float_t        qlong;
    Float_t        qfast;
    Float_t        qslow;
    Float_t        light;
    Float_t        cfd;
    Float_t        intrise;
    unsigned short nbzero;
    Float_t        cclf;
    Float_t        ccfpga;
} NEUTRON_DATA;

struct {
    ULong64_t TS;
    unsigned short TSboard;     //board which gave TS of the event;
    unsigned short TSchannel;   //channel, ditto
    ULong64_t      TSentry;     //entry, ditto
    ULong64_t      time_ref;    // time reference for the event
    ULong64_t      time_ref_type;  // type of the time reference for the event
    unsigned short lastTS;
    unsigned short nitems, neagle, nge, nneda, nacs, ntrig, nrf, nrfcfd, ngesumbus;
    unsigned short gid[MAX_GE];
    Float_t        eg_raw[MAX_GE];
    Float_t        eg[MAX_GE];
    unsigned short gTS[MAX_GE];
    Float_t        gtime_raw[MAX_GE];
    Float_t        gtime[MAX_GE];    // time calculated using TS (relative to EventTS) and CFD
    Float_t        grtime[MAX_GE];   // time vs event time ref;
    Bool_t         gtime_good[MAX_GE];
    Bool_t         g_acsOK[MAX_GE];
    Float_t        gcfd[MAX_GE];
    Bool_t         gpur[MAX_GE];
    Bool_t         gpll[MAX_GE];
    unsigned short gboard[MAX_GE];
    unsigned short gchannel[MAX_GE];


    unsigned short acsid[MAX_GE];
    Float_t        acse[MAX_GE];
    unsigned short acsTS[MAX_GE];
    Float_t        atime_raw[MAX_GE];
    Float_t        atime[MAX_GE];  // not yet used
    Float_t        artime[MAX_GE]; // time vs. event time ref;
    unsigned short gacsTS[MAX_GE]; // TS of ACS of particular Ge, indexed with gid (not i_list)
    Float_t        gacsrtime[MAX_GE]; // rtime of ACS of Ge,  indexed with gid (not i_list)
    Float_t        acscfd[MAX_GE];
    Bool_t         atime_good[MAX_GE];
    Bool_t         acspresent[MAX_GE]; //indexed with gid (not i_list)
    Bool_t         acspur[MAX_GE];
    Bool_t         acspll[MAX_GE];

    NEDA_DATA      neda[MAX_NEDA];

    unsigned short boardTS[N_CAEN_BOARDS];


    unsigned short n_neutrons;
    unsigned short n_neutrons_clean;
    unsigned short n_neutrons_cclf;
    unsigned short n_neutrons_ccfpga;

    unsigned short tTS;
    Float_t        tqlong;
    Float_t        tqshort;
    Float_t        tcfd;

    unsigned short rfTS;
    Float_t        rfqlong;
    Float_t        rfqshort;
    Float_t        rfcfd;

} event;

struct {
    ULong64_t TS;
} tevent;

//################################################################

In preloop Neda_array


file_time_alignment = ifstream(filename_neda_time_alignment);

// petla wczytująca
detector_neda[it_->first]->make_preloop_action(it_->second);

// ---- początek pliku
0             0.0
1            -2.53315
2             4.97865
3            -2.94648
4            -1.60142
5            -0.23543
6            -0.36359
7             2.83639
// dla każdego NEDA robimy operację

alignment_correcting_value  = -value_zero +  value;

// --- to tyle PRZED pętlą analizującą zdarzenia

// Potem, w analizie pojedynczego detektora----------
real_time = (time_stamp * 10) + (cfd + gRandom->Uniform(-5, 5) /1000) ;// Marcin decided this formula
//cout << "Wg formuly marcina = real_time tego detektora = " << real_time << endl;
//align_neda_times_to_neda_nr_0();

aligned_real_time =  real_time + alignment_correcting_value ;
real_time  =aligned_real_time;



=== w oryginale :
    event.neda[i_list].time_raw = event.neda[i_list].TS *10
    + (event.neda[i_list].cfd + gRandom->Rndm()*10-5)/1000.0 ;   // in ns, cfd is given in ps

// korekcja wartoscia z pliku
Float_t correction = -neda_time_positions[id]  + neda_time_positions[NEDA_REF_ID];

event.neda[i_list].time =  event.neda[i_list].time_raw + correctio
        //event.neda[i_list].time =   event.neda[i_list].time - card_shifts[id/16+2];

        #endif
