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

#include "ShaderProgram.h"
#include "ShaderParameter.h"
#include "Texture.h"
#include "Convert.h"

#include <GCommon/FileFinder.h>
#include <GCommon/Logger.h>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace GCommon;

namespace GVis {

using namespace std;

static void loadFile(std::string& outString, const std::string& filepath)
{
	std::ifstream stream(filepath, std::ios::in);
	if(stream.is_open())
	{
		std::string Line = "";
		while(getline(stream, Line))
			outString += "\n" + Line;
		stream.close();
	}
	else
	{
		throw std::runtime_error("Could not open file: " + filepath);
	}
}

static std::string preprocessIncludes(const std::string& source, const std::string& filename, int level = 0)
{
	if(level > 32)
		throw std::runtime_error("header inclusion depth limit reached, might be caused by cyclic header inclusion");
	using namespace std;
 
	static const boost::regex re("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
	stringstream input;
	stringstream output;
	input << source;
 
	size_t line_number = 1;
	boost::smatch matches;
 
	string line;
	while(std::getline(input,line))
	{
		if (boost::regex_search(line, matches, re))
		{
			std::string include_file = matches[1];
			std::string include_string;
 
			std::string includeFilePath = boost::filesystem::path(filename).parent_path().string() + "/" + include_file;

			loadFile(include_string, includeFilePath);
			output << preprocessIncludes(include_string, includeFilePath, level + 1) << endl;
		}
		else
		{
			output <<  line << endl;
		}
		++line_number;
	}
	return output.str();
}

static void compileShaderCode(GLuint shaderId, const std::string& shaderCode)
{
	// Compile shader
	char const * sourcePointer = shaderCode.c_str();
	glShaderSource(shaderId, 1, &sourcePointer , NULL);
	glCompileShader(shaderId);

	// Check shader
	GLint result = GL_FALSE;
	int infoLogLength;

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		std::vector<char> errorMessage(infoLogLength);
		glGetShaderInfoLog(shaderId, infoLogLength, NULL, &errorMessage[0]);
		defaultLogger()->logLine(std::string(&errorMessage[0]));
	}
}

static std::string defineMacrosInGlsl(const std::vector<std::string>& macroDefinitions)
{
	std::string result;
	BOOST_FOREACH(const std::string& macro, macroDefinitions)
	{
		result += "#define " + macro + "\n";
	}
	return result;
}

static void compileShaderFromFile(GLuint shaderId, const std::string& shaderFilepath, const std::vector<std::string>& macroDefinitions)
{
	defaultLogger()->logLine("Compiling shader: " + shaderFilepath);

	std::string shaderCode;
	loadFile(shaderCode, shaderFilepath);
	shaderCode = preprocessIncludes(shaderCode, shaderFilepath);

	shaderCode = defineMacrosInGlsl(macroDefinitions) + shaderCode;

	compileShaderCode(shaderId, shaderCode);
}

ShaderProgramPtr ShaderProgram::createShaderProgram(const ShaderProgramConfig& config)
{
	// Create the shaders
	std::vector<GLuint> shaderIds;
	BOOST_FOREACH(const ShaderProgramConfig::ShaderFilepaths::value_type& value, config.shaderFilepaths)
	{
		GLuint shaderId = glCreateShader(toGlShaderType(value.first));
		compileShaderFromFile(shaderId, value.second, config.macroDefinitions);
		shaderIds.push_back(shaderId);
	}

	// Link the program
	defaultLogger()->logLine("Linking program");
	GLuint programId = glCreateProgram();

	BOOST_FOREACH(GLuint shaderId, shaderIds)
	{
		glAttachShader(programId, shaderId);
	}

	glLinkProgram(programId);

	// Check the program
	GLint result = GL_FALSE;
	int infoLogLength;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> ProgramErrorMessage( max(infoLogLength, int(1)) );
	glGetProgramInfoLog(programId, infoLogLength, NULL, &ProgramErrorMessage[0]);
	defaultLogger()->logLine(std::string(&ProgramErrorMessage[0]));

	// Clean up
	BOOST_FOREACH(GLuint shaderId, shaderIds)
	{
		glDeleteShader(shaderId);
	}

	return ShaderProgramPtr(new ShaderProgram(programId));
}

ShaderProgram::ShaderProgram(GLuint programId) :
	m_programId(programId)
{
}

} // namespace GVis
