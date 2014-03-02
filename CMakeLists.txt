cmake_minimum_required (VERSION 2.8.3)
project(Graphtane)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")

OPTION(BUILD_Graphtane_STATIC "Build Static Libraries" 0)
if(BUILD_Graphtane_STATIC)
	set(Graphtane_LIB_TYPE "STATIC")
else()
	set(Graphtane_LIB_TYPE "SHARED")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_DEBUG_POSTFIX d)
link_directories(${CMAKE_BINARY_DIR}/lib)

find_package(Boost COMPONENTS thread regex system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(src/GAppFramework)
add_subdirectory(src/GCommon)
add_subdirectory(src/GSparseVolumes)
add_subdirectory(src/GVis)

add_subdirectory(src/TestGeometryShader)
add_subdirectory(src/TestTessellationShader)
add_subdirectory(src/TestVolumeShader)

option(BUILD_OPENCL_PROJECTS "Build OpenCL projects" TRUE)
if(BUILD_OPENCL_PROJECTS)
	add_subdirectory(src/GCompute)
	add_subdirectory(src/GComputeVis)
	add_subdirectory(src/GFluid)
	add_subdirectory(src/TestFluid2d)
	add_subdirectory(src/TestFluid3d)
endif()

option(BUILD_OPENVDB_PROJECTS "Build OpenVdb projects" TRUE)
if(BUILD_OPENVDB_PROJECTS)
	include(AddOpenVdbDefinitions)

	add_subdirectory(src/GSparseVolumesVdb)
	add_subdirectory(src/VdbViewerApp)
endif()
