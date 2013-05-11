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

#include "Viewport.h"
#include "VisSystem.h"

namespace GVis {

Viewport::Viewport(const ViewportConfig& config) :
	m_config(config),
	m_renderQueueIdMask(~0),
	m_faceVisibility(FaceVisibility_Front),
	m_backgroundColor(vec4Zero())
{
}

void Viewport::setCamera(const CameraPtr& camera)
{
	m_camera = camera;
}

void Viewport::_render(const VisSystem& visSystem, TechniqueCategory category)
{
	glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);

	if (m_camera)
	{
		CALL_LISTENERS(viewport_preRender());

		switch(m_faceVisibility)
		{
		case FaceVisibility_Front:
			glFrontFace(GL_CCW);
			break;
		case FaceVisibility_Back:
			glFrontFace(GL_CW);
		}

		glViewport(m_config.left, m_config.top, m_config.width, m_config.height);
		glScissor(m_config.left, m_config.top, m_config.width, m_config.height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		visSystem._renderScene(*this, m_camera, category);

		CALL_LISTENERS(viewport_postRender());
	}
}

void Viewport::setRenderQueueIdMask(RenderQueueIdMask mask)
{
	m_renderQueueIdMask = mask;
}

void Viewport::setBackgroundColor(const glm::vec4& color)
{
	m_backgroundColor = color;
}

void Viewport::setFaceVisibility(FaceVisiblilty visibility)
{
	m_faceVisibility = visibility;
}

} // namespace GVis
