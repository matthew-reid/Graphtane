set(APP_NAME TestTessellationShader)

file(GLOB CoreFiles *.cpp *.h)

set(SourceFiles ${CoreFiles})

SOURCE_GROUP("Source Files" FILES ${CoreFiles})

include_directories(..)

find_package(GAppFramework)
include_directories(${GAppFramework_INCLUDE_DIRS})

add_executable(${APP_NAME} ${SourceFiles})

target_link_libraries (${APP_NAME} ${GAppFramework_LIBRARIES})
