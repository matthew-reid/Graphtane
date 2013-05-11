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

#include <GCommon/Logger.h>

#include <GVis/Camera.h>
#include <GVis/VisSystem.h>
#include <GVis/Window.h>
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
#include <GVis/Io/ObjLoader.h>
#include <GVis/Primitives/RectangleMeshFactory.h>

#include <exception>

#include <boost/scoped_array.hpp>

using namespace GAppFramework;
using namespace GCommon;
using namespace GVis;

class HardSurfaceMaterialFactory : public MaterialFactory
{
public:
	HardSurfaceMaterialFactory(const TexturePtr& shadowTexture, const Application* app) :
		m_shadowTexture(shadowTexture),
		m_app(app)
	{
	}

	virtual MaterialPtr createMaterial(const TextureRoles& textureRoles)
	{
		MaterialPtr material(new Material);

		// Main technique
		{
			ShaderProgramPtr shader = ShaderProgram::createShaderProgram(ShaderProgramConfig("Shaders/Common/SimpleTexturedShadowed.vert", "Shaders/Common/SimpleColorDiffuseShadowed.frag"));
		
			TechniquePtr technique(new Technique(shader));
			TextureUnit unit(m_shadowTexture, "shadowSampler");
			technique->addTextureUnit(unit);

			material->setTechnique(technique);
		}

		// Shadow caster technique
		{
			ShaderProgramPtr shadowShader = ShaderProgram::createShaderProgram(ShaderProgramConfig("Shaders/Volume/HardSurfaceDeepShadowCaster.vert", "Shaders/Volume/HardSurfaceDeepShadowCaster.frag"));
			TechniquePtr technique(new Technique(shadowShader));
			material->setTechnique(technique, TechniqueCategory_DepthRtt);
		}

		return material;
	}

private:
	TexturePtr m_shadowTexture;
	const Application* m_app;
};

static TexturePtr createDensityTexture(int width)
{
	int height = width;
	int depth = width;
	int size = width * height * depth;
	boost::scoped_array<unsigned char> buffer(new unsigned char[size]);
			
	unsigned char* p = buffer.get();
	for (int z = 0; z < depth; ++z)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				// Gyroid
				float scale = 10.0f / (float)width;
				float s = std::cos((float)x * scale) * std::sin((float)y * scale) +
						  std::cos((float)y * scale) * std::sin((float)z * scale) +
						  std::cos((float)z * scale) * std::sin((float)x * scale);
				s /= 3.0; // [-1, 1]

				// Sphere
				glm::vec3 pos((float)x / (float)width, (float)y / (float)height, (float)z / (float)depth);
				pos = pos * 2.0f - 1.0f;

				s *= ((glm::length(pos) - 0.95f) < 0.0f);

				*p++ = s * 150;
			}
		}
	}

	ImageTextureConfig config = ImageTextureConfig::createDefault();
	config.width = width;
	config.height = height;
	config.depth = depth;
	config.is3d = true;
	config.format = PixelFormat_R8;
	config.textureAddressMode = TextureAddressMode_Clamp;
	config.data = buffer.get();
	return TexturePtr(new Texture(config));
}

class TestApplication : public Application
{
public:
	TestApplication(const ApplicationConfig& config) :
		Application(config)
	{
	}

	void setupScene()
	{
		m_window->getViewports().front()->setRenderQueueIdMask(~RenderQueueId_OffscreenTransparentObjects);

		LightPtr light(new Light);
		light->setDirection(glm::normalize(glm::vec3(1.5, -1, -0.5)));
		light->setPosition(glm::vec3(0, 0.8, 0) + light->getDirection() * -10.0f);
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

		// Scene geo
		{
			HardSurfaceMaterialFactory materialFactory(m_shadowProjector->getShadowTexture(), this);
			// Ground plane
			{
				RectangleConfig config = RectangleConfig::defaultWithSize(glm::vec2(5, 5));
				config.orientation = glm::angleAxis(halfPi(), glm::vec3(-1, 0, 0));
				MeshPtr mesh = RectangleMeshFactory::createMesh(config);

				GeoPtr geo(new Geo(mesh, materialFactory.createMaterial(TextureRoles())));
				RenderableNodePtr renderableNode = RenderableNode::createWithSingleGeo(geo);
				m_visSystem->addRenderableNode(renderableNode);
			}

			// Column
			{
				RenderableNodePtr renderableNode = ObjLoader::load("Meshes/Cylinder.obj", materialFactory);
				renderableNode->setPosition(glm::vec3(-0.4, 0.3, 0.4));
				m_visSystem->addRenderableNode(renderableNode);
			}
		}

		// Volume
		{
			int width = 128;
			TexturePtr densityTexture = createDensityTexture(width);


			glm::vec3 boxSize(1, 1, 1);
			glm::vec3 volumeTextureSize(width);

			MaterialPtr material(new Material);

			// Main technique
			{
				ShaderProgramPtr shader = ShaderProgram::createShaderProgram(ShaderProgramConfig("Shaders/Volume/RaymarchedVolume.vert", "Shaders/Volume/RaymarchedVolume.frag"));
				
				
				TechniquePtr technique(new Technique(shader));
				{
					Vec3ShaderParameterPtr boxModelSizeParameter(new Vec3ShaderParameter("volumeSize_modelSpace", boxSize));
					technique->addCustomShaderParameter(boxModelSizeParameter);

					Vec3ShaderParameterPtr oneOnVolumeTextureSizeParameter(new Vec3ShaderParameter("oneOnVolumeTextureSize", 1.0f / volumeTextureSize));
					technique->addCustomShaderParameter(oneOnVolumeTextureSizeParameter);

					technique->setAlphaBlendingMode(AlphaBlendingMode_PreMultiplied);
					{
						TextureUnit unit(densityTexture, "albedoSampler");
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

					{
						TextureUnit unit(densityTexture, "albedoSampler");
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

				RenderableNodePtr renderableNode = RenderableNode::createWithSingleGeo(geo, glm::vec3(0, 0.5, 0));
				m_visSystem->addRenderableNode(renderableNode, RenderQueueId_OffscreenTransparentObjects);
			}
		}

		int volumeTargetWidth = m_window->getWidth() / 2;
		int volumeTargetHeight = m_window->getHeight() / 2;

		// Create offscreen transparent objects target
		TexturePtr offscreenTransparentObjectsTexture;
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
				RenderableNodePtr renderableNode = RenderableNode::createWithSingleGeo(geo);
				m_visSystem->addRenderableNode(renderableNode, RenderQueueId_CompositeOffscreenTransparentObjects);
			}
		}

		m_camera->setPosition(glm::vec3(2.2, 1.0, 2.2));
		m_cameraController->rotate(0.65, -0.2);
	}

	void simulate(float dt) {}

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

private:
	boost::scoped_ptr<ShadowProjector> m_shadowProjector;
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