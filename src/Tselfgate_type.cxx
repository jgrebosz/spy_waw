#include "Tselfgate_type.h"

#include <iostream>

// static
std::map<Tselfgate_type::Types, std::vector<std::string > >  Tselfgate_type::selfgate_types_map;

Tselfgate_type      obligatory_object_to_run_the_constructor(Tselfgate_type::not_available); // This guarantees that the map will be loaded

//************************************************************************************************************
Tselfgate_type::Tselfgate_type(Types t):my_type(t)
{
    if(selfgate_types_map.empty())
    {
        // loading the map of selfgate types
        selfgate_types_map[not_available] = { "not_available", ".not_available", "No selfgate" };
        selfgate_types_map[german_crystal] = { "german_crystal", ".self_gate_ger_crystal", "Single germanium crystal" };
        selfgate_types_map[hector] = { "hector", ".self_gate_hec_crystal", "Hector BaF detector " };
        selfgate_types_map[miniball] = {"miniball", ".self_gate_mib_crystal", "MINIBALL germanium detectors"} ;
        selfgate_types_map[cluster_addback] = {"cluster_addback", ".self_gate_ger_addback_cluster", "Addback in cluster of crystals of germanium (Rising)"} ;
        selfgate_types_map[agata_psa] = {"agata_psa", ".self_gate_agata_psa", "AGATA germanium detectors - for pulseshape analysis"} ;
		  selfgate_types_map[galileo_ge] = {"galileo_ge", ".self_gate_galileo_ge", "Galileo germanium detectors "} ;
		  
		  

    }
}
// ====================EXAMPLE HOW TO USE IT ===================================
#if 0
void example()
{
    using TSG = Tselfgate_type ;    // useful, make it localy of course

    Tselfgate_type obj{Tselfgate_type::german_crystal} ;
    std::cout << "typ german " << Tselfgate_type::german_crystal
         << " has extension : "  << obj.give_type_extension( )
         << " has filter  : " << obj.give_type_filter()
         << " explanation = " << obj.give_type_info()
         << std::endl;



    // Taking all information from the whole map
    for(auto x  :   TSG::selfgate_types_map)               // for each loop
    {
        Tselfgate_type obj { x.first} ;
        std::cout
                << " type called: "  <<  obj.give_type_info()
                << ", has a extension : " <<   obj.give_type_extension()
                << ", has a filter: "  << obj.give_type_filter()
                << ",  filter = " <<   obj.give_type_extension()
                << ",  info = "   <<   obj.give_type_info( )
                << std::endl;
    }
}
#endif
//======================================================================


Tselfgate_type::Types Tselfgate_type::give_type_of_selfgate_called(std::string fname)
{
    // take only extension
    std::string extension = fname ;
    size_t pos = fname.rfind(".");
    if(pos != std::string::npos)
    {
        extension = fname.substr(pos);
    }
    //int nr = 0 ;
    for(auto x  :   selfgate_types_map)               // for each loop
    {
        Tselfgate_type ttt(x.first);
        if(extension ==  ttt.give_type_extension())
        {

            return x.first;
        }
    }
    return Types::not_available;
}
