# Once done this will define
#  GAppFramework_FOUND
#  GAppFramework_INCLUDE_DIRS
#  GAppFramework_LIBRARIES
#

set(GAppFramework_INCLUDE_DIRS)
set(GAppFramework_LIBRARIES optimized GAppFramework debug GAppFrameworkd)

find_package(GCommon REQUIRED)
list(APPEND GAppFramework_INCLUDE_DIRS ${GCommon_INCLUDE_DIRS})
list(APPEND GAppFramework_LIBRARIES ${GCommon_LIBRARIES})

find_package(GVis REQUIRED)
list(APPEND GAppFramework_INCLUDE_DIRS ${GVis_INCLUDE_DIRS})
list(APPEND GAppFramework_LIBRARIES ${GVis_LIBRARIES})

set(GAppFramework_FOUND "YES")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GAppFramework DEFAULT_MSG GAppFramework_INCLUDE_DIRS GAppFramework_LIBRARIES)