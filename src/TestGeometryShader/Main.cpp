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

#include <GCommon/Logger.h>

#include <GVis/Geo.h>
#include <GVis/Light.h>
#include <GVis/Material.h>
#include <GVis/Primitives/BoxMeshFactory.h>
#include <GVis/RenderableNode.h>
#include <GVis/ShaderProgram.h>
#include <GVis/VisSystem.h>

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

		ShaderProgramConfig config("Shaders/Common/PassThrough.vert", "Shaders/Common/SimpleColor.frag");
		config.shaderFilepaths[ShaderType_Geometry] = "Shaders/DuplicateMirrored.geom";
		ShaderProgramPtr shader = ShaderProgram::createShaderProgram(config);

		MeshPtr mesh = BoxMeshFactory::createMesh(BoxConfig::defaultWithSize(glm::vec3(1, 1, 1)));
		GeoPtr geo(new Geo(mesh, Material::createWithSingleTechnique(shader)));

		RenderableNodePtr node = RenderableNode::createWithSingleGeo(geo, glm::vec3(-1,0,0));
		m_visSystem->addRenderableNode(node);
	}

	void simulate(float dt) {}
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