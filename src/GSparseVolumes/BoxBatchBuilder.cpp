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

#include "BoxBatchBuilder.h"

using namespace GSparseVolumes;
using namespace GVis;

BoxBatchBuilder::BoxBatchBuilder(int reserveBoxCount)
{
	m_positions.reserve(reserveBoxCount * 8);
	m_uvs.reserve(reserveBoxCount * 8);
	m_indices.reserve(reserveBoxCount * 12);
}

void BoxBatchBuilder::addBox(const glm::vec3& centerPosition, const glm::vec3& size)
{
	GLint i = (GLint)m_positions.size();
	const GLint indexBuffer[] = {
		i+1, i+3, i+2, // -Z
		i+1, i+0, i+3, // -Z
		i+5, i+1, i+2, // +X
		i+5, i+2, i+6, // +X
		i+4, i+5, i+6, // +Z
		i+4, i+6, i+7, // +Z
		i+0, i+4, i+7, // -X
		i+0, i+7, i+3, // -X
		i+6, i+2, i+3, // +Y
		i+6, i+3, i+7, // +Y
		i+5, i+4, i+0, // -Y
		i+5, i+0, i+1 // -Y
	};
	m_indices.insert(m_indices.end(), indexBuffer, indexBuffer+36);

	glm::vec3 halfSize = size * 0.5f;

	m_positions.push_back(glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z) + centerPosition);
	m_positions.push_back(glm::vec3(halfSize.x, -halfSize.y, -halfSize.z) + centerPosition);
	m_positions.push_back(glm::vec3(halfSize.x, halfSize.y, -halfSize.z) + centerPosition);
	m_positions.push_back(glm::vec3(-halfSize.x, halfSize.y, -halfSize.z) + centerPosition);
	m_positions.push_back(glm::vec3(-halfSize.x, -halfSize.y, halfSize.z) + centerPosition);
	m_positions.push_back(glm::vec3(halfSize.x, -halfSize.y, halfSize.z) + centerPosition);
	m_positions.push_back(glm::vec3(halfSize.x, halfSize.y, halfSize.z) + centerPosition);
	m_positions.push_back(glm::vec3(-halfSize.x, halfSize.y, halfSize.z) + centerPosition);

	m_uvs.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	m_uvs.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	m_uvs.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
	m_uvs.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	m_uvs.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	m_uvs.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
	m_uvs.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	m_uvs.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
}

// Returns null if mesh is empty
MeshPtr BoxBatchBuilder::build(BufferUsage usage) const
{
	MeshPtr mesh;
	
	if (!m_positions.empty())
	{
		mesh.reset(new Mesh);
		mesh->addVertexAttribute((GLfloat&)m_positions[0], (int)m_positions.size() * sizeof(glm::vec3), 3);
		mesh->addVertexAttribute((GLfloat&)m_uvs[0], (int)m_uvs.size() * sizeof(glm::vec3), 3);

		mesh->setIndexBuffer(m_indices[0], (int)m_indices.size()* sizeof(GLint), usage);
	}
	return mesh;
}
