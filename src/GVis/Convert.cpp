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

#include "Convert.h"

namespace GVis {

GLenum toGlTargetType(TextureType type)
{
	switch (type)
	{
	case TextureType_2d:
		return GL_TEXTURE_2D;
	case TextureType_3d:
		return GL_TEXTURE_3D;
	case TextureType_Buffer:
		return GL_TEXTURE_BUFFER;
	default:
		assert(0);
	}
	return 0;
}

GLint toGlTextureWrap(TextureAddressMode mode)
{
	switch (mode)
	{
	case TextureAddressMode_Wrap:
		return GL_REPEAT;
	case TextureAddressMode_Clamp:
		return GL_CLAMP_TO_EDGE;
	default:
		assert(0);
	}
	return 0;
}

GLint toGlFrameBufferAttachment(FrameBufferAttachment attachment, int attachmentIndex)
{
	switch (attachment)
	{
	case FrameBufferAttachment_Color:
		return GL_COLOR_ATTACHMENT0 + attachmentIndex;
	case FrameBufferAttachment_Depth:
		return GL_DEPTH_ATTACHMENT;
	default:
		assert(0);
	}
	return 0;
}

GLenum toGlShaderType(ShaderType type)
{
	switch (type)
	{
	case ShaderType_Vertex:
		return GL_VERTEX_SHADER;
	case ShaderType_Fragment:
		return GL_FRAGMENT_SHADER;
	case ShaderType_Geometry:
		return GL_GEOMETRY_SHADER;
	case ShaderType_TessellationControl:
		return GL_TESS_CONTROL_SHADER;
	case ShaderType_TessellationEvaluation:
		return GL_TESS_EVALUATION_SHADER;
	default:
		assert(0);
	}
	return 0;
}

GLenum toGlBufferUsage(BufferUsage usage)
{
	switch (usage)
	{
	case BufferUsage_Static:
		return GL_STATIC_DRAW;
	case BufferUsage_Dynamic:
		return GL_DYNAMIC_DRAW;
	default:
		assert(0);
	}
	return 0;
}

} // namespace GVis
