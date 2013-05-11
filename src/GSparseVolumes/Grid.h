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

#include "GSparseVolumesFwd.h"

namespace GSparseVolumes {

class InternalNodeIterator
{
public:
	~InternalNodeIterator() {}

	virtual InternalNodePtr next() = 0;
	virtual bool hasNext() const = 0;
	virtual InternalNodeIteratorPtr clone() const = 0;
};

/*!
Grid structure consists of one level of sparsely distributed InternalNodes.
InternalNodes must all have the same dimensions and may not overlap.

Each InternalNode contains sparsely distributed Leaf nodes.
Leaves must all have the same dimensions and may not overlap.

Each Leaf contains uniformly (not sparse) distributed values.

*/
class Grid
{
public:
	virtual ~Grid() {}
	virtual size_t getMaxLeafCountPerInternalNode() const = 0;
	virtual size_t getMaxLeafCountPerInternalNodeDimension() const = 0;
	virtual size_t getVoxelCountPerLeafDimension() const = 0;
	virtual size_t getInternalNodeCount() const = 0;
	virtual InternalNodeIteratorPtr createInternalNodeIterator() const = 0;
	virtual int getChannelCount() const = 0;
};

} // namespace GSparseVolumes