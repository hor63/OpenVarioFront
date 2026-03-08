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
#include "OVFCommon.h"

#include "GLES/TexHelper/JpegReader.h"
#include "ExceptionBase.h"

#include <jpeglib.h>

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

JpegReader::JpegReader(std::string const &fileName) 
	:ImageReaderBase(fileName)
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.JpegReader");
	}
#endif
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__);
}

JpegReader::JpegReader(
	char const *memLocation,
	int memLength,
	std::string const &imageName)
	:ImageReaderBase(memLocation, memLength, imageName)
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.JpegReader");
	}
#endif

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__);
}

JpegReader::~JpegReader() {}

void JpegReader::setupReadFromFile(jpegDEcompressRef jpegInfo,FILE* &jpegFile){

		jpegFile = fopen(fileName.c_str(),"rb");
		LOG4CXX_DEBUG(logger,"Opened PNG file \"" << fileName 
		<< "\". pngFile pointer = " << jpegFile);

		if (jpegFile == nullptr) {
			errorMessage = 
				fmt::format(fmt::runtime(_(
					"Could not open JPEG file {0}. errno = {1}: {2}")),
					fileName,errno,std::strerror(errno));
			throw JpegReaderException(errorMessage.c_str());
		}

		jpeg_stdio_src(&jpegInfo, jpegFile);
		LOG4CXX_DEBUG(logger,"Called jpeg_stdio_src");
	
};

void JpegReader::setupReadFromMemory(jpegDEcompressRef jpegInfo) {
	
	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
		<< ": memLocation = " << reinterpret_cast<void const *>(memLocation)
		<< ", memLength = " << memLength
		);

	jpeg_mem_src(&jpegInfo, 
		reinterpret_cast<unsigned char const *>(memLocation),
		memLength);

}

/// \brief Store as client_data in the decompression structure.
struct JpegClientData {
	/// \brief Jump buffer for longjmp() out of the error_exit function
	/// 	in case of a non-recoverable error during JPEG decompression.
	std::jmp_buf jumpBuffer;
	JpegReader* jpegReaderObj;
};

void JpegReader::jpegErrorExit (jpegCommonPtr cinfo) {
	auto clientData = reinterpret_cast<JpegClientData*>(cinfo->client_data);
	char buffer[JMSG_LENGTH_MAX];

	// Format and store the error message
	(*cinfo->err->format_message) (cinfo, buffer);
	clientData->jpegReaderObj->errorMessage = buffer;
	
	// get the hell outa here
	std::longjmp(clientData->jumpBuffer,1);

}

void JpegReader::jpegOutputMessage(jpegCommonPtr cinfo)
{
	auto clientData = reinterpret_cast<JpegClientData*>(cinfo->client_data);
	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	cinfo->err->format_message (cinfo, buffer);

	LOG4CXX_INFO (logger,"Message for JPEG image " 
		<< clientData->jpegReaderObj->fileName
		<< ": " << buffer);

}

void JpegReader::jpegEmitMessage(jpegCommonPtr cinfo, int msgLevel)
{
	auto clientData = reinterpret_cast<JpegClientData*>(cinfo->client_data);
	char buffer[JMSG_LENGTH_MAX];
	struct jpeg_error_mgr *err = cinfo->err;

	if (msgLevel < 0) {
		/* It's a warning message.  Since corrupt files may generate many
		 * warnings, I am concatenating them in the JpegReader's warnMessage
		 */
		err->num_warnings++;
		
		// Create the message
		cinfo->err->format_message (cinfo, buffer);
		
		auto newWarnMsg = fmt::format(
			"{}\nWarning #{}: {}",
			clientData->jpegReaderObj->warnMessage,
			err->num_warnings,
			buffer);
		
		clientData->jpegReaderObj->warnMessage = newWarnMsg;

	} else {
		/* It's a trace message.  Show it if trace_level >= msg_level. */
		if (err->trace_level >= msgLevel) {
			// Create the message
			cinfo->err->format_message (cinfo, buffer);
			
			LOG4CXX_TRACE(logger,"Trace for JPEG image " 
				<< clientData->jpegReaderObj->fileName
				<< ", msgLevel = " << msgLevel
				<< ": " << buffer);
		}
	}
}

void JpegReader::readImageToTexture(TextureData &textureData) {

	FILE					*jpegFile = 0;
	jpeg_decompress_struct	jpegInfo;
	jpeg_error_mgr 			jpegError;
	TextureData 			*result = 0;

	memset(&jpegInfo,0,sizeof(jpegInfo));
	memset(&jpegError,0,sizeof(jpegError));

	try {
		JpegClientData clientData {
			.jumpBuffer{0},
			.jpegReaderObj = this
		};

		// Client data will survive jpeg_create_decompress()
		jpegInfo.client_data = &clientData;

		if (setjmp(clientData.jumpBuffer) != 0) {
			// Something catastrophic happened during jpeg decompression
			// which called JpegReader::jpegErrorExit.
			throw JpegReaderException(errorMessage.c_str());
		}

		jpegInfo.err = jpeg_std_error(&jpegError);
		// Set function pointer to my own implementation.
		jpegInfo.err->error_exit = JpegReader::jpegErrorExit;
		jpegInfo.err->emit_message = JpegReader::jpegEmitMessage;
		jpegInfo.err->output_message = JpegReader::jpegOutputMessage;
		if (logger->isTraceEnabled()) {
			jpegInfo.err->trace_level = 5;
		}

		jpeg_create_decompress(&jpegInfo);
		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "Created decompress struct.");

		if (memLocation != nullptr) {
			// switch to reading from memory
			setupReadFromMemory(jpegInfo);
		} else {
			// Setup reading from file; open the file.
			if (fileName.empty()) {
				throw JpegReaderException(fmt::format(fmt::runtime(_(
					"Error: Neither a JPEG file name was set nor in-memrory data provided."))
					).c_str());
			}
			setupReadFromFile(jpegInfo,jpegFile);
		}

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
		LOG4CXX_DEBUG(logger,"\t output_width          = " << jpegInfo.output_width);


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
				auto errMsg = fmt::format(fmt::runtime(_(
					"JPEG image {0}: Un-supported JPEG out_color_space {1}")),
					fileName,
					static_cast<int>(jpegInfo.out_color_space));
				throw JpegReaderException(errMsg.c_str());
			}

		}

		textureData = TextureData(
			jpegInfo.output_width,
			jpegInfo.output_height,
			textureFormat,
			textureDataType);

		// Most of the libjpeg specific stuff is coming directly from
		// https://raw.githubusercontent.com/libjpeg-turbo/libjpeg-turbo/main/src/example.c
		// Output row buffer
		JSAMPARRAY buffer = NULL;
		auto rowStride = jpegInfo.output_width * jpegInfo.output_components;
		
		// Do a sanity check
		LOG4CXX_DEBUG(logger,"\t JPEG rowStride = " << rowStride
			<< ", TextureData stride = " << textureData.getWidth() * textureData.getBytesPerTexel());
		if (rowStride != textureData.getWidth() * textureData.getBytesPerTexel()) {
			auto errMsg = fmt::format(fmt::runtime(_(
				"Error: JPEG rowStride = {0} but textureData stride is {1}")),
				rowStride,
				textureData.getWidth() * textureData.getBytesPerTexel()
				);
			throw JpegReaderException (errMsg.c_str());
		}
		
		buffer = (jpegInfo.mem->alloc_sarray)
			(/*(j_common_ptr)*/reinterpret_cast<j_common_ptr>(&jpegInfo),
			JPOOL_IMAGE, rowStride, 1);

		int textureLineNo = textureData.getHeight() - 1;
		auto textureDataPtr = 
			reinterpret_cast<uint8_t *>(textureData.getDataPtr());
		
		while (jpegInfo.output_scanline < jpegInfo.output_height) {
			if (textureLineNo < 0 ){
				auto errMsg = fmt::format (fmt::runtime(_(
					"Error copying JPEG lines to textureData: "
					"Texture buffer overrun at JPEG scan line #{0} of {1} lines")),
					jpegInfo.output_scanline, jpegInfo.output_height
				);
				throw JpegReaderException (errMsg.c_str());
			}
			
			jpeg_read_scanlines(&jpegInfo, buffer, 1);
			
			auto textureLinePtr = &textureDataPtr[textureLineNo * rowStride];
			
			memcpy(textureLinePtr,buffer[0],rowStride);
			
			textureLineNo --;
		}
		LOG4CXX_DEBUG(logger,"\ttextureLineNo = " << textureLineNo);
		
		if (!warnMessage.empty()) {
			auto errMsg = fmt::format(fmt::runtime(_(
				"Warnings during decoding image {} occured. "
				"The image is probably corrupted. Warnings are: {}")),
				fileName,warnMessage);
			
			throw JpegReaderException(errMsg.c_str());
		}
		
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
		jpeg_destroy_decompress(&jpegInfo);
		if(jpegFile) {
			fclose (jpegFile);
		}

		throw;
	}
}

bool JpegReader::checkImageValidity () {
	FILE					*jpegFile = 0;
	jpeg_decompress_struct	jpegInfo;
	jpeg_error_mgr 			jpegError;
	TextureData 			*result = 0;

	memset(&jpegInfo,0,sizeof(jpegInfo));
	memset(&jpegError,0,sizeof(jpegError));

	try {
		JpegClientData clientData {
			.jumpBuffer{0},
			.jpegReaderObj = this
		};

		// Client data will survive jpeg_create_decompress()
		jpegInfo.client_data = &clientData;

		if (setjmp(clientData.jumpBuffer) != 0) {
			// Something catastrophic happened during jpeg decompression
			// which called JpegReader::jpegErrorExit.
			throw JpegReaderException(errorMessage.c_str());
		}

		jpegInfo.err = jpeg_std_error(&jpegError);
		// Set function pointer to my own implementation.
		jpegInfo.err->error_exit = JpegReader::jpegErrorExit;
		jpegInfo.err->emit_message = JpegReader::jpegEmitMessage;
		jpegInfo.err->output_message = JpegReader::jpegOutputMessage;
		if (logger->isTraceEnabled()) {
			jpegInfo.err->trace_level = 5;
		}

		jpeg_create_decompress(&jpegInfo);
		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "Created decompress struct.");

		if (memLocation != nullptr) {
			// switch to reading from memory
			setupReadFromMemory(jpegInfo);
		} else {
			// Setup reading from file; open the file.
			if (fileName.empty()) {
				throw JpegReaderException(fmt::format(fmt::runtime(_(
					"Error: Neither a JPEG file name was set nor in-memrory data provided."))
					).c_str());
			}
			setupReadFromFile(jpegInfo,jpegFile);
		}

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
		LOG4CXX_DEBUG(logger,"\t output_width          = " << jpegInfo.output_width);



		// Cleanup
		LOG4CXX_DEBUG(logger,"\tCalling jpeg_abort_decompress");
		jpeg_abort_decompress(&jpegInfo);

		LOG4CXX_DEBUG(logger,"\tCalling jpeg_destroy_decompress");
		jpeg_destroy_decompress(&jpegInfo);
		if(jpegFile) {
			fclose (jpegFile);
		}

	}
	catch (std::exception const &e) {

		// Perform internal cleanup before re-throwing the exception
		jpeg_destroy_decompress(&jpegInfo);
		if(jpegFile) {
			fclose (jpegFile);
		}

		LOG4CXX_DEBUG(logger,
			"\tException caught. return false. Exception caused by = " << e.what());

		return false;
	}

	LOG4CXX_DEBUG(logger,"\tJpeg image " << fileName << " seems valid. return true.");

	return true;
}


} /* namespace OevGLES */
