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

#include "RenderableNode.h"
#include "Renderable.h"
#include "Geo.h"

#include <GCommon/VectorHelper.h>

#include <boost/foreach.hpp>

using namespace GCommon;

namespace GVis {

RenderableNode::RenderableNode() :
	m_renderQueueId(0),
	m_visible(true),
	m_wireframeModeEnabled(false)
{
}

void RenderableNode::addRenderable(const RenderablePtr& renderable)
{
	m_renderables.push_back(renderable);
}

void RenderableNode::removeRenderable(const RenderablePtr& renderable)
{
	vectorErase(m_renderables, renderable);
}

void RenderableNode::render(const RenderContext& context, TechniqueCategory techniqueCategory)
{
	if (m_visible)
	{
		BOOST_FOREACH(const RenderablePtr& renderable, m_renderables)
		{
			renderable->render(context, techniqueCategory);
		}
	}
}

void RenderableNode::setVisible(bool visible)
{
	m_visible = visible;
}

RenderableNodePtr RenderableNode::createWithSingleGeo(const GeoPtr& geo, const glm::vec3& position)
{
	RenderableNodePtr node(new RenderableNode);
	node->addRenderable(geo);
	node->setPosition(position);
	return node;
}

} // namespace GVis
