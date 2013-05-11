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

namespace GVis {

class Projection
{
public:
	Projection();
	virtual ~Projection() {}

	const glm::mat4& getMatrix() const {return m_matrix;}

	float getNearClipDistance() const {return m_nearClip;}
	float getFarClipDistance() const {return m_farClip;}

	void setNearClipDistance(float nearClip);
	void setFarClipDistance(float farClip);

	//! Z value resultant from multiplication with Projection matrix at far clip distance
	virtual float getMaxClipSpaceDepth() const = 0;
	virtual void updateMatrix() = 0;

protected:
	glm::mat4 m_matrix;
	float m_nearClip;
	float m_farClip;
};

// ----------------------------------------------------------------------------

class PerspectiveProjection : public Projection
{
public:
	PerspectiveProjection();

	//! aspectRatio is width / height
	void setAspectRatio(float aspectRatio);

	float getMaxClipSpaceDepth() const {return m_farClip;}

	void setFovY_degrees(float value);

private:
	virtual void updateMatrix();

	float m_fovY_degrees; //!< in degrees
	float m_aspectRatio; //!< width / height
};

// ----------------------------------------------------------------------------

class OrthographicProjection : public Projection
{
public:
	OrthographicProjection();

	void setDimensions(float width, float height);

	float getMaxClipSpaceDepth() const {return 1;}

private:
	virtual void updateMatrix();

	float m_halfWidth;
	float m_halfHeight;
};

} // namespace GVis
