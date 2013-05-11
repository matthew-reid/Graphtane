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

#include "TempBufferPool.h"
#include <GCompute/ClIncludes.h>
#include <GCompute/ClSystem.h>

using namespace GCompute;

namespace GFluid {

TempBufferPool::TempBufferPool(const ClSystem& system, int bufferElementCount) :
	m_bufferElementCount(bufferElementCount)
{
	int bufferSize = bufferElementCount * sizeof(cl_float3);
	
	for (int i = 0; i < m_floatBufferCount; ++i)
	{
		int err;
		m_floatBuffers[i].reset(new cl::Buffer(system._getContext(), CL_MEM_READ_WRITE, bufferSize, 0, &err));
		checkError(err);
	}
}

cl::Buffer& TempBufferPool::getFloatBuffer(int index)
{
	assert(index < getFloatBufferCount());
	return *m_floatBuffers[index];
}

} // namespace GFluid
