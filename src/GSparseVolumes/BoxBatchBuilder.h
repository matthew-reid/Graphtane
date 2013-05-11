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

#include <GVis/BufferUsage.h>
#include <GVis/Mesh.h>
#include <GVis/Math.h>

namespace GSparseVolumes {

class BoxBatchBuilder
{
public:
	explicit BoxBatchBuilder(int reserveBoxCount = 0);

	void addBox(const glm::vec3& centerPosition, const glm::vec3& size);

	// Returns null if mesh is empty
	GVis::MeshPtr build(GVis::BufferUsage usage = GVis::BufferUsage_Static) const;

	const std::vector<GLint>& getIndices() const {return m_indices;}

private:
	std::vector<glm::vec3> m_positions;
	std::vector<glm::vec3> m_uvs;
	std::vector<GLint> m_indices;
};

} // namespace GSparseVolumes