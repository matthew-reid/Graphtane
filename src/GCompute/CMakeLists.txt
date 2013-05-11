file(GLOB CoreFiles *.cpp *.h)

set(SourceFiles ${CoreFiles})

SOURCE_GROUP("Source Files" FILES ${CoreFiles})

find_package(OPENCL REQUIRED)
include_directories(${OPENCL_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

find_package(GCommon REQUIRED)
include_directories(${GCommon_INCLUDE_DIRS})

include_directories(..)

add_library(GCompute ${Graphtane_LIB_TYPE} ${SourceFiles})

target_link_libraries(GCompute ${GCommon_LIBRARIES} ${OPENCL_LIBRARIES})
