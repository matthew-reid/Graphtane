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

#include "FileFinder.h"

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

using namespace GCommon;

FileFinder::FileFinder(const std::vector<std::string>& searchPaths) :
	m_searchPaths(searchPaths)
{
}

std::string FileFinder::find(const std::string& filename) const
{
	std::string result;
	
	boost::filesystem::path path(filename);
	if (path.is_absolute())
	{
		if (boost::filesystem::exists(filename))
		{
			return filename;
		}
	}
	else // relative path
	{
		BOOST_FOREACH(const std::string& path, m_searchPaths)
		{
			std::string tryPath = path + "/" + filename;
			if (boost::filesystem::exists(tryPath))
			{
				return tryPath;
			}
		}
	}

	throw std::runtime_error("File not found in any search paths: " + filename);
}