/*
 * JpegReader.h
 *
 *  Created on: Jul 13, 2025
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

#ifndef JpegReader_H_
#define JpegReader_H_

#include <string>

#include "GLES/TexHelper/TextureData.h"

#include <jpeglib.h>

namespace OevGLES {

/**
 *
 *
 * \see https://raw.githubusercontent.com/libjpeg-turbo/libjpeg-turbo/main/doc/libjpeg.txt
*/
class JpegReader {
public:
	/// \brief Read a PNG image from a file
	JpegReader(std::string const &fileName);
	/// Read a PNG image from a memory location within the program
	JpegReader(char const *memLocation,int memLength,std::string const &imageName);
	virtual ~JpegReader();

	void readJpegToTexture(TextureData &textureData);

private:

	static void pngErrorCallback(jpeg_decompress_struct& jpegInfo,char const* error_msg);
	static void pngWarningCallback(jpeg_decompress_struct& jpegInfo,char const* warn_msg);

	static void readPngDataFromMemoryCallback(jpeg_decompress_struct& jpegInfo,
							unsigned char* data, size_t dataLength);	
	void setupReadFromFile(jpeg_decompress_struct& jpegInfo,FILE* &pngFile);
	void setupReadFromMemory(jpeg_decompress_struct& jpegInfo);


	/// \brief Either the image file name or the image name when \ref memLocation
	/// is not \p nullptr.
	std::string fileName;
	char const *memLocation = nullptr;
	int memLength = 0;
	int posInMemLocation = 0;
	
	std::string warnMesage;
	std::string errorMessage;

};

} /* namespace OevGLES */

#endif /* JpegReader_H_ */
