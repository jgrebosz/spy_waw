/***************************************************************************
                          user_spectrum_description.cpp  -  description
                             -------------------
    begin                : Fri Jul 25 2003
    copyright            : (C) 2003 by Dr. Jerzy Grebosz, IFJ Krakow, Poland
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#include "user_spectrum_description.h"
#include "paths.h"
#include <iostream>
#include <fstream>
#include "Tfile_helper.h"


//*************************************************************************
user_spectrum_description::user_spectrum_description()
{

    name_of_spectrum = "user_" ;
    enabled = true ;

    dimmension= spec_1D;
    bins_x = 8192 ;
    beg_x = 0;
    end_x = 8192;

    bins_y = 256 ;
    beg_y = 0;
    end_y = 8192;
    policy_when_increm  = 0 ; // always
    condition_name = "No_condition" ;

}
//**************************************************************************
user_spectrum_description::~user_spectrum_description()
{

}
//**************************************************************************
/** read the definitons from the disk */
void user_spectrum_description::read_from(std::string pathed_name)
{
    incrementers_x.clear();
    incrementers_y.clear();

    std::string file_name = pathed_name ;
    // path.user_def_spectra + name_of_spectrum + ".user_definition" ;
    std::ifstream plik(file_name.c_str());
    if (!plik)
    {
        std::cout << "Can't open the file "  << file_name
             << " for reading " << std::endl;
        return ;
    }

    try
    {

        //Tfile_helper::spot_in_file(plik, "spectrum_name");
        // plik >> name_of_spectrum ;   // will be without the path and extension
        name_of_spectrum = Tfile_helper::find_in_file<std::string>(plik, "spectrum_name");


        enabled = (bool)Tfile_helper::find_in_file<bool>(plik, "enabled");

        dimmension = (type_of_data) Tfile_helper::find_in_file<int>(plik, "dimmension");

        bins_x = (int) Tfile_helper::find_in_file<int>(plik, "bins_x");
        beg_x = Tfile_helper::find_in_file<double>(plik, "beg_x");
        end_x = Tfile_helper::find_in_file<double>(plik, "end_x");

        bins_y = (int) Tfile_helper::find_in_file<int>(plik, "bins_y");
        beg_y = Tfile_helper::find_in_file<double>(plik, "beg_y");
        end_y = Tfile_helper::find_in_file<double>(plik, "end_y");



        // reading X incrementers-------------------------
        try {
            Tfile_helper::spot_in_file(plik, "incrementers_x");
        } catch (...)
        {
            Tfile_helper::repair_the_stream(plik);
            Tfile_helper::spot_in_file(plik, "incrementors_x");
        }
        std::string klamra ;
        plik >> zjedz >> klamra ; // "{";
        std::string wyraz, self_gate ;

        do
        {
            plik >> wyraz >> self_gate;
            if (wyraz == "}" || !plik ) break ;
            incrementers_x.push_back( std::pair<std::string,std::string>(wyraz, self_gate)) ;
        } while (plik);

        // reading Y incrementers------------------------

        try {
            Tfile_helper::spot_in_file(plik, "incrementers_y");
        } catch (...)
        {
            Tfile_helper::repair_the_stream(plik);
            Tfile_helper::spot_in_file(plik, "incrementors_y");
        }
		  
		  plik >> zjedz >> klamra ; // "{";

        do
        {
            plik >> wyraz >> self_gate;
            if (wyraz == "}" || !plik ) break ;
            incrementers_y.push_back(std::pair<std::string,std::string>(wyraz, self_gate)) ;
        } while (plik);

        policy_when_increm  =  (int) Tfile_helper::find_in_file<int>(plik, "policy_when_increment");

        condition_name = Tfile_helper::find_in_file<std::string>(plik, "condition_name");
//        Tfile_helper::spot_in_file(plik, "condition_name");
//        plik >> condition_name ;
       // std::cout << "Read condition name = " << condition_name << std::endl ;



    }
    catch (Tfile_helper_exception &m)
    {
        std::cout << "user_spectrum_description::read_from(..)   ->Error during      reading the spectrum description file "
             << file_name << " : "
             << m.message << std::endl;
    }
}
//**************************************************************************
/** save the definition on the disk */
void user_spectrum_description::write_definitions(std::string path_only)
{
    std::string file_name = path_only + name_of_spectrum + ".user_definition" ;

    std::ofstream plik(file_name.c_str());
    if (!plik)
    {
        std::cout << "Can't open the file " <<file_name
             << " for writing " << std::endl;
        return ;
    }

    plik
    << "// This is a definiton of the conditional spectrum\n"
    << "// enables are 0 or 1 which means false or true\n"
    << "// comments are marked using two slashes: // comment \n\n"
    << "spectrum_name\t\t " << name_of_spectrum << "\n"
    << "enabled\t\t" << enabled << "\n\n"


    << "dimmension\t\t" << dimmension
    << "\n//-----------------------------------------------------\n\n"

    << "bins_x\t\t " << bins_x <<  "\n"
    << "beg_x\t\t " << beg_x <<  "\t\t// left edge of the first bin\n"
    << "end_x\t\t " << end_x <<  "\t\t// right edge of the last bin\n\n"
    << "bins_y\t\t " << bins_y <<  "\n"
    << "beg_y\t\t " << beg_y <<  "\t\t// left edge of the first bin\n"
    << "end_y\t\t " << end_y <<  "\t\t// right edge of the last bin\n\n"

    << std::endl;


    plik << "\nincrementers_x                // and their self_gates\n{\n";
    for (unsigned i = 0 ; i < incrementers_x.size() ; i++)
    {
        plik << incrementers_x[i].first << "\t"
        << incrementers_x[i].second   << "\n" ;
    }
    plik << "}\n";

    plik << "\nincrementers_y               // and their self_gates\n{\n";
    for (unsigned i = 0 ; i < incrementers_y.size() ; i++)
    {
        plik <<  incrementers_y[i].first << "\t" << incrementers_y[i].second << "\n" ;
    }
    plik << "}\n\n";


    plik << "policy_when_increment\t"
    << policy_when_increm
    << "\t// 0= always, 1=only when from DIFFERENT detector, 2 = only when from SAME detectror"
    << "\n" ;


    if (condition_name.find(".cnd") == std::string::npos)
    {
        condition_name += ".cnd" ;
    }
    plik  << "\ncondition_name\t\t" << condition_name << std::endl ;





}
//*********************************************************************
/** No descriptions */
void user_spectrum_description::set_name(std::string s)
{
    name_of_spectrum = s ;
}
//********************************************************************
/** To know is somebody just changed some parameters so,
that we must re-create it (changed dimmension or binning) */
bool user_spectrum_description::are_parameters_identical(const user_spectrum_description & u) const
{
    if (dimmension != u.dimmension) return false ;
    if (dimmension == spec_1D)
    {
        return
            (
                bins_x == u.bins_x
                &&
                beg_x == u.beg_x
                &&
                end_x == u.end_x) ;
    }
    else
    {
        return
            (
                bins_x == u.bins_x
                &&
                beg_x == u.beg_x
                &&
                end_x == u.end_x
                &&
                bins_y == u.bins_y
                &&
                beg_y == u.beg_y
                &&
                end_y == u.end_y
            ) ;
    }

}
