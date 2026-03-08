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
#include "OVFCommon.h"

#include "GLES/TexHelper/ImageReaderBase.h"

#include "GLES/TexHelper/PngReader.h"
#include "ExceptionBase.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

PngReader::PngReader(std::string const &fileName) 
	:ImageReaderBase(fileName)
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.PngReader");
	}
#endif

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__);
}

PngReader::PngReader(
	char const *memLocation,
	int memLength,
	std::string const &imageName)
	:ImageReaderBase(memLocation, memLength, imageName)
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.PngReader");
	}
#endif

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__);
}

PngReader::~PngReader() {}

void PngReader::readPngDataFromMemoryCallback(png_struct* pngPtr,
							unsigned char* data, size_t dataLength) {

	PngReader* tis = reinterpret_cast<PngReader*>(png_get_io_ptr(pngPtr));

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__ << ": tis = " << tis
		<< ", data = " << reinterpret_cast<void const*>(data)
		<< ", length = " << dataLength
		<< ", memLocation = " << reinterpret_cast<void const*>(tis->memLocation)
		<< ", memLength " << tis->memLength
		<< ", posInMemLocation " << tis->posInMemLocation
		);

	if (dataLength <= (tis->memLength - tis->posInMemLocation)) {
		std::memcpy(data,&tis->memLocation[tis->posInMemLocation],dataLength);
		tis->posInMemLocation += dataLength;
	} else {
		tis->errorMessage = fmt::format(
				fmt::runtime(_("Error reading PNG image {0} from memory."
				" Requested length is {1} bytes, but only {2} bytes of {3} are left for reading.")),
				tis->fileName,dataLength,(tis->memLength - tis->posInMemLocation),
				tis->memLength
			);
		LOG4CXX_ERROR(logger,"\t" << tis->errorMessage);
		png_error(pngPtr,
			tis->errorMessage.c_str());
	}

}

void PngReader::pngErrorCallback(png_struct* pngPtr,char const* errorMsg){
	PngReader* tis = reinterpret_cast<PngReader*>(png_get_error_ptr(pngPtr));

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__ << ": errorMsg = " << errorMsg);
	LOG4CXX_DEBUG(logger,"\t errorMsg ptr = " << reinterpret_cast<const void*>(errorMsg)
		<< " tis->errorMessage ptr = " 
		<< reinterpret_cast<const void*>(tis->errorMessage.c_str()));
	
	if (tis->errorMessage.c_str() != errorMsg) {
	tis->errorMessage = errorMsg;
	}
	
	longjmp(png_jmpbuf(pngPtr), 2);
}

void PngReader::pngWarningCallback(png_struct* pngPtr,char const* warnMsg){
	PngReader* tis = reinterpret_cast<PngReader*>(png_get_error_ptr(pngPtr));

	tis->warnMessage = warnMsg;
	
}

void PngReader::setupReadFromFile(png_struct* pngPtr,FILE* &pngFile){

		pngFile = fopen(fileName.c_str(),"rb");
		LOG4CXX_DEBUG(logger,"Opened PNG file \"" << fileName 
		<< "\". pngFile pointer = " << pngFile);

		if (pngFile == nullptr) {
			errorMessage = 
				fmt::format(fmt::runtime(_(
					"Could not open PNG file {0}. errno = {1}: {2}")),
					fileName,errno,std::strerror(errno));
			throw PngReaderException(errorMessage.c_str());
		}

		png_init_io(pngPtr,pngFile);
		LOG4CXX_DEBUG(logger,"Called png_init_io");
	
};
void PngReader::setupReadFromMemory(png_struct* pngPtr) {
	
	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__);

	posInMemLocation = 0;
	png_set_read_fn(pngPtr,this,
		readPngDataFromMemoryCallback);
	png_init_io(pngPtr,reinterpret_cast<FILE*>(this));

}


void PngReader::readImageToTexture(TextureData &textureData) {

	FILE			*pngFile = 0;
	png_structp 	pngPtr = 0;
	png_infop   	pngInfo = 0;
	TextureData 	*result = 0;
	png_bytep	 	*rowPointers = 0;


	try {

		pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
		LOG4CXX_DEBUG(logger,"Created read struct. pngPtr = " << pngPtr);
		if (!pngPtr) {
			throw PngReaderException("png_create_read_struct() failed.");
		}

		pngInfo = png_create_info_struct(pngPtr);
		LOG4CXX_DEBUG(logger,"Created info struct. pngInfo = " << pngInfo);
		if (!pngInfo) {
			throw PngReaderException("png_create_info_struct() failed.");
		}

		png_set_error_fn(pngPtr,
			this, pngErrorCallback,
			pngWarningCallback);

		if (memLocation != nullptr) {
			// switch to reading from memory
			setupReadFromMemory(pngPtr);
		} else {
			// Setup reading from file; open the file.
			if (fileName.empty()) {
				throw PngReaderException(fmt::format(fmt::runtime(_(
					"Error: Neither a PNG file name was set nor in-memrory data was provided."))
					).c_str());
			}
			setupReadFromFile(pngPtr,pngFile);
		}

		if (setjmp(png_jmpbuf(pngPtr))) {
			LOG4CXX_ERROR(logger,"LibPng called longjmp during reading PNG file with message: "
			<< errorMessage);
			throw PngReaderException(
				fmt::format (
					fmt::runtime(_("Error reading PNG image {0}: {1}")),
					fileName,errorMessage).c_str());
		}

		png_set_sig_bytes(pngPtr,0);
		LOG4CXX_DEBUG(logger,"Called png_set_sig_bytes");

		png_read_png(pngPtr,pngInfo,
			PNG_TRANSFORM_EXPAND // expand index to palettes to RGB
			|PNG_TRANSFORM_STRIP_16 // truncate 16-bit samples to 8 bit
			|PNG_TRANSFORM_PACKING // Expand < 8 bit samples to 8 bit
			,0);
		LOG4CXX_DEBUG(logger,"Called png_read_png");

		png_uint_32 width = 0,height =0;
		int bitDepth = 0, colorType = 0;
		png_get_IHDR(pngPtr,pngInfo,&width,
			&height,&bitDepth,&colorType,
			NULL,NULL,NULL);
		LOG4CXX_DEBUG(logger,"Called png_get_IHDR");
		LOG4CXX_DEBUG(logger,"width = "<< width << ", height = "<< height 
			<< ", bitDepth = "<< bitDepth << ", colorType = "<< colorType );

		TextureData::GlFormat textureFormat;
		TextureData::DataType textureDataType;
		// Build the texture buffer object according to the information from the PNG file
		switch (colorType) {
		case PNG_COLOR_TYPE_GRAY:
			LOG4CXX_DEBUG(logger,"Color type is PNG_COLOR_TYPE_GRAY");
			if (bitDepth != 8) {
				auto errMsg = fmt::format(fmt::runtime(_(
					"PNG image {0}: Bit depth of color type {1} is {2}. This depth is not supported.")),
					fileName,"PNG_COLOR_TYPE_GRAY",bitDepth);
				throw PngReaderException(errMsg.c_str());
			} else {
				textureFormat = TextureData::Luminance;
				textureDataType = TextureData::Byte;
			}
			break;

		case PNG_COLOR_TYPE_GRAY_ALPHA:
			LOG4CXX_DEBUG(logger,"Color type is PNG_COLOR_TYPE_GRAY_ALPHA");
			if (bitDepth != 8) {
				auto errMsg = fmt::format(fmt::runtime(_(
					"PNG image {0}: Bit depth of color type {1} is {2}. This depth is not supported.")),
					fileName,"PNG_COLOR_TYPE_GRAY_ALPHA",bitDepth);
				throw PngReaderException(errMsg.c_str());
			} else {
				textureFormat = TextureData::LuminanceA;
				textureDataType = TextureData::Byte;
			}
			break;

		case PNG_COLOR_TYPE_RGB:
			LOG4CXX_DEBUG(logger,"Color type is PNG_COLOR_TYPE_RGB");
			if (bitDepth != 8) {
				auto errMsg = fmt::format(fmt::runtime(_(
					"PNG image {0}: Bit depth of color type {1} is {2}. This depth is not supported.")),
					fileName,"PNG_COLOR_TYPE_RGB",bitDepth);
				throw PngReaderException(errMsg.c_str());
			} else {
				textureFormat = TextureData::RGB;
				textureDataType = TextureData::Byte;
			}
			break;

		case PNG_COLOR_TYPE_RGB_ALPHA:
			LOG4CXX_DEBUG(logger,"Color type is PNG_COLOR_TYPE_RGBA");
			if (bitDepth != 8) {
				auto errMsg = fmt::format(fmt::runtime(_(
					"PNG image {0}: Bit depth of color type {1} is {2}. This depth is not supported.")),
					fileName,"PNG_COLOR_TYPE_RGB_ALPHA",bitDepth);
				throw PngReaderException(errMsg.c_str());
			} else {
				textureFormat = TextureData::RGBA;
				textureDataType = TextureData::Byte;
			}
			break;

		default:
			{
				auto errMsg = fmt::format(fmt::runtime(
					_("PNG image {0}: Un-supported PNG color type {1}")),
					fileName,colorType);
				throw PngReaderException(errMsg.c_str());
			}

		}

		textureData = TextureData(width,height,textureFormat,textureDataType);
		png_bytep texDataPtr = png_bytep (textureData.getDataPtr());
		LOG4CXX_DEBUG(logger,"PNG buffer length is " << (png_get_rowbytes(pngPtr,pngInfo) * height) <<
				", the length of the texturedata buffer is " << textureData.getDataBufferLength());
		if (textureData.getDataBufferLength() != png_get_rowbytes(pngPtr,pngInfo) * height) {
			auto errMsg = fmt::format(fmt::runtime(_(
				"Error in PNG image {0}: PNG buffer length is {1} but the length of the texture data buffer is {2}."
				)),
				fileName,(png_get_rowbytes(pngPtr,pngInfo) * height),
				textureData.getDataBufferLength());
			throw PngReaderException(errMsg.c_str());
		}

		rowPointers = png_get_rows(pngPtr,pngInfo);
		LOG4CXX_DEBUG(logger,"Read image into rows");


		png_bytep currRow = texDataPtr;
		png_size_t bytesPerRow = png_get_rowbytes(pngPtr,pngInfo);

		LOG4CXX_DEBUG(logger,"Bytes per row = " << bytesPerRow);
		LOG4CXX_DEBUG(logger,"Bytes per pixel = " << bytesPerRow/width);


		// Read the rows bottom to top into the texture buffer
		LOG4CXX_DEBUG(logger,"Copy the buffer");
		for (int i = height - 1; i >= 0; i--) {
			LOG4CXX_TRACE(logger,"Copy from line " << i << " to line " << ((currRow - texDataPtr) / bytesPerRow));
			memcpy (currRow,rowPointers[i],bytesPerRow);
			currRow += bytesPerRow;
		}

		// Cleanup
		LOG4CXX_DEBUG(logger,"Destroy the PNG structures.");
		png_destroy_read_struct(&pngPtr,&pngInfo,NULL);

		if(pngFile) {
			fclose (pngFile);
		}


	}
	catch (std::exception const &e) {

		LOG4CXX_ERROR(logger,__PRETTY_FUNCTION__ 
			<< "Exception: " << e.what());

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

bool PngReader::checkImageValidity () {

	FILE			*pngFile = 0;
	png_structp 	pngPtr = 0;
	png_infop   	pngInfo = 0;


	try {

		pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
		LOG4CXX_DEBUG(logger,"Created read struct. pngPtr = " << pngPtr);
		if (!pngPtr) {
			throw PngReaderException("png_create_read_struct() failed.");
		}

		pngInfo = png_create_info_struct(pngPtr);
		LOG4CXX_DEBUG(logger,"Created info struct. pngInfo = " << pngInfo);
		if (!pngInfo) {
			throw PngReaderException("png_create_info_struct() failed.");
		}

		png_set_error_fn(pngPtr,
			this, pngErrorCallback,
			pngWarningCallback);

		if (memLocation != nullptr) {
			// switch to reading from memory
			setupReadFromMemory(pngPtr);
		} else {
			// Setup reading from file; open the file.
			if (fileName.empty()) {
				throw PngReaderException(fmt::format(fmt::runtime(_(
					"Error: Neither a PNG file name was set nor in-memrory data was provided."))
					).c_str());
			}
			setupReadFromFile(pngPtr,pngFile);
		}

		if (setjmp(png_jmpbuf(pngPtr))) {
			LOG4CXX_ERROR(logger,"LibPng called longjmp during reading PNG file with message: "
			<< errorMessage);
			throw PngReaderException(
				fmt::format (fmt::runtime(
					_("Error reading PNG image {0}: {1}")),
					fileName,errorMessage).c_str());
		}

		png_set_sig_bytes(pngPtr,0);
		LOG4CXX_DEBUG(logger,"Called png_set_sig_bytes");

		png_read_png(pngPtr,pngInfo,
			PNG_TRANSFORM_EXPAND // expand index to palettes to RGB
			|PNG_TRANSFORM_STRIP_16 // truncate 16-bit samples to 8 bit
			|PNG_TRANSFORM_PACKING // Expand < 8 bit samples to 8 bit
			,0);
		LOG4CXX_DEBUG(logger,"Called png_read_png");

		png_uint_32 width = 0,height =0;
		int bitDepth = 0, colorType = 0;
		png_get_IHDR(pngPtr,pngInfo,&width,
			&height,&bitDepth,&colorType,
			NULL,NULL,NULL);
		LOG4CXX_DEBUG(logger,"Called png_get_IHDR");
		LOG4CXX_DEBUG(logger,"width = "<< width << ", height = "<< height 
			<< ", bitDepth = "<< bitDepth << ", colorType = "<< colorType );


		// Cleanup
		LOG4CXX_DEBUG(logger,"Destroy the PNG structures.");
		png_destroy_read_struct(&pngPtr,&pngInfo,NULL);

		if(pngFile) {
			fclose (pngFile);
		}


	}
	catch (std::exception const &e) {

		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ 
			<< "Exception: " << e.what());

		if (pngInfo) {
			png_destroy_info_struct(pngPtr,&pngInfo);
		}

		if (pngPtr) {
			png_destroy_read_struct(&pngPtr,NULL,NULL);
		}

		if (pngFile) {
			fclose (pngFile);
		}

		return false;
	}

	return true;
}


} /* namespace OevGLES */
