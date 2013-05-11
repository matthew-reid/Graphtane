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
#include <GComputeVis/Convert.h>

#include <GCommon/Logger.h>
#include <GCompute/ClSystem.h>

#include <GFluid/FluidSolver.h>
#include <GFluid/TempBufferPool.h>

#include <GVis/ShaderProgram.h>
#include <GVis/Texture.h>
#include <GVis/VisSystem.h>
#include <GVis/RenderableNode.h>

#include <exception>

#include <boost/scoped_array.hpp>

using namespace GAppFramework;
using namespace GCommon;
using namespace GCompute;
using namespace GComputeVis;
using namespace GFluid;
using namespace GVis;

/*!
	2D fluid sim demo. Left click to add fluid.
*/
class TestApplication : public Application
{
public:
	TestApplication(const ApplicationConfig& config) :
		Application(config)
	{
		m_window->setMouseVisible(true);

		setSimulationTimeStep(0.02f);
	}

	~TestApplication()
	{
		m_solver.reset();
	}

	void setupScene()
	{
		m_clSystem.reset(new ClSystem); // Must create ClSystem before OpenGL textures

		m_densityTextureConfig = ImageTextureConfig::createDefault();
		m_densityTextureConfig.width = m_densityTextureConfig.height = 512;
		m_densityTextureConfig.depth = 1;
		m_densityTextureConfig.is3d = true;
		m_densityTextureConfig.format = PixelFormat_R8;
		m_densityTextureConfig.textureAddressMode = TextureAddressMode_Clamp;
		TexturePtr densityTexture(new Texture(m_densityTextureConfig));
		
		ShaderProgramPtr shader = ShaderProgram::createShaderProgram(ShaderProgramConfig("Shaders/Common/ScreenQuad.vert", "Shaders/Volume/VolumeTextureSlice.frag"));
		RenderableNodePtr node = m_renderableFactory->createScreenQuad(shader, densityTexture);
		m_visSystem->addRenderableNode(node);

		int tempBufferElementCount = m_densityTextureConfig.width * m_densityTextureConfig.height;
		m_solver = createFluidSolver(*m_clSystem, getGlTexture(densityTexture),
						TempBufferPoolPtr(new TempBufferPool(*m_clSystem, tempBufferElementCount)), "Kernels/Fluid");

		m_solver->setOutputWriteGammaPower(1.0 / 2.2); // gives better colour precision when writing to 8 bit
	}

	void simulate(float dt)
	{ 
		glm::vec2 mousePos = m_window->getMousePosition();
		glm::vec2 mousePosInTexCoords(mousePos.x * (float)m_densityTextureConfig.width, (1.0f-mousePos.y) * (float)m_densityTextureConfig.height);

		if (m_window->isMousePressed(0))
		{
			float density = 2 * dt;
			float temperature = 10;
			m_solver->setFluid(Float3(mousePosInTexCoords.x, mousePosInTexCoords.y, 0), density, temperature);
		}

		m_solver->update(dt);
	}

private:
	ClSystemPtr m_clSystem;
	FluidSolverPtr m_solver;
	ImageTextureConfig m_densityTextureConfig;

	float mPrevMouseX;
	float mPrevMouseY;
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