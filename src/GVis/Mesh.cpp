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

#include "Mesh.h"
#include "Convert.h"

#include <assert.h>

namespace GVis {

struct VertexAttribute
{
	VertexAttribute(int index, int size) :
		m_index(index),
		m_attributeSize(size),
		m_bufferId(0)
	{
	}

	GLuint createBuffer(const GLfloat* data, int size, BufferUsage usage)
	{
		assert(data);
		assert(m_bufferId == 0);

		glGenBuffers(1, &m_bufferId);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
		glBufferData(GL_ARRAY_BUFFER, size, data, toGlBufferUsage(usage));
		return m_bufferId;
	}

	void destroyBuffer()
	{
		glDeleteBuffers(1, &m_bufferId);
	}

	void applyState()
	{
		// 1st attribute buffer : positions
		glEnableVertexAttribArray(m_index);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferId);
		glVertexAttribPointer(
		   m_index,             // attribute
		   m_attributeSize,     // size
		   GL_FLOAT,           // type
		   GL_FALSE,           // normalized?
		   0,                  // stride
		   (void*)0            // array buffer offset
		);
	}

	void unapplyState()
	{
		glDisableVertexAttribArray(m_index);
	}

private:
	int m_index;
	int m_attributeSize;
	GLuint m_bufferId;
};


static GLuint createElementBuffer(const GLint* data, int size, BufferUsage usage)
{
	assert(data);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, toGlBufferUsage(usage));
	return buffer;
}

// --------------------------------------------------------------------

GLenum toGlPrimitiveType(PrimitiveType type)
{
	switch(type)
	{
	case PrimitiveType_Points:
		return GL_POINTS;
	case PrimitiveType_Lines:
		return GL_LINES;
	case PrimitiveType_Triangles:
		return GL_TRIANGLES;
	case PrimitiveType_Patches:
		return GL_PATCHES;
	default:
		assert(!"Invalid PrimitiveType");
	}
	return 0;
}

// --------------------------------------------------------------------

Mesh::Mesh(PrimitiveType type, int patchVertCount) :
	m_vertexArray(0),
	m_indexBuffer(0),
	m_indexCount(0),
	m_glPrimitiveType(toGlPrimitiveType(type)),
	m_patchVertCount(patchVertCount),
	m_instanceCount(1)
{
	glGenVertexArrays(1, &m_vertexArray);
	glBindVertexArray(m_vertexArray);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_vertexArray);

	if (m_indexBuffer)
	{
		glDeleteBuffers(1, &m_indexBuffer);
	}
}

void Mesh::render()
{
	int vertexAttributeCount = m_vertexAttributes.size();
	for (int i = 0; i < vertexAttributeCount; i++)
	{
		m_vertexAttributes[i].applyState();
	}

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

	if (m_glPrimitiveType == GL_PATCHES)
	{
		glPatchParameteri(GL_PATCH_VERTICES, m_patchVertCount);
	}

	if (m_instanceCount > 1)
	{
		glDrawElementsInstanced(m_glPrimitiveType, m_indexCount, GL_UNSIGNED_INT,
			(void*)0, 			// element array buffer offset
			m_instanceCount
		);
	}
	else
	{
		glDrawElements(m_glPrimitiveType, m_indexCount, GL_UNSIGNED_INT,
			(void*)0			// element array buffer offset
		);
	}

	for (int i = 0; i < vertexAttributeCount; i++)
	{
		m_vertexAttributes[i].unapplyState();
	}
}

GLuint Mesh::setIndexBuffer(const GLint& data, int sizeBytes, BufferUsage usage)
{
	if (m_indexBuffer)
	{
		glDeleteBuffers(1, &m_indexBuffer);
	}

	m_indexCount = sizeBytes / sizeof(GLint);
	m_indexBuffer = createElementBuffer(&data, sizeBytes, usage);
	return m_indexBuffer;
}

void Mesh::updateIndexBuffer(const GLint& data, int sizeBytes, int offset)
{
	assert(m_indexBuffer != 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, sizeBytes, &data);
}

GLuint Mesh::addVertexAttribute(const GLfloat& data, int sizeBytes, int elementsPerVertex, BufferUsage usage)
{
	int attributeIndex = m_vertexAttributes.size();
	m_vertexAttributes.push_back(VertexAttribute(attributeIndex, elementsPerVertex));
	GLuint buffer = m_vertexAttributes.back().createBuffer(&data, sizeBytes, usage);
	return buffer;
}

void Mesh::setInstanceCount(int count)
{
	assert(count > 0);
	m_instanceCount = count;
}

} // namespace GVis
