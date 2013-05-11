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

#include "ShadowProjector.h"

#include "Camera.h"
#include "Light.h"
#include "RenderTextureTarget.h"
#include "SceneNode.h"
#include "Texture.h"
#include "Viewport.h"

namespace GVis {


ShadowProjector::ShadowProjector(const ShadowProjectorConfig& projectorConfig) :
	m_sceneCamera(projectorConfig.sceneCamera),
	m_offsetFromSceneCamera(projectorConfig.offsetFromSceneCamera)
{
	// Create texture
	{
		PixelFormat pixelFormat;
		switch(projectorConfig.shadowFormat)
		{
			case ShadowFormat_RGB32F:
			{
				pixelFormat = PixelFormat_RGB32F;
				break;
			}
			default:
				assert(0);
		}

		ImageTextureConfig config = ImageTextureConfig::createDefault();
		config.format = pixelFormat;
		config.textureAddressMode = TextureAddressMode_Clamp;
		config.width = projectorConfig.textureWidth;
		config.height = projectorConfig.textureHeight;
		config.filter = projectorConfig.textureFilter;

		m_texture.reset(new Texture(config));
	}

	// Create render target
	{
		FrameBufferAttachment attachment;
		switch(projectorConfig.shadowFormat)
		{
			case ShadowFormat_R32F:
			case ShadowFormat_RGB32F:
			{
				attachment = FrameBufferAttachment_Color;
				break;
			}
			default:
				assert(0);
		}

		RenderTextureTargetConfig config;
		config.texture = m_texture;
		config.attachment = attachment;
		config.techniqueCategory = TechniqueCategory_DepthRtt;

		m_target.reset(new RenderTextureTarget(config));
	}

	// Create projector camera
	{
		OrthographicProjectionPtr projection(new OrthographicProjection);
		projection->setDimensions(projectorConfig.sceneWidth, projectorConfig.sceneHeight);

		m_projectorCamera.reset(new Camera);
		m_projectorCamera->setProjection(projection);
	}

	// Create viewport
	{
		ViewportPtr viewport = m_target->addDefaultViewport(m_projectorCamera);
		viewport->addListener(this);
		viewport->setBackgroundColor(glm::vec4(1,1,1,1));
	}
}

ShadowProjector::~ShadowProjector()
{
	setLight(LightPtr()); // detatch from light
}

void ShadowProjector::setLight(const LightPtr& light)
{
	if (m_light)
	{
		m_light->m_shadowProjector = 0;
	}

	m_light = light;

	if (m_light)
	{
		m_light->m_shadowProjector = this;
	}
}

void ShadowProjector::viewport_preRender()
{
	if (m_light)
	{
		glm::vec3 pos;
		if (m_sceneCamera)
		{
			pos = m_sceneCamera->getPosition() - m_light->getDirection() * m_offsetFromSceneCamera;
		}
		else
		{
			pos = m_light->getPosition();
		}

		m_projectorCamera->setPosition(pos);
		m_projectorCamera->setOrientation(m_light->getOrientation());
	}
}

} // namespace GVis
