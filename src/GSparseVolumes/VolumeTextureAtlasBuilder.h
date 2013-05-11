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

#include "ImageBuffer.h"
#include "GSparseVolumesFwd.h"

#include <boost/scoped_ptr.hpp>

#include <GVis/GVisFwd.h>
#include <GVis/Math.h>

namespace GSparseVolumes {

class VolumeTextureAtlasBuilder
{
public:
	VolumeTextureAtlasBuilder(int maxItemCount, int itemTextureWidth, int channelCount);
	~VolumeTextureAtlasBuilder();

	void addTexture(const Leaf& leaf);

	GVis::TexturePtr build() const;

	glm::ivec3 getMaxItemCountPerDimension() const {return m_maxItemCountPerDimension;}

private:
	boost::scoped_ptr<ImageBufferUChar> m_imageBuffer;
	int m_itemCount;
	glm::ivec3 m_maxItemCountPerDimension;
	int m_itemTextureWidth;
};

} // namespace GSparseVolumes