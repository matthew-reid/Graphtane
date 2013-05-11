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

#include "Technique.h"
#include "sceneNode.h"
#include "ShaderProgram.h"
#include "ShaderParameter.h"
#include "Camera.h"
#include "Light.h"
#include "ShadowProjector.h"
#include "Convert.h"

#include <assert.h>

namespace GVis {

Technique::Technique(const ShaderProgramPtr& shader) :
	m_shader(shader),
	m_alphaBlendingMode(AlphaBlendingMode_None),
	m_alphaToCoverageEnabled(false),
	m_depthCheckEnabled(true)
{
	assert(m_shader);

	m_farClipDistance.reset(new FloatShaderParameter("farClipDistance", 0));
	m_parameters.push_back(m_farClipDistance);

	m_maxClipSpaceDepth.reset(new FloatShaderParameter("maxClipSpaceDepth", 0));
	m_parameters.push_back(m_maxClipSpaceDepth);

	m_shadowMaxClipSpaceDepth.reset(new FloatShaderParameter("shadowMaxClipSpaceDepth", 0));
	m_parameters.push_back(m_shadowMaxClipSpaceDepth);

	m_modelViewProjectionMatrix.reset(new Mat4ShaderParameter("modelViewProj", glm::mat4(1)));
	m_parameters.push_back(m_modelViewProjectionMatrix);

	m_viewProjectionMatrix.reset(new Mat4ShaderParameter("viewProj", glm::mat4(1)));
	m_parameters.push_back(m_viewProjectionMatrix);

	m_invModelMatrix.reset(new Mat4ShaderParameter("invModelMat", glm::mat4(1)));
	m_parameters.push_back(m_invModelMatrix);

	m_cameraPosition_modelSpace.reset(new Vec3ShaderParameter("cameraPosition_modelSpace", glm::vec3(0)));
	m_parameters.push_back(m_cameraPosition_modelSpace);

	m_cameraDirection_modelSpace.reset(new Vec3ShaderParameter("cameraDirection_modelSpace", glm::vec3(0)));
	m_parameters.push_back(m_cameraDirection_modelSpace);

	m_cameraForwardDirection_modelSpace.reset(new Vec3ShaderParameter("cameraForwardDirection_modelSpace", glm::vec3(0)));
	m_parameters.push_back(m_cameraForwardDirection_modelSpace);
	
	m_lightDirection_modelSpace.reset(new Vec3ShaderParameter("lightDirection_modelSpace", glm::vec3(0)));
	m_parameters.push_back(m_lightDirection_modelSpace);

	m_shadowModelViewProjectionMatrix.reset(new Mat4ShaderParameter("shadowModelViewProj", glm::mat4(1)));
	m_parameters.push_back(m_shadowModelViewProjectionMatrix);

	m_invShadowViewProjectionMatrix.reset(new Mat4ShaderParameter("invShadowViewProj", glm::mat4(1)));
	m_parameters.push_back(m_invShadowViewProjectionMatrix);

	m_viewportResolution.reset(new Vec2ShaderParameter("viewportResolution", glm::vec2(0)));
	m_parameters.push_back(m_viewportResolution);
}

void Technique::addTextureUnit(const TextureUnit& unit)
{
	assert(unit.texture);
	m_textureUnits.push_back(unit);
}

void Technique::applyState()
{
	// Set alpha blending mode
	{
		switch (m_alphaBlendingMode)
		{
		case AlphaBlendingMode_None:
			glDisable(GL_BLEND);
			break;
		case AlphaBlendingMode_Straight:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case AlphaBlendingMode_PreMultiplied:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			break;
		default:
			assert(0);
		}
	}

	if (m_depthCheckEnabled)
	{
		glDepthFunc(GL_LESS);
	}
	else
	{
		glDepthFunc(GL_ALWAYS);
	}	
	
	if (m_alphaToCoverageEnabled)
	{
		glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	}
	else
	{
		glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	}

	// Use shader
	GLint programId = m_shader->_getProgramId();
	glUseProgram(programId);

	// Set textures
	{
		int count;
		count = m_textureUnits.size();
		for (int i = 0; i < count; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			const TextureUnit& unit = m_textureUnits[i];
			glBindTexture(toGlTargetType(unit.texture->getType()), unit.texture->_getGlTextureId());

			GLuint uniformLocation = glGetUniformLocation(programId, unit.samplerName.c_str());
			glUniform1i(uniformLocation, i);
		}
	}

	// Set uniforms
	{
		int count = (int)m_parameters.size();
		for (int i = 0; i < count; i++)
		{
			m_parameters[i]->apply(*m_shader);
		}
	}
}

void Technique::updateShaderParameters(const RenderContext& context)
{
	m_viewportResolution->setValue(context.viewportResolution);

	ProjectionPtr projection = context.camera->getProjection();
	m_farClipDistance->setValue(projection->getFarClipDistance());
	m_maxClipSpaceDepth->setValue(projection->getMaxClipSpaceDepth());

	glm::mat4 viewProj = context.camera->getViewProjectionMatrix();
	m_viewProjectionMatrix->setValue(viewProj);

	m_modelViewProjectionMatrix->setValue(viewProj * context.sceneNode->getTransform());

	glm::mat4 invModelMatrix = glm::inverse(context.sceneNode->getTransform());
	m_invModelMatrix->setValue(invModelMatrix);
	{ // camera
		glm::vec4 v = invModelMatrix * glm::vec4(context.camera->getPosition(), 1);
		glm::vec3 pos(v.x, v.y, v.z);
		m_cameraPosition_modelSpace->setValue(pos);
		m_cameraDirection_modelSpace->setValue(glm::normalize(pos));
		m_cameraForwardDirection_modelSpace->setValue(glm::mat3(invModelMatrix) * context.camera->getDirection());
	}
	

	if (context.light)
	{
		glm::vec3 dir = glm::inverse(context.sceneNode->getOrientation()) * context.light->getDirection();
		m_lightDirection_modelSpace->setValue(-dir); // light direction in shader is direction to light

		const ShadowProjector* projector = context.light->getShadowProjector();
		if (projector)
		{
			static glm::mat4 biasMatrix(
				0.5, 0.0, 0.0, 0.0, 
				0.0, 0.5, 0.0, 0.0,
				0.0, 0.0, 0.5, 0.0,
				0.5, 0.5, 0.5, 1.0
			);

			glm::mat4 shadowModelViewProj = projector->getCamera()->getViewProjectionMatrix() * context.sceneNode->getTransform();

			m_shadowModelViewProjectionMatrix->setValue(biasMatrix * shadowModelViewProj);

			m_shadowMaxClipSpaceDepth->setValue(projector->getCamera()->getProjection()->getMaxClipSpaceDepth());

			m_invShadowViewProjectionMatrix->setValue(glm::inverse(biasMatrix * projector->getCamera()->getViewProjectionMatrix()));
		}
	}
	else
	{
		m_lightDirection_modelSpace->setValue(vec3Zero());
	}
}

void Technique::addCustomShaderParameter(const ShaderParameterPtr& parameter)
{
	assert(parameter);
	m_parameters.push_back(parameter);
}

void Technique::setAlphaBlendingMode(AlphaBlendingMode mode)
{
	m_alphaBlendingMode = mode;
}

void Technique::setAlphaToCoverageEnabled(bool enabled)
{
	m_alphaToCoverageEnabled = enabled;
}

} // namespace GVis
