# This module defines
#  GLEW_FOUND, if false, do not try to link
#  GLEW_LIBRARIES, the libraries to link against
#  GLEW_INCLUDE_DIR, where to find headers


set(GLEW_DIR $ENV{GLEW_HOME})


FIND_PATH(GLEW_INCLUDE_DIR GL/glew.h
  PATHS
  ${GLEW_DIR}/include
)

find_library(GLEW_LIBRARY
  NAMES GLEW glew32s
  PATHS
  ${GLEW_DIR}/lib
)

find_library(GLEW_MX_LIBRARY
  NAMES GLEWmx glew32mxs
  PATHS
  ${GLEW_DIR}/lib
  /opt/local/
)

set(GLEW_LIBRARIES ${GLEW_LIBRARY} ${GLEW_MX_LIBRARY})

IF(GLEW_LIBRARIES AND GLEW_INCLUDE_DIR)
  SET(GLEW_FOUND "YES")
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLEW DEFAULT_MSG GLEW_INCLUDE_DIR GLEW_LIBRARIES)