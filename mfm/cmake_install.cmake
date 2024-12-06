# Install script for directory: /home/grebosz/zro/waw/moj_diamant/mfm

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMFM_Diamant.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMFM_Diamant.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMFM_Diamant.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/grebosz/zro/waw/moj_diamant/mfm/libMFM_Diamant.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMFM_Diamant.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMFM_Diamant.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMFM_Diamant.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MFMtest" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MFMtest")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MFMtest"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/grebosz/zro/waw/moj_diamant/mfm/MFMtest")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MFMtest" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MFMtest")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MFMtest"
         OLD_RPATH "/home/grebosz/zro/waw/moj_diamant/mfm:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/MFMtest")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/grebosz/zro/waw/moj_diamant/mfm/tinyxml/tinyxml.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/tinyxml/tinystr.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/Cobo.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/DataGenParameters.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/DataPar.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/DataParameters.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/DataScal.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/DataScalers.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MError.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMAllFrames.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMBasicFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMBlobFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMBoxDiagFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMChimeraFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMCoboFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMCoboTopoFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMCommonFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMDiamantFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMEbyedatFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMExogamFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMHelloFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMMergeFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMMutantFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMNedaCompFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMNedaFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMNedaFrame_old.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMNumExoFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMOscilloFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMRibfFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMS3AlphaFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMS3BaF2Frame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMS3RuthFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMS3SynchroFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMS3eGUNFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMScalerDataFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMTypes.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMVamosICFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMVamosPDFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMVamosTACFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMXmlDataDescriptionFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMXmlFileHeaderFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/XmlTags.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMReaGenericFrame.h"
    "/home/grebosz/zro/waw/moj_diamant/mfm/sources/MFMReaScopeFrame.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/MFM_Diamant-config.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/MFM_Diamant-config.cmake"
         "/home/grebosz/zro/waw/moj_diamant/mfm/CMakeFiles/Export/lib/cmake/MFM_Diamant-config.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/MFM_Diamant-config-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/MFM_Diamant-config.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "/home/grebosz/zro/waw/moj_diamant/mfm/CMakeFiles/Export/lib/cmake/MFM_Diamant-config.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake" TYPE FILE FILES "/home/grebosz/zro/waw/moj_diamant/mfm/CMakeFiles/Export/lib/cmake/MFM_Diamant-config-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/grebosz/zro/waw/moj_diamant/mfm/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
