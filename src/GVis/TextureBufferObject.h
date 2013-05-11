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

namespace GVis {

class TextureBufferObject
{
public:
	virtual ~TextureBufferObject() {}

	virtual GLuint getBuffer() const = 0;
	virtual size_t getElementCount() const = 0;
};

class ScopedTextureBufferObject : public TextureBufferObject
{
public:
	//! Fills buffer with copy of data
	//! data can be null
	ScopedTextureBufferObject(void* data, size_t elementCount, size_t elementSizeBytes, BufferUsage usage = BufferUsage_Static);
	~ScopedTextureBufferObject();

	//! updates buffer with new data by copy
	//! @param sizeBytes specifies the size of the region to be replaced
	//! @param offset specifies the offset at which to begin data replacment
	void setData(void* data, int sizeBytes, int offset);

	GLuint getBuffer() const;
	size_t getElementCount() const;

private:
	GLuint m_tbo;
	size_t m_elementCount;
};

class ExternalTextureBufferObject : public TextureBufferObject
{
public:
	ExternalTextureBufferObject(GLuint tbo, size_t elementCount) :
		m_tbo(tbo),
		m_elementCount(elementCount)
	{
	}

	GLuint getBuffer() const
	{
		return m_tbo;
	}

	size_t getElementCount() const
	{
		return m_elementCount;
	}

private:
	GLuint m_tbo;
	size_t m_elementCount;
};

} // namespace GVis