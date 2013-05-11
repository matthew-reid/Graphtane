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

#include "GSparseVolumes/InternalNode.h"
#include "VdbLeaf.h"
#include "VdbUtil.h"

namespace GSparseVolumes {

template <typename ChildIter>
class VdbLeafIterator : public LeafIterator
{
	typedef typename ChildIter::ValueType LeafType;
public:
	VdbLeafIterator(const ChildIter& it) :
		m_it(it)
	{
	}

	virtual LeafPtr next()
	{
		LeafPtr leaf;
		if (m_it)
		{
			leaf.reset(new VdbLeaf<LeafType>(*m_it, m_it.offset()));
			++m_it;
		}
		return leaf;
	}

	virtual bool hasNext() const
	{
		return m_it;
	}

private:
	ChildIter m_it;
};

template <typename NodeType>
class VdbInternalNode : public InternalNode
{
public:
	VdbInternalNode(const NodeType* node) :
		m_node(node)
	{
	}

	LeafIteratorPtr createLeafIterator() const
	{
		return LeafIteratorPtr(new VdbLeafIterator<typename NodeType::ChildOnCIter>(m_node->cbeginChildOn()));
	}

	glm::vec3 getBoundingBoxSize() const
	{
		return toVec3(m_node->getNodeBoundingBox().dim().asVec3d());
	}

	glm::vec3 getBoundingBoxCenter() const
	{
		return toVec3(m_node->getNodeBoundingBox().getCenter());
	}

	NodeType* getWrappedNode() const
	{
		return m_node;
	}

private:
	const NodeType* m_node;
};

} // namespace GSparseVolumes