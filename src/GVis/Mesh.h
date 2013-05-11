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
#include "BufferUsage.h"

#include <vector>

namespace GVis {

enum PrimitiveType
{
	PrimitiveType_Points,
	PrimitiveType_Lines,
	PrimitiveType_Triangles,
	PrimitiveType_Patches,
};

class Mesh
{
public:
	//! @param quadVertCount is used when PrimitiveType_Patches is set
	Mesh(PrimitiveType type = PrimitiveType_Triangles, int patchVertCount = 4);
	virtual ~Mesh();

	void render();

	//! Creates index buffer and copies in data. Replaces any existing index buffer. Returns OpenGL buffer name.
	GLuint setIndexBuffer(const GLint& data, int sizeBytes, BufferUsage usage = BufferUsage_Static);

	//! updates existing index buffer with new data. Buffer must have previously been created with setIndexBuffer.
	//! @param sizeBytes specifies the size of the region to be replaced
	//! @param offset specifies the offset at which to begin data replacment
	void updateIndexBuffer(const GLint& data, int sizeBytes, int offset = 0);

	//! Adds by copy
	//! Returns OpenGL buffer name
	GLuint addVertexAttribute(const GLfloat& data, int sizeBytes, int elementsPerVertex, BufferUsage usage = BufferUsage_Static);

	void setInstanceCount(int count);

private:
	GLuint m_vertexArray;
	std::vector<struct VertexAttribute> m_vertexAttributes;
	GLuint m_indexBuffer;
	int m_indexCount;
	GLenum m_glPrimitiveType;
	int m_patchVertCount;
	int m_instanceCount;
};

} // namespace GVis
