file(GLOB CoreFiles *.cpp *.h)

set(SourceFiles ${CoreFiles})

SOURCE_GROUP("Source Files" FILES ${CoreFiles})

include_directories(..)

find_package(GVis)
include_directories(${GVis_INCLUDE_DIRS})

find_package(OpenVdb)
include_directories(${OpenVdb_INCLUDE_DIRS})
AddOpenVdbDefinitions()

add_library(GSparseVolumesVdb ${Graphtane_LIB_TYPE} ${SourceFiles})

target_link_libraries (GSparseVolumesVdb GSparseVolumes ${OpenVdb_LIBRARIES})