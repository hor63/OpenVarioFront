/*
 * GLTexture.cpp
 *
 *  Created on: Jun 2, 2018
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


#include "GLES/GLTexture.h"
#include "GLES/ExceptionBase.h"

namespace OevGLES {

GLTexture::GLTexture() {

}


GLTexture::~GLTexture() {
	if (textureHandle != 0) {
		glDeleteTextures(1,&textureHandle);
	}
}

void GLTexture::createTextureHandle() {

	if (textureHandle == 0) {

		glGenTextures(1,&textureHandle);

		if (textureHandle == 0) {
			throw TextureException("glGenTextures did not return a valid texture handle");
		}
	}

}

void GLTexture::setTextureData(const TextureData& textureData, GLint mipMapLevel)
{
	createTextureHandle();

	glBindTexture(GL_TEXTURE_2D,textureHandle);

	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glTexImage2D(
			GL_TEXTURE_2D,
			mipMapLevel,
			textureData.getGlFormat(),
			textureData.getWidth(),
			textureData.getHeight(),
			0,
			textureData.getGlFormat(),
			textureData.getDataType(),
			textureData.getDataPtr()
			);

}

void GLTexture::generateMipmap()
{
	createTextureHandle();
	glBindTexture(GL_TEXTURE_2D,textureHandle);

	glGenerateMipmap(GL_TEXTURE_2D);
}

void GLTexture::setMinificationFilter(TextureFilter filterType)
{
	minFilterType = filterType;
}

void GLTexture::setMagnificationFilter(TextureFilter filterType)
{
	magFilterType = filterType;
}

void GLTexture::setWrapMode(TextureWrapMode wrapModeS, TextureWrapMode wrapModeT)
{
	wrapS =  wrapModeS;
	wrapT =  wrapModeT;
}

void GLTexture::bindToUniformLocation(
		GLenum textureUnit,
		GLint textureUnitNo,
		GLint uniformLocation) {

	createTextureHandle();

	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D,textureHandle);
	glUniform1i(uniformLocation,textureUnitNo);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,magFilterType);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minFilterType);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,wrapS);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,wrapT);

}


} /* namespace OevGLES */
