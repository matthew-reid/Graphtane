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

#define DEFINE_NEIGHBORS_STRUCT(STRUCT_NAME, DATA_TYPE) \
typedef struct \
{ \
	DATA_TYPE c; \
	DATA_TYPE n; \
	DATA_TYPE s; \
	DATA_TYPE e; \
	DATA_TYPE w; \
	DATA_TYPE u; \
	DATA_TYPE d; \
} STRUCT_NAME;

int getElement()
{
	return get_global_id(0) + get_global_id(1) * get_global_size(0) + get_global_id(2) * get_global_size(0) * get_global_size(1);
}

int getElementAt(int x, int y, int z)
{
	x = clamp(x, 0, (int)get_global_size(0) - 1);
	y = clamp(y, 0, (int)get_global_size(1) - 1);
	z = clamp(z, 0, (int)get_global_size(2) - 1);
	return x + y * get_global_size(0) + z * get_global_size(0) * get_global_size(1);
}

float3 getPosition()
{
	return (float3)(get_global_id(0), get_global_id(1), get_global_id(2));
}

#define RETURN_NEIGHBORS_STRUCT_GENERIC(NEIGHBORS_STRUCT_TYPE, GRID) \
	int maxX = get_global_size(0) - 1; \
	int maxY = get_global_size(1) - 1; \
	int maxZ = get_global_size(2) - 1; \
	\
	int strideZ = get_global_size(0) * get_global_size(1); \
	int elementC = getElement(); \
	int elementW = (get_global_id(0) > 0) ? elementC - 1 : elementC; \
	int elementE = (get_global_id(0) < maxX) ? elementC + 1 : elementC; \
	int elementN = (get_global_id(1) > 0) ? elementC - get_global_size(0) : elementC; \
	int elementS = (get_global_id(1) < maxY) ? elementC + get_global_size(0) : elementC; \
	int elementD = (get_global_id(2) > 0) ? elementC - strideZ : elementC; \
	int elementU = (get_global_id(2) < maxZ) ? elementC + strideZ : elementC; \
	\
	NEIGHBORS_STRUCT_TYPE n; \
	n.c = grid[elementC]; \
	n.n = grid[elementN]; \
	n.s = grid[elementS]; \
	n.e = grid[elementE]; \
	n.w = grid[elementW]; \
	n.u = grid[elementU]; \
	n.d = grid[elementD]; \
	return n;