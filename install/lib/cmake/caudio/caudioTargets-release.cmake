#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "caudio::caudio" for configuration "Release"
set_property(TARGET caudio::caudio APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(caudio::caudio PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/caudio.lib"
  )

list(APPEND _cmake_import_check_targets caudio::caudio )
list(APPEND _cmake_import_check_files_for_caudio::caudio "${_IMPORT_PREFIX}/lib/caudio.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
