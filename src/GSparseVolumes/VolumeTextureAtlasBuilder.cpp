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

#include "VolumeTextureAtlasBuilder.h"
#include "Leaf.h"

#include <GVis/Texture.h>

using namespace GSparseVolumes;
using namespace GVis;

//! Compute the next highest power of 2 of 32-bit unsigned int
static unsigned int roundUpToNearestPowerOfTwo(unsigned int v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

VolumeTextureAtlasBuilder::VolumeTextureAtlasBuilder(int maxItemCount, int itemTextureWidth, int channelCount) :
	m_itemCount(0),
	m_itemTextureWidth(itemTextureWidth)
{
	assert(maxItemCount > 0);

	m_maxItemCountPerDimension.x = ceil(pow((double)maxItemCount, 1.0 / 3.0));
	m_maxItemCountPerDimension.x = roundUpToNearestPowerOfTwo(m_maxItemCountPerDimension.x);

	m_maxItemCountPerDimension.y = ceil(sqrt((double)maxItemCount / (double)m_maxItemCountPerDimension.x));
	m_maxItemCountPerDimension.y = roundUpToNearestPowerOfTwo(m_maxItemCountPerDimension.y);

	m_maxItemCountPerDimension.z = ceil((double)maxItemCount / (double)(m_maxItemCountPerDimension.x * m_maxItemCountPerDimension.y));
	m_maxItemCountPerDimension.z = roundUpToNearestPowerOfTwo(m_maxItemCountPerDimension.z);

	m_imageBuffer.reset(new ImageBufferUChar(m_maxItemCountPerDimension.x * itemTextureWidth,
												m_maxItemCountPerDimension.y * itemTextureWidth,
												m_maxItemCountPerDimension.z * itemTextureWidth, channelCount));

	memset(m_imageBuffer->data, 0, m_imageBuffer->getSizeInBytes());
}

VolumeTextureAtlasBuilder::~VolumeTextureAtlasBuilder()
{
}

TexturePtr VolumeTextureAtlasBuilder::build() const
{
	ImageTextureConfig textureConfig = ImageTextureConfig::createDefault();
	textureConfig.width = m_imageBuffer->width;
	textureConfig.height = m_imageBuffer->height;
	textureConfig.depth = m_imageBuffer->depth;
	textureConfig.is3d = true;
	textureConfig.manualMipmapCount = 0;
			
	if (m_imageBuffer->channelCount == 1)
	{
		textureConfig.format = PixelFormat_R8;
	}
	else if (m_imageBuffer->channelCount == 3)
	{
		textureConfig.format = PixelFormat_RGB8;
	}
	else if (m_imageBuffer->channelCount == 4)
	{
		textureConfig.format = PixelFormat_RGBA8;
	}
	else
	{
		throw std::runtime_error("Image format is unsupported");
	}
			
			
	textureConfig.textureAddressMode = TextureAddressMode_Clamp;
	textureConfig.data = m_imageBuffer->data;
	
	return TexturePtr(new Texture(textureConfig));
}

void VolumeTextureAtlasBuilder::addTexture(const Leaf& leaf)
{
	int x = m_itemCount % m_maxItemCountPerDimension.x;
	int y = (m_itemCount / m_maxItemCountPerDimension.x) % m_maxItemCountPerDimension.y;
	int z = m_itemCount / (m_maxItemCountPerDimension.x * m_maxItemCountPerDimension.y);

	assert(leaf.getVoxelCountPerDimension() == m_itemTextureWidth);
	leaf.toImageBuffer(*m_imageBuffer, glm::ivec3(x * m_itemTextureWidth, y * m_itemTextureWidth, z * m_itemTextureWidth));

	++m_itemCount;
}