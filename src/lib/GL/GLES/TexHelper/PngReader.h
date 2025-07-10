/*
 * PngReader.h
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

#ifndef PNGREADER_H_
#define PNGREADER_H_

#include <string>

#include "GLES/TexHelper/TextureData.h"

#include <libpng16/png.h>

namespace OevGLES {

class PngReader {
public:
	/// \brief Read a PNG image from a file
	PngReader(std::string const &fileName);
	/// Read a PNG image from a memory location within the program
	PngReader(char const *memLocation,int memLength,std::string const &imageName);
	virtual ~PngReader();

	void readPngToTexture(TextureData &textureData);

private:

	static void pngErrorCallback(png_struct* pngPtr,char const* error_msg);
	static void pngWarningCallback(png_struct* pngPtr,char const* warn_msg);

	static void readPngDataFromMemoryCallback(png_struct* pngPtr,
							unsigned char* data, size_t dataLength);	
	void setupReadFromFile(png_struct* pngPtr,FILE* &pngFile);
	void setupReadFromMemory(png_struct* pngPtr);


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

#endif /* PNGREADER_H_ */
