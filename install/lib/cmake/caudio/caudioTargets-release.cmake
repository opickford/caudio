#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "sfc::sfc" for configuration "Release"
set_property(TARGET sfc::sfc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(sfc::sfc PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/sfc.lib"
  )

list(APPEND _cmake_import_check_targets sfc::sfc )
list(APPEND _cmake_import_check_files_for_sfc::sfc "${_IMPORT_PREFIX}/lib/sfc.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
