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

#include "ShaderParameter.h"
#include "ShaderProgram.h"
#include <GCommon/Logger.h>

#include <GL/glew.h>

#include <assert.h>

namespace GVis {


ShaderParameter::ShaderParameter(const std::string& name) :
	m_name(name)
{
}

void ShaderParameter::apply(ShaderProgram& shader)
{
	GLuint uniformLocation = glGetUniformLocation(shader._getProgramId(), m_name.c_str());
	if (uniformLocation != -1)
	{
		apply(uniformLocation);
	}
}

void FloatShaderParameter::apply(GLuint uniformLocation)
{
	glUniform1fv(uniformLocation, 1, &m_value);
}

void IntShaderParameter::apply(GLuint uniformLocation)
{
	glUniform1iv(uniformLocation, 1, &m_value);
}

void Mat4ShaderParameter::apply(GLuint uniformLocation)
{
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &m_value[0][0]);
}

void Vec2ShaderParameter::apply(GLuint uniformLocation)
{
	glUniform2fv(uniformLocation, 1, &m_value[0]);
}

void Vec3ShaderParameter::apply(GLuint uniformLocation)
{
	glUniform3fv(uniformLocation, 1, &m_value[0]);
}

void IVec3ShaderParameter::apply(GLuint uniformLocation)
{
	glUniform3iv(uniformLocation, 1, &m_value[0]);
}

} // namespace GVis
