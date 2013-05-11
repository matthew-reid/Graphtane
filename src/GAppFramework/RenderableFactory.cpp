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

#include "RenderableFactory.h"

#include <GVis/Primitives/RectangleMeshFactory.h>
#include <GVis/Material.h>
#include <GVis/Geo.h>
#include <GVis/RenderableNode.h>
#include <GVis/ShaderProgram.h>
#include <GVis/Technique.h>

using namespace GVis;

namespace GAppFramework {

RenderableFactory::RenderableFactory()
{
	m_screenQuadProgram = ShaderProgram::createShaderProgram(ShaderProgramConfig("Shaders/Common/ScreenQuad.vert", "Shaders/Common/SimpleTextured.frag"));
}

RenderableNodePtr RenderableFactory::createScreenQuad(const ShaderProgramPtr& program, const GVis::TexturePtr& texture, float scale)
{
	TechniquePtr technique(new Technique(program));
	MaterialPtr material(new Material);
	material->setTechnique(technique);
		
	TextureUnit unit(texture, "albedoSampler");
	technique->addTextureUnit(unit);

	RectangleConfig rectangleConfig = RectangleConfig::defaultWithSize(glm::vec2(2, 2) * scale);
	MeshPtr rectangleMesh = RectangleMeshFactory::createMesh(rectangleConfig);
	GeoPtr rectangle(new Geo(rectangleMesh, material));

	return RenderableNode::createWithSingleGeo(rectangle);
}

RenderableNodePtr RenderableFactory::createScreenQuad(const TexturePtr& texture, float scale) const
{
	return createScreenQuad(m_screenQuadProgram, texture, scale);
}

} // namespace GAppFramework
