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
#include "Trilinear.h"
#include "Neighbors3d.h"
#include "Params.h"

__constant float h = 1;
__constant int brushRadius = 4;


DEFINE_NEIGHBORS_STRUCT(Neighbors_float2, float2)
DEFINE_NEIGHBORS_STRUCT(Neighbors_float3, float3)
DEFINE_NEIGHBORS_STRUCT(Neighbors_FluidState, FluidState)

Neighbors_float2 getNeighbors_float2(__global const float2* grid)
{
	RETURN_NEIGHBORS_STRUCT_GENERIC(Neighbors_float2, grid)
}

Neighbors_float3 getNeighbors_float3(__global const float3* grid)
{
	RETURN_NEIGHBORS_STRUCT_GENERIC(Neighbors_float3, grid)
}

Neighbors_FluidState getNeighbors_FluidState(__global const FluidState* grid)
{
	RETURN_NEIGHBORS_STRUCT_GENERIC(Neighbors_FluidState, grid)
}

__kernel void applyForces(__global float3* velocityGrid, const __global FluidState* fluidStateGridIn, float dt, __constant struct Params* params)
{
	int element = getElement();

	float buoyancyForce = params->temperatureBuoyancy * fluidStateGridIn[element].y;
	float gravityForce = -params->densityWeight * fluidStateGridIn[element].x;
	float3 acceleration = (buoyancyForce + gravityForce) * (float3)(0.0, 1.0, 0.0)
						 -params->drag * velocityGrid[element];

	velocityGrid[element] += acceleration * dt;
}

__kernel void stepVelocityProject_stage1(const __global float3* velocityGridIn, __global float2* divAndP)
{
	Neighbors_float3 n = getNeighbors_float3(velocityGridIn);
	int currentElement = getElement();
	divAndP[currentElement].x = -0.5 * h * (n.e.x - n.w.x + n.s.y - n.n.y + n.u.z - n.d.z);
	divAndP[currentElement].y = 0;
}

__kernel void stepVelocityProject_stage2(__global float2* divAndP)
{
	Neighbors_float2 n = getNeighbors_float2(divAndP);
	divAndP[getElement()].y = (n.c.x + n.w.y + n.e.y + n.n.y + n.s.y + n.u.y + n.d.y) / 6;
}

__kernel void stepVelocityProject_stage3(__global float3* velocityGrid, const __global float2* divAndP)
{
	Neighbors_float2 n = getNeighbors_float2(divAndP);
	int currentElement = getElement();
	float3 pressureGradient = 0.5 * (float3)(n.e.y - n.w.y, n.s.y - n.n.y, n.u.y - n.d.y) / h;
	
	velocityGrid[currentElement] -= pressureGradient;
}

__kernel void coolFluid(__global FluidState* fluidStateGrid, float dt, __constant struct Params* params)
{
	int i = getElement();
	fluidStateGrid[i].y -= fluidStateGrid[i].y * params->coolingRate * dt;
}

__kernel void addFluid(__global FluidState* fluidStateGrid, float4 position, FluidState fluidState)
{
	float3 diff;
	diff.x = (int)get_global_id(0) - position.x;
	diff.y = (int)get_global_id(1) - position.y;
	diff.z = (int)get_global_id(2) - position.z;

	if (fast_length(diff) < brushRadius)
	{
		int element = getElement();
		fluidStateGrid[element].x += fluidState.x; // density accumulates
		fluidStateGrid[element].y = fluidState.y; // temperature set directly
	}
}

__kernel void setFluid(__global FluidState* fluidStateGrid, float4 position, FluidState fluidState)
{
	float3 diff;
	diff.x = (int)get_global_id(0) - position.x;
	diff.y = (int)get_global_id(1) - position.y;
	diff.z = (int)get_global_id(2) - position.z;

	float dist = fast_length(diff);
	
	if (dist < brushRadius)
	{
		float weight = 1;
	
		int element = getElement();
		fluidStateGrid[element].x = max(fluidStateGrid[element].x, fluidState.x * weight);
		fluidStateGrid[element].y = max(fluidStateGrid[element].y, fluidState.y * weight);
	}
}

__kernel void applyImpulse(__global float3* velocityGrid, float4 position, float4 impulse)
{
	float3 diff;
	diff.x = (int)get_global_id(0) - position.x;
	diff.y = (int)get_global_id(1) - position.y;
	diff.z = (int)get_global_id(2) - position.z;

	float dist = fast_length(diff);

	if (dist < brushRadius)
	{
		velocityGrid[getElement()] += impulse.xyz;
	}
}

__kernel void visFluid(__write_only image3d_t image, const __global FluidState* fluidStateGrid, float gammaPower)
{
	int element = getElement(); 

	float density = fluidStateGrid[element].x;
	float temperature = fluidStateGrid[element].y;

	float alpha = clamp(density, 0.0f, 1.0f);
	alpha = pow(alpha, gammaPower);

	float combustion = clamp(temperature * 2 - 1, 0.0f, 1.0f);
	float4 color = (float4)(alpha, combustion, 0, 0);
	write_imagef(image, (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0), color);
}

__kernel void visVelocity(__write_only image3d_t image, const __global float3* velocityGrid)
{
	int element = getElement();
	float3 velocity = velocityGrid[element];
	
	float3 vel = clamp(velocity / 10 + 0.5, 0.0f, 1.0f);
	float4 colour = (float4)(vel.x, vel.y, vel.z, 1);

	write_imagef(image, (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0), (float4)(vel.y));
}
