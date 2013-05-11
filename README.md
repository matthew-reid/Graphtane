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
Build solution using CMake.

## Examples
* Executables must be run with working directory set to the root checkout directory.
* Camera can be moved with WASD keys and mouse when in free camera mode.
* Use `VdbViewerApp -h` to see command line options for viewing VDB files.

## License
See License.txt