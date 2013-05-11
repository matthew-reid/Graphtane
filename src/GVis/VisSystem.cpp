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

#include "VisSystem.h"
#include "Light.h"
#include "RenderContext.h"
#include "RenderTarget.h"
#include "RenderableNode.h"
#include "Viewport.h"
#include "Geo.h"
#include "Camera.h"

#include <GCommon/VectorHelper.h>

#include <boost/foreach.hpp>
#include <stdexcept>

using namespace GCommon;

namespace GVis {

float VisSystem::ms_maxLightSearchRange = 100000000;

VisSystem::VisSystem() :
	m_wireframeModeEnabled(false)
{
	if(!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}
}

VisSystem::~VisSystem()
{
	glfwTerminate();
}

void VisSystem::render() const
{
	if (m_rootRenderTarget)
	{
		m_rootRenderTarget->_render(*this);
	}
}

static void applyWireframePolygonMode(bool wireframeModeEnabled)
{
	if (wireframeModeEnabled)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void VisSystem::renderNode(const RenderableNodePtr& renderableNode, const Viewport& viewport, const CameraPtr& camera, TechniqueCategory category) const
{
	applyWireframePolygonMode(m_wireframeModeEnabled || renderableNode->isWireframeModeEnabled());

	RenderContext context;
	context.sceneNode = renderableNode;
	context.camera = camera;
	context.viewportResolution = glm::vec2(viewport.getWidth(), viewport.getHeight());
	context.light = findNearestLight(renderableNode->getPosition());

	renderableNode->render(context, category);
}

struct SortableRenderable
{
	float distanceSquared;
	RenderableNodePtr renderableNode; 
};

static bool backToFrontPredicate(const SortableRenderable& a, const SortableRenderable& b)
{
	return a.distanceSquared > b.distanceSquared;
}

void VisSystem::_renderScene(const Viewport& viewport, const CameraPtr& camera, TechniqueCategory category) const
{
	BOOST_FOREACH(const RenderQueues::value_type& value, m_renderQueues)
	{
		if (value.first & viewport.getRenderQueueIdMask())
		{
			const RenderQueue& queue = *value.second;
			const RenderQueue::RenderableNodes& renderableNodes = queue.getRenderableNodes();

			switch(queue.getSortingMode())
			{
			case RenderSortingMode_BackToFront:
				{
					std::vector<SortableRenderable> sortables;
					BOOST_FOREACH(const RenderableNodePtr& renderableNode, renderableNodes)
					{
						SortableRenderable sortable;
						glm::vec3 diff = camera->getPosition() - renderableNode->getPosition();
						sortable.distanceSquared = glm::dot(diff, diff);
						sortable.renderableNode = renderableNode;
						sortables.push_back(sortable);
					}

					std::sort(sortables.begin(), sortables.end(), backToFrontPredicate);

					BOOST_FOREACH(const SortableRenderable& sortable, sortables)
					{
						renderNode(sortable.renderableNode, viewport, camera, category);
					}
				}
				break;
			case RenderSortingMode_None:
				{
					BOOST_FOREACH(const RenderableNodePtr& renderableNode, renderableNodes)
					{
						renderNode(renderableNode, viewport, camera, category);
					}
				}
				break;
			default:
				assert(!"Invalid sorting mode");
			}
		}
	}
}

void VisSystem::addRenderableNode(const RenderableNodePtr& renderableNode, RenderQueueId renderQueueId)
{
	assert(renderQueueId);
	assert(renderableNode);
	assert(!renderableNode->m_renderQueueId);

	RenderQueues::iterator i = m_renderQueues.find(renderQueueId);
	if (i == m_renderQueues.end())
	{
		std::pair<RenderQueues::iterator, bool> r = m_renderQueues.insert(RenderQueues::value_type(renderQueueId, RenderQueuePtr(new RenderQueue)));
		i = r.first;
	}

	renderableNode->m_renderQueueId = renderQueueId;
	i->second->addRenderableNode(renderableNode);
}

void VisSystem::removeRenderableNode(const RenderableNodePtr& renderableNode)
{
	assert(renderableNode);

	RenderQueues::iterator i = m_renderQueues.find(renderableNode->m_renderQueueId);
	assert(i != m_renderQueues.end());

	i->second->removeRenderableNode(renderableNode);
	renderableNode->m_renderQueueId = 0;
}

RenderQueuePtr VisSystem::getRenderQueue(RenderQueueId id) const
{
	RenderQueues::const_iterator i = m_renderQueues.find(id);
	if (i != m_renderQueues.end())
	{
		return i->second;
	}

	return RenderQueuePtr();
}

void VisSystem::addLight(const LightPtr& light)
{
	m_lights.push_back(light);
}

void VisSystem::removeLight(const LightPtr& light)
{
	vectorErase<LightPtr>(m_lights, light);
}

void VisSystem::setRootRenderTarget(const RenderTargetPtr& renderTarget)
{
	m_rootRenderTarget = renderTarget;
}

LightPtr VisSystem::findNearestLight(const glm::vec3& position) const
{
	LightPtr result;
	if (!m_lights.empty())
	{
		float nearestDistanceSq = ms_maxLightSearchRange;
		BOOST_FOREACH(const LightPtr& light, m_lights)
		{
			glm::vec3 diff = light->getPosition() -  position;
			float distanceSq = glm::dot(diff, diff);
			if (distanceSq < nearestDistanceSq)
			{
				nearestDistanceSq = distanceSq;
				result = light;
			}
		}
	}

	return result;
}

} // namespace GVis
