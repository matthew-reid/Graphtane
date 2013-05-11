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

#include "Window.h"

#include <boost/foreach.hpp>
#include <stdexcept>

namespace GVis {

Window* gWindow = 0;

class InputEventHandler
{
public:
	InputEventHandler(Window* window) :
		m_window(window)
	{
	}

	static int eventMouseButtonGLFW(int glfwButton, int glfwAction)
	{
		BOOST_FOREACH(WindowListener* listener, gWindow->m_listeners)
		{
			listener->window_mouseButtonPressed(glfwButton, glfwAction == GLFW_PRESS);
		}
		return GL_TRUE;
	}

	static int eventMouseWheelGLFW(int wheelPos)
	{
		BOOST_FOREACH(WindowListener* listener, gWindow->m_listeners)
		{
			listener->window_mouseWheelScrolled((float)wheelPos);
		}
		return GL_TRUE;
	}

	static int eventKeyGLFW(int glfwKey, int glfwAction)
	{
		BOOST_FOREACH(WindowListener* listener, gWindow->m_listeners)
		{
			listener->window_keyPressed(glfwKey, glfwAction == GLFW_PRESS);
		}
		return GL_TRUE;
	}

	static int eventCharGLFW(int glfwChar, int glfwAction)
	{
		BOOST_FOREACH(WindowListener* listener, gWindow->m_listeners)
		{
			listener->window_charPressed(glfwChar, glfwAction == GLFW_PRESS);
		}
		return GL_TRUE;
	}

	static int eventCloseWindow(GLFWwindow*)
	{
		gWindow->m_closeRequested = true;
		return GL_TRUE;
	}

private:
	Window* m_window;
};


// ------------------------------------------------------------------

Window::Window(const WindowConfig& config) :
	m_width(config.width),
	m_height(config.height),
	m_mouseVisible(true),
	m_window(0),
	m_closeRequested(false)
{
	assert(!gWindow);
	gWindow = this;

	glfwWindowHint(GLFW_SAMPLES, config.antiAliasingSampleCount);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
 
	// Open a window and create its OpenGL context
	m_window = glfwCreateWindow(config.width, config.height, config.title.c_str(), 0, 0);
	if(!m_window)
	{
		glfwTerminate();
		std::runtime_error("Failed to open GLFW window");
	}

	glfwMakeContextCurrent(m_window);

	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

	if (glewInit() != GLEW_OK)
	{
		std::runtime_error("Failed to initialize GLEW");
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); 
	glEnable(GL_CULL_FACE);
	glEnable(GL_FRAMEBUFFER_SRGB);

	m_prevMousePos = getMousePosition();

	glfwSetMouseButtonCallback(m_window, (GLFWmousebuttonfun)InputEventHandler::eventMouseButtonGLFW);
	glfwSetScrollCallback(m_window, (GLFWscrollfun)InputEventHandler::eventMouseWheelGLFW);
	glfwSetKeyCallback(m_window, (GLFWkeyfun)InputEventHandler::eventKeyGLFW);
	glfwSetCharCallback(m_window, (GLFWcharfun)InputEventHandler::eventCharGLFW);
	glfwSetWindowCloseCallback(m_window, (GLFWwindowclosefun)InputEventHandler::eventCloseWindow);
}

Window::~Window()
{
	if (m_window)
	{
		glfwDestroyWindow(m_window);
	}
}

int Window::eventMouseButtonGLFW(int glfwButton, int glfwAction)
{
	return true;
}


bool Window::isCloseRequested()
{
	return (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS || m_closeRequested);
}



void Window::setMouseVisible(bool visible)
{
	m_mouseVisible = visible;
	if (m_mouseVisible)
	{
		glfwSetInputMode(m_window, GLFW_CURSOR_MODE, GLFW_CURSOR_NORMAL);
	}
	else
	{
		glfwSetInputMode(m_window, GLFW_CURSOR_MODE, GLFW_CURSOR_CAPTURED);
	}
}

void Window::updateInput()
{
	glm::vec2 pos = getMousePosition();
	m_mousePosDelta = pos - m_prevMousePos;

	if (m_mouseVisible)
	{
		m_prevMousePos = pos;
	}
	else
	{
		glfwSetCursorPos(m_window, m_width / 2, m_height / 2);
		m_prevMousePos = getMousePosition();
	}
}


glm::vec2 Window::getMousePosition() const
{
	int intX, intY;
	glfwGetCursorPos(m_window, &intX, &intY);

	glm::vec2 pos;

	pos.x = (float)intX / (float)m_width;
	pos.y = (float)intY / (float)m_height;
	return pos;
}

glm::vec2 Window::getMousePositionDelta() const
{
	return m_mousePosDelta;
}

bool Window::isMousePressed(int button)
{
	return (glfwGetMouseButton(m_window, button) == GLFW_PRESS);
}

bool Window::isKeyPressed(int asciiValue)
{
	return (glfwGetKey(m_window, asciiValue) == GLFW_PRESS);
}

void Window::_render(const VisSystem& system)
{
	RenderTarget::_render(system);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Window::setTitle(const std::string& title)
{
	glfwSetWindowTitle(m_window, title.c_str());
}

} // namespace GVis
