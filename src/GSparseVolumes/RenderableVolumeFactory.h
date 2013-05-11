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

#include "GSparseVolumesFwd.h"
#include <GVis/GVisFwd.h>
#include <GVis/Math.h>

#include <map>

namespace GSparseVolumes {

typedef std::map<GridPtr, GVis::TexturePtr> GridTexturesMap;

struct SparseVolumeMaterialConfig
{
	glm::vec3 boxSize;

	//! Texture atlas for each grid
	GridTexturesMap leafAtlases;

	//! 1D texture which maps {internalNodeIndex * maxLeafCountPerInternalNode + leafIndexInInternalNode} to {leafIndexInAtlas}
	GVis::TexturePtr nodeIndirectionTexture;

	//! Index of the first InternalNode in the geometry with this material assigned
	int firstInternalNodeIndex;

	int maxLeafCountPerInternalNodeDimension;
	glm::ivec3 maxLeafCountPerAtlasDimension;
};

class SparseVolumeMaterialFactory
{
public:
	virtual ~SparseVolumeMaterialFactory() {}

	virtual GVis::MaterialPtr createMaterial(const SparseVolumeMaterialConfig& config) const = 0;
};

typedef shared_ptr<SparseVolumeMaterialFactory> SparseVolumeMaterialFactoryPtr;

struct RenderableVolume
{
	std::vector<GVis::RenderableNodePtr> nodes;
};

struct RenderableVolumeConfig
{
	RenderableVolumeConfig() :
		scale(1.0f),
		maxLeavesPerAtlas(4096),
		batchBoxes(false)
	{
	}

	/*! All grids must have the same structure, but may be different data types,
		e.g one grid for density, one grid for normals etc.
		One texture atlas will be created for each grid.
		All internalNodes must have the same bounding box size.
		All leaves must have the same bounding box size.
	*/
	std::vector<GridPtr> grids;

	SparseVolumeMaterialFactoryPtr materialFactory;

	/*! Specifies how renderable box volumes will be batched.
		When true, as many boxes as possible will be batched into a single draw call.
		When false, one box (draw call) per internal node will be generated.
		Batched boxes will generally render faster but can't be sorting for transparency.
	*/
	bool batchBoxes;

	float scale;
	int maxLeavesPerAtlas;
};

/*!
A RenderableVolume consists of one or more renderable box meshes, each box corresponding to an InternalNode in the grid.
Each box will be assigned a material with the following resources:
 - atlases containing packed leaf nodes from grid
 - Indirection texture which maps leaf node ID to coordinate of leaf in texture atlas
*/
class RenderableVolumeFactory
{
public:
	static RenderableVolumePtr createRenderableVolume(const RenderableVolumeConfig& config);
};

} // namespace GSparseVolumes