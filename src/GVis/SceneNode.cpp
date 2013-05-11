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

#include "SceneNode.h"

#include <GCommon/VectorHelper.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace GCommon;

namespace GVis {

SceneNode::SceneNode(const glm::vec3& position, const glm::quat& orientation) :
	m_position(position),
	m_orientation(orientation),
	m_parent(0)
{
	updateTransform();
}

void SceneNode::setPosition(const glm::vec3& position)
{
	if (m_parent)
	{
		m_position = glm::vec3(glm::inverse(m_parent->getTransform()) * glm::vec4(position, 1.0f));
	}
	else
	{
		m_position = position;
	}

	updateTransform();
}

void SceneNode::setPosition_parentSpace(const glm::vec3& position)
{
	m_position = position;
	updateTransform();
}

void SceneNode::setOrientation(const glm::quat& orientation)
{
	if (m_parent)
	{
		m_orientation = glm::inverse(m_parent->getOrientation()) * orientation;
	}
	else
	{
		m_orientation = orientation;
	}
	updateTransform();
}

void SceneNode::setOrientation_parentSpace(const glm::quat& orientation)
{
	m_orientation = orientation;
	updateTransform();
}

void SceneNode::setDirection(const glm::vec3& direction, const glm::vec3& upVec)
{
	glm::vec3 zVec = -direction;
	glm::vec3 xVec = glm::cross(upVec, zVec);
	if (xVec == glm::vec3(0,0,0))
	{
		xVec = glm::vec3(1,0,0);
	}
	else
	{
		xVec = glm::normalize(xVec);
	}

	glm::vec3 yVec = glm::cross(zVec, xVec);
	glm::quat ori(glm::mat3(xVec, yVec, zVec));
	setOrientation(ori);
}

//! @return position in world space
glm::vec3 SceneNode::getPosition() const
{
	return glm::vec3(m_cachedTransform[3][0], m_cachedTransform[3][1], m_cachedTransform[3][2]);
}

//! @return orientation in world space
glm::quat SceneNode::getOrientation() const
{
	return glm::quat_cast(m_cachedTransform);
}

glm::vec3 SceneNode::getDirection() const
{
	return getOrientation() * glm::vec3(0,0,-1);
}

void SceneNode::translate(const glm::vec3& translation)
{
	setPosition(getPosition() + translation);
}

void SceneNode::translateLocal(const glm::vec3& translation)
{
	m_position += m_orientation * translation;
	updateTransform();
}

void SceneNode::updateTransform()
{
	m_cachedTransform = glm::mat4(); // identity
	if (m_parent)
	{
		glm::vec3 position = m_parent->getPosition() + m_parent->getOrientation() * m_position;
		m_cachedTransform = glm::translate(m_cachedTransform, position);
		m_cachedTransform = m_cachedTransform * glm::mat4_cast(m_parent->getOrientation() * m_orientation);
	}
	else
	{
		m_cachedTransform = glm::translate(m_cachedTransform, m_position);
		m_cachedTransform = m_cachedTransform * glm::mat4_cast(m_orientation);
	}

	size_t count = m_children.size();
	for (size_t i = 0; i < count; ++i)
	{
		m_children[i]->updateTransform();
	}
}

void SceneNode::setParent(const SceneNode* parent)
{
	m_parent = parent;
	updateTransform();
}

void SceneNode::addChild(const SceneNodePtr& child)
{
	m_children.push_back(child);
	child->setParent(this);
}

void SceneNode::removeChild(const SceneNodePtr& child)
{
	child->setParent(0);
	vectorErase<SceneNodePtr>(m_children, child);
}

} // namespace GVis
