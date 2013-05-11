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

#include "ImageBuffer.h"

#include <GVis/Math.h>

namespace GSparseVolumes {

class Leaf
{
public:
	virtual ~Leaf() {}

	//! Returns index of this leaf's position within parent InternalNode.
	//! Indicies do not need to be contiguious, i.e Leaves can be sparsely distributed within the InternalNode.
	virtual size_t getIndexInInternalNode() const = 0;

	virtual int getVoxelCountPerDimension() const = 0;

	virtual void toImageBuffer(ImageBufferUChar& buffer, const glm::ivec3& fillOffset) const = 0;
};

} // namespace GSparseVolumes