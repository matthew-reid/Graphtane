# This module defines
#  GLFW_FOUND, if false, do not try to link
#  GLFW_LIBRARIES, the libraries to link against
#  GLFW_INCLUDE_DIR, where to find headers


set(GLFW_DIR $ENV{GLFW_HOME})


FIND_PATH(GLFW_INCLUDE_DIR GLFW/glfw3.h
  PATHS
  ${GLFW_DIR}/include
)

find_library(GLFW_LIBRARIES
  NAMES glfw GLFW3
  PATHS
  ${GLFW_DIR}/lib
  /opt/local/
)

IF(GLFW_LIBRARIES AND GLFW_INCLUDE_DIR)
  SET(GLFW_FOUND "YES")
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW DEFAULT_MSG GLFW_INCLUDE_DIR GLFW_LIBRARIES)