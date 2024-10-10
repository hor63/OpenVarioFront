/*
 * TextureData.cpp
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

#include <memory.h>

#include "GLES/TexHelper/TextureData.h"

#include "GLES/ExceptionBase.h"

namespace OevGLES {


TextureData::~TextureData() {

	if (data) {
		delete data;
	}

}

TextureData::TextureData(
		GLuint width,
		GLuint height,
		GlFormat glFormat,
		DataType dataType
		)
	: width{width},
	  height{height},
	glFormat{glFormat},
	dataType{dataType}
	{

	// Check data type against the texture format. Some combinations are not allowed
	switch (dataType) {
	case Byte:
		// No restrictions to the format
		// but the number pf bytes per texel varies heavily
		switch (glFormat) {
		case RGB:
			bytesPerTexel = 3;
			break;
		case RGBA:
			bytesPerTexel = 4;
			break;
		case Luminance:
			bytesPerTexel = 1;
			break;
		case LuminanceA:
			bytesPerTexel = 2;
			break;
		default:
			throw TextureException("TextureData::TextureData: Datatype undefined!");
		}
		break;
	case Short4444:
		// fall through
	case Short5551:
		if (glFormat != RGBA) {
			throw TextureException("TextureData::TextureData: For dataType Short4444 and Short5551 the glFormat must be RGBA!");
		}
		bytesPerTexel = 2;
		break;
	case Short565:
		if (glFormat != RGB) {
			throw TextureException("TextureData::TextureData: For dataType and Short565 the glFormat must be RGB!");
		}
		bytesPerTexel = 2;
		break;
	default:
		throw TextureException("TextureData::TextureData: Datatype undefined!");
		break;
	}

}

TextureData::TextureData( TextureData const &source)
	: width{source.width},
	  height{source.height},
	  glFormat{source.glFormat},
	  dataType{source.dataType},
	  lenData{source.lenData},
	  bytesPerTexel{source.bytesPerTexel}

{
	data = nullptr;

	if (source.lenData > 0 && source.data != 0) {
		data = new char[lenData];
		memcpy (data,source.data,lenData);
	}

}

void *TextureData::getDataPtr() {

	if (!data) {
		lenData = width * height * bytesPerTexel;
		data = new char[lenData];
		memset(data,0,lenData);
	}

	return data;

}

void const *TextureData::getDataPtr () const {

	return data;

}

void TextureData::writeData (
		void * const newData,
		GLuint newDataLength) {

	if (!data) {
		lenData = width * height * bytesPerTexel;
		data = new char[lenData];
	}

	if (newDataLength != lenData) {
		throw TextureException ("TextureData::writeData: dataLength is different from the internally computed buffer length!");
	}

	memcpy(data,newData,lenData);

}



} /* namespace OevGLES */
