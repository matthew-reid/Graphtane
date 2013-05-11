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

#include "GridVectorIterators.h"
#include "InternalNode.h"
#include "Grid.h"

using namespace GSparseVolumes;

InternalNodesIterator::InternalNodesIterator(const std::vector<GridPtr>& grids)
{
	m_internalNodes.resize(grids.size());
	m_internalNodeIterators.resize(grids.size());
	for (size_t i = 0; i < grids.size(); ++i)
	{
		m_internalNodeIterators[i] = grids[i]->createInternalNodeIterator();
	}
}

const InternalNodes* InternalNodesIterator::next()
{
	for (size_t i = 0; i < m_internalNodeIterators.size(); ++i)
	{
		m_internalNodes[i] = m_internalNodeIterators[i]->next();
	}

	if (m_internalNodes[0] != 0)
	{
		return &m_internalNodes;
	}
	return 0;
}

bool InternalNodesIterator::hasNext() const
{
	if (!m_internalNodeIterators.empty())
	{
		return m_internalNodeIterators.back()->hasNext();
	}
	return false;
}


LeavesIterator::LeavesIterator(const InternalNodes& internalNodes)
{
	m_leaves.resize(internalNodes.size());
	for (size_t i = 0; i < internalNodes.size(); ++i)
	{
		m_leafIterators.push_back(internalNodes[i]->createLeafIterator());
	}
}

const Leaves* LeavesIterator::next()
{
	for (size_t i = 0; i < m_leafIterators.size(); ++i)
	{
		m_leaves[i] = m_leafIterators[i]->next();
	}

	if (m_leaves[0] != 0)
	{
		return &m_leaves;
	}
	return 0;
}

bool LeavesIterator::hasNext() const
{
	if (!m_leafIterators.empty())
	{
		return m_leafIterators.back()->hasNext();
	}
	return false;
}