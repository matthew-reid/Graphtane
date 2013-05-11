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

#include "RenderTarget.h"
#include "Viewport.h"
#include "VisSystem.h"

#include "GCommon/VectorHelper.h"

#include <boost/foreach.hpp>

#include <algorithm>

using namespace GCommon;

namespace GVis {

RenderTarget::RenderTarget() :
	m_framebuffer(0)
{
}

ViewportPtr RenderTarget::addDefaultViewport(const CameraPtr& camera)
{
	ViewportConfig config = ViewportConfig::createDefault();
	config.width = getWidth();
	config.height = getHeight();

	ViewportPtr viewport(new Viewport(config));
	viewport->setCamera(camera);

	addViewport(viewport);
	return viewport;
}

void RenderTarget::addViewport(const ViewportPtr& viewport)
{
	m_viewports.push_back(viewport);
}

void RenderTarget::removeViewport(const ViewportPtr& viewport)
{
	Viewports::iterator i = std::find(m_viewports.begin(), m_viewports.end(), viewport);
	if (i != m_viewports.end())
	{
		m_viewports.erase(i);
	}
}

void RenderTarget::addRenderTarget(const RenderTargetPtr& target)
{
	m_renderTargets.push_back(target);
}

void RenderTarget::removeRenderTarget(const RenderTargetPtr& target)
{
	vectorErase<RenderTargetPtr>(m_renderTargets, target);
}

void RenderTarget::_render(const VisSystem& visSystem)
{
	// Render children
	BOOST_FOREACH(const RenderTargetPtr& target, m_renderTargets)
	{
		target->_render(visSystem);
	}

	// Render self
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

	BOOST_FOREACH(const ViewportPtr& viewport, m_viewports)
	{
		viewport->_render(visSystem, getTechniqueCategory());
	}
}

} // namespace GVis
