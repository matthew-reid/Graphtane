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

#ifndef SHADOWS_H
#define SHADOWS_H

float shadowMapSize = 512.0f;
float oneOnShadowMapSize = 1.0f / shadowMapSize;
float bias = 0.0005;
float oneOnBlockingStartFadeInDistance = 200.0;

float getVisibility(sampler2D shadowSampler, vec3 shadowTexcoord)
{
	return float(texture(shadowSampler, shadowTexcoord.xy).r > shadowTexcoord.z - bias);
}

float getVisibility_bilinear(sampler2D shadowSampler, vec3 shadowTexcoord)
{
	vec2 texel = shadowTexcoord.xy * shadowMapSize - 0.5;
	vec2 flooredTexel = floor(texel);
	vec4 taps = textureGather(shadowSampler, flooredTexel * oneOnShadowMapSize, 0);
#ifdef HARD_VISIBILITY_EDGE
	vec4 visibility = vec4(greaterThan(taps, vec4(shadowTexcoord.z - bias)));
#else
	vec4 visibility = clamp(oneOnBlockingStartFadeInDistance * (taps - vec4(shadowTexcoord.z - bias)), 0, 1);
#endif
	vec2 weights = texel - flooredTexel;
	float a = mix(visibility.w, visibility.z, weights.x);
	float b = mix(visibility.x, visibility.y, weights.x);
	return mix(a, b, weights.y);
}

float getVisibility_bilinear_dsm(sampler2D shadowSampler, vec3 shadowTexcoord)
{
	vec2 texel = shadowTexcoord.xy * shadowMapSize - 0.5;
	vec2 flooredTexel = floor(texel);
	vec4 startDist = textureGather(shadowSampler, flooredTexel * oneOnShadowMapSize, 0);
	vec4 visibility = vec4(greaterThan(startDist, vec4(shadowTexcoord.z - bias)));
	
	vec4 endVisibility = textureGather(shadowSampler, flooredTexel * oneOnShadowMapSize, 2);
	
	visibility = mix(endVisibility, vec4(1), visibility);
	
	vec2 weights = texel - flooredTexel;
	float a = mix(visibility.w, visibility.z, weights.x);
	float b = mix(visibility.x, visibility.y, weights.x);
	return mix(a, b, weights.y);
}


float getVisibility_pcf(sampler2D shadowSampler, vec3 shadowTexcoord)
{
	float total = 0;
	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			vec3 offset = vec3(vec2(x,y) * oneOnShadowMapSize, 0);
			total += getVisibility(shadowSampler, shadowTexcoord + offset);
		}
	}
	return total / 9;
}

float getVisibility_pcf_bilinear(sampler2D shadowSampler, vec3 shadowTexcoord)
{
	float total = 0;
	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			vec3 offset = vec3(vec2(x,y) * oneOnShadowMapSize, 0);
			total += getVisibility_bilinear(shadowSampler, shadowTexcoord + offset);
		}
	}
	return total / 9;
}

vec2 poissonDisk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

// Returns a random number based on a vec3 and an int.
float random(vec2 seed)
{
	float dot_product = dot(seed, vec2(12.9898,78.233));
	return fract(sin(dot_product) * 43758.5453);
}

float getVisibility_poisson(sampler2D shadowSampler, vec3 shadowTexcoord)
{
	float random = 6.3 * random(gl_FragCoord.xy);
	vec2 rotations = vec2(sin(random), cos(random));
	mat2 rotMat = mat2(rotations.x, -rotations.y, rotations.y, rotations.x);
		
	float total = 0;
	for (int i = 0; i < 16; i++)
	{
		vec2 offset = poissonDisk[i] * oneOnShadowMapSize * 3.0;
		offset = rotMat * offset;
	
		total += float(texture(shadowSampler, shadowTexcoord.xy + offset).r > shadowTexcoord.z);
	}
	return total / 16.0;
}


uniform float shadowMaxClipSpaceDepth;

vec3 getShadowTexcoord(vec3 position_shadowSpace)
{
	return vec3(position_shadowSpace.xy, position_shadowSpace.z / shadowMaxClipSpaceDepth);
}

#endif