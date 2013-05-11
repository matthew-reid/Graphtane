# This module defines
#  Graphtane_FOUND, if false, do not try to link
#  Graphtane_INCLUDE_DIRS, where to find headers
#  Graphtane_LIBRARIES_DIR, where to find libraries
#  Graphtane_CMAKE_DIR, where to find CMAKE scripts

set(Graphtane_DIR $ENV{Graphtane_HOME})
set(Graphtane_BUILD_DIR $ENV{Graphtane_BUILD})


find_path(Graphtane_INCLUDE_DIRS GCommon/CommonFwd.h
  PATHS ${Graphtane_DIR}
  PATH_SUFFIXES src
)

find_path(Graphtane_CMAKE_DIR FindGVis.cmake
  PATHS ${Graphtane_DIR}
  PATH_SUFFIXES CMake
)

find_library(TEMP_LIB
  NAMES Common Commond
  PATHS ${Graphtane_BUILD_DIR}
  PATH_SUFFIXES lib/Release / lib/Debug
)
mark_as_advanced(TEMP_LIB)
get_filename_component(TEMP_LIB ${TEMP_LIB} DIRECTORY) # Back up to leaf directory (release or debug folder)
get_filename_component(Graphtane_LIBRARIES_DIR ${TEMP_LIB} DIRECTORY CACHE) # Back up to lib folder)


if(Graphtane_INCLUDE_DIRS AND Graphtane_CMAKE_DIR AND Graphtane_LIBRARIES_DIR)
  SET(Graphtane_FOUND "YES")
endif()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Graphtane DEFAULT_MSG Graphtane_INCLUDE_DIRS Graphtane_LIBRARIES_DIR Graphtane_CMAKE_DIR)