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

#include <boost/optional.hpp>

namespace GVis {

enum PixelFormat
{
	PixelFormat_R8,
	PixelFormat_R16,
	PixelFormat_RGB8,
	PixelFormat_SRGB8,
	PixelFormat_RGBA8,
	PixelFormat_SRGB8_ALPHA8,
	PixelFormat_RGB32F,
	PixelFormat_R32F,
	PixelFormat_R32I,
	PixelFormat_DepthComponent24,
	PixelFormat_DXT1,
	PixelFormat_DXT1_SRGB_ALPHA,
	PixelFormat_DXT3,
	PixelFormat_DXT3_SRGB_ALPHA,
	PixelFormat_DXT5,
	PixelFormat_DXT5_SRGB_ALPHA,
};

enum TextureType
{
	TextureType_2d,
	TextureType_3d,
	TextureType_Buffer
};

enum TextureAddressMode
{
	TextureAddressMode_Wrap,
	TextureAddressMode_Clamp
};

enum TextureFilter
{
	TextureFilter_Nearest,
	TextureFilter_Bilinear,
};

struct ImageTextureConfig
{
	static ImageTextureConfig createDefault()
	{
		ImageTextureConfig c;
		c.width = 512;
		c.height = 512;
		c.depth = 1;
		c.is3d = false;
		c.format = PixelFormat_RGBA8;
		c.textureAddressMode = TextureAddressMode_Wrap;
		c.data = 0; // texture will be blank
		c.manualMipmapCount = 0;
		c.filter = TextureFilter_Bilinear;
		return c;
	}

	int width;
	int height;
	int depth; //! For 3d textures
	bool is3d;
	boost::optional<int> manualMipmapCount; //! Mipmaps will be generated manualMipmapCount is not specified
	PixelFormat format;
	TextureAddressMode textureAddressMode;
	TextureFilter filter;
	unsigned char* data;
};

//! Config to create a texture which wraps a texture buffer object
struct BufferTextureConfig
{
	BufferTextureConfig(PixelFormat format, const TextureBufferObjectPtr& buffer) :
		format(format),
		buffer(buffer)
	{
	}

	PixelFormat format;
	TextureBufferObjectPtr buffer;
};

class Texture
{
public:
	Texture(const ImageTextureConfig& config);
	Texture(const BufferTextureConfig& config);
	~Texture();

	GLuint _getGlTextureId() const {return m_textureId;}

	TextureType getType() const {return m_type;}

	int getWidth() const {return m_width;}
	int getHeight() const {return m_height;}
	int getDepth() const {return m_depth;}

	void setTextureAddressMode(TextureAddressMode mode);

protected:
	GLuint m_textureId;
	int m_width;
	int m_height;
	int m_depth;
	TextureType m_type;

	//! Only valid for uncompressed formats
	boost::optional<int> m_glPixelFormat;

	//! Only valid for uncompressed formats
	boost::optional<GLenum> m_componentDataType;

	TextureBufferObjectPtr m_tbo; // Non-null when Texture created as a BufferTexture
};

extern int getByteSizeofPixel(PixelFormat format);

} // namespace GVis
