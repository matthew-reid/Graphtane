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

#include "RenderTarget.h"
#include "GVisFwd.h"
#include "Math.h"

#include <GCommon/Listenable.h>

#include <boost/scoped_ptr.hpp>
#include <string>

namespace GVis {

struct WindowConfig
{
	static WindowConfig createDefault()
	{
		WindowConfig c;
		c.width = 1280;
		c.height = 720;
		c.antiAliasingSampleCount = 8;
		c.title = "Window";
		return c;
	}

	int width;
	int height;
	int antiAliasingSampleCount;
	std::string title;
};

class WindowListener : public GCommon::Listener
{
public:
	virtual void window_mouseButtonPressed(int button, bool pressed) = 0;
	virtual void window_mouseWheelScrolled(float pos) = 0;
	virtual void window_keyPressed(int button, bool pressed) = 0;
	virtual void window_charPressed(int button, bool pressed) = 0;
};

class Window : public RenderTarget, public GCommon::Listenable<WindowListener>
{
	friend class InputEventHandler;
public:
	Window(const WindowConfig& config);
	~Window();

	bool isCloseRequested();

	void updateInput();

	void setMouseVisible(bool visible);

	glm::vec2 getMousePosition() const;
	glm::vec2 getMousePositionDelta() const;

	bool isMousePressed(int button);
	bool isKeyPressed(int asciiValue);

	int getWidth() const {return m_width;}
	int getHeight() const {return m_height;}

	virtual TechniqueCategory getTechniqueCategory() const {return TechniqueCategory_Main;}

	int eventMouseButtonGLFW(int glfwButton, int glfwAction);

	void setTitle(const std::string& title);

protected:
	// RenderTarget interface
	virtual void _render(const VisSystem& system);

private:
	GLFWwindow* m_window;
	int m_width;
	int m_height;
	glm::vec2 m_prevMousePos;
	glm::vec2 m_mousePosDelta;
	bool m_mouseVisible;
	bool m_closeRequested;
};

} // namespace GVis
