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
#include "Texture.h"
#include "ViewportListener.h"

namespace GVis {

enum ShadowFormat
{
	ShadowFormat_R32F,
	ShadowFormat_RGB32F
};

struct ShadowProjectorConfig
{
	static ShadowProjectorConfig createDefault()
	{
		ShadowProjectorConfig config;
		config.sceneWidth = 30;
		config.sceneHeight = 30;
		config.textureWidth = 512;
		config.textureHeight = 512;
		config.offsetFromSceneCamera = 10;
		config.shadowFormat = ShadowFormat_R32F;
		config.textureFilter = TextureFilter_Bilinear;

		return config;
	}

	float sceneWidth;
	float sceneHeight;
	int textureWidth;
	int textureHeight;

	//! Camera to follow. If null, projector will attach to light position.
	CameraPtr sceneCamera;
	//! projector will follow scene camera with this offset.
	float offsetFromSceneCamera;
	ShadowFormat shadowFormat;
	TextureFilter textureFilter;
};

class ShadowProjector : public ViewportListener
{
public:
	ShadowProjector(const ShadowProjectorConfig& config);
	~ShadowProjector();

	TexturePtr getShadowTexture() const {return m_texture;}
	const RenderTextureTargetPtr& getRenderTextureTarget() const {return m_target;}

	void setLight(const LightPtr& light);

	const CameraPtr& getCamera() const {return m_projectorCamera;}

protected:
	// ViewportListener interface
	virtual void viewport_preRender();
	virtual void viewport_postRender() {};

protected:
	RenderTextureTargetPtr m_target;

private:
	LightPtr m_light;
	TexturePtr m_texture;
	CameraPtr m_projectorCamera;
	CameraPtr m_sceneCamera;

	float m_offsetFromSceneCamera;
};

} // namespace GVis
