#ifndef _DSSD_ring_
#define _DSSD_ring_
#include "randomizer.h"
#include "spectrum.h"
#include <string>
#include "Tfile_helper.h"
#include "Tincrementer_donnor.h"

#include <TVector3.h>
#include <TMath.h>
#include "TF1.h"
#include <TRandom.h>
#include "doppler_enums.h"
#include "DataFrames.h"

#include "TDSSDDetector.h"

//================



#include <cstdlib>
#include <cstring>  // for memset

#include "TNeedle.h"
#include "TNeedle_element.h"
#include "spectrum.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "Tnamed_pointer.h"


#include <iomanip>

#include "TGalileoEvent.h"
#include "Tincrementer_donnor.h"
using namespace std;

// #define COTO  cout << "File: " << __FILE__ << ", line: " << __LINE__ << ", "; // endl;
// #define COTO  cout << "line: " << __LINE__ << ", "; // endl;

//class focus_point ;
//typedef unsigned int calkow;
typedef int calkow;

class TDSSDDetector;
constexpr int nr_of_channels {16*3} ;
//////////////////////////////////////////////////////////////////////////////
class TDSSD_ring : public TNeedle_element , public Tincrementer_donnor
{

    std::map<int,TDSSD_ring*> detector_dssd ;

    static constexpr int RINGS_NR {16};
    static constexpr int SECTORS_NR {32};
    static constexpr int nr_of_signals = 3;


//    int rings_data[RINGS_NR][nr_of_signals];
//    int sectors_data[SECTORS_NR][nr_of_signals];

    // raw data
    uint64_t  timestamp_raw[nr_of_channels];
    int  energy_raw[nr_of_channels];
    int  cfd_raw[nr_of_channels];


    double energy_cal[nr_of_channels];
    //double cfd_cal[nr_of_channels];

    std::vector< std::vector<double> >  cal_factors ;



    int detector_nr = 0 ; // which nr of the silicon detector vector
    int threshold_of_raw = 0;
    int rings_multpilicity, sectors_multpilicity;

    int active_ring_nr ;
    int active_sector_nr;
    bool multiplicity1_validator;

    string signal_name[3];

//    double xx[SECTORS_NR];  //
//    double yy[RINGS_NR];

    // helpfull for polar plot
    struct Tpara {
        double x;
        double y;
    } ;

    Tpara xy[SECTORS_NR][RINGS_NR];

    // Tpara theta_phi_degrees[SECTORS_NR][RINGS_NR];
    double rings_theta[RINGS_NR];
    double sectors_phi[SECTORS_NR];

public:
    //  constructor
    TDSSD_ring(string _name, int nr_);

    void analyse_current_event()  ;
    void make_preloop_action(ifstream &) ;   // read the calibration factors, gates
    bool give_current_theta_phi(double * th, double *ph);
protected:
    void create_my_spectra();

    spectrum_1D * spec_chan[nr_of_channels];
    spectrum_1D * spec_chan_cal[nr_of_channels];

    spectrum_2D * spec_xy;
    spectrum_2D * spec_polar;

//    spectrum_2D * spec_xz;

    spectrum_1D * spec_hardware_channels_fan;
    spectrum_1D * spec_rings_fan;
    spectrum_1D * spec_sectors_fan;

    spectrum_1D * spec_sectors_multiplicity;
    spectrum_1D * spec_rings_multiplicity;

};
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // _dssd_ring_H_


