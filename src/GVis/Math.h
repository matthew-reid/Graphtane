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

#pragma once

#define GLM_FORCE_RADIANS

#include "GVisFwd.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace GVis {

inline glm::mat3 mat3Identity() {return glm::mat3(1,0,0, 0,1,0, 0,0,1);}

inline glm::quat quatIdentity() {return glm::quat(1,0,0,0);}

inline glm::vec3 vec3Zero() {return glm::vec3(0,0,0);}

inline glm::vec4 vec4Zero() {return glm::vec4(0,0,0,0);}

inline glm::vec3 vec3Up() {return glm::vec3(0,1,0);}

inline float pi() {return 3.14159265f;}
inline float halfPi() {return pi() * 0.5f;}
inline float twoPi() {return pi() * 2.0f;}

inline float unitRandom()
{
	return (float)rand() / (float)RAND_MAX;
}

inline float rangeRandom(float low, float high)
{
	return (high-low) * unitRandom() + low;
}

template <typename T> T clamp(const T& value, const T& low, const T& high) 
{
  return value < low ? low : (value > high ? high : value); 
}

} // namespace GVis
