/*
 * PngReader.cpp
 *
 *  Created on: May 24, 2018
 *      Author: hor
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2018  Kai Horstmann
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <png.h>

#include "GLES/TexHelper/PngReader.h"
#include "GLES/ExceptionBase.h"

namespace OevGLES {

PngReader::PngReader(char const *fileName)
	:fileName{fileName}
{ }

PngReader::~PngReader() {}

void PngReader::readPngToTexture(TextureData &textureData) {

	FILE* pngFile = 0;
	png_structp pngPtr = 0;
	png_infop   pngInfo = 0;


	try {

		pngFile = fopen(fileName.c_str(),"rb");

		if (!pngFile) {
			std::ostringstream errMsg;
			errMsg << "Could not open png input file \"" << fileName << "\"";
			throw PngReaderException(errMsg.str().c_str());
		}

		pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
		if (!pngPtr) {
			throw PngReaderException("png_create_read_struct() failed");
		}

		if (setjmp(png_jmpbuf(pngPtr))) {
			throw PngReaderException("Setjump called due to internal png error");
		}

		png_init_io(pngPtr,pngFile);

		png_set_sig_bytes(pngPtr,0);

		png_read_png(pngPtr,pngInfo,PNG_TRANSFORM_EXPAND|PNG_TRANSFORM_STRIP_16|PNG_TRANSFORM_PACKING,0);

		png_uint_32 width = 0,height =0;
		int bitDepth = 0, colorType = 0;
		png_get_IHDR(pngPtr,pngInfo,&width,&height,&bitDepth,&colorType,NULL,NULL,NULL);
#error evaluate the color type and bitDepth, allocate row buffers, and read the data

		// Cleanup
		png_destroy_read_struct(&pngPtr,&pngInfo,NULL);

		fclose (pngFile);


	}
	catch (std::exception const &e) {

		// Perform internal cleanup before re-throwing the exception

		if (pngInfo) {
			png_destroy_info_struct(pngPtr,&pngInfo);
		}

		if (pngPtr) {
			png_destroy_read_struct(&pngPtr,NULL,NULL);
		}

		if (pngFile) {
			fclose (pngFile);
		}

		throw;
	}




}


} /* namespace OevGLES */
