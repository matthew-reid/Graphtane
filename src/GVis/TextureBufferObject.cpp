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

#include "TextureBufferObject.h"
#include "Convert.h"

namespace GVis {

ScopedTextureBufferObject::ScopedTextureBufferObject(void* data, size_t elementCount, size_t elementSizeBytes, BufferUsage usage) :
	m_elementCount(elementCount)
{
	glGenBuffers(1, &m_tbo);
	glBindBuffer(GL_TEXTURE_BUFFER, m_tbo);
	glBufferData(GL_TEXTURE_BUFFER, elementCount * elementSizeBytes, data, toGlBufferUsage(usage));
}

ScopedTextureBufferObject::~ScopedTextureBufferObject()
{
	glBindBuffer(1, m_tbo);
	glDeleteBuffers(1, &m_tbo);
}

void ScopedTextureBufferObject::setData(void* data, int sizeBytes, int offset)
{
	assert(data);
	glBindBuffer(GL_TEXTURE_BUFFER, m_tbo);
	glBufferSubData(GL_TEXTURE_BUFFER, offset, sizeBytes, data);
}

GLuint ScopedTextureBufferObject::getBuffer() const
{
	return m_tbo;
}

size_t ScopedTextureBufferObject::getElementCount() const
{
	return m_elementCount;
}

} // namespace GVis