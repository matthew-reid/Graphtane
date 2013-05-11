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

#include "RenderTarget.h"

namespace GVis {

enum FrameBufferAttachment
{
	FrameBufferAttachment_Color,
	FrameBufferAttachment_Depth
};

struct RenderTextureTargetConfig
{
	RenderTextureTargetConfig() : techniqueCategory(TechniqueCategory_Main) {}

	TexturePtr texture;
	FrameBufferAttachment attachment;
	TechniqueCategory techniqueCategory;
};

class RenderTextureTarget : public RenderTarget
{
public:
	RenderTextureTarget(const RenderTextureTargetConfig& config);
	~RenderTextureTarget();

	void addColorAttachment(const Texture& texture);

	int getWidth() const;
	int getHeight() const;

	TechniqueCategory getTechniqueCategory() const {return m_techniqueCategory;}

	const TexturePtr& getTexture() const {return m_texture;}

private:
	TexturePtr m_texture;
	TechniqueCategory m_techniqueCategory;
	GLuint m_depthBuffer;

	int m_colorAttachmentCount;
};

} // namespace GVis
