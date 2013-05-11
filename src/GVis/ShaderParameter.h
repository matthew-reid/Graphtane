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

#include "GVisFwd.h"
#include "Math.h"

#include <string>

namespace GVis {

class ShaderParameter
{
	friend class Technique;

public:
	ShaderParameter(const std::string& name);
	virtual ~ShaderParameter() {};

private:
	virtual void apply(ShaderProgram& shader);
	virtual void apply(GLuint uniformLocation) = 0;

protected:
	std::string m_name;
};

template <typename T>
class ShaderParameterT : public ShaderParameter
{
public:
	ShaderParameterT(const std::string& name, const T& value) :
		ShaderParameter(name),
		m_value(value)
	{
	}

	virtual ~ShaderParameterT() {}

	const T& getValue() const {return m_value;}

	void setValue(const T& value)
	{
		m_value = value;
	}

protected:
	T m_value;
};

class FloatShaderParameter : public ShaderParameterT<float>
{
public:
	FloatShaderParameter(const std::string& name, float value) : ShaderParameterT(name, value) {}

protected:
	void apply(GLuint uniformLocation);
};

class IntShaderParameter : public ShaderParameterT<int>
{
public:
	IntShaderParameter(const std::string& name, int value) : ShaderParameterT(name, value) {}

protected:
	void apply(GLuint uniformLocation);
};

class Mat4ShaderParameter : public ShaderParameterT<glm::mat4>
{
public:
	Mat4ShaderParameter(const std::string& name, const glm::mat4& value) : ShaderParameterT(name, value) {}

protected:
	void apply(GLuint uniformLocation);
};

class Vec2ShaderParameter : public ShaderParameterT<glm::vec2>
{
public:
	Vec2ShaderParameter(const std::string& name, const glm::vec2& value) : ShaderParameterT(name, value) {}

protected:
	void apply(GLuint uniformLocation);
};

class Vec3ShaderParameter : public ShaderParameterT<glm::vec3>
{
public:
	Vec3ShaderParameter(const std::string& name, const glm::vec3& value) : ShaderParameterT(name, value) {}

protected:
	void apply(GLuint uniformLocation);
};

class IVec3ShaderParameter : public ShaderParameterT<glm::ivec3>
{
public:
	IVec3ShaderParameter(const std::string& name, const glm::ivec3& value) : ShaderParameterT(name, value) {}

protected:
	void apply(GLuint uniformLocation);
};

} // namespace GVis
