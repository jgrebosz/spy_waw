#ifndef PATHS_H
#define PATHS_H
#include <string>


//#include <cstdlib>

class Tpaths
{
public:
    std::string spectra ;
    std::string polygons ;
    std::string gates_1D ;
    std::string commands ;
    std::string groups ;
    std::string experiment_setup;
    std::string mbs_settings ;
    std::string spy_dir ;
    std::string options ;
    std::string user_def_spectra ;
    std::string autom ;
    std::string calibration ;
    std::string conditions ;
    std::string my_binnings;
    std::string incrementers;

    Tpaths()
    {
        spy_dir = "./" ;
		  check_for_slashes() ;
    }
    // checks for ending slash
    void check_for_slashes() ;

    bool read_from_disk() ;
    void save_to_disk() ;
    std::string dir_spectra() { return spectra.c_str(); }
    std::string dir_polygons() { return polygons.c_str(); }
    std::string dir_gates() { return gates_1D.c_str(); }
    std::string dir_commands() { return commands.c_str(); }
    std::string dir_groups() { return groups.c_str(); }
    std::string dir_mbs_settings() { return mbs_settings.c_str(); }
    std::string dir_spy_dir() { return spy_dir.c_str(); }
    std::string dir_options() { return options.c_str(); }
    std::string dir_user_def_spectra() { return user_def_spectra.c_str(); }
    std::string dir_autom() { return autom.c_str(); }
    std::string dir_calibration() { return calibration.c_str(); }
    std::string dir_conditions() { return conditions.c_str(); }
    std::string dir_incrementers() { return incrementers.c_str(); }
    std::string dir_my_binnings() { return my_binnings.c_str(); }
    std::string dir_experiment_setup() {return experiment_setup.c_str();}
    

    // checking if the current path points to the legal
    // tree of directories

    //bool is_it_legal();
    void set_spy_dir(std::string p) {
        spy_dir = p;
        //is_it_legal();
    }
};

extern Tpaths path ;


extern const std::string user_spec_ext ;
extern const std::string user_spec_filter ;
extern const std::string user_cond_ext ;
extern const std::string user_cond_filter ;
extern const std::string user_incr_ext ;
extern const std::string user_incr_filter ;
#endif //PATHS_H
