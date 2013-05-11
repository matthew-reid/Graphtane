# This module defines
#  OpenEXRHalf_FOUND, if false, do not try to link
#  OpenEXRHalf_INCLUDE_DIR, where to find headers
#  OpenEXRHalf_LIBRARIES, where to find headers

set(OpenEXR_DIR $ENV{OpenEXR_HOME})


FIND_PATH(OpenEXRHalf_INCLUDE_DIR OpenEXR/half.h
  PATHS ${OpenEXR_DIR}
  PATH_SUFFIXES include
)

FIND_LIBRARY(OpenEXRHalf_LIBRARY_RELEASE
	NAMES Half.lib Half
	PATHS ${OpenEXR_DIR}
	PATH_SUFFIXES lib lib/release
)

FIND_LIBRARY(OpenEXRHalf_LIBRARY_DEBUG
	NAMES Halfd.lib Halfd
	PATHS ${OpenEXR_DIR}
	PATH_SUFFIXES lib lib/debug
)

IF(OpenEXRHalf_INCLUDE_DIR AND OpenEXRHalf_LIBRARY_RELEASE AND OpenEXRHalf_LIBRARY_DEBUG)
  SET(OpenEXRHalf_FOUND "YES")
  SET(OpenEXRHalf_LIBRARIES optimized ${OpenEXRHalf_LIBRARY_RELEASE} debug ${OpenEXRHalf_LIBRARY_DEBUG})
ENDIF()
