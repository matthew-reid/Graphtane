# Graphtane

## Overview
Key libraries:
* **GFluid** - OpenCL volumetric fluid dynamics solver
* **GSparseVolumes** - ray-marching sparse volume rendering
* **GSparseVolumesVdb** - OpenVDB volume rendering support
* **VdbViewerApp** - application for viewing VDB files
* **GCompute** - thin C++ OpenCL wrapper
* **GVis** - thin OpenGL wrapper

## Building
The solution can be build with CMake. The build can be configured through CMake using build option variables (BUILD_*).

Required dependencies:
* Boost
* GLEW
* GLFW
* GLM

Optional dependencies:
* OpenCL C++ libraries (required by OpenCL projects)
* OpenVDB (required by OpenVDB projects)


## Examples
* Executables must be run with working directory set to the root repository directory.
* Camera can be moved with WASD keys and mouse when in free camera mode.
* Use `VdbViewerApp -h` to see command line options for viewing VDB files.

## License
See License.txt