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

#include "MeshFactory.h"
#include "MeshData.h"
#include "ObjLoader.h"
#include <GCommon/Logger.h>

#include <stdexcept>

using namespace GVis;

namespace GVis {

MeshPtr MeshFactory::createMesh(const MeshData& data, PrimitiveType type, int patchVertCount)
{
	if (!data.indices.empty() &&
		!data.positions.empty() &&
		!data.uvs.empty() &&
		!data.normals.empty())
	{
		MeshPtr mesh(new Mesh(type, patchVertCount));
		mesh->setIndexBuffer(data.indices[0], data.indices.size() * sizeof(GLint));
		mesh->addVertexAttribute((const GLfloat&)data.positions[0], data.positions.size() * 3 * sizeof(GLfloat), 3);
		mesh->addVertexAttribute((const GLfloat&)data.uvs[0], data.uvs.size() * 2 * sizeof(GLfloat), 2);
		mesh->addVertexAttribute((const GLfloat&)data.normals[0], data.normals.size() * 3 * sizeof(GLfloat), 3);

		return mesh;
	}

	throw std::runtime_error("Mesh contains an empty buffer");
	return MeshPtr();
}

} // namespace GVis
