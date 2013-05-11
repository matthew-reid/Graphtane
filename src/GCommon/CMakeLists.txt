file(GLOB CoreFiles *.cpp *.h)

set(SourceFiles ${CoreFiles})

SOURCE_GROUP("Source Files" FILES ${CoreFiles})

add_library(GCommon ${Graphtane_LIB_TYPE} ${SourceFiles})
