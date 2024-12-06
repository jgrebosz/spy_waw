project(spy)
cmake_minimum_required(VERSION 2.8)

include(FindROOT.cmake)




# --- For No debug version
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -L./  -pthread ")

# --- For Debug version
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -L./  -pthread")
SET(CMAKE_BUILD_TYPE Debug)
#---------------------------------------------

# subdirs(src)

#INCLUDEPATH += ./mfm/include ./mfm/tinyxml
#add_library(mfm/lib)

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR} ${LIBS} )
set(INCLUDE_DIRECTORIES ${ROOT_INCLUDE_DIR} /usr/include/boost169 src )
#mfm/include mfm/tinyxml include )

link_directories(${LINK_DIRECTORIES} )
include_directories(${INCLUDE_DIRECTORIES} )
link_libraries(${ROOT_LIBRARIES} )
#mfm/lib mfm/tinyxml)


include_directories(include mfm/include mfm/tinyxml)

# UWAGA: Aby wejść do edycji "blokowej" w Qtcreatorze
# wystarczy po wcisnięciu myszki wcisnąć też klawisz ALT
# i wtedy ciągnąć myszką zaznaczenie bloku.



#TO MAKE  ROOT/NOROOT VERSION - UNCOMMENT ONE OF THE FOLLOWING COMMANDS

#   if  CERN_ROOT_INSTALLED == true we can link with root functions
# link_libraries(  rt  Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript
# Matrix Physics MathCore Thread Gui pthread    m dl
# )



add_executable( spy
  src/MainUserAnalysis.cxx
  src/DataFrames.cpp
  src/Misc.cpp
  src/T32_signal_module.cxx 
  src/TGalileoAnalysis.cxx 
  src/TGalileoCalibrateFactory.cxx 
  src/TGalileoCalibrateProcessor.cxx 
  src/TGalileoCalibratedEvent.cxx 
  src/TGalileoCalibration.cxx 
  src/TGalileoEvent.cxx 
  src/TGalileoEventFactory.cxx 
  src/TGalileoEventProcessor.cxx 
  src/Tcondition_description.cxx 
  src/Tgrebware_exception.cxx 
  src/Tfile_helper.cxx 
  src/TNeedle.cxx 
  src/TNeedle_element.cxx 
  src/Tincrementer_donnor.cxx
  src/TjurekPolyCond.cpp 
  src/Tnamed_pointer.cxx 
  src/Tone_signal_module.cxx 
  src/Tone_signal_pattern_module.cxx 
  src/Tself_gate_abstract_descr.cpp
  src/Tself_gate_ger_addback_descr.cpp 
  src/Tself_gate_ger_descr.cpp
  src/Tself_gate_hec_descr.cpp 
  src/Tself_gate_ge_galileo_descr.cpp
  src/Tsynchro_watchdog_curved.cxx 
  src/Ttarget_galileo.cpp
  src/Ttarget_scintillator.cxx 
  src/Ttwo_signal_module.cxx 
  src/Tuser_condition.cxx 
  src/Tuser_gated_spectrum_definition.cxx 
  src/Tuser_incrementer.cxx 
  src/Tuser_spectrum.cxx 
  src/data_psa_65000_1.cpp  
  src/psa_hit.cpp 
  src/spectrum.cxx 
  src/tbuffer_of_data_block.cpp 
  src/tdata_tracked_65_0.cpp 
  src/tgeneric_data.cpp 
  src/tjurekabstracteventelement.cxx 
  src/tjurekabstracteventprocessor.cpp 
  src/tsocket_for_data.cxx
  src/ttracked_hit.cpp 
  src/tunpacked_event_data.cpp 
  src/upack.cpp 
  src/Tself_gate_agata_psa_descr.cxx 
  src/Tself_gate_mib_descr.cpp 
  src/user_spectrum_description.cxx
  src/paths_spy.cxx
  src/Tselfgate_type.cxx
  src/TEAGLE_crystal.cxx
  src/TEAGLE_array.cxx
  src/TDSSD_ring.cpp
  src/TNEDA_crystal.cxx
  src/TNEDA_array.cxx
  src/Tscaler.cxx
  src/Tscaler_template.h
  src/Ttimestamp_handler.cpp
  src/TDiamant_array.cpp
  src/TDiamant_crystal.cpp
  src/Tlookup_table_doublets.cxx
  src/Tgo4eventsourceparameter.cpp
  )



target_link_libraries(${PROJECT_NAME}
${CMAKE_SOURCE_DIR}/mfm/lib/libMFM.a
${CMAKE_SOURCE_DIR}/mfm/tinyxml/libtinyxml.so
#${CMAKE_SOURCE_DIR}/mfm/libMFM.so

)



install(TARGETS spy DESTINATION ./building/ )
