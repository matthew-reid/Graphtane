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

#include "DivergenceFreeProjector.h"
#include "KernelRunner.h"
#include <GCompute/ClSystem.h>

#include <boost/lexical_cast.hpp>

using namespace GCompute;

namespace GFluid {

DivergenceFreeProjector::DivergenceFreeProjector(const KernelRunnerPtr& kernelRunner, const cl::Program& program, cl::Buffer* tempFloat2Grid) :
	m_kernelRunner(kernelRunner),
	m_divergenceAndPressureGrid(tempFloat2Grid)
{
	assert(m_kernelRunner);
	assert(m_divergenceAndPressureGrid);

	for (int i = 0; i < projectVelocity_stageCount; i++)
	{
		std::string name = "stepVelocityProject_stage" + boost::lexical_cast<std::string>(i+1);
		ClSystem::createKernel(m_kernel_projectVelocity_stages[i], program, name);
	}
}

void DivergenceFreeProjector::makeDivergenceFree(const cl::Buffer& buffer)
{
	checkError(m_kernel_projectVelocity_stages[0].setArg(0, buffer));
	checkError(m_kernel_projectVelocity_stages[0].setArg(1, *m_divergenceAndPressureGrid));
	m_kernelRunner->run(m_kernel_projectVelocity_stages[0]);

	checkError(m_kernel_projectVelocity_stages[1].setArg(0, *m_divergenceAndPressureGrid));
	const int pressureFromDivergenceCalculationIterationCount = 20;
	for (int i = 0; i < pressureFromDivergenceCalculationIterationCount; ++i)
	{
		m_kernelRunner->run(m_kernel_projectVelocity_stages[1]);
	}

	checkError(m_kernel_projectVelocity_stages[2].setArg(0, buffer));
	checkError(m_kernel_projectVelocity_stages[2].setArg(1, *m_divergenceAndPressureGrid));
	m_kernelRunner->run(m_kernel_projectVelocity_stages[2]);
}

} // namespace GCompute
