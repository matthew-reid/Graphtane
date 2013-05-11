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

#include "BoxMeshFactory.h"

#include "../Mesh.h"

namespace GVis {

MeshPtr BoxMeshFactory::createMesh(const BoxConfig& c)
{
	glm::vec3 halfSize = c.size * 0.5f;

	const GLfloat posBufferData[] = {
		-halfSize.x, -halfSize.y, -halfSize.z,
		halfSize.x, -halfSize.y, -halfSize.z,
		halfSize.x, halfSize.y, -halfSize.z,
		-halfSize.x, halfSize.y, -halfSize.z,
		-halfSize.x, -halfSize.y, halfSize.z,
		halfSize.x, -halfSize.y, halfSize.z,
		halfSize.x, halfSize.y, halfSize.z,
		-halfSize.x, halfSize.y, halfSize.z
	};

	const GLfloat uvBufferData[] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
	};

	const GLint indexBufferData[] = {
		1, 3, 2, // -Z
		1, 0, 3, // -Z
		5, 1, 2, // +X
		5, 2, 6, // +X
		4, 5, 6, // +Z
		4, 6, 7, // +Z
		0, 4, 7, // -X
		0, 7, 3, // -X
		6, 2, 3, // +Y
		6, 3, 7, // +Y
		5, 4, 0, // -Y
		5, 0, 1 // -Y
	};

	MeshPtr mesh(new Mesh);
	
	mesh->addVertexAttribute(*posBufferData, sizeof(posBufferData), 3);
	mesh->addVertexAttribute(*uvBufferData, sizeof(uvBufferData), 3);

	mesh->setIndexBuffer(*indexBufferData, sizeof(indexBufferData));

	return mesh;
}


} // namespace GVis
