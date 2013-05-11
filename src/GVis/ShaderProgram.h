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

#include <GCommon/GCommonFwd.h>

#include <string>
#include <map>
#include <vector>

namespace GVis {


enum ShaderType
{
	ShaderType_Vertex,
	ShaderType_Fragment,
	ShaderType_Geometry,
	ShaderType_TessellationControl,
	ShaderType_TessellationEvaluation,
};

struct ShaderProgramConfig
{
	ShaderProgramConfig() {}

	ShaderProgramConfig(const std::string& vertexFilepath, const std::string& fragmentFilepath)
	{
		shaderFilepaths[ShaderType_Vertex] = vertexFilepath;
		shaderFilepaths[ShaderType_Fragment] = fragmentFilepath;
	}

	//! Paths to shaders. Vertex and fragment are required, the rest are optional.
	typedef std::map<ShaderType, std::string> ShaderFilepaths;
	ShaderFilepaths shaderFilepaths;
	std::vector<std::string> macroDefinitions;
};


class ShaderProgram
{
public:
	static ShaderProgramPtr createShaderProgram(const ShaderProgramConfig& config);

	GLuint _getProgramId() const {return m_programId;}

protected:
	ShaderProgram(GLuint programId);

private:
	GLuint m_programId;
};

} // namespace GVis
