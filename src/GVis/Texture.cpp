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

#include "Texture.h"
#include "TextureBufferObject.h"
#include "Convert.h"

#include <boost/scoped_array.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>

namespace GVis {

struct PixelFormatInfo
{
	int glFormat;
	int pixelSizeBytes;
	GLenum componentDataType; //!< GL_UNSIGNED_BYTE, GL_FLOAT etc
};

bool isCompressed(PixelFormat format)
{
	switch(format)
	{
	case PixelFormat_DXT1:
	case PixelFormat_DXT1_SRGB_ALPHA:
	case PixelFormat_DXT3:
	case PixelFormat_DXT3_SRGB_ALPHA:
	case PixelFormat_DXT5:
	case PixelFormat_DXT5_SRGB_ALPHA:
		return true;
	}

	return false;
}

void getUncompressedPixelFormatInfo(PixelFormatInfo& info, PixelFormat format)
{
	assert(!isCompressed(format));

	switch (format)
	{
	case PixelFormat_R8:
		info.glFormat = GL_RED;
		info.pixelSizeBytes = 1;
		info.componentDataType = GL_UNSIGNED_BYTE;
		break;
	case PixelFormat_R16:
		info.glFormat = GL_RED;
		info.pixelSizeBytes = 2;
		info.componentDataType = GL_UNSIGNED_SHORT;
		break;
	case PixelFormat_RGB8:
	case PixelFormat_SRGB8:
		info.glFormat = GL_RGB;
		info.pixelSizeBytes = 3;
		info.componentDataType = GL_UNSIGNED_BYTE;
		break;
	case PixelFormat_RGBA8:
	case PixelFormat_SRGB8_ALPHA8:
		info.glFormat = GL_RGBA;
		info.pixelSizeBytes = 4;
		info.componentDataType = GL_UNSIGNED_BYTE;
		break;
	case PixelFormat_RGB32F:
		info.glFormat = GL_RGB;
		info.pixelSizeBytes = 12;
		info.componentDataType = GL_FLOAT;
		break;
	case PixelFormat_R32F:
		info.glFormat = GL_R;
		info.pixelSizeBytes = 4;
		info.componentDataType = GL_FLOAT;
		break;
	case PixelFormat_R32I:
		info.glFormat = GL_R;
		info.pixelSizeBytes = 4;
		info.componentDataType = GL_INT;
		break;
	case PixelFormat_DepthComponent24:
		info.glFormat = GL_DEPTH_COMPONENT;
		info.pixelSizeBytes = 3;
		info.componentDataType = GL_FLOAT;
		break;
	default:
		throw std::runtime_error("Invalid pixel format: " + boost::lexical_cast<std::string>(format));
	}
}

int getGlPixelFormat(PixelFormat format)
{
	PixelFormatInfo info;
	getUncompressedPixelFormatInfo(info, format);
	return info.glFormat;
}

int getGlInternalPixelFormat(PixelFormat format)
{
	switch (format)
	{
	case PixelFormat_R8:
		return GL_R8;
	case PixelFormat_R16:
		return GL_R16;
	case PixelFormat_RGB8:
		return GL_RGB8;
	case PixelFormat_SRGB8:
		return GL_SRGB8;
	case PixelFormat_RGBA8:
		return GL_RGBA8;
	case PixelFormat_SRGB8_ALPHA8:
		return GL_SRGB8_ALPHA8;
	case PixelFormat_RGB32F:
		return GL_RGB32F;
	case PixelFormat_R32F:
		return GL_R32F;
	case PixelFormat_R32I:
		return GL_R32I;
	case PixelFormat_DepthComponent24:
		return GL_DEPTH_COMPONENT24;
	case PixelFormat_DXT1:
		return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	case PixelFormat_DXT1_SRGB_ALPHA:
		return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
	case PixelFormat_DXT3:
		return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	case PixelFormat_DXT3_SRGB_ALPHA:
		return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
	case PixelFormat_DXT5:
		return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	case PixelFormat_DXT5_SRGB_ALPHA:
		return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
	default:
		throw std::runtime_error("Invalid pixel format: " + boost::lexical_cast<std::string>(format));
	}
}

int getByteSizeofPixel(PixelFormat format)
{
	PixelFormatInfo info;
	getUncompressedPixelFormatInfo(info, format);
	return info.pixelSizeBytes;
}

GLenum getGlComponentDataType(PixelFormat format)
{
	PixelFormatInfo info;
	getUncompressedPixelFormatInfo(info, format);
	return info.componentDataType;
}


int getImageSizeBytes(int width, int height, int depth, PixelFormat format)
{
	if (isCompressed(format))
	{
		if (depth > 1)
		{
			throw std::runtime_error("3D Compressed textures not supported");
		}

		int blockSize = (format == PixelFormat_DXT1 || format == PixelFormat_DXT1_SRGB_ALPHA) ? 8 : 16;
		return ((width+3)/4)*((height+3)/4)*blockSize;
	}
	else
	{
		return getByteSizeofPixel(format) * width * height * depth;
	}
}

Texture::Texture(const ImageTextureConfig& config) :
	m_width(config.width),
	m_height(config.height),
	m_depth(config.depth)
{
	m_type = config.is3d ? TextureType_3d : TextureType_2d;

	assert(m_width > 0);
	assert(m_height > 0);
	assert(!config.is3d || m_depth > 0);

	GLenum target = toGlTargetType(m_type);

	glGenTextures(1, &m_textureId);

	glBindTexture(target, m_textureId);

	int loadableMipmapCount = config.manualMipmapCount ? std::max(1, *config.manualMipmapCount) : 1;

	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	if (config.filter == TextureFilter_Nearest)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else if (config.filter == TextureFilter_Bilinear)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		assert(0);
	}

	glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);  //Always set the base and max mipmap levels of a texture.
	
	if (config.manualMipmapCount)
	{
		glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, loadableMipmapCount - 1);
	}

	setTextureAddressMode(config.textureAddressMode);

	GLint internalFormat = getGlInternalPixelFormat(config.format);

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);


	bool compressed = isCompressed(config.format);
	if (!compressed)
	{
		m_glPixelFormat = getGlPixelFormat(config.format);
		m_componentDataType = getGlComponentDataType(config.format);
	}

	// Load each mipmap level
	int width = m_width;
	int height = m_height;
	int depth = m_depth;
	int offset = 0;
	for (int level = 0; level < loadableMipmapCount && (width || height || depth); ++level) 
	{
		int pixelSizeBytes = getImageSizeBytes(width, height, depth, config.format);

		if (target == GL_TEXTURE_2D)
		{
			if (isCompressed(config.format))
			{
				glCompressedTexImage2D(target, level, internalFormat, width, height, 0, pixelSizeBytes, config.data + offset); 
			}
			else
			{
				glTexImage2D(target, level, internalFormat, width, height, 0, *m_glPixelFormat, *m_componentDataType, config.data + offset);
			}
		}
		else if (target == GL_TEXTURE_3D)
		{
			glTexImage3D(target, level, internalFormat, width, height, depth, 0, *m_glPixelFormat, *m_componentDataType, config.data + offset);
		}
		else
		{
			assert(0);
		}
 
		offset += pixelSizeBytes; 
		width  /= 2; 
		height /= 2;
		if (target == GL_TEXTURE_3D)
		{
			depth /= 2;
		}
	}

	if (!config.manualMipmapCount)
	{
		glGenerateMipmap(target);
	}
}

Texture::Texture(const BufferTextureConfig& config) :
	m_height(1),
	m_depth(1),
	m_tbo(config.buffer)
{
	assert(config.buffer);
	m_width = config.buffer->getElementCount();
	assert(m_width > 0);

	m_type = TextureType_Buffer;
	GLenum target = toGlTargetType(m_type);

	glGenTextures(1, &m_textureId);
	glBindTexture(target, m_textureId);

	GLint internalFormat = getGlInternalPixelFormat(config.format);
	glTexBuffer(target, internalFormat, config.buffer->getBuffer());
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureId);
}

void Texture::setTextureAddressMode(TextureAddressMode mode)
{
	GLint wrapMode = toGlTextureWrap(mode);
	GLenum target = toGlTargetType(m_type);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapMode);
}

} // namespace GVis
