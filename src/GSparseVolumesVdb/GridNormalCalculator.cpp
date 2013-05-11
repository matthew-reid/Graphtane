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

#include "GridNormalCalculator.h"

#include <openvdb/math/Operators.h>
#include <openvdb/tools/Filter.h>

namespace GSparseVolumes {

// Use a custom gradient function instead of openvdb::tools::Gradient so that we can operate on all values, not use 'On' values.
// Need to operate on all values because zero density values from which a meaningful normal can be calculated may be represented as 'Off' values near the isosurface.
class GridNormalCalculator
{
public:
	GridNormalCalculator(Vec3UByteGrid& outputGrid, const openvdb::FloatGrid& inputGrid) :
		m_outputGrid(outputGrid),
		m_inputGrid(inputGrid)
	{
	}

    template<typename MapT>
    void operator()(const MapT& map)
	{
		// Iterate over grid and calculate normals
		const openvdb::FloatGrid::ConstAccessor& accessor = m_inputGrid.getConstAccessor();
		const openvdb::math::Transform& transform = m_inputGrid.transform();

		for (Vec3UByteGrid::ValueAllIter iter = m_outputGrid.beginValueAll(); iter.test(); ++iter) {
			openvdb::Vec3f value = openvdb::math::Gradient<MapT, openvdb::math::CD_2ND>::result(map, accessor, iter.getCoord());
			if (value.dot(value) > 0.0001f)
			{
				float length = value.length();
				value /= std::max(0.01f, length);
				value = value * 0.5f + 0.5f;

				Vec3UByte byteValue(value.x() * 255, value.y() * 255, value.z() * 255);
				iter.setValue(byteValue);
			}
		}
	}

private:
	const openvdb::FloatGrid& m_inputGrid;
	Vec3UByteGrid& m_outputGrid;
};

Vec3UByteGrid::Ptr createNormalGrid(const openvdb::FloatGrid& inputGrid)
{
	// Copy input grid
	Vec3UByte background(127,127,127);
	Vec3UByteGrid::TreePtrType tree(new Vec3UByteGrid::TreeType(inputGrid.tree(), background, openvdb::TopologyCopy()));
    Vec3UByteGrid::Ptr normalGrid(new Vec3UByteGrid(tree));
	normalGrid->setTransform(const_cast<openvdb::FloatGrid&>(inputGrid).transformPtr());

	openvdb::math::processTypedMap<openvdb::math::Transform, GridNormalCalculator>(const_cast<openvdb::FloatGrid&>(inputGrid).transform(), GridNormalCalculator(*normalGrid, inputGrid));
	
	return normalGrid;
}

} // namespace GSparseVolumes