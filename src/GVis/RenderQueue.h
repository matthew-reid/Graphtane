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

#include "GVisFwd.h"

#include <vector>

namespace GVis {

enum RenderSortingMode
{
	RenderSortingMode_BackToFront,
	RenderSortingMode_None
};

class RenderQueue
{
public:
	RenderQueue(RenderSortingMode sortingMode = RenderSortingMode_None);

	typedef std::vector<RenderableNodePtr> RenderableNodes;

	void addRenderableNode(const RenderableNodePtr& renderable);
	void removeRenderableNode(const RenderableNodePtr& renderable);

	const RenderableNodes& getRenderableNodes() const {return m_renderableNodes;}

	void setSortingMode(RenderSortingMode mode);
	RenderSortingMode getSortingMode() const {return m_sortingMode;}

private:
	RenderableNodes m_renderableNodes;
	RenderSortingMode m_sortingMode;
};

extern int getDefaultRenderQueueId();

} // namespace GVis
