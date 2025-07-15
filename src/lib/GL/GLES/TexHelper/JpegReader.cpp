/*
 * JpegReader.cpp
 *
 *  Created on: Created on: Jul 13, 2025
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


#include "glib.h"
#include <csetjmp>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include "OVFCommon.h"

#include "gettext.h"
#include "fmt/format.h"

#include "GLES/TexHelper/JpegReader.h"
#include "GLES/ExceptionBase.h"

#include <jpeglib.h>

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

JpegReader::JpegReader(std::string const &fileName)
	:fileName{fileName}
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.JpegReader");
	}
#endif
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
		<< ", fileName = " << this->fileName);
}

JpegReader::JpegReader(
	char const *memLocation,
	int memLength,
	std::string const &imageName)
	:memLocation{memLocation},
	memLength{memLength},
	fileName{imageName} 
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.JpegReader");
	}
#endif
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
		<< ": memLocation = "
		<< reinterpret_cast<void const *>(this->memLocation)
		<< ", lenPNGData = " << this->memLength
		<< ", fileName = " << this->fileName);
}

JpegReader::~JpegReader() {}

/*
void JpegReader::readPngDataFromMemoryCallback(jpeg_decompress_struct& jpegInfo,
							unsigned char* data, size_t dataLength) {

	JpegReader* tis = reinterpret_cast<JpegReader*>(png_get_io_ptr(pngPtr));

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
				_("Error reading PNG image {0} from memory."
				" Requested length is {1} bytes, but only {2} bytes of {3} are left for reading."),
				tis->fileName,dataLength,(tis->memLength - tis->posInMemLocation),
				tis->memLength
			);
		LOG4CXX_ERROR(logger,"\t" << tis->errorMessage);
		png_error(pngPtr,
			tis->errorMessage.c_str());
	}

}
*/

/*
void JpegReader::pngErrorCallback(jpeg_decompress_struct& jpegInfo,char const* errorMsg){
	JpegReader* tis = reinterpret_cast<JpegReader*>(png_get_error_ptr(pngPtr));

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__ << ": errorMsg = " << errorMsg);
	LOG4CXX_DEBUG(logger,"\t errorMsg ptr = " << reinterpret_cast<const void*>(errorMsg)
		<< " tis->errorMessage ptr = " 
		<< reinterpret_cast<const void*>(tis->errorMessage.c_str()));
	
	if (tis->errorMessage.c_str() != errorMsg) {
	tis->errorMessage = errorMsg;
	}
	
	longjmp(png_jmpbuf(pngPtr), 2);
}

void JpegReader::pngWarningCallback(jpeg_decompress_struct& jpegInfo,char const* warnMsg){
	JpegReader* tis = reinterpret_cast<JpegReader*>(png_get_error_ptr(pngPtr));

	tis->warnMesage = warnMsg;
	
}
*/

void JpegReader::setupReadFromFile(jpeg_decompress_struct& jpegInfo,FILE* &jpegFile){

		jpegFile = fopen(fileName.c_str(),"rb");
		LOG4CXX_DEBUG(logger,"Opened PNG file \"" << fileName 
		<< "\". pngFile pointer = " << jpegFile);

		if (jpegFile == nullptr) {
			errorMessage = 
				fmt::format(_(
					"Could not open JPEG file {0}. errno = {1}: {2}"),
					fileName,errno,std::strerror(errno));
			throw JpegReaderException(errorMessage.c_str());
		}

		jpeg_stdio_src(&jpegInfo, jpegFile);
		LOG4CXX_DEBUG(logger,"Called jpeg_stdio_src");
	
};

void JpegReader::setupReadFromMemory(jpeg_decompress_struct& jpegInfo) {
	
	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__);
	throw JpegReaderException("JPEG from memory is not supported yet.");
/*

	png_set_read_fn(jpegInfo,this,
		readPngDataFromMemoryCallback);
	png_init_io(pngPtr,reinterpret_cast<FILE*>(this));
*/

}

void JpegReader::readJpegToTexture(TextureData &textureData) {

	FILE					*jpegFile = 0;
	jpeg_decompress_struct	jpegInfo;
	jpeg_error_mgr 			jpegError;
	TextureData 			*result = 0;

	memset(&jpegInfo,0,sizeof(jpegInfo));
	memset(&jpegError,0,sizeof(jpegError));

	

	try {

		jpegInfo.err = jpeg_std_error(&jpegError);

		jpeg_create_decompress(&jpegInfo);
		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "Created decompress struct.");

		if (memLocation != nullptr) {
			// switch to reading from memory
			setupReadFromMemory(jpegInfo);
		} else {
			// Setup reading from file; open the file.
			if (fileName.empty()) {
				throw JpegReaderException(fmt::format(
					_("Error: Neither a JPEG file name was set nor in-memrory data provided.")
					).c_str());
			}
			setupReadFromFile(jpegInfo,jpegFile);
		}

/*
		if (setjmp(png_jmpbuf(pngPtr))) {
			LOG4CXX_ERROR(logger,"LibPng called longjmp during reading PNG file with message: "
			<< errorMessage);
			throw JpegReaderException(
				fmt::format (
					_("Error reading PNG file {0}: {1}"),
					fileName,errorMessage).c_str());
		}

		png_set_sig_bytes(pngPtr,0);
		LOG4CXX_DEBUG(logger,"Called png_set_sig_bytes");
*/

		jpeg_read_header(&jpegInfo, TRUE);

		LOG4CXX_DEBUG(logger,"\tImage size = "
			<< jpegInfo.image_width << 'x' << jpegInfo.image_height);
			
		LOG4CXX_DEBUG(logger,"\tout_color_space = " << static_cast<int>(jpegInfo.out_color_space));
		
		// Select the format for me.
		// I only support greyscale or RGB, no Alpha channel, which is poorly
		// defined for JPEG anyway.
		// If I need an image with Alpha channel I will use PNG.
		// Natural images like photos, art are the natural domain of JPEG. But
		// they usually do not have an Alpha channel.
		if (jpegInfo.jpeg_color_space == JCS_GRAYSCALE) {
			jpegInfo.out_color_space = JCS_GRAYSCALE;
		} else {
			jpegInfo.out_color_space = JCS_RGB;
		}
		// I do not want color maps.
		jpegInfo.quantize_colors = FALSE;
		// I will do only one reading pass over the image.
		jpegInfo.buffered_image = FALSE;

		jpeg_start_decompress(&jpegInfo);
		LOG4CXX_DEBUG(logger,"\tAfter decompressing: Image size = "
			<< jpegInfo.output_width << 'x' << jpegInfo.output_height);
		LOG4CXX_DEBUG(logger,"\t dct_method            = " << jpegInfo.dct_method);
		LOG4CXX_DEBUG(logger,"\t do_fancy_upsampling   = " << jpegInfo.do_fancy_upsampling);
		LOG4CXX_DEBUG(logger,"\t do_block_smoothing    = " << jpegInfo.do_block_smoothing);
		LOG4CXX_DEBUG(logger,"\t enable_1pass_quant    = " << jpegInfo.enable_1pass_quant);
		LOG4CXX_DEBUG(logger,"\t enable_external_quant = " << jpegInfo.enable_external_quant);
		LOG4CXX_DEBUG(logger,"\t enable_2pass_quant    = " << jpegInfo.enable_2pass_quant);
		LOG4CXX_DEBUG(logger,"\t rec_outbuf_heigh      = " << jpegInfo.rec_outbuf_height);
		LOG4CXX_DEBUG(logger,"\t out_color_components  = " << jpegInfo.out_color_components);
		LOG4CXX_DEBUG(logger,"\t data_precision        = " << jpegInfo.data_precision);
		LOG4CXX_DEBUG(logger,"\t out_color_components  = " << jpegInfo.out_color_components);
		LOG4CXX_DEBUG(logger,"\t output_components     = " << jpegInfo.output_components);

		jpeg_read_scanlines

		TextureData::GlFormat textureFormat;
		TextureData::DataType textureDataType;
		// Build the texture buffer object according to the information from the PNG file
		switch (jpegInfo.out_color_space) {
		case JCS_GRAYSCALE:
			LOG4CXX_DEBUG(logger,"Color type is JCS_GRAYSCALE");
			textureFormat = TextureData::Luminance;
			textureDataType = TextureData::Byte;
			break;

		case JCS_RGB:
			LOG4CXX_DEBUG(logger,"Color type is JCS_RGB");
			textureFormat = TextureData::RGB;
			textureDataType = TextureData::Byte;
			break;


		default:
			{
				auto errMsg = fmt::format(
					"JPEG image {0}: Un-supported JPEG out_color_space {1}",
					fileName,jpegInfo.out_color_space);
				throw JpegReaderException(errMsg.c_str());
			}

		}

		textureData = TextureData(
			jpegInfo.output_width,
			jpegInfo.output_height,
			textureFormat,
			textureDataType);
			
			
		png_bytep texDataPtr = png_bytep (textureData.getDataPtr());
		LOG4CXX_DEBUG(logger,"PNG buffer length is " << (png_get_rowbytes(pngPtr,pngInfo) * height) <<
				", the length of the texturedata buffer is " << textureData.getDataBufferLength());
		if (textureData.getDataBufferLength() != png_get_rowbytes(pngPtr,pngInfo) * height) {
			std::ostringstream os;
			os << "Error in JpegReader: PNG buffer length is " << (png_get_rowbytes(pngPtr,pngInfo) * height) <<
					" whereas the length of the texturedata buffer is " << textureData.getDataBufferLength();
			throw JpegReaderException(os.str().c_str());
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
*/

		// Cleanup
		LOG4CXX_DEBUG(logger,"\tCalling jpeg_finish_decompress");
		jpeg_finish_decompress(&jpegInfo);

		LOG4CXX_DEBUG(logger,"\tCalling jpeg_destroy_decompress");
		jpeg_destroy_decompress(&jpegInfo);
		if(jpegFile) {
			fclose (jpegFile);
		}

	}
	catch (std::exception const &e) {

		// Perform internal cleanup before re-throwing the exception
		jpeg_finish_decompress(&jpegInfo);
		jpeg_destroy_decompress(&jpegInfo);
		if(jpegFile) {
			fclose (jpegFile);
		}

		throw;
	}

	throw JpegReaderException("Testing only");


}


} /* namespace OevGLES */
