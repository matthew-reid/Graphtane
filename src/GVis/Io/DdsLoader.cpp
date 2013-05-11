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

#include "DdsLoader.h"
#include "../Texture.h"

#include <stdexcept>

#include <boost/scoped_array.hpp>

namespace GVis {

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

TexturePtr DdsLoader::load(const std::string& filename, ColorSpace colorSpace)
{
	unsigned char header[124];

	FILE *fp; 
 
	/* try to open the file */ 
	fp = fopen(filename.c_str(), "rb"); 
	if (fp == NULL) 
	{
		throw std::runtime_error("Could not open file: " + filename);
	}
   
	/* verify the type of file */ 
	char filecode[4]; 
	fread(filecode, 1, 4, fp); 
	if (strncmp(filecode, "DDS ", 4) != 0)
	{ 
		fclose(fp); 
		throw std::runtime_error("File is not in DDS format: " + filename);
	}
	
	/* get the surface desc */ 
	fread(&header, 124, 1, fp); 

	unsigned int height      = *(unsigned int*)&(header[8 ]);
	unsigned int width	     = *(unsigned int*)&(header[12]);
	unsigned int linearSize	 = *(unsigned int*)&(header[16]);
	unsigned int manualMipmapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC      = *(unsigned int*)&(header[80]);

 
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */ 
	bufsize = manualMipmapCount > 1 ? linearSize * 2 : linearSize; 
	boost::scoped_array<unsigned char> buffer(new unsigned char[bufsize]); 
	fread(buffer.get(), 1, bufsize, fp); 
	/* close the file pointer */ 
	fclose(fp);

	unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
	PixelFormat format;
	switch(fourCC) 
	{ 
	case FOURCC_DXT1: 
		format = (colorSpace == ColorSpace_SRGB) ? PixelFormat_DXT1_SRGB_ALPHA : PixelFormat_DXT1;
		break; 
	case FOURCC_DXT3: 
		format = (colorSpace == ColorSpace_SRGB) ? PixelFormat_DXT3_SRGB_ALPHA : PixelFormat_DXT3; 
		break; 
	case FOURCC_DXT5: 
		format = (colorSpace == ColorSpace_SRGB) ? PixelFormat_DXT5_SRGB_ALPHA : PixelFormat_DXT5; 
		break; 
	default: 
		throw std::runtime_error("Unsupported DDS format in file: " + filename);
	}

	// Create texture
	ImageTextureConfig config = ImageTextureConfig::createDefault();
	config.manualMipmapCount.reset();
	config.width = width;
	config.height = height;
	config.format = format;
	if (manualMipmapCount > 1)
	{
		config.manualMipmapCount = manualMipmapCount;
	}
	config.data = buffer.get();

	return TexturePtr(new Texture(config));
}

} // namespace GVis
