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

#include "RectangleMeshFactory.h"
#include "../Mesh.h"

namespace GVis {

MeshPtr RectangleMeshFactory::createMesh(const RectangleConfig& c)
{
	glm::vec2 halfSize = c.size * 0.5f;

	// vertices
	std::vector<glm::vec3> posBuffer;
	std::vector<glm::vec2> uvBuffer;
	std::vector<glm::vec3> normalBuffer;

	int vertexRowCount = c.segmentCountX + 1;
	int vertexColumnCount = c.segmentCountY + 1;
	int vertexCount = vertexRowCount * vertexColumnCount;
	posBuffer.reserve(vertexCount);
	uvBuffer.reserve(vertexCount);

	for (int y = 0; y < vertexColumnCount; ++y)
	{
		float sy = (float)y / (float)c.segmentCountY;
		for (int x = 0; x < vertexRowCount; ++x)
		{
			float sx = (float)x / (float)c.segmentCountX;

			glm::vec3 position(-halfSize.x + sx * c.size.x, -halfSize.y + sy * c.size.y, 0.0f);
			position = c.orientation * position;
			posBuffer.push_back(position);

			normalBuffer.push_back(c.orientation * glm::vec3(0,0,1));

			glm::vec2 uv(sx, sy);

			if (c.flipV)
				uv.y = 1.0f - uv.y;

			uvBuffer.push_back(uv);
		}
	}

	// indices
	std::vector<GLint> indexBuffer;

	int indexCount = c.quads ? (c.segmentCountX * c.segmentCountY * 4) : (c.segmentCountX * c.segmentCountY * 6);
	indexBuffer.reserve(indexCount);

	int i = 0;
	for (int y = 0; y < c.segmentCountY; ++y)
	{
		for (int x = 0; x < c.segmentCountX; ++x)
		{
			if (c.quads)
			{
				indexBuffer.push_back(i);
				indexBuffer.push_back(i + 1);
				indexBuffer.push_back(i + 1 + vertexRowCount);
				indexBuffer.push_back(i + vertexRowCount);
			}
			else
			{
				indexBuffer.push_back(i);
				indexBuffer.push_back(i + 1);
				indexBuffer.push_back(i + 1 + vertexRowCount);

				indexBuffer.push_back(i);
				indexBuffer.push_back(i + 1 + vertexRowCount);
				indexBuffer.push_back(i + vertexRowCount);
			}

			++i;
		}
		++i;
	}
	
	PrimitiveType primitiveType = c.quads ? PrimitiveType_Patches : PrimitiveType_Triangles;

	MeshPtr mesh(new Mesh(primitiveType, 4));

	mesh->addVertexAttribute((float&)posBuffer[0], posBuffer.size() * sizeof(glm::vec3), 3);
	mesh->addVertexAttribute((float&)uvBuffer[0], uvBuffer.size() * sizeof(glm::vec2), 2);
	mesh->addVertexAttribute((float&)normalBuffer[0], normalBuffer.size() * sizeof(glm::vec3), 3);

	mesh->setIndexBuffer(indexBuffer[0], indexBuffer.size() * sizeof(GLint));

	return mesh;
}


} // namespace GVis
