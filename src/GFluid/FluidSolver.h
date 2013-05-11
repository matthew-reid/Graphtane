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
#include "BufferProvider.h"

#include <GCompute/GComputeFwd.h>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

namespace GFluid {

struct FluidSolverParams
{
	static FluidSolverParams createDefault()
	{
		FluidSolverParams params;
		params.densityWeight = 5000;
		params.temperatureBuoyancy = 120;
		params.coolingRate = 0.2;
		params.drag = 2;
		return params;
	};

	float densityWeight;
	float temperatureBuoyancy;
	float coolingRate;
	float drag;
};

struct Float3
{
	Float3(float x, float y, float z) :
		x(x), y(y), z(z) {}

	float x;
	float y;
	float z;
};

class FluidSolver : public BufferProvider
{
public:
	virtual ~FluidSolver() {};

	virtual void update(float dt) = 0;

	virtual void setFluid(const Float3& position, float density, float temperature) = 0;
	virtual void addFluid(const Float3& position, float density, float temperature) = 0;
	virtual void applyImpulse(const Float3& position, const Float3& impulse) = 0;

	virtual cl::Buffer& getOutputBuffer() const = 0;

	//! Power to use when writing to the output texture. Default is 1.0.
	virtual void setOutputWriteGammaPower(float power) = 0;
	virtual float getOutputWriteGammaPower() const = 0;

	const FluidSolverParamsPtr& getParams() const {return m_params;}

protected:
	FluidSolver();
	FluidSolverParamsPtr m_params;
};

extern FluidSolverPtr createFluidSolver(GCompute::ClSystem& system, const GCompute::GlTexture& fluidStateTexture,
										const TempBufferPoolPtr& tempBufferPool, const std::string& fluidKernalsDir);

} // namespace GFluid
