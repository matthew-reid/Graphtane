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

#define ON_DEVICE
#include "FluidDataTypes.h"
#include "Neighbors3d.h"

DEFINE_NEIGHBORS_STRUCT(Neighbors_FluidState, FluidState)

Neighbors_FluidState getNeighbors_FluidState(__global const FluidState* grid)
{
	RETURN_NEIGHBORS_STRUCT_GENERIC(Neighbors_FluidState, grid)
}

float3 getDensityGradient(const __global FluidState* fluidStateGrid)
{
	// FIXME: could be optimized. Don't need n.c. Can we re-use pressure gradient calc from projection?
	Neighbors_FluidState n = getNeighbors_FluidState(fluidStateGrid);
	return (float3)(n.w.x, n.n.x, n.d.x) - (float3)(n.e.x, n.s.x, n.u.x);
}

__kernel void calcDensityGradient(__global float3* gradient, const __global FluidState* fluidStateGrid)
{
	gradient[getElement()] = getDensityGradient(fluidStateGrid);
}

// input must be twice the dimensions of kernel and of output
__kernel void downScale2x(__global float3* output, const __global float3* input)
{
	int x = get_global_id(0);
	int y = get_global_id(1);
	int z = get_global_id(2);
	x = clamp(x * 2, 0, (int)get_global_size(0) * 2 - 1);
	y = clamp(y * 2, 0, (int)get_global_size(1) * 2 - 1);
	z = clamp(z * 2, 0, (int)get_global_size(2) * 2 - 1);

	int strideY = get_global_size(0) * 2;
	int strideZ = get_global_size(0) * get_global_size(1) * 4;
	
	int c = x + y * strideY + z * strideZ;
	
	float3 sum = 0;
	for (int offsZ = 0; offsZ <= 1; ++offsZ)
	{
		for (int offsY = 0; offsY <= 1; ++offsY)
		{
			for (int offsX = 0; offsX <= 1; ++offsX)
			{
				sum += input[c + offsX + strideY * offsY + strideZ * offsZ];
			}
		}
	}
	
	output[getElement()] = sum / 8;
}

__kernel void visNormal(__write_only image3d_t image, const __global float3* densityGradient)
{
	float3 gradient = densityGradient[getElement()];
	float gradientMagnitude = length(gradient);
	gradientMagnitude = max(0.01f, gradientMagnitude);
	float4 color = (float4)((gradient / gradientMagnitude) * 0.5 + 0.5, 0);
	
	write_imagef(image, (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0), color);
}