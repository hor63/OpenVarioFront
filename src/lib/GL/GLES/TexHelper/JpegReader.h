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

#include "ImageReaderBase.h"

// #include <jpeglib.h>
// Forward declaration is sufficient here.
typedef struct jpeg_decompress_struct &jpegDEcompressRef;
typedef struct jpeg_common_struct *jpegCommonPtr;

namespace OevGLES {

class JpegReader :public ImageReaderBase {
public:
	/// \brief Read The image image from a file
	JpegReader(std::string const &fileName);
	/// Read the image from a memory location within the program
	JpegReader(char const *memLocation,int memLength,std::string const &imageName);
	
	virtual ~JpegReader();

	virtual void readImageToTexture(TextureData &textureData) override;

private:

	static void jpegErrorExit (jpegCommonPtr cinfo);

	void setupReadFromFile(jpegDEcompressRef jpegInfo,FILE* &jpegFile);
	void setupReadFromMemory(jpegDEcompressRef jpegInfo);


};

} /* namespace OevGLES */

#endif /* JpegReader_H_ */
