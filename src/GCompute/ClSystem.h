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

#include "GComputeFwd.h"
#include <boost/scoped_ptr.hpp>
#include <vector>

namespace GCompute {


class ClSystem
{
public:
	ClSystem();
	~ClSystem();

	void loadProgram(cl::Program& program, const std::string &filename) const;

	void writeToDevice(const cl::Buffer& buffer, const void* data, int sizeBytes);
	void readFromDevice(void* data, const cl::Buffer& buffer, int sizeBytes);

	static void createKernel(cl::Kernel& kernel, const cl::Program& program, const std::string& name);
	void runKernel(cl::Kernel& kernel, const cl::NDRange& globalThreads, const cl::NDRange& localThreads) const;

	cl::Context& _getContext() const;
	const cl::Device& _getDevice() const;
	cl::Device& _getDevice();

	int getMaxWorkGroupSize() const;

private:
	ContextPtr m_context;
	std::vector<cl::Device> m_devices;
	boost::scoped_ptr<cl::CommandQueue> m_queue;
};

extern void checkError(int status, const std::string& contextMessage="");
extern void waitForComplete(const cl::Event& evt);

} // namespace GCompute
