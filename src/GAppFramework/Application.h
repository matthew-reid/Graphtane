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

#include "GAppFrameworkFwd.h"
#include "RenderableFactory.h"
#include <GCommon/GCommonFwd.h>
#include <GVis/GVisFwd.h>
#include <GVis/Window.h>

#include <boost/scoped_ptr.hpp>
#include <memory>
#include <boost/optional.hpp>

namespace GAppFramework {

struct ApplicationConfig
{
	ApplicationConfig()
	{
		windowConfig = GVis::WindowConfig::createDefault();
	}

	static ApplicationConfig fromArgs(int argc, char** argv);

	GVis::WindowConfig windowConfig;
};

class Application
{
public:
	Application(const ApplicationConfig& config);
	~Application();

	void run();

	void setCameraInputEnabled(bool enabled) {m_cameraInputEnabled = enabled;}

	//! If no timeStep is set, simulation will update at frame rate
	void setSimulationTimeStep(boost::optional<float> timeStep) {m_simTimeStep = timeStep;}

protected:
	virtual GVis::CameraPtr createCamera();
	virtual void setupScene() = 0;
	virtual void simulate(float dt) = 0;
	virtual void moveCamera(float dt);
	virtual void render();

	double getElapsedTime() const {return m_elapsedTime;}

private:
	void viewport_preRender() {};
	void viewport_postRender();

	void displayFramerate(double timeSinceLastFrame);

protected:
	GVis::VisSystemPtr m_visSystem;
	GVis::WindowPtr m_window;
	GVis::ViewportPtr m_viewport;
	boost::scoped_ptr<class CameraController> m_cameraController;
	boost::scoped_ptr<class RenderableFactory> m_renderableFactory; // provided for convenience
	GVis::CameraPtr m_camera;
	bool m_cameraInputEnabled;

	double m_elapsedTime;
	boost::optional<float> m_simTimeStep;
	boost::scoped_ptr<GCommon::RollingMean> m_meanTimeSinceLastUpdate;
};

} // namespace GAppFramework
