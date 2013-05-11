file(GLOB CoreFiles *.cpp *.h)

set(SourceFiles ${CoreFiles})

SOURCE_GROUP("Source Files" FILES ${CoreFiles})

include_directories(..)

find_package(GCompute)
include_directories(${GCompute_INCLUDE_DIRS})

find_package(GVis)
include_directories(${GVis_INCLUDE_DIRS})

add_library(GComputeVis ${Graphtane_LIB_TYPE} ${SourceFiles})

target_link_libraries(GComputeVis ${GCompute_LIBRARIES} ${GGVis_LIBRARIES})
