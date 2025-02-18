/***************************************************************************
                          user_spectrum_description.h  -  description
                             -------------------
    begin                : Fri Jul 25 2003
    copyright            : (C) 2003 by Dr. Jerzy Grebosz, IFJ Krakow, Poland
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/

#ifndef user_spectrum_description_H
#define user_spectrum_description_H

#include <string>
#include <vector>

/**This class contains the information about user defined conditional spectra
  *@author dr. Jerzy Grebosz, IFJ Krakow, Poland
  */
/////////////////////////////////////////////////////////////////////////
class user_spectrum_description
{
    // friend class cond_spec_wizard ;
    // friend class user_spectrum_wizard ;
    // friend class  menager_user_spectra;
public:
    user_spectrum_description();
    /** read the definitons from the disk */
    void read_from(std::string pathed_name);
    ~user_spectrum_description();
    /** save the definition on the disk */
    void write_definitions(std::string path_only);
    /** No descriptions */
    void set_name(std::string s);
    std::string get_name() {
        return name_of_spectrum ;
    }
    /** To know is somebody just changed some parameters so,
    that we must re-create it (changed dimmension or binning) */
    bool are_parameters_identical(const user_spectrum_description & u) const ;

    int give_dimmension() {
        return dimmension ;
    }
    int give_bins_x() {
        return bins_x ;
    }
    int give_bins_y() {
        return bins_y ;
    }

    double give_beg_x() {
        return beg_x ;
    }
    double give_beg_y() {
        return beg_y ;
    }

    double give_end_x() {
        return end_x ;
    }
    double give_end_y() {
        return end_y ;
    }
    std::string give_conditon_name() {
        return condition_name ;
    }
    int give_policy() {
        return policy_when_increm ;
    }
    bool spectrum_enabled() {
        return enabled ;
    }

    enum type_of_data { spec_1D = 1, spec_2D } ;

    std::vector< std::pair<std::string,std::string> > & give_x_incr() {
        return incrementers_x ;
    }
    std::vector< std::pair<std::string,std::string> > & give_y_incr() {
        return incrementers_y ;
    }

protected: // Protected attributes

    bool enabled ;

    /** the name, without the extension (*.spc) */
    std::string name_of_spectrum;
    type_of_data dimmension ;

    /** nr of bins in the spectrum  */
    int bins_x;

    /** left edge of the first bin */
    double beg_x;
    /** right edge of the last bin */
    double end_x;

    /** nr of bins in the spectrum */
    int bins_y;

    /** left edge of the first bin */
    double beg_y;
    /** right edge of the last bin */
    double end_y;


    std::vector<std::pair<std::string,std::string> > incrementers_x ;
    std::vector<std::pair<std::string,std::string> > incrementers_y ;

    int policy_when_increm ;    // 0 = always
    // 1 => only when from differnet detector
    // 2 => only when from the same detector

    std::string condition_name ;

};
/////////////////////////////////////////////////////////////////////////
#endif
