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
#include "Math.h"
#include "RenderContext.h"

#include <vector>

namespace GVis {

struct TextureUnit
{
	TextureUnit(const TexturePtr& texture, const std::string& samplerName) :
		texture(texture),
		samplerName(samplerName)
	{
	}

	TexturePtr texture;
	std::string samplerName;
};

enum AlphaBlendingMode
{
	AlphaBlendingMode_None,
	AlphaBlendingMode_Straight,
	AlphaBlendingMode_PreMultiplied,
};

class Technique
{
public:
	Technique(const ShaderProgramPtr& shader);

	void addTextureUnit(const TextureUnit& unit);

	void applyState();

	//! Light can be null
	virtual void updateShaderParameters(const RenderContext& context);

	void setAlphaBlendingMode(AlphaBlendingMode mode);
	void setAlphaToCoverageEnabled(bool enabled);
	void setDepthCheckEnabled(bool enabled) {m_depthCheckEnabled = enabled;}

	void addCustomShaderParameter(const ShaderParameterPtr& parameter);

protected:
	FloatShaderParameterPtr m_farClipDistance;
	FloatShaderParameterPtr m_maxClipSpaceDepth;
	FloatShaderParameterPtr m_shadowMaxClipSpaceDepth;
	Mat4ShaderParameterPtr m_modelViewProjectionMatrix;
	Mat4ShaderParameterPtr m_viewProjectionMatrix;
	Mat4ShaderParameterPtr m_invModelMatrix;
	Vec3ShaderParameterPtr m_cameraPosition_modelSpace;
	Vec3ShaderParameterPtr m_cameraDirection_modelSpace; // direction to camera
	Vec3ShaderParameterPtr m_cameraForwardDirection_modelSpace; // direction of camera
	Vec3ShaderParameterPtr m_lightDirection_modelSpace;
	Mat4ShaderParameterPtr m_shadowModelViewProjectionMatrix;
	Mat4ShaderParameterPtr m_invShadowViewProjectionMatrix;
	Vec2ShaderParameterPtr m_viewportResolution;

private:
	ShaderProgramPtr m_shader;
	AlphaBlendingMode m_alphaBlendingMode;
	bool m_alphaToCoverageEnabled;
	bool m_depthCheckEnabled;

	std::vector<ShaderParameterPtr> m_parameters;
	std::vector<TextureUnit> m_textureUnits;
};

} // namespace GVis
