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
#include <memory.h>
#include <sstream>
#include <libpng16/png.h>

#include "GLES/TexHelper/PngReader.h"
#include "GLES/ExceptionBase.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
log4cxx::LoggerPtr PngReader::logger = 0;
#endif

PngReader::PngReader(char const *fileName)
	:fileName{fileName}
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.PngReader");
	}
#endif
}

PngReader::~PngReader() {}

void PngReader::readPngToTexture(TextureData &textureData) {

	FILE			*pngFile = 0;
	png_structp 	pngPtr = 0;
	png_infop   	pngInfo = 0;
	TextureData 	*result = 0;
	png_bytep	 	*rowPointers = 0;


	try {

		pngFile = fopen(fileName.c_str(),"rb");
		LOG4CXX_DEBUG(logger,"Opened PNG file \"" << fileName << "\". pngFile = " << pngFile);

		if (!pngFile) {
			std::ostringstream errMsg;
			errMsg << "Could not open png input file \"" << fileName << "\"";
			throw PngReaderException(errMsg.str().c_str());
		}

		pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
		LOG4CXX_DEBUG(logger,"Created read struct. pngPtr = " << pngPtr);
		if (!pngPtr) {
			throw PngReaderException("png_create_read_struct() failed");
		}

		pngInfo = png_create_info_struct(pngPtr);
		LOG4CXX_DEBUG(logger,"Created info struct. pngInfo = " << pngInfo);
		if (!pngInfo) {
			throw PngReaderException("png_create_info_struct() failed");
		}

		if (setjmp(png_jmpbuf(pngPtr))) {
			LOG4CXX_ERROR(logger,"LibPng called longjmp during reading PNG file");
			throw PngReaderException("longjmp called due to internal png error");
		}

		png_init_io(pngPtr,pngFile);
		LOG4CXX_DEBUG(logger,"Called png_init_io");

		png_set_sig_bytes(pngPtr,0);
		LOG4CXX_DEBUG(logger,"Called png_set_sig_bytes");

		png_read_png(pngPtr,pngInfo,PNG_TRANSFORM_EXPAND|PNG_TRANSFORM_STRIP_16|PNG_TRANSFORM_PACKING,0);
		LOG4CXX_DEBUG(logger,"Called png_read_png");

		png_uint_32 width = 0,height =0;
		int bitDepth = 0, colorType = 0;
		png_get_IHDR(pngPtr,pngInfo,&width,&height,&bitDepth,&colorType,NULL,NULL,NULL);
		LOG4CXX_DEBUG(logger,"Called png_get_IHDR");
		LOG4CXX_DEBUG(logger,"width = "<< width << ", height = "<< height << ", bitDepth = "<< bitDepth << ", colorType = "<< colorType );

		TextureData::GlFormat textureFormat;
		TextureData::DataType textureDataType;
		// Build the texture buffer object according to the information from the PNG file
		switch (colorType) {
		case PNG_COLOR_TYPE_GRAY:
			LOG4CXX_DEBUG(logger,"Color type is PNG_COLOR_TYPE_GRAY");
			if (bitDepth != 8) {
				std::ostringstream os;
				os << "Bit depth of color type PNG_COLOR_TYPE_GRAY is " << bitDepth << ". This depth is not supported.";
				throw PngReaderException(os.str().c_str());
			} else {
				textureFormat = TextureData::Luminance;
				textureDataType = TextureData::Byte;
			}
			break;

		case PNG_COLOR_TYPE_GRAY_ALPHA:
			LOG4CXX_DEBUG(logger,"Color type is PNG_COLOR_TYPE_GRAY_ALPHA");
			if (bitDepth != 8) {
				std::ostringstream os;
				os << "Bit depth of color type PNG_COLOR_TYPE_GRAY_ALPHA is " << bitDepth << ". This depth is not supported.";
				throw PngReaderException(os.str().c_str());
			} else {
				textureFormat = TextureData::LuminanceA;
				textureDataType = TextureData::Byte;
			}
			break;

		case PNG_COLOR_TYPE_RGB:
			LOG4CXX_DEBUG(logger,"Color type is PNG_COLOR_TYPE_RGB");
			if (bitDepth != 8) {
				std::ostringstream os;
				os << "Bit depth of color type PNG_COLOR_TYPE_RGB is " << bitDepth << ". This depth is not supported.";
				throw PngReaderException(os.str().c_str());
			} else {
				textureFormat = TextureData::RGB;
				textureDataType = TextureData::Byte;
			}
			break;

		case PNG_COLOR_TYPE_RGB_ALPHA:
			LOG4CXX_DEBUG(logger,"Color type is PNG_COLOR_TYPE_RGBA");
			if (bitDepth != 8) {
				std::ostringstream os;
				os << "Bit depth of color type PNG_COLOR_TYPE_RGB_ALPHA is " << bitDepth << ". This depth is not supported.";
				throw PngReaderException(os.str().c_str());
			} else {
				textureFormat = TextureData::RGBA;
				textureDataType = TextureData::Byte;
			}
			break;

		default:
			{
				std::ostringstream os;
				os << "Un-supported PNG color type" << colorType;
				LOG4CXX_ERROR(logger,os.str().c_str());
				throw PngReaderException(os.str().c_str());
			}

		}

		textureData = TextureData(width,height,textureFormat,textureDataType);
		png_bytep texDataPtr = png_bytep (textureData.getDataPtr());
		LOG4CXX_DEBUG(logger,"PNG buffer length is " << (png_get_rowbytes(pngPtr,pngInfo) * height) <<
				", the length of the texturedata buffer is " << textureData.getDataBufferLength());
		if (textureData.getDataBufferLength() != png_get_rowbytes(pngPtr,pngInfo) * height) {
			std::ostringstream os;
			os << "Error in PngReader: PNG buffer length is " << (png_get_rowbytes(pngPtr,pngInfo) * height) <<
					" whereas the length of the texturedata buffer is " << textureData.getDataBufferLength();
			throw PngReaderException(os.str().c_str());
		}

		rowPointers = png_get_rows(pngPtr,pngInfo);
		LOG4CXX_DEBUG(logger,"Read image into rows");


		png_bytep currRow = texDataPtr;
		png_size_t bytesPerRow = png_get_rowbytes(pngPtr,pngInfo);

		LOG4CXX_DEBUG(logger,"Bytes per row = " << bytesPerRow);
		LOG4CXX_DEBUG(logger,"Bytes per pixel = " << bytesPerRow/width);


		// Read the rows bottom to top into the texture buffer
		for (int i = height - 1; i >= 0; i--) {
			LOG4CXX_TRACE(logger,"Copy from line " << i << " to line " << ((currRow - texDataPtr) / bytesPerRow));
			memcpy (currRow,rowPointers[i],bytesPerRow);
			currRow += bytesPerRow;
		}

		LOG4CXX_DEBUG(logger,"Copied the buffer");

		// Cleanup
		png_destroy_read_struct(&pngPtr,&pngInfo,NULL);
		LOG4CXX_DEBUG(logger,"Destroyed the PNG structures.");

		fclose (pngFile);


	}
	catch (std::exception const &e) {

		// Perform internal cleanup before re-throwing the exception

		if (rowPointers) {
			delete rowPointers;
		}

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
