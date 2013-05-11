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
#include "SceneNode.h"
#include "TechniqueCategory.h"

#include <vector>

namespace GVis {

class RenderableNode : public SceneNode
{
	friend class VisSystem;

public:
	RenderableNode();
	void addRenderable(const RenderablePtr& renderable);
	void removeRenderable(const RenderablePtr& renderable);

	void render(const RenderContext& context, TechniqueCategory techniqueCategory);

	void setVisible(bool visible);
	bool isVisible() const {return m_visible;}

	void setWireframeModeEnabled(bool enabled) {m_wireframeModeEnabled = enabled;}
	bool isWireframeModeEnabled() const {return m_wireframeModeEnabled;}

	static RenderableNodePtr createWithSingleGeo(const GeoPtr& geo, const glm::vec3& position = vec3Zero());

private:
	std::vector<RenderablePtr> m_renderables;
	RenderQueueId m_renderQueueId;

	bool m_visible;
	bool m_wireframeModeEnabled;
};


} // namespace GVis
