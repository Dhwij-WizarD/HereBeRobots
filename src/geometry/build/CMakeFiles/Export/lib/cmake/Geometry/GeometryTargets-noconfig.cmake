#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Geometry::Geometry" for configuration ""
set_property(TARGET Geometry::Geometry APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(Geometry::Geometry PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libGeometry.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS Geometry::Geometry )
list(APPEND _IMPORT_CHECK_FILES_FOR_Geometry::Geometry "${_IMPORT_PREFIX}/lib/libGeometry.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
