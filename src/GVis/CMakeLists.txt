file(GLOB CoreFiles *.cpp *.h)
file(GLOB PrimitivesFiles Primitives/*.cpp Primitives/*.h)
file(GLOB IoFiles Io/*.cpp Io/*.h)

set(SourceFiles ${CoreFiles} ${PrimitivesFiles} ${IoFiles} ${UtilityFiles})

SOURCE_GROUP("Source Files" FILES ${CoreFiles})
SOURCE_GROUP("Source Files\\Primitives" FILES ${PrimitivesFiles})
SOURCE_GROUP("Source Files\\Io" FILES ${IoFiles})


find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

include_directories(..)

add_library(GVis ${Graphtane_LIB_TYPE} ${SourceFiles})

target_link_libraries (GVis GCommon ${OPENGL_LIBRARIES}
			${GLEW_LIBRARIES} ${GLFW_LIBRARIES}
)
