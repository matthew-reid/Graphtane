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

#include "GVis/GVisFwd.h"
#include <map>

namespace GVis {

enum TextureRole
{
	TextureRole_Albedo,
	TextureRole_Normal,
	TextureRole_Count
};

typedef std::map<TextureRole, TexturePtr> TextureRoles;

inline TexturePtr getTexture(const TextureRoles& roles, TextureRole role)
{
	TexturePtr texture;
	TextureRoles::const_iterator i = roles.find(role);
	if (i != roles.end())
	{
		return i->second;
	}
	return texture;
}


class MaterialFactory
{
public:
	//! @param textureRoles contains textures to use. If empty, no textures will be used.
	//! @return never null
	virtual MaterialPtr createMaterial(const TextureRoles& textureRoles) = 0;
};

typedef shared_ptr<MaterialFactory> MaterialFactoryPtr;

} // namespace GVis