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

#include "GSparseVolumes/Grid.h"
#include "VdbInternalNode.h"

namespace GSparseVolumes {

template <typename GridType>
class VdbInternalNodeIterator : public InternalNodeIterator
{
public:
	typedef typename GridType::TreeType TreeType;
	typedef typename TreeType::NodeCIter NodeCIter;
	typedef typename TreeType::RootNodeType  RootType;  // level 3 RootNode
	typedef typename RootType::ChildNodeType Int1Type;  // level 2 InternalNode
	typedef typename Int1Type::ChildNodeType Int2Type;  // level 1 InternalNode
	static const int leafParentNodeDepth = 2;

public:
	VdbInternalNodeIterator(const NodeCIter& it) :
		m_it(it)
	{
	}

	InternalNodePtr next()
	{
		for (; m_it; ++m_it)
		{
			switch (m_it.getDepth())
			{
				case leafParentNodeDepth:
				{
					const Int2Type* node = 0;
					m_it.getNode(node);
					assert(node);

					++m_it;
					return InternalNodePtr(new VdbInternalNode<Int2Type>(node));
				}
			}
		}
		return InternalNodePtr();
	}

	bool hasNext() const
	{
		VdbInternalNodeIterator i = *this;
		return i.next();
	}

	InternalNodeIteratorPtr clone() const
	{
		return InternalNodeIteratorPtr(new VdbInternalNodeIterator<GridType>(m_it));
	}

private:
	NodeCIter m_it;
};

/*
VdbGrid provides a view of a source vdb tree, flattened to the last two levels (last internal node level and leaf level).
*/
template <typename GridType>
class VdbGrid : public Grid
{
	typedef typename GridType::Ptr GridTypePtr;
	typedef typename GridType::TreeType::RootNodeType::ChildNodeType::ChildNodeType Int2Type;
public:
	VdbGrid(const GridTypePtr& grid, int channelCount) :
		m_grid(grid),
		m_channelCount(channelCount)
	{
		// count number of internal nodes
		m_internalNodeCount = 0;
		InternalNodeIteratorPtr i = createInternalNodeIterator();
		while (i->next())
		{
			m_internalNodeCount++;
		}
	}

	size_t getMaxLeafCountPerInternalNode() const
	{
		return Int2Type::NUM_VALUES;
	}

	size_t getMaxLeafCountPerInternalNodeDimension() const
	{
		return 1 << Int2Type::LOG2DIM;
	}

	size_t getVoxelCountPerLeafDimension() const
	{
		return GridType::TreeType::LeafNodeType::DIM;
	}

	size_t getInternalNodeCount() const
	{
		return m_internalNodeCount;
	}

	InternalNodeIteratorPtr createInternalNodeIterator() const
	{
		return InternalNodeIteratorPtr(new VdbInternalNodeIterator<GridType>(m_grid->constTree().beginNode()));
	}

	int getChannelCount() const
	{
		return m_channelCount;
	}

private:
	GridTypePtr m_grid;
	int m_channelCount;
	size_t m_internalNodeCount;
};

} // namespace GSparseVolumes