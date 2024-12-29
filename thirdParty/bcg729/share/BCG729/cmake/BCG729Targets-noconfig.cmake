#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "bcg729" for configuration ""
set_property(TARGET bcg729 APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(bcg729 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libbcg729.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS bcg729 )
list(APPEND _IMPORT_CHECK_FILES_FOR_bcg729 "${_IMPORT_PREFIX}/lib/libbcg729.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
