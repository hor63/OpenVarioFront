/*
 * ImageReaderBase.h
 *
 *  Created on: Jul 20, 2025
 *      Author: hor
*
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2025  Kai Horstmann
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

#ifndef LIB_GL_GLES_TEXHELPER_IMAGEREADERBASE_H_
#define LIB_GL_GLES_TEXHELPER_IMAGEREADERBASE_H_


#include <string>

#include "GLES/TexHelper/TextureData.h"

namespace OevGLES {

class ImageReaderBase {
public:
	/// \brief Read The image image from a file
	ImageReaderBase(std::string const &fileName);
	/// Read the image from a memory location within the program
	ImageReaderBase(char const *memLocation,int memLength,std::string const &imageName);

	virtual ~ImageReaderBase();

	virtual void readImageToTexture(TextureData &textureData) = 0;
	
	virtual bool checkImageValidity () = 0;
	
protected:

	/// \brief Either the image file name or the image name when \ref memLocation
	/// is not \p nullptr.
	std::string fileName;
	char const *memLocation = nullptr;
	int memLength = 0;
	
	std::string warnMessage;
	std::string errorMessage;


};

} /* namespace OevGLES */

#endif /* LIB_GL_GLES_TEXHELPER_IMAGEREADERBASE_H_ */
