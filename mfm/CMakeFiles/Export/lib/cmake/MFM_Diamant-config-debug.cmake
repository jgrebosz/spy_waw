#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "MFM_Diamant::MFM_Diamant" for configuration "Debug"
set_property(TARGET MFM_Diamant::MFM_Diamant APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(MFM_Diamant::MFM_Diamant PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libMFM_Diamant.so"
  IMPORTED_SONAME_DEBUG "libMFM_Diamant.so"
  )

list(APPEND _IMPORT_CHECK_TARGETS MFM_Diamant::MFM_Diamant )
list(APPEND _IMPORT_CHECK_FILES_FOR_MFM_Diamant::MFM_Diamant "${_IMPORT_PREFIX}/lib/libMFM_Diamant.so" )

# Import target "MFM_Diamant::MFMtest" for configuration "Debug"
set_property(TARGET MFM_Diamant::MFMtest APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(MFM_Diamant::MFMtest PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/MFMtest"
  )

list(APPEND _IMPORT_CHECK_TARGETS MFM_Diamant::MFMtest )
list(APPEND _IMPORT_CHECK_FILES_FOR_MFM_Diamant::MFMtest "${_IMPORT_PREFIX}/bin/MFMtest" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
