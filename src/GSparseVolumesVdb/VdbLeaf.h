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

#include "GSparseVolumes/Leaf.h"
#include "VdbTypes.h" // rename to VdbTypes or make consistant
#include <openvdb/openvdb.h>

namespace GSparseVolumes {

extern void leafToPixel(unsigned char* p, int channelCount, const openvdb::Coord& coord, const openvdb::FloatGrid::TreeType::LeafNodeType& leaf);
extern void leafToPixel(unsigned char* p, int channelCount, const openvdb::Coord& coord, const Vec3UByteGrid::TreeType::LeafNodeType& leaf);

template <typename T>
class VdbLeaf : public Leaf
{
public:
	VdbLeaf(const T& leaf, size_t offsetInInternalNode) :
		m_leaf(leaf),
		m_offsetInInternalNode(offsetInInternalNode)
	{

	}

	virtual size_t getIndexInInternalNode() const
	{
		return m_offsetInInternalNode;
	}

	virtual int getVoxelCountPerDimension() const
	{
		return m_leaf.dim();
	}

	virtual void toImageBuffer(ImageBufferUChar& buffer, const glm::ivec3& fillOffset) const
	{
		int leafWidth = m_leaf.dim();

		assert(fillOffset.x + leafWidth <= buffer.width);
		assert(fillOffset.y + leafWidth <= buffer.height);
		assert(fillOffset.z + leafWidth <= buffer.depth);

		// We access openvdb data by Coord instead of linearly walking through buffer by index
		// because openvdb leaf buffer is packed in zyx order and we store our texture in xyz order
		for (int z = 0; z < leafWidth; ++z)
		{
			for (int y = 0; y < leafWidth; ++y)
			{
				unsigned char* p = buffer.getElement(fillOffset.x, fillOffset.y + y, fillOffset.z + z);
				for (int x = 0; x < leafWidth; ++x)
				{
					leafToPixel(p, buffer.channelCount, openvdb::Coord(x,y,z), m_leaf);
					p += buffer.channelCount;
				}
			}
		}
	}

private:
	T m_leaf;
	size_t m_offsetInInternalNode;
};

} // namespace GSparseVolumes