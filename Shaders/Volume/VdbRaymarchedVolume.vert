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

#version 330 core

layout(location = 0) in vec4 vertexPosition_modelSpace;
layout(location = 1) in vec3 vertexTexCoord;

out vec3 texCoord;
out vec3 position_modelSpace;
out flat int nodeIndirectionBaseIndex;

uniform mat4 modelViewProj;
uniform int firstInternalNodeIndex;
uniform int maxLeafCountPerInternalNode;

const int verticesPerInternalNode = 8;

void main()
{
	gl_Position = modelViewProj * vertexPosition_modelSpace;
	texCoord = vertexTexCoord;
	
	int internalNodeIndex = firstInternalNodeIndex + (gl_VertexID / verticesPerInternalNode);
	nodeIndirectionBaseIndex = internalNodeIndex * maxLeafCountPerInternalNode;
	
	position_modelSpace = vertexPosition_modelSpace.xyz;
}