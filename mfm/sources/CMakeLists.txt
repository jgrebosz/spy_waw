# Nous voulons un cmake "récent" pour utiliser les dernières fonctionnalités
cmake_minimum_required(VERSION 2.8.12)
project(MFMlib)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

macro(DATE RESULT)
	if(UNIX)
		execute_process(COMMAND "date" "+%y.%m.%d" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "\n" "" ${RESULT} ${${RESULT}})
	else()
		message(SEND_ERROR "Unable to detect date")
		SET(${RESULT} 00.00.00)
	endif()
endmacro()

macro(TIME RESULT)
	if(UNIX)
		execute_process(COMMAND "date" "+%H:%M:%S" OUTPUT_VARIABLE ${RESULT})
		string(REGEX REPLACE "\n" "" ${RESULT} ${${RESULT}})
	else()
		message(SEND_ERROR "Unable to detect time")
		set(${RESULT} UNKNOWN)
	endif()
endmacro()

DATE(BUILD_MFM_DATE)
TIME(BUILD_MFM_TIME)

message(STATUS "Compilation date = ${BUILD_MFM_DATE} ${BUILD_MFM_TIME}")

configure_file(MFMlib.in MFMlib.in.h)

# force -fPIC option n even in .a library
set(CMAKE_POSITION_INDEPENDENT_CODE ON) 

if(NO_MFMNXML)
message(STATUS "compilation without tinyxml")

#target_compile_options(MFMlib NO_MFMNXML)
#target_compile_options(MFMliba NO_MFMNXML)
#target_compile_options(MFMtest.exe NO_MFMNXML)
#target_compile_options(MFMFileTest.exe NO_MFMNXML)
add_definitions(-DNO_MFMXML)

else()
find_library(TinyXML_LIBRARIES tinyxml HINTS ${MYTINYXMLDIR} )
	if(TinyXML_LIBRARIES)
   	message(STATUS "-- found local ${TinyXML_LIBRARIES}")
	else()
	find_library(TinyXML_LIBRARIES tinyxml  )
		if (TinyXML_LIBRARIES)
		message(STATUS "-- found  ${TinyXML_LIBRARIES}")
		else()
   		message(WARNING "-- TinyXML_LIBRARIES not found")
   		endif()
	endif()
endif()

if(CMAKE_INSTALL_PREFIX)
   message(STATUS "-- found CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
else()
   message(WARNING "-- CMAKE_INSTALL_PREFIX not found ")
endif()

include_directories(${CMAKE_BINARY_DIR})
message("-- CMAKE_CXX_FLAGS_RELEASE =  ${CMAKE_CXX_FLAGS_RELEASE}")
message("-- CMAKE_CXX_FLAGS_DEBUG =  ${CMAKE_CXX_FLAGS_DEBUG}")

# Créé des variables avec les fichiers à compiler
set(SRCSlib
DataGenParameters.cc  MFMBlobFrame.cc      MFMEbyedatFrame.cc   MFMNedaFrame.cc     MFMS3RuthFrame.cc      MFMXmlDataDescriptionFrame.cc
DataParameters.cc     MFMBoxDiagFrame.cc   MFMExogamFrame.cc    MFMNumExoFrame.cc   MFMS3SynchroFrame.cc   MFMXmlFileHeaderFrame.cc
DataPar.cc            MFMChimeraFrame.cc   MFMOscilloFrame.cc   MFMScalerDataFrame.cc
DataScal.cc           MFMCoboFrame.cc      MFMHelloFrame.cc     MFMRibfFrame.cc     DataScalers.cc
MFMCoboTopoFrame.cc   MFMMergeFrame.cc     MFMS3AlphaFrame.cc   MFMVamosICFrame.cc
MError.cc             MFMCommonFrame.cc    MFMMutantFrame.cc    MFMS3BaF2Frame.cc   MFMVamosPDFrame.cc    MFMReaGenericFrame.cc
MFMBasicFrame.cc      MFMDiamantFrame.cc   MFMNedaCompFrame.cc  MFMS3eGUNFrame.cc   MFMVamosTACFrame.cc   MFMReaScopeFrame.cc
)
	
set(HEADERSlib
Cobo.h               MError.h           MFMCoboFrame.h      MFMHelloFrame.h     MFMNumExoFrame.h   MFMS3RuthFrame.h      MFMVamosTACFrame.h
DataGenParameters.h  MFMAllFrames.h     MFMCoboTopoFrame.h  MFMMergeFrame.h     MFMOscilloFrame.h  MFMS3SynchroFrame.h   MFMXmlDataDescriptionFrame.h
DataParameters.h     MFMBasicFrame.h    MFMCommonFrame.h    MFMMutantFrame.h    MFMRibfFrame.h     MFMScalerDataFrame.h  MFMXmlFileHeaderFrame.h
DataPar.h            MFMBlobFrame.h     MFMDiamantFrame.h   MFMNedaCompFrame.h  MFMS3AlphaFrame.h  MFMTypes.h            XmlTags.h
DataScalers.h        MFMBoxDiagFrame.h  MFMEbyedatFrame.h   MFMNedaFrame.h      MFMS3BaF2Frame.h   MFMVamosICFrame.h     MFMReaScopeFrame.h
DataScal.h           MFMChimeraFrame.h  MFMExogamFrame.h    MFMNedaFrame_old.h  MFMS3eGUNFrame.h   MFMVamosPDFrame.h     MFMReaGenericFrame.h
)
	


add_library(MFMa STATIC ${SRCSlib} ${HEADERSlib})
set_target_properties(MFMa PROPERTIES OUTPUT_NAME MFM)
target_link_libraries(MFMa)

add_library(MFM SHARED    ${SRCSlib} ${HEADERSlib})
target_link_libraries(MFM  ${TinyXML_LIBRARIES})

add_executable(MFMtest.exe MFMtest.cc ${HEADERSlib})
target_link_libraries(MFMtest.exe MFMa ${TinyXML_LIBRARIES} )

#add_executable(MFMFileTest.exe $MFMFileTest.cc ${HEADERSexe})
#target_link_libraries(MFMFileTest.exe  ${TinyXML_LIBRARIES} )

install(TARGETS MFMtest.exe MFMa MFM DESTINATION bin
		LIBRARY   DESTINATION lib
	        ARCHIVE   DESTINATION lib
	        INCLUDES  DESTINATION include
		COMPONENT libraries)

install(FILES ${HEADERSlib} DESTINATION include)
