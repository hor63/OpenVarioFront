/*
 * TextureData.h
 *
 *  Created on: May 24, 2018
 *      Author: kai_horstmann
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

#ifndef TEXTUREDATA_H_
#define TEXTUREDATA_H_

#include <GLES2/gl2.h>

namespace OevGLES {

/** \brief Texture data manager
 *
 * Manages the data buffer based on the format (RGB, RGBA, Luminance, Luminance+Alpha), and the data type, (byte per channel or texel packed in an UInt)
 * Row data are tightly packed, and not aligned.
 * Textures should be in any case square and dimensions a power of 2 (128, 256, 512...).
 *
 */
class TextureData {
public:

	/** \brief Format of the texture from OpenGL standpoint
	 * Enums correspond with the GL_XXX constants.
	 * Only supported constants are enumerated here.
	 */
	enum GlFormat {
		UndefFormat	= GL_NONE,
		RGB			= GL_RGB,
		RGBA		= GL_RGBA,
		Luminance	= GL_LUMINANCE,
		LuminanceA	= GL_LUMINANCE_ALPHA
	};

	/** \brief Format of the host data format.
	 * Enums correspond with the GL_UNSIGNED_XXX constants.
	 * Only supported constants are enumerated here.
	 *
	 */
	enum DataType {
		undefType	= GL_NONE,
		Byte 		= GL_UNSIGNED_BYTE,
		Short4444	= GL_UNSIGNED_SHORT_4_4_4_4,
		Short5551	= GL_UNSIGNED_SHORT_5_5_5_1,
		Short565	= GL_UNSIGNED_SHORT_5_6_5
	};

	/** \Brief constructor
	 *
	 * Creates the object. The actual data buffer creation is delayed until \ref getDataPrt or \ref writeData are called
	 *
	 * @param width Width of the texture in Texel
	 * @param height Height of the texture in Texel
	 * @param glFormat Format as \ref GlFormat enum
	 * @param dataFormat Data type of the buffer as \ref DataType enum
	 */
	TextureData(
			GLuint width,
			GLuint height,
			GlFormat glFormat,
			DataType dataFormat
			);

	/** \brief Copy constructor
	 *
	 * A deep copy is performed, i.e. if the data buffer in source was allocated a new buffer is allocated in the target and data is copied.
	 *
	 * @param source Source buffer object.
	 */
	TextureData( TextureData const &source);

	/** \brief destructor
	 *
	 * Deletes the internal buffer.
	 * Pointers to the buffer obtained with \ref getDataPtr point to invalid data afterwards. Do not use.
	 */
	virtual ~TextureData();

	/** \brief Returns pointer to the internal texture data buffer
	 *
	 * The buffer is created if required.
	 *
	 * When it is created luminance or RGB values are initialized to 0, Alpha channels to the max. value of the format.
	 *
	 * When directly writing the data (which is explicitly forseen here) verify your own idea of the length of the data buffer with \ref getDataBufferLength().
	 *
	 * @return Pointer to the texture data
	 */
	void *getDataPtr();

	/** \brief Returns pointer to the internal texture data buffer
	 *
	 * \see void *getDataPtr()
	 *
	 * @return Pointer to the texture data
	 */
	void const *getDataPtr () const;

	/** \brief Writes data into the texture data buffer
	 *
	 * The method is supposed to fill the entire texture.
	 * \ref dataLength is compared with the internally computed length of the buffer. If there is a mismatch an exception is thrown.
	 *
	 * If you want to update parts of the texture obtain the pointer to the internal buffer with \ref getDataPtr, and work on the buffer data yourself.
	 *
	 * @param data
	 * @param dataLength
	 */
	void writeData (
			void * const data,
			GLuint dataLength);

	/**
	 *
	 * @return
	 */
	GLuint getWidth () const {
		return width;
	}
	GLuint getHeight () const {
		return height;
	}

	GlFormat getGlFormat () {
		return glFormat;
	}

	DataType getDataType () const {
		return dataType;
	}

	GLuint getDataBufferLength() const {
		return lenData;
	}


private:

	GLuint width = 0;
	GLuint height = 0;
	GlFormat glFormat = UndefFormat;
	DataType dataType = undefType;

	void *data = 0;
	GLuint lenData = 0;

};

} /* namespace OevGLES */

#endif /* TEXTUREDATA_H_ */
