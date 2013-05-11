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

#define RETURN_VALUE_TRILINEAR_GENERIC(DATA_TYPE, GRID, POS) \
	POS = clamp(POS, (float3)(0.001f, 0.001f, 0.001f), (float3)((float)get_global_size(0) - 0.001f, (float)get_global_size(1) - 0.001f, (float)get_global_size(2) - 0.001f)); \
	int x0 = (int)POS.x; \
	int y0 = (int)POS.y; \
	int z0 = (int)POS.z; \
	\
	int maxX = get_global_size(0)-1; \
	int maxY = get_global_size(1)-1; \
	int maxZ = get_global_size(2)-1; \
	\
	int x1 = min(x0 + 1, maxX); \
	int y1 = min(y0 + 1, maxY); \
	int z1 = min(z0 + 1, maxZ); \
	\
	float fracX = POS.x - (float)x0; \
	float fracY = POS.y - (float)y0; \
	float fracZ = POS.z - (float)z0; \
	\
	int strideZ = get_global_size(0) * get_global_size(1); \
	DATA_TYPE v000 = GRID[x0 + y0 * get_global_size(0) + z0 * strideZ]; \
	DATA_TYPE v100 = GRID[x1 + y0 * get_global_size(0) + z0 * strideZ]; \
	DATA_TYPE v010 = GRID[x0 + y1 * get_global_size(0) + z0 * strideZ]; \
	DATA_TYPE v110 = GRID[x1 + y1 * get_global_size(0) + z0 * strideZ]; \
	DATA_TYPE v001 = GRID[x0 + y0 * get_global_size(0) + z1 * strideZ]; \
	DATA_TYPE v101 = GRID[x1 + y0 * get_global_size(0) + z1 * strideZ]; \
	DATA_TYPE v011 = GRID[x0 + y1 * get_global_size(0) + z1 * strideZ]; \
	DATA_TYPE v111 = GRID[x1 + y1 * get_global_size(0) + z1 * strideZ]; \
	\
	DATA_TYPE v00 = v000 + fracX * (v100 - v000); \
	DATA_TYPE v10 = v010 + fracX * (v110 - v010); \
	DATA_TYPE v01 = v001 + fracX * (v101 - v001); \
	DATA_TYPE v11 = v011 + fracX * (v111 - v011); \
	DATA_TYPE v0 = v00 + fracY * (v10 - v00); \
	DATA_TYPE v1 = v01 + fracY * (v11 - v01); \
	return v0 + fracZ * (v1 - v0);