file(GLOB CoreFiles *.cpp *.h)

set(SourceFiles ${CoreFiles})

SOURCE_GROUP("Source Files" FILES ${CoreFiles})

add_library(GCommon ${Graphtane_LIB_TYPE} ${SourceFiles})
target_link_libraries(GCommon ${Boost_LIBRARIES})
