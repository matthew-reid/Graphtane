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
#include "Math.h"

#include <GCommon/Listenable.h>
#include "ViewportListener.h"
#include "TechniqueCategory.h"

namespace GVis {

enum FaceVisiblilty
{
	FaceVisibility_Front,
	FaceVisibility_Back
};

struct ViewportConfig
{
	static ViewportConfig createDefault()
	{
		ViewportConfig c;
		c.top = 0;
		c.left = 0;
		c.width = 1024;
		c.height = 768;
		return c;
	}

	int top;
	int left;
	int width;
	int height;
};

class Viewport : public GCommon::Listenable<ViewportListener>
{
public:
	Viewport(const ViewportConfig& config);

	void setCamera(const CameraPtr& camera);
	CameraPtr getCamera() const {return m_camera;}

	void _render(const VisSystem& visSystem, TechniqueCategory category);

	RenderQueueIdMask getRenderQueueIdMask() const {return m_renderQueueIdMask;}
	void setRenderQueueIdMask(RenderQueueIdMask mask);

	void setBackgroundColor(const glm::vec4& color);

	//! default is FaceVisibility_Front
	void setFaceVisibility(FaceVisiblilty visibility);

	int getWidth() const {return m_config.width;}
	int getHeight() const {return m_config.height;}

private:
	CameraPtr m_camera;
	ViewportConfig m_config;
	RenderQueueIdMask m_renderQueueIdMask;
	FaceVisiblilty m_faceVisibility;
	glm::vec4 m_backgroundColor;
};

} // namespace GVis
