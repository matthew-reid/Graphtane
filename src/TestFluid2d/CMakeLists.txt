set(APP_NAME TestFluid2d)

file(GLOB CoreFiles *.cpp *.h)

set(SourceFiles ${CoreFiles} ${GVisFiles} ${PrimitivesFiles})

SOURCE_GROUP("Source Files" FILES ${CoreFiles})

include_directories(..)

find_package(GAppFramework)
include_directories(${GAppFramework_INCLUDE_DIRS})

find_package(GCompute)
include_directories(${GCompute_INCLUDE_DIRS})

add_executable(${APP_NAME} ${SourceFiles})

target_link_libraries (${APP_NAME} ${GCompute_LIBRARIES} GFluid GComputeVis ${GAppFramework_LIBRARIES})
