/***************************************************************************
                          Tself_gate_ge_cristal_galileo_descr.h  -  description
                             -------------------
    begin                : Mon Sep 8 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz, IFJ Krakow, Poland
    email                : jerzy.grebosz@ifj.edu.pl

***************************************************************************/


#ifndef TSELF_GATE_GE_CRISTAL_GALILEO_DESCR_H
#define TSELF_GATE_GE_CRISTAL_GALILEO_DESCR_H

#include <string>

#include "Tself_gate_abstract_descr.h"
#include "TjurekPolyCond.h"


/**Text file which describes the gate on the germanium cristal
  *@author dr. Jerzy Grebosz, IFJ Krakow, Poland
  */
//////////////////////////////////////////////////////////////////////////
class Tself_gate_neutronWall_descr : public Tself_gate_abstract_descr
{
public:

    bool enable_zcogate ;
    double zco_gate[2];

    bool enable_qvcgate ;
    double qvc_gate[2];

    bool enable_tof_gate ;
    double tof_gate[2];

    // ---------------- for polygon gate on zco vs tof (one of  three times)
    bool  enable_zco_tof_polygon_gate;
    std::string zco_tof_polygon_gate;

    // ---------------- for polygon gate on qvc vs tof (one of  three times)
    bool  enable_qvc_tof_polygon_gate;
    std::string qvc_tof_polygon_gate;


    TjurekPolyCond * polygon ;

    //------------------------------------------
    bool enable_geom_theta_gate ;
    double geom_theta_gate[2];

    bool enable_geom_phi_gate ;
    double geom_phi_gate[2];

    // gamma-particle angle
    bool enable_gp_theta_gate ;
    double gp_theta_gate[2];

    bool enable_gp_phi_gate ;
    double gp_phi_gate[2];


    // For Marek Pfutzner
    bool enable_xy_gamma_tof_difference;
    bool flag_this_is_for_X_incrementer;
    double safe_for_x_tof;
    //---------------two virtual functions
    void this_is_x_incrementer() {
        flag_this_is_for_X_incrementer = true;
        safe_for_x_tof = 0;
    }
    void this_is_y_incrementer()  {
        flag_this_is_for_X_incrementer = false;
    }

    // true when we use the option xy difference
    bool not_for_1Dspectrum()  {
        return (enable_xy_gamma_tof_difference ?  true : false) ;
    }

    //----------------------------------
    Tself_gate_neutronWall_descr();
    virtual ~Tself_gate_neutronWall_descr() {} ;
    void read_definition_from(std::string pathed_name);
    void write_definitions(std::string path_only);
};

#endif
