// Copyright (c) 2013-2014 Matthew Paul Reid

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include "GFluidFwd.h"
#include <GCompute/GComputeFwd.h>

#include <boost/scoped_ptr.hpp>
#include <string>

namespace GFluid {

//! Scales computes filtered isosurface normals of a 3D density buffer
//! Output normalTexture will be downscaled to half input dimensions
//! Takes a BufferProvider instead of a Buffer directly so as to support double buffering on the input side
class IsosurfaceNormalCalculator
{
public:
	virtual ~IsosurfaceNormalCalculator() {}
	virtual void updateTexture() = 0;
};

//! @param normalTexture is the output texture. RGB=XYZ, values in range [0,1]
//! @param densityBuffer must be 2x the size in each dimension of the normalTexture
extern IsosurfaceNormalCalculatorPtr createIsosurfaceNormalCalculator(const GCompute::ClSystem& system, const GCompute::GlTexture& normalTexture,
																	  const BufferProviderPtr& densityBufferProvider, const TempBufferPoolPtr& tempBufferPool,
																	  const std::string& fluidKernelsDir);

} // namespace GFluid
