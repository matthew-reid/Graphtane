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

#include "Application.h"
#include "CameraController.h"
#include "RenderableFactory.h"

#include <GVis/VisSystem.h>
#include <GVis/Camera.h>
#include <GVis/Viewport.h>
#include <GVis/Window.h>
#include <GVis/Math.h>
#include <GCommon/RollingMean.h>

#include <boost/algorithm/string.hpp>
#include <boost/date_time.hpp>
#include <boost/program_options.hpp>
#include <assert.h>

using namespace GCommon;
using namespace GVis;
using namespace boost::posix_time;
using namespace boost;

namespace GAppFramework {

ApplicationConfig ApplicationConfig::fromArgs(int argc, char** argv)
{
	// read command line options
	program_options::options_description description("Application");
	description.add_options()
	("help,h", "displays help");

	program_options::variables_map vm;
	program_options::store(program_options::command_line_parser(argc, argv).options(description).run(), vm);
	program_options::notify(vm);

	// create config
	ApplicationConfig config;
	if ( vm.count("help")  ) 
	{ 
		std::cout << description << std::endl; 
	} 

	return config;
}

Application::Application(const ApplicationConfig& config) :
	m_elapsedTime(0),
	m_simTimeStep(1.0f / 30.0f),
	m_cameraInputEnabled(true),
	m_meanTimeSinceLastUpdate(new RollingMean(40))
{
	m_visSystem.reset(new VisSystem);

	m_window.reset(new Window(config.windowConfig));
	m_window->setMouseVisible(false);

	ViewportConfig viewportConfig = ViewportConfig::createDefault();
	viewportConfig.width = m_window->getWidth();
	viewportConfig.height = m_window->getHeight();
	m_viewport.reset(new Viewport(viewportConfig));
	m_window->addViewport(m_viewport);

	m_visSystem->setRootRenderTarget(m_window);

	m_viewport->setBackgroundColor(glm::vec4(0,0.1,0.3, 1.0));

	m_renderableFactory.reset(new RenderableFactory);
}

Application::~Application()
{
}

void Application::run()
{
	m_camera = createCamera();
	assert(m_camera);
	m_viewport->setCamera(m_camera);
	m_cameraController.reset(new CameraController(m_camera));

	setupScene();

	ptime previousTime = microsec_clock::local_time();
	float accumulatedSimTimeDebt = 0;
	while(!m_window->isCloseRequested())
	{
		ptime currentTime = boost::posix_time::microsec_clock::local_time();
		time_duration duration = currentTime - previousTime;
		previousTime = currentTime;
		float timeSinceLastUpdate = float(duration.total_milliseconds()) / 1000.0f;
		m_elapsedTime += timeSinceLastUpdate;
		accumulatedSimTimeDebt += timeSinceLastUpdate;
		accumulatedSimTimeDebt = std::min(1.0f / 15.0f, accumulatedSimTimeDebt); // maintain minimum frame rate

		if (m_simTimeStep)
		{
			while (accumulatedSimTimeDebt > *m_simTimeStep)
			{
				accumulatedSimTimeDebt -= *m_simTimeStep;
				simulate(*m_simTimeStep);
			}
		}
		else
		{
			simulate(timeSinceLastUpdate);
		}

		m_window->updateInput();
		moveCamera(timeSinceLastUpdate);
		render();

		m_meanTimeSinceLastUpdate->addValue(timeSinceLastUpdate);
		displayFramerate(m_meanTimeSinceLastUpdate->getMean());
	}
}

CameraPtr Application::createCamera()
{
	PerspectiveProjectionPtr projection(new PerspectiveProjection);
	projection->setAspectRatio((float)m_window->getWidth() / (float)m_window->getHeight());

	CameraPtr camera(new Camera);
	camera->setPosition(glm::vec3(0, 0, 5));
	camera->setProjection(projection);
	return camera;
}

void Application::moveCamera(float dt)
{
	if (m_cameraController && m_cameraInputEnabled)
	{
		static const float moveSpeed = 2;
		static const float rotationSpeed = 4;

		glm::vec3 dir;
		dir.x = (float)((int)m_window->isKeyPressed('D') - (int)m_window->isKeyPressed('A'));
		dir.y = 0;
		dir.z = (float)((int)m_window->isKeyPressed('S') - (int)m_window->isKeyPressed('W'));

		glm::vec2 mouseDelta = m_window->getMousePositionDelta();
		m_cameraController->translate(dir * moveSpeed * dt);
		m_cameraController->rotate(-mouseDelta.x * rotationSpeed, -mouseDelta.y * rotationSpeed);
	}
}

void Application::render()
{
	m_visSystem->render();
}

void Application::displayFramerate(double timeSinceLastFrame)
{
	std::stringstream ss;
    ss << std::fixed;
	ss << "FPS: "  << std::setprecision(0) << (1.0f / timeSinceLastFrame);
	ss << "  Frame duration: " << std::setprecision(4) << timeSinceLastFrame;

	m_window->setTitle(ss.str());
}

} // namespace GAppFramework
