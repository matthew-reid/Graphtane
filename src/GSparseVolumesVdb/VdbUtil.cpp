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

#include "VdbUtil.h"
#include <GCommon/Logger.h>

#include <boost/foreach.hpp>

using namespace GCommon;

namespace GSparseVolumes {

 std::vector<openvdb::GridBase::Ptr> loadGridsFromFile(const std::string& filename, const std::vector<std::string>& gridNames)
 {
	  std::vector<openvdb::GridBase::Ptr> grids;

	// Get first grid from file
	openvdb::initialize();

	openvdb::io::File file(filename);
	file.open();

	defaultLogger()->logLine("Available grids:");
	for (openvdb::io::File::NameIterator nameIterator = file.beginName(); nameIterator != file.endName(); ++nameIterator)
	{
		std::string gridName = *nameIterator;
		defaultLogger()->logLine(gridName);
	}

	if (file.beginName() == file.endName())
	{
		throw std::runtime_error("No grids found in file");
	}

	BOOST_FOREACH(const std::string& gridName, gridNames)
	{
		defaultLogger()->logLine("Loading grid: '" + gridName + "'");

		openvdb::GridBase::Ptr grid = file.readGrid(gridName);
		if (grid)
		{
			grids.push_back(grid);
		}
		else
		{
			throw std::runtime_error("Grid '" + gridName + "' not found");
		}
	}
	file.close();

	return grids;
 }

} // namespace GSparseVolumes