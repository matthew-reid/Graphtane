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

#include <GAppFramework/Application.h>
#include <GAppFramework/CameraController.h>
#include <GAppFramework/RenderQueueIds.h>
#include <GComputeVis/Convert.h>

#include <GCommon/Logger.h>
#include <GCompute/ClSystem.h>

#include <GFluid/FluidSolver.h>
#include <GFluid/IsosurfaceNormalCalculator.h>
#include <GFluid/TempBufferPool.h>

#include <GVis/Camera.h>
#include <GVis/VisSystem.h>
#include <GVis/Primitives/BoxMeshFactory.h>
#include <GVis/Material.h>
#include <GVis/Geo.h>
#include <GVis/ShaderProgram.h>
#include <GVis/Light.h>
#include <GVis/ShaderParameter.h>
#include <GVis/ShadowProjector.h>
#include <GVis/RenderableNode.h>
#include <GVis/RenderTextureTarget.h>
#include <GVis/Technique.h>
#include <GVis/Viewport.h>
#include <GVis/Io/MaterialFactory.h>
#include <GVis/Primitives/RectangleMeshFactory.h>

#include <exception>

#include <boost/scoped_array.hpp>

using namespace GAppFramework;
using namespace GCommon;
using namespace GCompute;
using namespace GComputeVis;
using namespace GFluid;
using namespace GVis;

class ShadowedMaterialFactory : public MaterialFactory
{
public:
	ShadowedMaterialFactory(const TexturePtr& shadowTexture) :
		m_shadowTexture(shadowTexture)
	{
	}

	virtual MaterialPtr createMaterial(const TextureRoles& textureRoles)
	{
		MaterialPtr material(new Material);


		// Main technique
		{
			ShaderProgramPtr shader = ShaderProgram::createShaderProgram(
				ShaderProgramConfig("Shaders/Common/SimpleTexturedShadowed.vert", "Shaders/Common/SimpleColorDiffuseShadowed.frag"));

			TechniquePtr technique(new Technique(shader));

			{
				TextureUnit unit(m_shadowTexture, "shadowSampler");
				technique->addTextureUnit(unit);
			}

			material->setTechnique(technique);
		}

		// Shadow caster technique
		{
			ShaderProgramPtr shadowShader = ShaderProgram::createShaderProgram(
				ShaderProgramConfig("Shaders/Volume/HardSurfaceDeepShadowCaster.vert", "Shaders/Volume/HardSurfaceDeepShadowCaster.frag"));

			TechniquePtr technique(new Technique(shadowShader));
			material->setTechnique(technique, TechniqueCategory_DepthRtt);
		}

		return material;
	}

private:
	TexturePtr m_shadowTexture;
	const Application* m_app;
};

class TestApplication : public Application
{
public:
	TestApplication(const ApplicationConfig& config) :
		Application(config)
	{
	}

	RenderTextureTargetPtr createDepthTarget(const CameraPtr& camera, int width, int height)
	{
		ImageTextureConfig config = ImageTextureConfig::createDefault();
		config.width = width;
		config.height = height;
		config.textureAddressMode = TextureAddressMode_Clamp;
		config.format = PixelFormat_DepthComponent24;

		TexturePtr texture(new Texture(config));

		RenderTextureTargetPtr target;

		{
			RenderTextureTargetConfig config;
			config.texture = texture;
			config.attachment = FrameBufferAttachment_Depth;
			config.techniqueCategory = TechniqueCategory_DepthRtt;
			target.reset(new RenderTextureTarget(config));

			{
				ViewportPtr viewport = target->addDefaultViewport(m_camera);
				viewport->setBackgroundColor(glm::vec4(1,1,1,1));
			}

			m_window->addRenderTarget(target);
		}
		return target;
	}

	void setupScene()
	{
		// Must create ClSystem before OpenGL textures. FIXME: this constraint should be more explicit
		m_clSystem.reset(new ClSystem);

		m_window->getViewports().front()->setRenderQueueIdMask(~RenderQueueId_OffscreenTransparentObjects);

		LightPtr light(new Light);
		light->setDirection(glm::normalize(glm::vec3(1.5, -1, -0.5)));
		light->setPosition(glm::vec3(0, 1.0, 0) + light->getDirection() * -10.0f);
		m_visSystem->addLight(light);

		{
			ShadowProjectorConfig config = ShadowProjectorConfig::createDefault();
			config.sceneHeight = config.sceneWidth = 2;
			config.shadowFormat = ShadowFormat_RGB32F;
			m_shadowProjector.reset(new ShadowProjector(config));
			m_shadowProjector->setLight(light);
			m_window->addRenderTarget(m_shadowProjector->getRenderTextureTarget());
		}

		RenderTextureTargetPtr sceneDepthTarget = createDepthTarget(m_camera, m_window->getWidth(), m_window->getHeight());
		sceneDepthTarget->getViewports().front()->setRenderQueueIdMask(~RenderQueueId_OffscreenTransparentObjects);
		TexturePtr sceneDepthTexture = sceneDepthTarget->getTexture();

		// Volume
		{
			int width = 64;
			int height = 128;
			int depth = 128;
			int size = width * height * depth * 4;
			m_imageData.reset(new unsigned char[size]);
			memset(m_imageData.get(), 0, size);

			// Density texture
			m_textureConfig = ImageTextureConfig::createDefault();
			m_textureConfig.width = width;
			m_textureConfig.height = height;
			m_textureConfig.depth = depth;
			m_textureConfig.is3d = true;
			m_textureConfig.format = PixelFormat_RGBA8;
			m_textureConfig.textureAddressMode = TextureAddressMode_Clamp;
			m_textureConfig.data = m_imageData.get();
			m_fluidStateTexture.reset(new Texture(m_textureConfig));

			// Normal texture
			{
				ImageTextureConfig normalTextureConfig = m_textureConfig;
				normalTextureConfig.width /= 2;
				normalTextureConfig.height /= 2;
				normalTextureConfig.depth /= 2;
				normalTextureConfig.data = 0;
				m_normalTexture.reset(new Texture(normalTextureConfig));
			}


			glm::vec3 boxSize(0.5, 1, 1);
			glm::vec3 volumeTextureSize(m_textureConfig.width, m_textureConfig.height, m_textureConfig.depth);

			MaterialPtr material(new Material);

			// Main technique
			{
				ShaderProgramPtr shader = ShaderProgram::createShaderProgram(ShaderProgramConfig("Shaders/Volume/RaymarchedVolume.vert", "Shaders/Volume/VolumetricSmoke.frag"));
				
				
				TechniquePtr technique(new Technique(shader));
				{
					Vec3ShaderParameterPtr boxModelSizeParameter(new Vec3ShaderParameter("volumeSize_modelSpace", boxSize));
					technique->addCustomShaderParameter(boxModelSizeParameter);

					Vec3ShaderParameterPtr oneOnVolumeTextureSizeParameter(new Vec3ShaderParameter("oneOnVolumeTextureSize", 1.0f / volumeTextureSize));
					technique->addCustomShaderParameter(oneOnVolumeTextureSizeParameter);
					technique->addCustomShaderParameter(ShaderParameterPtr(new FloatShaderParameter("opacityMultiplier", 30.0)));

					technique->setAlphaBlendingMode(AlphaBlendingMode_PreMultiplied);
					{
						TextureUnit unit(m_fluidStateTexture, "albedoSampler");
						technique->addTextureUnit(unit);
					}

					{
						TextureUnit unit(sceneDepthTexture, "sceneDepthSampler");
						technique->addTextureUnit(unit);
					}

					{
						TextureUnit unit(m_shadowProjector->getShadowTexture(), "shadowSampler");
						technique->addTextureUnit(unit);
					}

					{
						TextureUnit unit(m_normalTexture, "normalSampler");
						technique->addTextureUnit(unit);
					}
				}

				material->setTechnique(technique);
			}

			// Deep shadow caster technique
			{
				ShaderProgramPtr shader = ShaderProgram::createShaderProgram(ShaderProgramConfig("Shaders/Volume/RaymarchedVolume.vert", "Shaders/Volume/VolumeShadowCaster.frag"));

				TechniquePtr technique(new Technique(shader));
				{
					Vec3ShaderParameterPtr boxModelSizeParameter(new Vec3ShaderParameter("volumeSize_modelSpace", boxSize));
					technique->addCustomShaderParameter(boxModelSizeParameter);

					Vec3ShaderParameterPtr oneOnVolumeTextureSizeParameter(new Vec3ShaderParameter("oneOnVolumeTextureSize", 1.0f / volumeTextureSize));
					technique->addCustomShaderParameter(oneOnVolumeTextureSizeParameter);
					technique->addCustomShaderParameter(ShaderParameterPtr(new FloatShaderParameter("opacityMultiplier", 0.8)));

					{
						TextureUnit unit(m_fluidStateTexture, "albedoSampler");
						technique->addTextureUnit(unit);
					}
				}

				material->setTechnique(technique, TechniqueCategory_DepthRtt);
			}

			{
				BoxConfig config;
				config.size = boxSize;
				MeshPtr mesh = BoxMeshFactory::createMesh(config);
				GeoPtr geo(new Geo(mesh, material));

				RenderableNodePtr renderableNode(new RenderableNode);
				renderableNode->addRenderable(geo);
				renderableNode->setPosition(glm::vec3(0, 0.5, 0));
				m_visSystem->addRenderableNode(renderableNode, RenderQueueId_OffscreenTransparentObjects);
			}
		}

		// Scene geo
		{
			ShadowedMaterialFactory materialFactory(m_shadowProjector->getShadowTexture());
			// Ground plane
			{
				RectangleConfig config = RectangleConfig::defaultWithSize(glm::vec2(5, 5));
				config.orientation = glm::angleAxis(halfPi(), glm::vec3(-1, 0, 0));
				MeshPtr mesh = RectangleMeshFactory::createMesh(config);

				GeoPtr geo(new Geo(mesh, materialFactory.createMaterial(TextureRoles())));
				RenderableNodePtr renderableNode = RenderableNode::createWithSingleGeo(geo);
				m_visSystem->addRenderableNode(renderableNode);
			}
		}

		int volumeTargetWidth = m_window->getWidth() / 2;
		int volumeTargetHeight = m_window->getHeight() / 2;

		TexturePtr offscreenTransparentObjectsTexture;
		// Create offscreen transparent objects target
		{
			ImageTextureConfig config = ImageTextureConfig::createDefault();
			config.width = volumeTargetWidth;
			config.height = volumeTargetHeight;
			config.textureAddressMode = TextureAddressMode_Clamp;
			offscreenTransparentObjectsTexture.reset(new Texture(config));

			{
				RenderTextureTargetConfig config;
				config.texture = offscreenTransparentObjectsTexture;
				config.attachment = FrameBufferAttachment_Color;
				RenderTextureTargetPtr target(new RenderTextureTarget(config));

				{
					ViewportPtr viewport = target->addDefaultViewport(m_camera);
					viewport->setRenderQueueIdMask(RenderQueueId_OffscreenTransparentObjects);
					viewport->setBackgroundColor(glm::vec4(0,0,0,0));
				}

				m_window->addRenderTarget(target);
			}
		}

		// Composite offscreen particles
		{
			TextureUnit textureUnit(offscreenTransparentObjectsTexture, "albedoSampler");

			ShaderProgramPtr shader = ShaderProgram::createShaderProgram(ShaderProgramConfig("Shaders/Common/ScreenQuad.vert", "Shaders/Common/SimpleTextured.frag"));
			TechniquePtr technique(new Technique(shader));
			technique->addTextureUnit(textureUnit);
			technique->setAlphaBlendingMode(AlphaBlendingMode_PreMultiplied);
			MaterialPtr material(new Material);
			material->setTechnique(technique);

			{
				RectangleConfig config = RectangleConfig::defaultWithSize(glm::vec2(2, 2));
				MeshPtr mesh = RectangleMeshFactory::createMesh(config);
				GeoPtr geo(new Geo(mesh, material));

				RenderableNodePtr renderableNode(new RenderableNode);
				renderableNode->addRenderable(geo);
				m_visSystem->addRenderableNode(renderableNode, RenderQueueId_CompositeOffscreenTransparentObjects);
			}
		}

		int tempBufferElementCount = m_textureConfig.width * m_textureConfig.height * m_textureConfig.depth;

		std::string fluidKernelsDir = "Kernels/Fluid";

		TempBufferPoolPtr tempBufferPool(new TempBufferPool(*m_clSystem, tempBufferElementCount));
		m_solver = createFluidSolver(*m_clSystem, getGlTexture(m_fluidStateTexture), tempBufferPool, fluidKernelsDir);
		FluidSolverParamsPtr params = m_solver->getParams();
		params->temperatureBuoyancy = 15;
		params->densityWeight = 2.0;
		params->drag = 0.05;

		m_isosurfaceNormalCalculator = createIsosurfaceNormalCalculator(*m_clSystem, getGlTexture(m_normalTexture), m_solver, tempBufferPool, fluidKernelsDir);

		m_camera->setPosition(glm::vec3(2.2, 0.5, 0.7));
		m_cameraController->rotate(1.2, -0.05);
	}

	void simulate(float dt)
	{
		dt *= 3; // Take bigger steps to produce more turbulence / instability. Looks more interesting, but a bit dodgy.

		float x = 0 * sin(getElapsedTime()*0.4) * 0.2 + 0.5;
		float z = sin((getElapsedTime())*1.0 - GVis::halfPi()) * 0.25 + 0.5;
		float density = 0.3;
		float temperature = 1;
		Float3 position(15 + x * (m_textureConfig.width - 30), 8, 15 + z * (m_textureConfig.depth -30));
		m_solver->setFluid(position, density, temperature);

		m_solver->update(dt);
		m_isosurfaceNormalCalculator->updateTexture();
	}

private:
	boost::scoped_array<unsigned char> m_imageData;

	ImageTextureConfig m_textureConfig;
	TexturePtr m_fluidStateTexture;
	TexturePtr m_normalTexture;

	boost::scoped_ptr<ShadowProjector> m_shadowProjector;

	ClSystemPtr m_clSystem;
	FluidSolverPtr m_solver;
	IsosurfaceNormalCalculatorPtr m_isosurfaceNormalCalculator;
};

int main(int argc, char** argv)
{
	try
	{
		TestApplication app(ApplicationConfig::fromArgs(argc, argv));
		app.run();
	}
	catch(const std::exception& e)
	{
		defaultLogger()->logLine(e.what());
	}
}