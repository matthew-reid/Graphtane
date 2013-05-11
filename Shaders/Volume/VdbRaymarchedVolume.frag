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

#include "DeepShadowMaps.h"
#include "VolumeUtil.h"
#include "../Common/Lambert.h"

in vec3 texCoord;
in vec3 position_modelSpace;
in flat int nodeIndirectionBaseIndex;

out vec4 color;

uniform sampler3D albedoSampler;
uniform sampler3D normalSampler;
uniform sampler3D temperatureSampler;
uniform sampler2D temperatureRampSampler;
uniform isamplerBuffer nodeIndirectionSampler;

uniform float maxDepth;
uniform vec3 cameraPosition_modelSpace;
uniform vec3 lightDirection_modelSpace;

uniform vec3 volumeSize_modelSpace;
uniform vec3 oneOnVolumeTextureSize;

uniform float thresholdAlpha = 0.001;
uniform float opacityMultiplier = 500.0f;

const float rayTerminationOpacity = 0.98f;
const int maxIterationCount = 200;

uniform ivec3 maxLeafCountPerAtlasDimension;
uniform int maxLeafCountPerInternalNodeDimension;

vec3 getLeafAtlasOffset(int atlasTextureIndex)
{
	vec3 atlasCoord;
	atlasCoord.x = atlasTextureIndex % maxLeafCountPerAtlasDimension.x;
	atlasCoord.y = (atlasTextureIndex / maxLeafCountPerAtlasDimension.x) % maxLeafCountPerAtlasDimension.y;
	atlasCoord.z = atlasTextureIndex / (maxLeafCountPerAtlasDimension.x * maxLeafCountPerAtlasDimension.y);
	
	return atlasCoord / maxLeafCountPerAtlasDimension;
}

int normalizedCoordToLinearOffset(vec3 pos)
{
	ivec3 coord = clamp(ivec3(pos * float(maxLeafCountPerInternalNodeDimension)), ivec3(0), ivec3(15));
	// VDB is indexed in zyx order
	return coord.z + coord.y * maxLeafCountPerInternalNodeDimension + coord.x * maxLeafCountPerInternalNodeDimension * maxLeafCountPerInternalNodeDimension;
}

vec4 accumulateColorOverRaySegment(vec4 color, vec3 texCoord, float alpha, float segmentLength)
{
#ifdef USE_NORMAL_SAMPLER
	vec3 normal = normalize(textureGrad(normalSampler, texCoord, vec3(0), vec3(0)).rgb);
	float lambert = calcLambert(lightDirection_modelSpace, normal, 0.3);
#else
	float lambert = 1.0;
#endif	
	
	vec3 sampleColor = vec3(1) * lambert;
	
	float opacity = min(1.0, alpha * segmentLength);

	vec4 result;
	result.rgb = color.rgb + sampleColor.rgb * opacity * (1 - color.a);
	result.a = color.a + opacity * (1 - color.a);
	
	return result;
}

vec4 accumulateColorOverRaySegmentTemperatureMapped(vec4 color, vec3 texCoord, float alpha, float segmentLength)
{
	float temperature = texture(temperatureSampler, texCoord).r;
	vec3 sampleColor = textureGrad(temperatureRampSampler, vec2(temperature, 0.5), vec2(0), vec2(0)).rgb;
	
	float opacity = alpha * segmentLength;

	vec4 result;
	result.rgb = color.rgb + sampleColor.rgb * opacity * (1 - color.a);
	result.a = color.a + opacity * (1 - color.a);
	
	return result;
}

void main()
{
	color = vec4(0);

	vec3 stepSize = vec3(1.0 / 128.0); // TODO: use adaptive step sizes
	vec3 step = normalize(position_modelSpace - cameraPosition_modelSpace) * stepSize;
	vec3 step_modelSpace = step * volumeSize_modelSpace;
	vec3 pos = texCoord;

	float rayStartDepth = gl_FragCoord.z / (gl_FragCoord.w * maxDepth); // normalized linear depth
	float normalizedLinearDepthStep = length(step_modelSpace) / maxDepth;
	
	vec3 texCoordScale =  vec3(1.0 / maxLeafCountPerAtlasDimension);
	
    for(int i = 0; i < maxIterationCount; i++)
	{
		int atlasTextureIndex = texelFetch(nodeIndirectionSampler, nodeIndirectionBaseIndex + normalizedCoordToLinearOffset(pos)).r;
		if (atlasTextureIndex != -1)
		{
			vec3 coordFloat = min(pos, 0.9999) * float(maxLeafCountPerInternalNodeDimension);
			ivec3 coordInt = ivec3(coordFloat);
			vec3 frac = coordFloat - coordInt;
			
			vec3 texCoordOffset = getLeafAtlasOffset(atlasTextureIndex);
		
			vec3 sampleTexCoord = clamp(frac, 0.5/8.0, 7.5/8.0) * texCoordScale + texCoordOffset;
		
			vec4 albedoSample = textureGrad(albedoSampler, sampleTexCoord, vec3(0), vec3(0));
			float alpha = albedoSample.r;

			if (alpha > thresholdAlpha)
			{
				alpha *= opacityMultiplier;
			#ifdef USE_TEMPERATURE_SAMPLER
				color = accumulateColorOverRaySegmentTemperatureMapped(color, sampleTexCoord, alpha, stepSize.x);
			#else
				color = accumulateColorOverRaySegment(color, sampleTexCoord, alpha, stepSize.x);
			#endif
			}

			// Break from the loop when alpha gets high enough
			if(color.a >= rayTerminationOpacity)
			{
				color.a = rayTerminationOpacity;
				break; 
			}
		}
     
        // Advance the current position
        pos.xyz += step;
	 
        // Break if the position is outside volume
        if(isNormalisedVolumeCoordOutsideVolume(pos))
		{
            break;
		}
    }
	
	if (color.a == 0)
	{
		discard;
	}

	color.a /= rayTerminationOpacity; // remap alpha between 0 and 1

}