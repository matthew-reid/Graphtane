# Once done this will define
#  GCommon_FOUND
#  GCommon_INCLUDE_DIRS
#  GCommon_LIBRARIES
#

set(GCommon_INCLUDE_DIRS)
set(GCommon_LIBRARIES optimized GCommon debug GCommond)
set(GCommon_FOUND "YES")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GCommon DEFAULT_MSG GCommon_LIBRARIES)