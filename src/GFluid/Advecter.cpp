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

#include "Advecter.h"
#include "KernelRunner.h"
#include <GCompute/ClSystem.h>

using namespace GCompute;

static bool useMacCormackAdvection = true;

namespace GFluid {

Advecter::Advecter(const KernelRunnerPtr& runner, cl::Kernel kernel_advect, cl::Kernel kernel_advect_macCormack, cl::Buffer* tempStateGrid) :
	m_runner(runner),
	m_kernel_advect(kernel_advect),
	m_kernel_advect_macCormack(kernel_advect_macCormack),
	m_tempStateGrid(tempStateGrid)
{
	assert(m_runner);
	assert(m_tempStateGrid);
}

void Advecter::advect(cl::Buffer& output, const cl::Buffer& input, const cl::Buffer& velocity, float dt)
{
	// GFluid State Advection
	{
		// Forward
		checkError(m_kernel_advect.setArg(0, velocity));
		checkError(m_kernel_advect.setArg(1, input));
		checkError(m_kernel_advect.setArg(2, output));
		checkError(m_kernel_advect.setArg(3, dt));

		m_runner->run(m_kernel_advect);

		if (useMacCormackAdvection)
		{
			// Backward from the forward result
			checkError(m_kernel_advect.setArg(0, velocity));
			checkError(m_kernel_advect.setArg(1, output));
			checkError(m_kernel_advect.setArg(2, *m_tempStateGrid)); // result
			checkError(m_kernel_advect.setArg(3, -dt)); 

			m_runner->run(m_kernel_advect);

			// Correct error in forward
			checkError(m_kernel_advect_macCormack.setArg(0, velocity));
			checkError(m_kernel_advect_macCormack.setArg(1, output)); // forward
			checkError(m_kernel_advect_macCormack.setArg(2, *m_tempStateGrid)); // backward from forward
			checkError(m_kernel_advect_macCormack.setArg(3, input)); 
			checkError(m_kernel_advect_macCormack.setArg(4, output));
			checkError(m_kernel_advect_macCormack.setArg(5, dt)); 

			m_runner->run(m_kernel_advect_macCormack);
		}
	}
}

AdvecterPtr createAdvecter(const KernelRunnerPtr& kernelRunner, const cl::Program& program, cl::Buffer* tempStateGrid)
{
	cl::Kernel advectKernel;
	cl::Kernel macCormackCorrectionKernel;
	ClSystem::createKernel(advectKernel, program, "advectBacktrace");
	ClSystem::createKernel(macCormackCorrectionKernel, program, "applyMacCormackCorrection");

	return AdvecterPtr(new Advecter(kernelRunner, advectKernel, macCormackCorrectionKernel, tempStateGrid));
}

} // namespace GCompute
