/***************************************************************************
                          Tself_gate_hec_descr.h  -  description
                             -------------------
    begin                : Mon Sep 8 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz, IFJ Krakow, Poland
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/


#ifndef TSELF_GATE_HEC_DESCR_H
#define TSELF_GATE_HEC_DESCR_H

#include <string>

#include "Tself_gate_abstract_descr.h"


/**Text file which describes the gate on the germanium crystal
  *@author dr. Jerzy Grebosz, IFJ Krakow, Poland
  */
//////////////////////////////////////////////////////////////////////////
class Tself_gate_hec_descr : public Tself_gate_abstract_descr
{
public:


  bool enable_energy_fast_gate ;
  double en_fast_gate[2];

  bool enable_energy_slow_gate ;
  double en_slow_gate[2];

  bool enable_time_gate ;
  double time_gate[2];

  bool enable_geom_theta_gate ;
  double geom_theta_gate[2];

  bool enable_geom_phi_gate ;
  double geom_phi_gate[2];

  // gamma - particle angle
  bool enable_gp_theta_gate ;
  double gp_theta_gate[2];

  bool enable_gp_phi_gate ;
  double gp_phi_gate[2];



  Tself_gate_hec_descr();
  virtual ~Tself_gate_hec_descr() {} ;
  void read_definition_from(std::string pathed_name);
  void write_definitions(std::string path_only);
};

#endif
