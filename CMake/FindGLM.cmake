# This module defines
#  GLM_FOUND, if false, do not try to link
#  GLM_INCLUDE_DIR, where to find headers


set(GLM_DIR $ENV{GLM_HOME})
mark_as_advanced(GLM_DIR)

FIND_PATH(GLM_INCLUDE_DIR glm/glm.hpp
  PATHS
  ${GLM_DIR}
)

IF(GLM_INCLUDE_DIR)
  SET(GLM_FOUND "YES")
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLM DEFAULT_MSG GLM_INCLUDE_DIR)