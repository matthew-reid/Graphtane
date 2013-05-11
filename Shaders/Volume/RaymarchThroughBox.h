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

#ifndef RAYMARCH_THROUGH_BOX_H
#define RAYMARCH_THROUGH_BOX_H

#include "VolumeUtil.h"

const float rayTerminationOpacity = 0.98f; // ray will terminate when accumulated opacity reaches this value
const int maxStepCount = 200; // ray will terminate when reaching this value

// Callbacks. Caller of raymarchThroughBox() should implement these.
vec4 getColorAlongRayStep(vec3 normalizedVolumeCoord, float stepLength, float extinctionCoefficient, RayContext rayContext);
float getNormalisedScreenSpaceSceneDepth(vec2 screenTexcoord); // normalised over maxClipSpaceDepth


struct RaymarchArgs
{
	vec3 boxTexCoordAtStartOfRay; // box texcoords are assumed to be 0,0,0 at one corner, 1,1,1 at opposite corner
	vec3 position_modelSpace;
	vec3 volumeSize_modelSpace;
	float maxClipSpaceDepth; // projection matrix * far clip distance
	vec3 cameraPosition_modelSpace;
	vec3 oneOnVolumeTextureSize;
	vec2 viewportResolution;
	float extinctionCoefficient;
	RayContext rayContext;
};

vec4 raymarchThroughBox(RaymarchArgs args)
{
	vec4 color = vec4(0);

	vec3 stepSize = args.oneOnVolumeTextureSize;
	vec3 step = normalize(args.position_modelSpace - args.cameraPosition_modelSpace) * stepSize;
	vec3 step_modelSpace = step * args.volumeSize_modelSpace;
	vec3 pos = args.boxTexCoordAtStartOfRay;

	vec2 screenTexcoord = gl_FragCoord.xy / args.viewportResolution;
	
	float rayStartDepth = gl_FragCoord.z / (gl_FragCoord.w * args.maxClipSpaceDepth); // normalized linear depth
	float distAlongRay_normalizedLinearDepthSpace = rayStartDepth; // in normalized
	float normalizedLinearDepthStep = length(step_modelSpace) / args.maxClipSpaceDepth;
	
	float transmissivity = 1.0;
	
    for(int i = 0; i < maxStepCount; i++)
    {
		// Break from loop when ray hits a scene object
		float stepEarlyCutoffMultiplier = 1.0;
		float distToSceneObject = getNormalisedScreenSpaceSceneDepth(screenTexcoord) - distAlongRay_normalizedLinearDepthSpace;
		if (distToSceneObject < 0)
		{
			stepEarlyCutoffMultiplier = (normalizedLinearDepthStep + distToSceneObject) / normalizedLinearDepthStep;
		}
	
		vec4 stepColor = getColorAlongRayStep(pos, stepSize.x * stepEarlyCutoffMultiplier, args.extinctionCoefficient, args.rayContext);
		
		color += stepColor * transmissivity * (stepColor.a / args.extinctionCoefficient);
		transmissivity *= 1.0 - stepColor.a;
		
		color.a = 1.0 - transmissivity;

		if (distToSceneObject < 0)
		{
			break;
		}
		
        // Break from the loop when alpha gets high enough
        if(color.a >= rayTerminationOpacity)
		{
			color.a = rayTerminationOpacity;
			break; 
		}
     
        // Advance the current position
        pos.xyz += step;
		distAlongRay_normalizedLinearDepthSpace += normalizedLinearDepthStep;
	 
        // Break if the position is outside volume
        if(isNormalisedVolumeCoordOutsideVolume(pos))
		{
            break;
		}
    }

	color.a /= rayTerminationOpacity; // remap alpha between 0 and 1
	return color;
}

#endif