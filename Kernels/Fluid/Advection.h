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

#include "Trilinear.h"
#include "Neighbors3d.h"

gentype getValueTrilinear(const __global gentype* grid, float3 pos)
{
	RETURN_VALUE_TRILINEAR_GENERIC(gentype, grid, pos)
}

kernel void advectBacktrace(const __global float3* velocityGridIn, const __global gentype* stateGridIn, __global gentype* stateGridOut, float dt)
{
	int element = getElement();
	float3 prevPosition = getPosition() - velocityGridIn[element] * dt;
	stateGridOut[element] = getValueTrilinear(stateGridIn, prevPosition);
}

gentype clampToNearestNeighbors(const gentype value, const __global gentype* stateGrid, float3 position)
{
	int3 minBound = (int3)(position.x, position.y, position.z);
	
	minBound.x = clamp(minBound.x, 0, (int)get_global_size(0) - 2);
	minBound.y = clamp(minBound.y, 0, (int)get_global_size(1) - 2);
	minBound.z = clamp(minBound.z, 0, (int)get_global_size(2) - 2);
	
	gentype state0 = stateGrid[getElementAt(minBound.x, minBound.y, minBound.z)];
	gentype state1 = stateGrid[getElementAt(minBound.x+1, minBound.y, minBound.z)];
	gentype state2 = stateGrid[getElementAt(minBound.x, minBound.y+1, minBound.z)];
	gentype state3 = stateGrid[getElementAt(minBound.x+1, minBound.y+1, minBound.z)];
	gentype state4 = stateGrid[getElementAt(minBound.x, minBound.y, minBound.z+1)];
	gentype state5 = stateGrid[getElementAt(minBound.x+1, minBound.y, minBound.z+1)];
	gentype state6 = stateGrid[getElementAt(minBound.x, minBound.y+1, minBound.z+1)];
	gentype state7 = stateGrid[getElementAt(minBound.x+1, minBound.y+1, minBound.z+1)];

	gentype minState = min(min(min(min(min(min(min(state0, state1), state2), state3), state4), state5), state6), state7);
	gentype maxState = max(max(max(max(max(max(max(state0, state1), state2), state3), state4), state5), state6), state7);
	
	return clamp(value, minState, maxState);
}

kernel void applyMacCormackCorrection(const __global float3* velocityGridIn, const __global gentype* forwardAdvected, const __global gentype* backwardAdvactedFromForwardAdvected,
							   const __global gentype* stateGridIn, __global gentype* stateGridOut, float dt)
{
	int element = getElement();
	float macCormackCorrectionStrength = 0.8; // [0, 1]
	gentype corrected = forwardAdvected[element] + 0.5 * macCormackCorrectionStrength * (stateGridIn[element] - backwardAdvactedFromForwardAdvected[element]);
	
	// Clamp semi-Lagrangian particle state values to be within neighbours (prevents instability)
	float3 prevPosition = getPosition() - velocityGridIn[element] * dt;
	stateGridOut[element] = clampToNearestNeighbors(corrected, stateGridIn, prevPosition);
}