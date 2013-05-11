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

#include "Shadows.h"

in vec3 texCoord;
in vec3 normal;
in vec4 position_shadowSpace;

out vec3 color;

uniform sampler2D shadowSampler;
uniform vec3 lightDirection_modelSpace;

const vec3 ambient = vec3(0.015);

void main()
{
	vec3 shadowTexcoord = getShadowTexcoord(position_shadowSpace.xyz);

	float lambert = clamp(dot(lightDirection_modelSpace, normal), 0, 1);
	vec3 albedo = vec3(0.4,0.8,0.6);
	
	color = albedo * (ambient + lambert * getVisibility_bilinear_dsm(shadowSampler, shadowTexcoord));
}