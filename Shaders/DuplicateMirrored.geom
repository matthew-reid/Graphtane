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

#version 150
 
layout(triangles) in;
layout(triangle_strip, max_vertices=6) out;

in vec3 vsTexCoord[];
in vec3 vsNormal[];

out vec3 texCoord;
out vec3 normal;

uniform mat4 modelViewProj;
uniform vec3 lightDirection_modelSpace;

void main()
{
	// Pass-through
	for(int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = modelViewProj * gl_in[i].gl_Position;
		texCoord = vsTexCoord[i];
		normal = vsNormal[i];
		EmitVertex();
	}
	EndPrimitive();

	// Mirrored geometry
	vec4 position;
	for(int i = 0; i < gl_in.length(); i++)
	{
		position = gl_in[i].gl_Position;
		position.x = -position.x + 2;
		gl_Position = modelViewProj * position;
		texCoord = vsTexCoord[i];
		normal = vsNormal[i];
		EmitVertex();
	}
	EndPrimitive();
}