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

#include "IsosurfaceNormalCalculator.h"
#include "BufferProvider.h"
#include "KernelRunner.h"
#include "TempBufferPool.h"
#include <GCompute/ClSystem.h>
#include <GCompute/GlTexture.h>

#include <string>

using namespace GCompute;

namespace GFluid {

class IsosurfaceNormalCalculatorI : public IsosurfaceNormalCalculator
{
public:
	IsosurfaceNormalCalculatorI(const ClSystem& system, const cl::Program& program, const GlTexture& normalTexture,
								const BufferProviderPtr& densityBufferProvider, const TempBufferPoolPtr& tempBufferPool) :
		m_tempBufferPool(tempBufferPool),
		m_densityBufferProvider(densityBufferProvider)
	{
		int width = normalTexture.width;
		int height = normalTexture.height;
		int depth = normalTexture.depth;

		assert(m_tempBufferPool);
		assert(m_tempBufferPool->getBufferElementCount() >= width * height * depth * 8); // input must be 2x dimensions of normalTexture

		ClSystem::createKernel(m_kernel_visNormal, program, "visNormal");
		ClSystem::createKernel(m_kernel_calcDensityGradient, program, "calcDensityGradient");
		ClSystem::createKernel(m_kernel_downScale2x, program, "downScale2x");

		cl_int err;
		m_normalImageBuffer = cl::ImageGL(system._getContext(), CL_MEM_WRITE_ONLY, normalTexture.target, 0, normalTexture.textureId, &err);
		checkError(err);


		// Create kernel runners
		{
			cl::CommandQueue queue = cl::CommandQueue(system._getContext(), system._getDevice(), 0, &err);

			cl::NDRange globalThreads = cl::NDRange(width * 2, height * 2, depth * 2);
			cl::NDRange localThreads = cl::NullRange; // automatically determined

			m_fullSizeKernelRunner.reset(new KernelRunner(queue, globalThreads, localThreads));

			globalThreads = cl::NDRange(width, height, depth);
			m_halfSizeKernelRunner.reset(new KernelRunner(queue, globalThreads, localThreads));
		}
	}

	void updateTexture()
	{
		checkError(m_kernel_calcDensityGradient.setArg(0, m_tempBufferPool->getFloatBuffer(0)));
		checkError(m_kernel_calcDensityGradient.setArg(1, m_densityBufferProvider->getOutputBuffer()));
		m_fullSizeKernelRunner->run(m_kernel_calcDensityGradient);

		checkError(m_kernel_downScale2x.setArg(0, m_tempBufferPool->getFloatBuffer(1)));
		checkError(m_kernel_downScale2x.setArg(1, m_tempBufferPool->getFloatBuffer(0)));
		m_halfSizeKernelRunner->run(m_kernel_downScale2x);

		checkError(m_kernel_visNormal.setArg(0, m_normalImageBuffer));
		checkError(m_kernel_visNormal.setArg(1, m_tempBufferPool->getFloatBuffer(1)));
		m_halfSizeKernelRunner->run(m_kernel_visNormal);
	}

private:
	cl::Kernel m_kernel_visNormal;
	cl::Kernel m_kernel_calcDensityGradient;
	cl::Kernel m_kernel_downScale2x;

	boost::scoped_ptr<KernelRunner> m_fullSizeKernelRunner;
	boost::scoped_ptr<KernelRunner> m_halfSizeKernelRunner;

	cl::ImageGL m_normalImageBuffer;
	BufferProviderPtr m_densityBufferProvider;
	TempBufferPoolPtr m_tempBufferPool;
};

IsosurfaceNormalCalculatorPtr createIsosurfaceNormalCalculator(const GCompute::ClSystem& system, const GCompute::GlTexture& normalTexture,
															   const BufferProviderPtr& densityBufferProvider, const TempBufferPoolPtr& tempBufferPool,
															   const std::string& fluidKernelsDir)
{
	cl::Program program;
	system.loadProgram(program, fluidKernelsDir + "/IsosurfaceNormals.cl");

	return IsosurfaceNormalCalculatorPtr(new IsosurfaceNormalCalculatorI(system, program, normalTexture, densityBufferProvider, tempBufferPool));
}

} // namespace GFluid
