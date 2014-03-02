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

#include "Context.h"
#include "ClError.h"

#include <assert.h>
#include <stdexcept>

#ifdef WIN32
#include "Context_gpu_win32.h"
#else
// TODO: add support for other platforms.
	GCompute::ContextPtr createGpuContext(const cl::Platform& platform)
	{
		throw std::runtime_error("No createGpuContext() implementation for this platform");
	}
#endif
#include "Context_cpu.h"

namespace GCompute {


cl::Platform getPlatform(int clDeviceType)
{
	std::vector<cl::Platform> platforms;
	cl_int err = cl::Platform::get(&platforms);

	if (err != CL_SUCCESS)
		throw std::runtime_error("Platform::get() failed. Reason: " + getOpenClErrorString(err));

    if(platforms.size() > 0)
    {
		for(std::vector<cl::Platform>::iterator i = platforms.begin(); i != platforms.end(); ++i)
        {
			cl::Platform& platform = *i;

			std::vector<cl::Device> devices;
			err = platform.getDevices(clDeviceType, &devices);

			if (err != CL_SUCCESS)
				throw std::runtime_error("Platform::getDevices() failed. Reason: " + getOpenClErrorString(err));

            if (!devices.empty())
				return platform;
        }
    }

	if (err != CL_SUCCESS)
		throw std::runtime_error("Platform::getInfo() failed. Reason: " + getOpenClErrorString(err));
	else
		throw std::runtime_error("No suitible platform found");
}

ContextPtr createContext(int clDeviceType, cl::Platform platform)
{
	if (clDeviceType == CL_DEVICE_TYPE_GPU)
		return createGpuContext(platform);
	else if (clDeviceType == CL_DEVICE_TYPE_CPU)
		return createCpuContext(platform);

	assert(!"Unhandled CL_DEVICE_TYPE");
	return ContextPtr();
}

} // namespace GCompute
