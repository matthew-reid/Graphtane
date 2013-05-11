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

#define RETURN_VALUE_BILINEAR_GENERIC(DATA_TYPE, GRID, POS) \
	int x0 = (int)POS.x; \
	int y0 = (int)POS.y; \
	\
	x0 = clamp(x0, 0, (int)get_global_size(0)-2); \
	y0 = clamp(y0, 0, (int)get_global_size(1)-2); \
	int x1 = x0 + 1; \
	int y1 = y0 + 1; \
	\
	float fracX = POS.x - (float)x0; \
	float fracY = POS.y - (float)y0; \
	\
	DATA_TYPE v00 = GRID[x0 + y0 * get_global_size(0)]; \
	DATA_TYPE v01 = GRID[x0 + y1 * get_global_size(0)]; \
	DATA_TYPE v10 = GRID[x1 + y0 * get_global_size(0)]; \
	DATA_TYPE v11 = GRID[x1 + y1 * get_global_size(0)]; \
	\
	DATA_TYPE v0 = mix(v00, v10, fracX); \
	DATA_TYPE v1 = mix(v01, v11, fracX); \
	return mix(v0, v1, fracY);