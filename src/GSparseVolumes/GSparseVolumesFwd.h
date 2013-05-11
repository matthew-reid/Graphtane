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

#include <memory>
#include <vector>

namespace GSparseVolumes {

using std::tr1::shared_ptr;

class InternalNode;
class InternalNodeIterator;
class Grid;
class Leaf;
class LeafIterator;
struct RenderableVolume;

typedef shared_ptr<InternalNode> InternalNodePtr;
typedef shared_ptr<InternalNodeIterator> InternalNodeIteratorPtr;
typedef shared_ptr<Leaf> LeafPtr;
typedef shared_ptr<LeafIterator> LeafIteratorPtr;
typedef shared_ptr<Grid> GridPtr;
typedef shared_ptr<RenderableVolume> RenderableVolumePtr;

typedef std::vector<InternalNodePtr> InternalNodes;
typedef std::vector<LeafPtr> Leaves;

} // namespace GSparseVolumes