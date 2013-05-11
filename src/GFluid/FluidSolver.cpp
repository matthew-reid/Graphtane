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

#include "FluidSolver.h"
#include "Advecter.h"
#include "DivergenceFreeProjector.h"
#include "KernelRunner.h"
#include "TempBufferPool.h"

#include <GCompute/ClSystem.h>
#include <GCompute/ClIncludes.h>
#include <GCompute/GlTexture.h>

#include <boost/scoped_array.hpp>

#include "../../Kernels/Fluid/FluidDataTypes.h"
#include "../../Kernels/Fluid/Params.h"

#include <string>

using namespace GCompute;
using namespace GFluid;

namespace GFluid {

static cl_float2 toClFloat2(float x, float y)
{
		cl_float2 v;
		v.s[0] = x;
		v.s[1] = y;
		return v;
}

static cl_float4 toClFloat4(const Float3& position)
{
	cl_float4 result;
	result.s[0] = position.x;
	result.s[1] = position.y;
	result.s[2] = position.z;
	result.s[3] = 0;
	return result;
}

static cl_float2 toClFloat2(const Float3& position)
{
	cl_float2 result;
	result.s[0] = position.x;
	result.s[1] = position.y;
	return result;
}

class FluidSolverI : public FluidSolver
{
public:
	FluidSolverI(ClSystem& system, const GlTexture& fluidStateTexture, const TempBufferPoolPtr& tempBufferPool, const std::string& fluidKernalsDir) :
		m_width(fluidStateTexture.width),
		m_height(fluidStateTexture.height),
		m_depth(fluidStateTexture.depth),
		m_tempBufferPool(tempBufferPool),
		m_outputWriteGammaPower(1.0)
	{
		assert(m_width > 0);
		assert(m_height > 0);
		assert(m_depth > 0);
		assert(m_tempBufferPool);

		int elementCount = m_width * m_height * m_depth;
		assert(m_tempBufferPool->getBufferElementCount() >= elementCount);

		// Create command queue
		cl_int err;
		m_queue = cl::CommandQueue(system._getContext(), system._getDevice(), 0, &err);

		// Create kernel runner
		{
			cl::NDRange globalThreads = cl::NDRange(m_width, m_height, m_depth);
			cl::NDRange localThreads;

			if (m_depth > 1)
			{
				localThreads = cl::NullRange; 

				// TODO: should specifiy localThreads here, but defaults run faster on my AMD 7770. Need to find better values.
				//localThreads = cl::NDRange(8, 8, 4);
			}
			else
			{
				localThreads = cl::NDRange(8, 16, 1);
			}

			m_kernelRunner.reset(new KernelRunner(m_queue, globalThreads, localThreads));
		}


		// Load kernels
		system.loadProgram(m_program, fluidKernalsDir + "/FluidDynamics.cl");

		ClSystem::createKernel(m_kernel_applyForces, m_program, "applyForces");
		ClSystem::createKernel(m_kernel_coolFluid, m_program, "coolFluid");
		ClSystem::createKernel(m_kernel_setFluid, m_program, "setFluid");
		ClSystem::createKernel(m_kernel_addFluid, m_program, "addFluid");

		ClSystem::createKernel(m_kernel_visFluid, m_program, "visFluid");
		ClSystem::createKernel(m_kernel_visVelocity, m_program, "visVelocity");

		ClSystem::createKernel(m_kernel_applyImpulse, m_program, "applyImpulse");

		// Create fluid state grids
		{
			int gridSize = elementCount * sizeof(FluidState);
			boost::scoped_array<FluidState> data(new FluidState[elementCount]);
			memset(data.get(), 0, gridSize);

			for (int i = 0; i < fluidStateGridCount; i++)
			{
				m_fluidStateGrids[i] = cl::Buffer(system._getContext(), CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE, gridSize, data.get(), &err);
				checkError(err);
			}

			m_fluidStateGridInputPtr = &m_fluidStateGrids[0];
			m_fluidStateGridOutputPtr = &m_fluidStateGrids[1];
		}

		cl::Buffer& tempBuffer = m_tempBufferPool->getFloatBuffer(1);

		m_divergenceFreeProjector.reset(new DivergenceFreeProjector(m_kernelRunner, m_program, &tempBuffer));

		system.loadProgram(m_advectFloat3Pogram, fluidKernalsDir + "/AdvectionFloat3.cl");
		m_float3Advecter = createAdvecter(m_kernelRunner, m_advectFloat3Pogram, &tempBuffer);

		system.loadProgram(m_advectFluidStatePogram, fluidKernalsDir + "/AdvectionFluidState.cl");
		m_fluidStateAdvecter = createAdvecter(m_kernelRunner, m_advectFluidStatePogram, &tempBuffer);


		// Create velocity grids
		{
			int elementSize = sizeof(cl_float3);

			int dataSize = elementCount * elementSize;
			boost::scoped_array<float> data(new float[dataSize]);
			memset(data.get(), 1, dataSize);

			for (int i = 0; i < velocityGridCount; i++)
			{
				m_velocityGrids[i] = cl::Buffer(system._getContext(), CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE, dataSize, data.get(), &err);
				checkError(err);
			}

			m_velocityGridInputPtr = &m_velocityGrids[0];
			m_velocityGridOutputPtr = &m_velocityGrids[1];
		}

		// Create fluid state image buffer
		m_fluidStateImageBuffer = cl::ImageGL(system._getContext(), CL_MEM_WRITE_ONLY, fluidStateTexture.target, 0, fluidStateTexture.textureId, &err);
		checkError(err);

		// Create parameters buffer
		m_paramsBuffer = cl::Buffer(system._getContext(), CL_MEM_READ_ONLY, sizeof(Params), NULL, &err);
	}

	void update(float dt)
	{
		uploadParams();
		simulateFluid(dt);
		visFluid();
	}

	void setFluid(const Float3& position, float density, float temperature)
	{
		FluidState fluidState;
		fluidState.s[0] = density;
		fluidState.s[1] = temperature;

		// Set arguments
		checkError(m_kernel_setFluid.setArg(0, *m_fluidStateGridInputPtr));
		checkError(m_kernel_setFluid.setArg(1, toClFloat4(position)));
		checkError(m_kernel_setFluid.setArg(2, fluidState));

		m_kernelRunner->run(m_kernel_setFluid);
	}

	void addFluid(const Float3& position, float density, float temperature)
	{
		FluidState fluidState;
		fluidState.s[0] = density;
		fluidState.s[1] = temperature;

		// Set arguments
		checkError(m_kernel_addFluid.setArg(0, *m_fluidStateGridInputPtr));
		checkError(m_kernel_addFluid.setArg(1, toClFloat4(position)));
		checkError(m_kernel_addFluid.setArg(2, fluidState));

		m_kernelRunner->run(m_kernel_addFluid);
	}

	void applyImpulse(const Float3& position, const Float3& impulse)
	{
		checkError(m_kernel_applyImpulse.setArg(0, *m_velocityGridInputPtr));
		checkError(m_kernel_applyImpulse.setArg(1, toClFloat4(position)));
		checkError(m_kernel_applyImpulse.setArg(2, toClFloat4(impulse)));

		m_kernelRunner->run(m_kernel_applyImpulse);
	}

	cl::Buffer& getOutputBuffer() const
	{
		return *m_fluidStateGridInputPtr;
	}

private:
	void simulateFluid(float dt)
	{
		// Velocity Advection
		m_float3Advecter->advect(*m_velocityGridOutputPtr, *m_velocityGridInputPtr, *m_velocityGridInputPtr, dt);

		// swap buffers
		std::swap(m_velocityGridInputPtr, m_velocityGridOutputPtr);

		// Apply forces
		{
			// Set arguments
			checkError(m_kernel_applyForces.setArg(0, *m_velocityGridInputPtr));
			checkError(m_kernel_applyForces.setArg(1, *m_fluidStateGridInputPtr));
			checkError(m_kernel_applyForces.setArg(2, dt));
			checkError(m_kernel_applyForces.setArg(3, m_paramsBuffer));

			m_kernelRunner->run(m_kernel_applyForces);
		}

		m_divergenceFreeProjector->makeDivergenceFree(*m_velocityGridInputPtr);

		// GFluid Cooling
		{
			checkError(m_kernel_coolFluid.setArg(0, *m_fluidStateGridInputPtr));
			checkError(m_kernel_coolFluid.setArg(1, dt));
			checkError(m_kernel_coolFluid.setArg(2, m_paramsBuffer));

			m_kernelRunner->run(m_kernel_coolFluid);
		}

		// GFluid State Advection
		m_fluidStateAdvecter->advect(*m_fluidStateGridOutputPtr, *m_fluidStateGridInputPtr, *m_velocityGridInputPtr, dt);

		// swap buffers
		std::swap(m_fluidStateGridInputPtr, m_fluidStateGridOutputPtr);
	}

	void visFluid()
	{
		cl::Event evt;

		std::vector<cl::Memory> memObjs;
		memObjs.push_back(m_fluidStateImageBuffer);

		glFinish();
		m_queue.enqueueAcquireGLObjects(&memObjs, 0, &evt);
		checkError(m_queue.flush());
		waitForComplete(evt);

		if (true)
		{
			checkError(m_kernel_visFluid.setArg(0, m_fluidStateImageBuffer));
			checkError(m_kernel_visFluid.setArg(1, *m_fluidStateGridInputPtr));
			checkError(m_kernel_visFluid.setArg(2, m_outputWriteGammaPower));
			m_kernelRunner->run(m_kernel_visFluid);
		}
		else
		{
			checkError(m_kernel_visVelocity.setArg(0, m_fluidStateImageBuffer));
			checkError(m_kernel_visVelocity.setArg(1, *m_velocityGridInputPtr));
			m_kernelRunner->run(m_kernel_visVelocity);
		}

		m_queue.enqueueReleaseGLObjects(&memObjs, 0, &evt);
		checkError(m_queue.flush());
		waitForComplete(evt);

		m_queue.finish();
	}

	void uploadParams()
	{
		Params params;
		params.densityWeight = m_params->densityWeight;
		params.temperatureBuoyancy = m_params->temperatureBuoyancy;
		params.coolingRate = m_params->coolingRate;
		params.drag = m_params->drag;

		cl::Event evt;
		checkError(m_queue.enqueueWriteBuffer(m_paramsBuffer, CL_TRUE, 0, sizeof(Params), &params, NULL, &evt));
		checkError(m_queue.flush());
		waitForComplete(evt);
	}

	void setOutputWriteGammaPower(float power)
	{
		m_outputWriteGammaPower = power;
	}

	float getOutputWriteGammaPower() const
	{
		return m_outputWriteGammaPower;
	}

private:
	int m_width;
	int m_height;
	int m_depth;

	cl::Program m_program;
	cl::Kernel m_kernel_addFluid;
	cl::Kernel m_kernel_setFluid;
	cl::Kernel m_kernel_applyImpulse;
	cl::Kernel m_kernel_applyForces;

	cl::Kernel m_kernel_coolFluid;
	cl::Kernel m_kernel_visFluid;
	cl::Kernel m_kernel_visVelocity;

	static const int fluidStateGridCount = 2;
	cl::Buffer m_fluidStateGrids[fluidStateGridCount];

	static const int velocityGridCount = 2;
	cl::Buffer m_velocityGrids[velocityGridCount];

	cl::Buffer m_paramsBuffer;
	cl::ImageGL m_fluidStateImageBuffer;

	cl::Buffer* m_velocityGridInputPtr;
	cl::Buffer* m_velocityGridOutputPtr;
	cl::Buffer* m_fluidStateGridInputPtr;
	cl::Buffer* m_fluidStateGridOutputPtr;

	cl::CommandQueue m_queue;

	AdvecterPtr m_float3Advecter;
	AdvecterPtr m_fluidStateAdvecter;
	DivergenceFreeProjectorPtr m_divergenceFreeProjector;
	KernelRunnerPtr m_kernelRunner;

	cl::Program m_advectFloat3Pogram;
	cl::Program m_advectFluidStatePogram;

	TempBufferPoolPtr m_tempBufferPool;

	float m_outputWriteGammaPower;
};


// --------------------------------------------------------------------------

FluidSolver::FluidSolver() :
	m_params(new FluidSolverParams(FluidSolverParams::createDefault()))
{
}

FluidSolverPtr createFluidSolver(ClSystem& system, const GlTexture& fluidStateTexture, const TempBufferPoolPtr& tempBufferPool, const std::string& fluidKernalsDir)
{
	return FluidSolverPtr(new FluidSolverI(system, fluidStateTexture, tempBufferPool, fluidKernalsDir));
}

} // namespace GCompute
