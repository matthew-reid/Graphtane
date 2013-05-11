# Once done this will define
#  GCompute_FOUND
#  GCompute_INCLUDE_DIRS
#  GCompute_LIBRARIES
#

set(GCompute_INCLUDE_DIRS)
set(GCompute_LIBRARIES optimized GCompute debug GComputed)

find_package(OPENCL REQUIRED)
list(APPEND GCompute_INCLUDE_DIRS ${OPENCL_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
list(APPEND GCompute_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
list(APPEND GCompute_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})

set(GCompute_FOUND "YES")