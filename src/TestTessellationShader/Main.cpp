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

#include <GCommon/Logger.h>

#include <GVis/VisSystem.h>
#include <GVis/Primitives/RectangleMeshFactory.h>
#include <GVis/Material.h>
#include <GVis/ShaderParameter.h>
#include <GVis/Geo.h>
#include <GVis/RenderableNode.h>
#include <GVis/ShaderProgram.h>
#include <GVis/Texture.h>
#include <GVis/Light.h>
#include <GVis/Technique.h>
#include <GVis/Camera.h>

#include <exception>

#include <boost/scoped_array.hpp>

using namespace GAppFramework;
using namespace GCommon;
using namespace GVis;

class TestApplication : public Application
{
public:
	TestApplication(const ApplicationConfig& config) :
		Application(config)
	{
	}

	void setupScene()
	{
		LightPtr light(new Light);
		light->setDirection(glm::normalize(glm::vec3(2, -1, -1)));
		m_visSystem->addLight(light);

		m_camera->getProjection()->setNearClipDistance(0.5);
		m_camera->getProjection()->setFarClipDistance(1000);

		// Create heightmap
		TexturePtr texture;
		{
			ImageTextureConfig config = ImageTextureConfig::createDefault();
			config.format = PixelFormat_R16;
			config.width = config.height = 512;
			
			boost::scoped_array<char16_t> buffer(new char16_t[config.width * config.height]);
			
			for (int y = 0; y < config.height; ++y)
			{
				for (int x = 0; x < config.width; ++x)
				{
					// interesting ripple pattern
					glm::vec2 r = glm::vec2((float)x / (float)config.width, (float)y / (float)config.height) * 2.0f - 1.0f;
					buffer[x + config.width * y] = 32768 + 32767 * std::sin(glm::length(r) * 30.0f);
				}
			}

			config.data = (unsigned char*)buffer.get();

			texture.reset(new Texture(config));
			texture->setTextureAddressMode(TextureAddressMode_Clamp);
		}

		// Create tessellated plane
		{
			ShaderProgramConfig config("Shaders/TessDisplacement/TessDisplacement.vert", "Shaders/Common/SimpleTextured.frag");
			config.shaderFilepaths[ShaderType_TessellationControl] = "Shaders/TessDisplacement/TessDisplacement.tctrl";
			config.shaderFilepaths[ShaderType_TessellationEvaluation] = "Shaders/TessDisplacement/TessDisplacement.teval";

			ShaderProgramPtr shader = ShaderProgram::createShaderProgram(config);
			TechniquePtr technique(new Technique(shader));
			technique->addTextureUnit(TextureUnit(texture, "heightSampler"));
			technique->addTextureUnit(TextureUnit(texture, "albedoSampler"));
			technique->addCustomShaderParameter(ShaderParameterPtr(new FloatShaderParameter("heightScale", 0.2f)));

			MaterialPtr material(new Material);
			material->setTechnique(technique);

			{
				RectangleConfig config = RectangleConfig::defaultWithSize(glm::vec2(10, 10));
				config.quads = true;
				config.segmentCountX = 32;
				config.segmentCountY = 32;
				config.orientation = glm::angleAxis(halfPi(), glm::vec3(-1, 0, 0));
				config.flipV = true;
				MeshPtr mesh = RectangleMeshFactory::createMesh(config);
				GeoPtr geo(new Geo(mesh, material));

				RenderableNodePtr node = RenderableNode::createWithSingleGeo(geo);
				node->setWireframeModeEnabled(true);
				m_visSystem->addRenderableNode(node);
			}
		}

		m_camera->setPosition(glm::vec3(0, 4, 6));
		m_cameraController->rotate(0, -0.3);
	}

	void simulate(float dt)	{}
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