file(GLOB CoreFiles *.cpp *.h)

set(SourceFiles ${CoreFiles})

SOURCE_GROUP("Source Files" FILES ${CoreFiles})

include_directories(..)

find_package(OPENCL REQUIRED)
include_directories(${OPENCL_INCLUDE_DIRS})

find_package(GCompute)
include_directories(${GCompute_INCLUDE_DIRS})

add_library(GFluid ${Graphtane_LIB_TYPE} ${SourceFiles})

target_link_libraries(GFluid ${OPENCL_LIBRARIES} ${GCompute_LIBRARIES})
