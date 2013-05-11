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

#include "VolumeUtil.h"

in vec3 texCoord;
in vec3 position_modelSpace;

out vec4 color;

uniform sampler3D albedoSampler;

uniform float farClipDistance;
uniform float maxClipSpaceDepth;
uniform vec3 cameraForwardDirection_modelSpace;

uniform vec3 oneOnVolumeTextureSize;
uniform vec3 volumeSize_modelSpace;

uniform float opacityMultiplier = 0.1f;

const float thresholdVisibility = 0.02f; // early-out when visibility reaches this value
const int iterationCount = 200;

const float stepSizeMultiplier = 2.0;

void main()
{
	color = vec4(1000, 0, 1, 0); // start, end, light visibility at end

	vec3 stepSize = oneOnVolumeTextureSize * stepSizeMultiplier;
	vec3 step = cameraForwardDirection_modelSpace * stepSize; // all rays are parallel because we assume orthographic projection
	vec3 worldStep = step * volumeSize_modelSpace;
	
	float rayStartDepth = gl_FragCoord.z / (gl_FragCoord.w * maxClipSpaceDepth); // normalized linear depth
	vec3 pos = texCoord;

	float distAlongRay_normalizedLinearDepthSpace = rayStartDepth;
	float normalizedLinearDepthStep = length(worldStep) / farClipDistance;
	
    for(int i = 0; i < iterationCount; i++)
    {
		float alpha = texture(albedoSampler, pos).r * opacityMultiplier;
		
		color.r = min(color.r, distAlongRay_normalizedLinearDepthSpace);
		color.g = max(color.g, distAlongRay_normalizedLinearDepthSpace);
		color.b = color.b * (1.0f - alpha);

        // Break from the loop when visibility reaches zero
        if(color.b <= thresholdVisibility)
		{
			color.b = thresholdVisibility;
			break; 
		}
     
        // Advance the current position
        pos.xyz += step;
		distAlongRay_normalizedLinearDepthSpace += normalizedLinearDepthStep;
	 
        if(isNormalisedVolumeCoordOutsideVolume(pos))
		{
            break;
		}
    }
	
	// Set z buffer value to depth where light visibility reaches zero
	gl_FragDepth = color.g * gl_FragCoord.w * maxClipSpaceDepth; // FIXME: may not be accurate for perspective projections - should use a different w?
	
	// remap output visibility from [thresholdVisibility, 1] to [0, 1]
	color.b = (color.b - thresholdVisibility) / (1.0 - thresholdVisibility);
}