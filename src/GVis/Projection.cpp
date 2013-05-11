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

#include "Projection.h"

#include <glm/gtc/matrix_transform.hpp>

namespace GVis {

Projection::Projection() :
	m_nearClip(0.1f),
	m_farClip(100.0f)
{
}

void Projection::setNearClipDistance(float nearClip)
{
	m_nearClip = nearClip;
	updateMatrix();
}

void Projection::setFarClipDistance(float farClip)
{
	m_farClip = farClip;
	updateMatrix();
}

// ----------------------------------------------------------------------------

PerspectiveProjection::PerspectiveProjection() :
	m_fovY_degrees(30),
	m_aspectRatio(16.0f / 9.0f)
{
	updateMatrix();
}

void PerspectiveProjection::setAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;
	updateMatrix();
}

void PerspectiveProjection::setFovY_degrees(float value)
{
	m_fovY_degrees = value;
}

void PerspectiveProjection::updateMatrix()
{
	m_matrix = glm::perspective(m_fovY_degrees, m_aspectRatio, m_nearClip, m_farClip);
}

// ----------------------------------------------------------------------------

OrthographicProjection::OrthographicProjection() :
	m_halfWidth(10),
	m_halfHeight(10)
{
	updateMatrix();
}


void OrthographicProjection::setDimensions(float width, float height)
{
	m_halfWidth = width * 0.5f;
	m_halfHeight = height * 0.5f;
	updateMatrix();
}

void OrthographicProjection::updateMatrix()
{
	m_matrix = glm::ortho(-m_halfWidth, m_halfWidth, -m_halfHeight, m_halfHeight, m_nearClip, m_farClip);
}

} // namespace GVis
