# Modern CMake for MFM
#
# O. Stezowski
#
cmake_minimum_required(VERSION 3.5)
#

# ////////// Project definitions //////////
#
project(MFM_Diamant
	VERSION 0.0.0
#	DESCRIPTION  "CMake project to build, compile & install a library containing the core function of the MFM library"
#	HOMEPAGE_URL 	
#	LANGUAGES C CXX
)
#
# default type
#
set(DEFAULT_BUILD_TYPE "Release")
#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
#
include(GNUInstallDirs)
#
# ////////// /////////////////// //////////

string(TIMESTAMP BUILD_MFM_DATE "%Y/%m/%d")
string(TIMESTAMP BUILD_MFM_TIME "%H:%M:%S")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/sources/MFMlib.in"  "${CMAKE_CURRENT_BINARY_DIR}/include/MFMlib.in.h")

# ////////// Targets definitions //////////
#
# Main target is to produce a shared library called   ${PROJECT_NAME}  MFMtest.cc
#
add_library(${PROJECT_NAME} SHARED 
	sources/DataGenParameters.cc
	sources/DataPar.cc
	sources/DataParameters.cc
	sources/DataScal.cc
	sources/DataScalers.cc
	sources/MError.cc
	sources/MFMBasicFrame.cc
	sources/MFMBlobFrame.cc
	sources/MFMChimeraFrame.cc
	sources/MFMCoboFrame.cc
	sources/MFMCoboTopoFrame.cc
	sources/MFMCommonFrame.cc
	sources/MFMDiamantFrame.cc
	sources/MFMEbyedatFrame.cc
	sources/MFMExogamFrame.cc
	sources/MFMFileTest.cc
	sources/MFMHelloFrame.cc
	sources/MFMMergeFrame.cc
	sources/MFMMutantFrame.cc
	sources/MFMNedaCompFrame.cc
	sources/MFMNedaFrame.cc
	sources/MFMNumExoFrame.cc
	sources/MFMOscilloFrame.cc
	sources/MFMRibfFrame.cc
	sources/MFMS3AlphaFrame.cc
	sources/MFMS3BaF2Frame.cc
	sources/MFMS3RuthFrame.cc
	sources/MFMS3SynchroFrame.cc
	sources/MFMS3eGUNFrame.cc
	sources/MFMScalerDataFrame.cc
	sources/MFMVamosICFrame.cc
	sources/MFMVamosPDFrame.cc
	sources/MFMVamosTACFrame.cc
	sources/MFMXmlDataDescriptionFrame.cc
	sources/MFMXmlFileHeaderFrame.cc
	sources/MFMBoxDiagFrame.cc
	sources/MFMReaGenericFrame.cc
	sources/MFMReaScopeFrame.cc
	tinyxml/tinyxml.cpp
	tinyxml/tinystr.cpp
	tinyxml/tinyxmlerror.cpp
	tinyxml/tinyxmlparser.cpp
)
add_library(MFM::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Define headers for the library. PRIVATE is for internal purposes (compilation. PUBLIC is for compilation and external users
target_include_directories(${PROJECT_NAME} 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sources>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tinyxml>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

#
# additional target, standard executable
#
add_executable(MFMtest
	sources/MFMtest.cc
)
#
target_link_libraries(MFMtest
    PRIVATE ${PROJECT_NAME}	
)
#
# ////////// /////////////////// //////////

# ////////// Install definitions //////////
#
# Places where to install libraries, exec and headers
#		
install(TARGETS ${PROJECT_NAME} MFMtest
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)
# public headers 
install(FILES 
	tinyxml/tinyxml.h
	tinyxml/tinystr.h
	sources/Cobo.h
	sources/DataGenParameters.h
	sources/DataPar.h
	sources/DataParameters.h
	sources/DataScal.h
	sources/DataScalers.h
	sources/MError.h
	sources/MFMAllFrames.h
	sources/MFMBasicFrame.h
	sources/MFMBlobFrame.h
	sources/MFMBoxDiagFrame.h
	sources/MFMChimeraFrame.h
	sources/MFMCoboFrame.h
	sources/MFMCoboTopoFrame.h
	sources/MFMCommonFrame.h
	sources/MFMDiamantFrame.h
	sources/MFMEbyedatFrame.h
	sources/MFMExogamFrame.h
	sources/MFMHelloFrame.h
	sources/MFMMergeFrame.h
	sources/MFMMutantFrame.h
	sources/MFMNedaCompFrame.h
	sources/MFMNedaFrame.h
	sources/MFMNedaFrame_old.h
	sources/MFMNumExoFrame.h
	sources/MFMOscilloFrame.h
	sources/MFMRibfFrame.h
	sources/MFMS3AlphaFrame.h
	sources/MFMS3BaF2Frame.h
	sources/MFMS3RuthFrame.h
	sources/MFMS3SynchroFrame.h
	sources/MFMS3eGUNFrame.h
	sources/MFMScalerDataFrame.h
	sources/MFMTypes.h
	sources/MFMVamosICFrame.h
	sources/MFMVamosPDFrame.h
	sources/MFMVamosTACFrame.h
	sources/MFMXmlDataDescriptionFrame.h
	sources/MFMXmlFileHeaderFrame.h
	sources/XmlTags.h
	sources/MFMReaGenericFrame.h
	sources/MFMReaScopeFrame.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
#
# export the targets in a target file saved in lib directory
#	
install(EXPORT ${PROJECT_NAME}Targets 
	FILE 
#		${PROJECT_NAME}Targets.cmake 
		${PROJECT_NAME}-config.cmake
#		${PROJECT_NAME}Config.cmake
	NAMESPACE
		${PROJECT_NAME}::		
	DESTINATION 
		lib/cmake/
)
#
# export the targets in a target file saved in lib directory
#	
export ( TARGETS ${PROJECT_NAME} FILE  ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake )
#export ( TARGETS ${PROJECT_NAME} FILE  ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}Config.cmake )
#export ( PACKAGE ${PROJECT_NAME} ) 
#install( EXPORT FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
#        DESTINATION lib/cmake/${PROJECT_NAME} 
#)
#
# ////////// /////////////////// //////////

#get_target_property(ALL_INCLUDEDIR ${PROJECT_NAME} INCLUDE_DIRECTORIES )
#MESSAGE("DIRECTORIES ${ALL_INCLUDEDIR}")

#INSTALL (
#DIRECTORY ${CMAKE_SOURCE_DIR}/include/
#DESTINATION include
#FILES_MATCHING PATTERN "*.h*")












