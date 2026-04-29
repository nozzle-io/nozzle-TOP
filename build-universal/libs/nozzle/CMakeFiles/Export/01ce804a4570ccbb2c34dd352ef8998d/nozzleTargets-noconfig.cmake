#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "bbb::nozzle" for configuration ""
set_property(TARGET bbb::nozzle APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(bbb::nozzle PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX;OBJCXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libnozzle.a"
  )

list(APPEND _cmake_import_check_targets bbb::nozzle )
list(APPEND _cmake_import_check_files_for_bbb::nozzle "${_IMPORT_PREFIX}/lib/libnozzle.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
