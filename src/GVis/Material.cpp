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

#include "Material.h"
#include "Technique.h"

namespace GVis {

void Material::setTechnique(TechniquePtr technique, TechniqueCategory category)
{
	std::pair<Techniques::iterator, bool> r = m_techniques.insert(Techniques::value_type(category, technique));
	assert(r.second);
}

TechniquePtr Material::getTechnique(TechniqueCategory category)
{
	Techniques::iterator i = m_techniques.find(category);
	if (i != m_techniques.end())
	{
		return i->second;
	}
	return TechniquePtr();
}

MaterialPtr Material::createWithSingleTechnique(const ShaderProgramPtr& shader, TechniqueCategory category)
{
	TechniquePtr technique(new Technique(shader));
	MaterialPtr material(new Material);
	material->setTechnique(technique, category);
	return material;
}

} // namespace GVis
