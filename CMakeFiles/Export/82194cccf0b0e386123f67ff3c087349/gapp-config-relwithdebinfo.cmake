#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "gapp::gapp" for configuration "RelWithDebInfo"
set_property(TARGET gapp::gapp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(gapp::gapp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/RelWithDebInfo/libgapp.a"
  )

list(APPEND _cmake_import_check_targets gapp::gapp )
list(APPEND _cmake_import_check_files_for_gapp::gapp "${_IMPORT_PREFIX}/lib/RelWithDebInfo/libgapp.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
