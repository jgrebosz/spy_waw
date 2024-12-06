#include "TNeedle_element.h"
#include "TNeedle.h"

#include <algorithm>

TNeedle *TNeedle_element::owner ;  // static it is only one frs
TGalileoEvent * TNeedle_element::event_unpacked ;  // static, 

// for spectra, they can contain a standard texts
std::string noraw = 
"Raw values are (very often) not defined as incrementers  - too many of them. "
"You can try to find them as vme[][] incrementers (see the lookup table)";
std::string noinc = 
"No such incrementer defined. Ask Jurek if you really need it";

// Done incrementers list   for
// MW, Tscintill, Taoq_calculator, Tbonn_magnet, Tdegrader, Tmusic_8_anodes
// Ttof_measurement, Tfocal_plane

// Ommited:
// Tsi_detector
// Tscaler
// Tsi_pin_diode
// Ttarget
// Target scintillator, Ttwo signal module
// Tclover
//**************************************************************************
TNeedle_element::TNeedle_element(std::string name) : name_of_this_element(name)
{
  frs_spectra_ptr = owner->address_spectra_frs() ;
}   
//**************************************************************************
