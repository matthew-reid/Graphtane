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

#include "../Common/TexturePcf.h"
#include "DeepShadowMaps.h"
#include "VolumeUtil.h"

#include "EmptyRayContext.h"
#include "RaymarchThroughBox.h"

in vec3 texCoord;
in vec3 position_modelSpace;

out vec4 color;

uniform sampler3D albedoSampler;
uniform sampler2D sceneDepthSampler;
uniform sampler2D shadowSampler;

uniform mat4 shadowModelViewProj;

uniform vec3 volumeSize_modelSpace;
uniform vec3 cameraPosition_modelSpace;
uniform vec3 oneOnVolumeTextureSize;
uniform float maxClipSpaceDepth;
uniform vec2 viewportResolution;

uniform float opacityMultiplier = 5.0f;
uniform vec3 diffuseColor = vec3(1);


vec4 getColorAlongRayStep(vec3 normalizedVolumeCoord, float stepLength, float extinctionCoefficient, RayContext rayContext)
{
	vec4 albedoSample = texturePcf2(albedoSampler, normalizedVolumeCoord, oneOnVolumeTextureSize / 2.0f);
	float alpha = albedoSample.r * opacityMultiplier;

	if (alpha > 0.001)
	{
		float deltaTransmissivity = exp(-extinctionCoefficient * stepLength * alpha);
		
		vec3 shadowTexcoord = calcShadowTexCoord(shadowModelViewProj, normalizedVolumeCoord, volumeSize_modelSpace);
		float visibility = getVisibility_dsm(shadowSampler, shadowTexcoord);
		
		return vec4(diffuseColor * visibility, 1.0 - deltaTransmissivity);
	}
	else
	{
		return vec4(0);
	}
}

float getNormalisedScreenSpaceSceneDepth(vec2 screenTexcoord)
{
	return texture(sceneDepthSampler, screenTexcoord).r;
}

void main()
{
	RaymarchArgs args;

	args.boxTexCoordAtStartOfRay = texCoord;
	args.position_modelSpace = position_modelSpace;
	args.volumeSize_modelSpace = volumeSize_modelSpace;
	args.maxClipSpaceDepth = maxClipSpaceDepth;
	args.cameraPosition_modelSpace = cameraPosition_modelSpace;
	args.oneOnVolumeTextureSize = oneOnVolumeTextureSize;
	args.viewportResolution = viewportResolution;
	args.extinctionCoefficient = 1;

	color = raymarchThroughBox(args);
}