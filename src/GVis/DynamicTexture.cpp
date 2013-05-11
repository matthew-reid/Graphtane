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

#include "DynamicTexture.h"
#include <GL/glew.h>

#include <string.h>

namespace GVis {

DynamicTexture::DynamicTexture(const ImageTextureConfig& config) :
	Texture(config)
{
	glGenBuffersARB(1, &m_bufferId);	
	m_dataSize = getByteSizeofPixel(config.format) * m_width * m_height * m_depth;
}

DynamicTexture::~DynamicTexture()
{
	glDeleteBuffersARB(1, &m_bufferId);
}

void DynamicTexture::setData(void* data)
{
	// Bind PBO to update texture source
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_bufferId);

	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_dataSize, 0, GL_STREAM_DRAW_ARB); // discards data in PBO so glMapBufferARB won't stall

	// map the buffer object into client's memory
	GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,
											GL_WRITE_ONLY_ARB);
	if(ptr)
	{
		// update data directly on the mapped buffer
		memcpy(ptr, data, m_dataSize);
		// release the mapped buffer
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
	}

	// Copy pixels from PBO to texture object
	switch (m_type)
	{
		case TextureType_2d:
		{
			glBindTexture(GL_TEXTURE_2D, m_textureId);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height,
							*m_glPixelFormat, *m_componentDataType, 0);
			break;
		}
		case TextureType_3d:
		{
			glBindTexture(GL_TEXTURE_3D, m_textureId);
			glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, m_width, m_height, m_depth,
							*m_glPixelFormat, *m_componentDataType, 0);
			glBindTexture(GL_TEXTURE_3D, 0);
			break;
		}
		default:
			assert(0);
	}

	// release PBO by 'binding' 0.
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

} // namespace GVis
