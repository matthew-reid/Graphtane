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

#include "ObjLoader.h"
#include "MeshData.h"
#include "MaterialFactory.h"
#include "MeshFactory.h"
#include "GVis/Geo.h"
#include "GVis/Material.h"
#include "GVis/RenderableNode.h"

#include <GCommon/Logger.h>

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <map>

namespace GVis {

template <typename T>
void copyElement(std::vector<T>& outVector, int outIndex, const std::vector<T>& inVector, int inIndex)
{
	if (inIndex >= (int)inVector.size())
		throw std::runtime_error("Invalid index specified");

	while (outIndex >= (int)outVector.size())
	{
		T defaultValue;
		outVector.push_back(defaultValue);
	}
	outVector[outIndex] = inVector[inIndex];
}

static std::string toVertexHash(int positionIndex, int uvIndex)
{
	return boost::lexical_cast<std::string>(positionIndex) + "_" + boost::lexical_cast<std::string>(uvIndex);
}

void ObjLoader::load(MeshData& outData, const std::string& filename)
{
	std::stringstream buffer;
	{
		std::ifstream file(filename);

		if(!file.is_open())
		{
			throw std::runtime_error("Could not load file: " + filename);
		}

		buffer << file.rdbuf();

		file.close();
	}

	std::vector<glm::vec3> tempPositions;
	std::vector<glm::vec2> tempUvs;
	std::vector<glm::vec3> tempNormals;

	typedef std::map<std::string, int> VertexMap; // maps vertex hash to vertex index
	VertexMap vertexMap;

	static const int lineBufferSize = 128;
	char lineBuffer[lineBufferSize];

	int faceVertexCount = 0;

	while(!buffer.eof())
	{
		// read the first word of the line
		std::string lineHeader;
		buffer >> lineHeader;

		if (lineHeader == "v" )
		{
			glm::vec3 position;
			buffer >> position.x;
			buffer >> position.y;
			buffer >> position.z;
			tempPositions.push_back(position);
		}
		else if (lineHeader == "vt")
		{
			glm::vec2 uv;
			buffer >> uv.x;
			buffer >> uv.y;
			uv.y = 1.0 - uv.y; // Invert V coordinate since we will only use DDS textures, which are inverted. Remove if you want to use TGA or BMP loaders.
			tempUvs.push_back(uv);
		}
		else if (lineHeader == "vn")
		{
			glm::vec3 normal;
			buffer >> normal.x;
			buffer >> normal.y;
			buffer >> normal.z;
			tempNormals.push_back(normal);
		}
		else if (lineHeader == "f")
		{
			buffer.getline(lineBuffer, lineBufferSize);
			int tempFaceVertexCount = 0;
			for (int i = 0; i < lineBufferSize; i++)
			{
				char& val = lineBuffer[i];
				if (val == 0)
				{
					break;
				}
				if (val == '/')
				{
					val = ' ';
				}
				else if (val == ' ')
				{
					++tempFaceVertexCount;
				}
			}

			if (faceVertexCount == 0)
			{
				faceVertexCount = tempFaceVertexCount;
			}
			else if (faceVertexCount != tempFaceVertexCount)
			{
				throw std::runtime_error("Faces have different number of vertices in " + filename);
			}

			std::stringstream ss(std::string(lineBuffer+1, lineBufferSize-1));

			unsigned int positionIndex[4], uvIndex[4], normalIndex[4];

			for (int i = 0; i < faceVertexCount; i++)
			{
				ss >> positionIndex[i];
				ss >> uvIndex[i];
				ss >> normalIndex[i];

				// Convert from 1 based to 0 based numbering
				--positionIndex[i];
				--uvIndex[i];
				--normalIndex[i];
			}

			if (ss.fail())
			{
				throw std::runtime_error("Can't read file format of " + filename);
			}

			for (int i = 0; i < faceVertexCount; i++)
			{
				int index = (int)outData.positions.size();

				std::string hash = toVertexHash(positionIndex[i], uvIndex[i]);
				std::pair<VertexMap::iterator, bool> p = vertexMap.insert(VertexMap::value_type(hash, index));
				
				if (p.second) // inserted a new vertex
				{
					copyElement<glm::vec3>(outData.positions, index, tempPositions, positionIndex[i]);
					copyElement<glm::vec2>(outData.uvs, index, tempUvs, uvIndex[i]);
					copyElement<glm::vec3>(outData.normals, index, tempNormals, normalIndex[i]);
				}
				else // use the existing vertex
				{
					index = p.first->second;
				}

				outData.indices.push_back(index);
			}
		}
	}

	switch(faceVertexCount)
	{
	case 3:
		outData.type = MeshData::Type_Triangles;
		break;
	case 4:
		outData.type = MeshData::Type_Quads;
		break;
	default:
		throw std::runtime_error("Can't handle faces with " + boost::lexical_cast<std::string>(faceVertexCount) + " vertices in " + filename);
	}
}

RenderableNodePtr ObjLoader::load(const std::string& filename, MaterialFactory& materialFactory)
{
	MeshData data;
	load(data, filename);
	MeshPtr mesh = MeshFactory::createMesh(data);
	
	MaterialPtr material = materialFactory.createMaterial(TextureRoles());
	GeoPtr geo(new Geo(mesh, material));

	return RenderableNode::createWithSingleGeo(geo);
}

} // namespace GVis
