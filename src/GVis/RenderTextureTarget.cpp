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

#include "RenderTextureTarget.h"
#include "Texture.h"
#include "Convert.h"

#include <stdexcept>

namespace GVis {

RenderTextureTarget::RenderTextureTarget(const RenderTextureTargetConfig& config) :
	m_texture(config.texture),
	m_techniqueCategory(config.techniqueCategory),
	m_depthBuffer(0),
	m_colorAttachmentCount(0)
{
	glGenFramebuffers(1, &m_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
 
	// Create depth buffer
	if (config.attachment == FrameBufferAttachment_Color)
	{
		glGenRenderbuffers(1, &m_depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, config.texture->getWidth(), config.texture->getHeight());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
	}

	glFramebufferTexture(GL_FRAMEBUFFER, toGlFrameBufferAttachment(config.attachment), config.texture->_getGlTextureId(), 0);
 
	switch(config.attachment)
	{
		case FrameBufferAttachment_Color:
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			++m_colorAttachmentCount;
			break;
		case FrameBufferAttachment_Depth:
			glDrawBuffer(GL_NONE); // No color buffer is drawn to.
			break;
	}
 
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("Could not create RenderTextureTarget");
	}
}

RenderTextureTarget::~RenderTextureTarget()
{
	if (m_depthBuffer)
		glDeleteRenderbuffers(1, &m_depthBuffer);

	glDeleteFramebuffers(1, &m_framebuffer);
}

void RenderTextureTarget::addColorAttachment(const Texture& texture)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, toGlFrameBufferAttachment(FrameBufferAttachment_Color) + m_colorAttachmentCount, texture._getGlTextureId(), 0);
	++m_colorAttachmentCount;

	std::vector<GLenum> buffers;
	for (int i = 0; i < m_colorAttachmentCount; ++i)
	{
		buffers.push_back(GL_COLOR_ATTACHMENT0_EXT + i);
	}
	assert(!buffers.empty());
	glDrawBuffers(m_colorAttachmentCount, &buffers[0]);
}

int RenderTextureTarget::getWidth() const
{
	return m_texture->getWidth();
}

int RenderTextureTarget::getHeight() const
{
	return m_texture->getHeight();
}

} // namespace GVis
