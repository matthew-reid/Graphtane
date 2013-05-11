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

#include "RenderableVolumeFactory.h"
#include "BoxBatchBuilder.h"
#include "InternalNode.h"
#include "Grid.h"
#include "GridVectorIterators.h"
#include "Leaf.h"
#include "VolumeTextureAtlasBuilder.h"

#include <GVis/RenderableNode.h>
#include <GVis/Geo.h>
#include <GVis/Texture.h>
#include <GVis/TextureBufferObject.h>
#include <GCommon/Logger.h>

#include <boost/scoped_array.hpp>

using namespace GCommon;
using namespace GVis;

namespace GSparseVolumes {

class NodeIndirectionMapBuilder
{
public:
	explicit NodeIndirectionMapBuilder(size_t size) :
		m_size(size)
	{
		// TODO: consider making this unsigned int to handle more nodes...only half the possible index values will be used
		int sizeBytes = size * sizeof(int);

		m_buffer.reset(new int[size]);
		memset(m_buffer.get(), -1, sizeBytes);
	}

	void addChildReference(int referenceLocation, int referencedChild)
	{
		m_buffer[referenceLocation] = referencedChild;
	}

	TexturePtr build()
	{
		TextureBufferObjectPtr tbo(new ScopedTextureBufferObject(m_buffer.get(), m_size, sizeof(int)));
		BufferTextureConfig bufferTextureConfig(PixelFormat_R32I, tbo);
		TexturePtr texture(new Texture(bufferTextureConfig));
		return texture;
	}

private:
	boost::scoped_array<int> m_buffer;
	size_t m_size;
};

TexturePtr buildNodeIndirectionTexture(const Grid& grid, std::vector<int> atlasInternalNodeCounts)
{
	InternalNodeIteratorPtr nodeIterator = grid.createInternalNodeIterator();

	int internalNodeIndex = 0;
	size_t maxLeafCountPerInternalNode = grid.getMaxLeafCountPerInternalNode();
	NodeIndirectionMapBuilder indirectionMapBuilder(grid.getInternalNodeCount() * maxLeafCountPerInternalNode);

	for (int atlasIndex = 0; atlasIndex < atlasInternalNodeCounts.size(); ++atlasIndex)
	{
		int leafIndexInAtlas = 0;
		int internalNodeIndexInAtlas = 0;
		while (internalNodeIndexInAtlas < atlasInternalNodeCounts[atlasIndex])
		{
			const InternalNodePtr& internalNode = nodeIterator->next();
			assert(internalNode);
			LeafIteratorPtr leafIterator = internalNode->createLeafIterator();

			while (const LeafPtr& leaf = leafIterator->next())
			{
				int tboIndex = internalNodeIndex * maxLeafCountPerInternalNode + leaf->getIndexInInternalNode();
				indirectionMapBuilder.addChildReference(tboIndex, leafIndexInAtlas);

				++leafIndexInAtlas;
			}
			++internalNodeIndex;
			++internalNodeIndexInAtlas;
		}
	}

	assert(internalNodeIndex > 0);
	return indirectionMapBuilder.build();
}

std::vector<int> calcAtlasInternalNodeCounts(const Grid& grid, int maxLeavesPerAtlas)
{
	std::vector<int> result;
	InternalNodeIteratorPtr i = grid.createInternalNodeIterator();

	while (i->hasNext())
	{
		// calculate InternalNode count for this atlas
		int internalNodeCount = 0;
		int leafCount = 0;
		while (i->hasNext()) // iterate InternalNodes
		{
			InternalNodeIteratorPtr prevIt = i->clone();
			const InternalNodePtr node = i->next();

			LeafIteratorPtr leafIt = node->createLeafIterator();
			while (leafIt->next())
			{
				leafCount++;
			}

			if (leafCount >= maxLeavesPerAtlas)
			{
				i = prevIt;
				break;
			}

			internalNodeCount++;
		}

		if (internalNodeCount == 0)
		{
			throw std::runtime_error("Too many leaves in InternalNode to fit in a single texture atlas");
		}
		result.push_back(internalNodeCount);
	}

	return result;
}

RenderableVolumePtr RenderableVolumeFactory::createRenderableVolume(const RenderableVolumeConfig& config)
{
	RenderableVolumePtr outputVolume(new RenderableVolume);

	if (!config.grids.empty())
	{
		// Calculate texture atlas sizes
		std::vector<int> atlasInternalNodeCounts = calcAtlasInternalNodeCounts(*config.grids.front(), config.maxLeavesPerAtlas);

		// Build node indirection texture
		TexturePtr nodeIndirectionTexture = buildNodeIndirectionTexture(*config.grids.front(), atlasInternalNodeCounts);

		int currentInternalNodeIndex = 0;
		InternalNodesIterator internalNodesIterator(config.grids);

		// Iterate atlases we are going to create
		for (int atlasIndex = 0; atlasIndex < atlasInternalNodeCounts.size(); ++atlasIndex)
		{
			typedef std::tr1::shared_ptr<VolumeTextureAtlasBuilder> VolumeTextureAtlasBuilderPtr;
			std::vector<VolumeTextureAtlasBuilderPtr> atlasBuilders;

			// Create an atlas builder for each grid
			for (int i = 0; i < config.grids.size(); ++i)
			{
				// TODO: Optimise texture size. Currently we always create texture full sized texture even if only a fraction of the space is used.
				// In practice only the final texture in a series of atlaes will benifit from this optimisation, so may not be worth implementing.
				VolumeTextureAtlasBuilderPtr atlasBuilder(new VolumeTextureAtlasBuilder(config.maxLeavesPerAtlas, config.grids[i]->getVoxelCountPerLeafDimension(), config.grids[i]->getChannelCount()));
				atlasBuilders.push_back(atlasBuilder);
			}

			// Create objects for creating renderable boxes
			std::vector<glm::vec3> boxCenters;
			std::vector<MeshPtr> boxMeshes;
			glm::vec3 boxSize;
			BoxBatchBuilder boxBatchBuilder;

			// Fill up this atlas with leaves from InternalNodes until atlas is full
			int firstInternalNodeIndexInAtlas = currentInternalNodeIndex;
			while (currentInternalNodeIndex - firstInternalNodeIndexInAtlas < atlasInternalNodeCounts[atlasIndex])
			{
				const InternalNodes* internalNodes = internalNodesIterator.next();
				assert(internalNodes);

				// Create renderable box for InternalNode
				{
					glm::vec3 thisBoxSize = internalNodes->front()->getBoundingBoxSize() * config.scale;
					glm::vec3 boxCenter = internalNodes->front()->getBoundingBoxCenter() * config.scale;

					if (!boxMeshes.empty())
					{
						assert(thisBoxSize == boxSize || boxMeshes.empty());
					}
					boxSize = thisBoxSize;

					if (config.batchBoxes) // one draw call per internal node batch (i.e per texture atlas)
					{
						boxBatchBuilder.addBox(boxCenter, boxSize);
					}
					else // one draw call per internal node
					{
						BoxBatchBuilder b;
						b.addBox(glm::vec3(0,0,0), boxSize);
						boxMeshes.push_back(b.build());
						boxCenters.push_back(boxCenter);
					}
				}

				// Add all leaves of this InternalNode to atlases
				LeavesIterator leavesIterator(*internalNodes);
				while (const Leaves* leaves = leavesIterator.next())
				{
					for (int i = 0; i < leaves->size(); ++i)
					{
						atlasBuilders[i]->addTexture(*(*leaves)[i]);
					}
				}

				currentInternalNodeIndex++;
			}

			// Atlas is now full. Create textures and meshes.

			if (config.batchBoxes)
			{
				boxMeshes.push_back(boxBatchBuilder.build());
				boxCenters.push_back(glm::vec3(0,0,0));
			}

			GridTexturesMap textures;
			for (int i = 0; i < atlasBuilders.size(); ++i)
			{
				textures[config.grids[i]] = atlasBuilders[i]->build();
			}

			for (int i = 0; i < boxMeshes.size(); ++i)
			{
				MeshPtr mesh = boxMeshes[i];

				SparseVolumeMaterialConfig materialConfig;
				materialConfig.boxSize = boxSize;
				materialConfig.leafAtlases = textures;
				materialConfig.nodeIndirectionTexture = nodeIndirectionTexture;
				materialConfig.firstInternalNodeIndex = firstInternalNodeIndexInAtlas + i;
				materialConfig.maxLeafCountPerInternalNodeDimension = config.grids.front()->getMaxLeafCountPerInternalNodeDimension();
				materialConfig.maxLeafCountPerAtlasDimension = atlasBuilders.front()->getMaxItemCountPerDimension();

				MaterialPtr material = config.materialFactory->createMaterial(materialConfig);

				GeoPtr geo(new Geo(mesh, material));

				RenderableNodePtr node(new RenderableNode);
				node->addRenderable(geo);
				node->setPosition(boxCenters[i]);
				outputVolume->nodes.push_back(node);
			}
		}
	}
	return outputVolume;
}

} // namespace GSparseVolumes