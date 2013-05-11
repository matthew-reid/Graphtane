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

#include <vector>

namespace GVis {

class SceneNode
{
public:
	SceneNode(const glm::vec3& position = vec3Zero(), const glm::quat& orientation = quatIdentity());

	//! @param position in world space
	void setPosition(const glm::vec3& position);

	//! @param position in parent space
	void setPosition_parentSpace(const glm::vec3& position);

	//! @param orientation in world space
	void setOrientation(const glm::quat& orientation);

	//! @param orientation in parent space
	void setOrientation_parentSpace(const glm::quat& orientation);

	//! @param direction in world space
	//! @param upVec in world space
	//! direction must be normalized
	void setDirection(const glm::vec3& direction, const glm::vec3& upVec = vec3Up());

	//! @return position in world space
	glm::vec3 getPosition() const;

	//! @return orientation in world space
	glm::quat getOrientation() const;
	
	//! @return direction in world space
	glm::vec3 getDirection() const;

	//! @return transform in world space
	const glm::mat4& getTransform() const {return m_cachedTransform;}

	//! @param translation in world space
	void translate(const glm::vec3& translation);

	//! @return translation in local space
	void translateLocal(const glm::vec3& translation);

	void addChild(const SceneNodePtr& child);
	void removeChild(const SceneNodePtr& child);

private:
	void updateTransform();
	void setParent(const SceneNode* parent);

private:
	glm::vec3 m_position; //!< Position in parent space
	glm::quat m_orientation; //!< Orientation in parent space
	glm::mat4 m_cachedTransform;
	std::vector<SceneNodePtr> m_children;
	const SceneNode* m_parent;
};

} // namespace GVis
