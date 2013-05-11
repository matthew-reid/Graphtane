# This module defines
#  OpenVdb_FOUND, if false, do not try to link
#  OpenVdb_LIBRARIES, the libraries to link against
#  OpenVdb_INCLUDE_DIRS, where to find headers


set(OpenVdb_DIR $ENV{OPEN_VDB_HOME})


FIND_PATH(OpenVdb_INCLUDE_DIRS openvdb/openvdb.h
  PATHS
  ${OpenVdb_DIR}/include
)

find_library(OpenVdb_LIBRARY_RELEASE
  NAMES libopenvdb.lib
  PATHS
  ${OpenVdb_DIR}/lib/Release
  ${OpenVdb_DIR}/lib
)

find_library(OpenVdb_LIBRARY_DEBUG
  NAMES libopenvdbd.lib
  PATHS
  ${OpenVdb_DIR}/lib/Debug
  ${OpenVdb_DIR}/lib
)

set(OpenVdb_LIBRARIES optimized ${OpenVdb_LIBRARY_RELEASE} debug ${OpenVdb_LIBRARY_DEBUG})

if(OpenVdb_LIBRARIES AND OpenVdb_INCLUDE_DIRS)
	set(OpenVdb_FOUND "YES")

	find_package(TBB REQUIRED)
	list(APPEND OpenVdb_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})
	list(APPEND OpenVdb_LIBRARIES optimized ${TBB_LIBRARIES} debug ${TBB_DEBUG_LIBRARIES})
	
	find_package(ILMBaseHalf REQUIRED)
	list(APPEND OpenVdb_INCLUDE_DIRS ${OpenEXRHalf_INCLUDE_DIR})
	list(APPEND OpenVdb_LIBRARIES ${OpenEXRHalf_LIBRARIES})
	
	find_package(ZLIB REQUIRED)
 	list(APPEND OpenVdb_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS})
	list(APPEND OpenVdb_LIBRARIES ${ZLIB_LIBRARIES})
	
endif()
