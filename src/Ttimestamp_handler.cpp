#include "Ttimestamp_handler.h"

#include "Tnamed_pointer.h"
using namespace std;

//***********************************************************************
//  constructor
Ttimestamp_handler::Ttimestamp_handler(string _name)  : TNeedle_element(_name)

{


   // x_left_ptr = x_right_ptr = y_down_ptr = y_up_ptr = nullptr ; // null

    create_my_spectra();


//    named_pointer[name_of_this_element + "_x_when_ok"]
//    = Tnamed_pointer(&x, &x_ok, this) ;
//    named_pointer[name_of_this_element + "_y_when_ok"]
//    = Tnamed_pointer(&y, &y_ok, this) ;


//    // for rates - purposes
//    named_pointer[name_of_this_element + "_left_raw_fired"]
//    = Tnamed_pointer(&left_raw, &left_raw_fired, this) ;

//    named_pointer[name_of_this_element + "_right_raw_fired"]
//    = Tnamed_pointer(&right_raw, &right_raw_fired, this) ;

//    named_pointer[name_of_this_element + "_up_raw_fired"]
//    = Tnamed_pointer(&up_raw, &up_raw_fired, this) ;

//    named_pointer[name_of_this_element + "_down_raw_fired"]
//    = Tnamed_pointer(&down_raw, &down_raw_fired, this) ;

//    named_pointer[name_of_this_element + "_anode_raw_fired"]
//    = Tnamed_pointer(&anode_raw, &anode_raw_fired, this) ;



}
//************************************************************************
//************************************************************************
void Ttimestamp_handler::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "multiwire/" + name_of_this_element ;


    //-----------
    string name = name_of_this_element + "_spec_all_ts"  ;
    spec_all_ts = new spectrum_1D(name, name,
                                10000, 0, 1000000,
                                folder, "", noraw);
    frs_spectra_ptr->push_back(spec_all_ts) ;

#if 0

    //-----------
    name = name_of_this_element + "_right_raw"  ;
    spec_right = new spectrum_1D(name, name,
                                 4095, 1, 4096,
                                 folder, "", noraw);
    frs_spectra_ptr->push_back(spec_right) ;

    //-----------

    name = name_of_this_element + "_up_raw"  ;
    spec_up = new spectrum_1D(name, name,
                              4095, 1, 4096,
                              folder, "", noraw);
    frs_spectra_ptr->push_back(spec_up) ;

    //-----------
    name = name_of_this_element + "_down_raw"  ;
    spec_down = new spectrum_1D(name, name,
                                4095, 1, 4096,
                                folder, "", noraw);
    frs_spectra_ptr->push_back(spec_down) ;


    //-----------
    name = name_of_this_element + "_anode_raw"  ;
    spec_anode = new spectrum_1D(name, name,
                                 4095, 1, 4096,
                                 folder, "", noraw);
    frs_spectra_ptr->push_back(spec_anode) ;



    // Note: mutliwires are 200 mm, but only 160 mm are active


    // X------------------------------------
    name = name_of_this_element + "_posX"  ;
    spec_x = new spectrum_1D(name, name,
                             400, -150, 150,
                             folder, "when the gate on sumX is passed", name_of_this_element + "_x_when_ok");
    frs_spectra_ptr->push_back(spec_x) ;


    // X_ SUM------------------------------
    name =  name_of_this_element + "_sumX"  ;

    spec_x_sum = new spectrum_1D(name, name,
                                 16000, -8000, 8000,
                                 folder,
                                 "GATE is for posX", noinc);
    frs_spectra_ptr->push_back(spec_x_sum) ;

    // Y------------------------------------
    name =  name_of_this_element + "_posY"  ;
    spec_y = new spectrum_1D(name, name,
                             400, -150, 150,
                             folder,
                             "when the gate on sumY is passed",
                             name_of_this_element + "_y_when_ok");
    frs_spectra_ptr->push_back(spec_y) ;


    // Y_SUM------------------------------
    name =  name_of_this_element + "_sumY"  ;

    spec_y_sum = new spectrum_1D(name, name,
                                 16000, -8000, 8000, //    8192, 0, 8192,
                                 folder,
                                 "GATE is for posY", noinc);

    frs_spectra_ptr->push_back(spec_y_sum) ;



    //--------------------------------------
    name =  name_of_this_element + "_xy"  ;

    spec_xy = new spectrum_2D(name, name,
                              400, -150, 150,
                              400, -150, 150,
                              folder, "",
                              string("as X:") + name_of_this_element + "_y_when_ok" +
                              ", as Y: " + name_of_this_element + "_y_when_ok");

    frs_spectra_ptr->push_back(spec_xy) ;

#endif

}
//**********************************************************************
void Ttimestamp_handler::analyse_current_event()
{


    // cout << __PRETTY_FUNCTION__ << endl;

     auto ts = event_unpacked->timestamp;

    if(flag_first_time){
        first_timestamp = ts;
    // take first ts and assume it as zero
         flag_first_time = false;
    }

    // take any next and make a difference
    // next - first

    double difference = ts - first_timestamp;

//    cout << "ts now = " << ts
//          << " ts first = " << first_timestamp
//            << " difference = " << difference
//             << endl ;

    spec_all_ts -> increment(difference ) ;
//    spec_right ->increment(event_unpacked->*x_right) ;
//    spec_anode ->increment(event_unpacked->*anode) ;
//    spec_up ->increment(event_unpacked->*y_up) ;
//    spec_down ->increment(event_unpacked->*y_down) ;



    calculations_already_done = true ;

}
//**************************************************************************
void  Ttimestamp_handler::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{

    cout << "Reading the calibration for " << name_of_this_element << endl ;

 flag_first_time = true;
    // in this file we look for a string

   // distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_distance");



    // for the new, adriana algoritm




//     cout << " reading the gates for " << name_of_this_element << " sum's " << endl ;


    //-----------------

    // here we
    //string gates_file = "gates/multiwire_sum_gates.txt" ;

    // the new style, gates are created by my viewer, or manually
    // so that in one file there is only one gate, for this particullar object

    string core_of_the_name = "gates/" + name_of_this_element ;
    string nam_gate = core_of_the_name + "_sumX_gate_GATE.gate" ;



//    ifstream plik_gatesY(nam_gate.c_str());
//    if(! plik_gatesY)
//    {
//        string mess = "I can't open  file: " + nam_gate
//                      + "\nThis gate you can produce interactively using a 'cracow' viewer program" ;

//        cout << mess
//             << "\nI assmue that the gate is wide open !!!!!!!!!!!!!! \a\a\a\n"
//             << endl ;

//        ofstream pliknew(nam_gate.c_str());
//        pliknew   << y_sum_gate[0] << "   "    // lower limit
//                  << y_sum_gate[1] << endl ;

//    }
//    else
//    {

//        plik_gatesY >> zjedz >> y_sum_gate[0]     // lower limit
//                    >> zjedz >> y_sum_gate[1] ;   // upper limit

//        if(! plik_gatesY)
//        {
//            string mess = "Error while reading 2 numbers from the gate file: " + nam_gate  ;
//            exit(-1) ;
//        }
//        plik_gatesY.close() ;

//    }


}
//***************************************************************

