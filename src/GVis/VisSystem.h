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
#include "TechniqueCategory.h"
#include "RenderQueue.h"

#include <vector>
#include <map>

namespace GVis {

class VisSystem
{
public:
	VisSystem();
	~VisSystem();

	void render() const;

	void addRenderableNode(const RenderableNodePtr& renderableNode, RenderQueueId renderQueueId = getDefaultRenderQueueId());
	void removeRenderableNode(const RenderableNodePtr& renderableNode);

	//! returns null if ID is invalid
	RenderQueuePtr getRenderQueue(RenderQueueId id) const;

	void addLight(const LightPtr& light);
	void removeLight(const LightPtr& light);

	void setRootRenderTarget(const RenderTargetPtr& renderTarget);

	void _renderScene(const Viewport& viewport, const CameraPtr& camera, TechniqueCategory category) const;

	void setWireframeModeEnabled(bool enabled) {m_wireframeModeEnabled = enabled;}
	bool isWireframeModeEnabled() const {return m_wireframeModeEnabled;}

private:
	void renderNode(const RenderableNodePtr& renderableNode, const Viewport& viewport, const CameraPtr& camera, TechniqueCategory category) const;

	//! Can return null
	LightPtr findNearestLight(const glm::vec3& position) const;

private:
	CameraPtr m_camera;

	typedef int RenderQueueId;
	typedef std::map<RenderQueueId, RenderQueuePtr> RenderQueues;
	RenderQueues m_renderQueues;

	typedef std::vector<LightPtr> Lights;
	Lights m_lights;

	RenderTargetPtr m_rootRenderTarget;

	static float ms_maxLightSearchRange;

	bool m_wireframeModeEnabled;
};

} // namespace GVis
