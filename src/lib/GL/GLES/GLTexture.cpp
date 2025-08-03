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

#include "OVFCommon.h"

#include "GLES/GLTexture.h"
#include "ExceptionBase.h"


#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

namespace OevGLES {

GLTexture::GLTexture() {
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLTexture");
	}
#endif


}

GLTexture::GLTexture (GLTexture&& source) :
			textureHandle {source.textureHandle},
			minFilterType {source.minFilterType},
			magFilterType {source.magFilterType},
			wrapS {source.wrapS},
			wrapT {source.wrapT}
			{
	source.textureHandle = 0;
}



GLTexture::~GLTexture() {
	if (textureHandle != 0) {
		glDeleteTextures(1,&textureHandle);
	}
}

GLTexture& GLTexture::operator = (GLTexture&& source) {

	if (textureHandle != 0) {
		glDeleteTextures(1,&textureHandle);
	}

	textureHandle = source.textureHandle;
	minFilterType = source.minFilterType;
	magFilterType = source.magFilterType;
	wrapS = source.wrapS;
	wrapT = source.wrapT;

	source.textureHandle = 0;

	return *this;
}

void GLTexture::createTextureHandle() {

	auto glError = glGetError();
	// Flush the error queue
	while (glError!= GL_NO_ERROR) {
		glError = glGetError();
	}

	if (textureHandle == 0) {

		glGenTextures(1,&textureHandle);

		if (logger->isDebugEnabled()) {
			glError = glGetError();
			while (glError!= GL_NO_ERROR) {
				LOG4CXX_DEBUG(logger,"Error in glGenTextures = " << glError);
				glError = glGetError();
			}

		}

		if (textureHandle == 0) {
			throw TextureException("glGenTextures did not return a valid texture handle");
		}
	}

}

void GLTexture::setTextureData(const TextureData& textureData, GLint mipMapLevel)
{
	GLint packAlignment = 8;
	createTextureHandle();

	auto glError = glGetError();
	// Flush the error queue
	while (glError!= GL_NO_ERROR) {
		glError = glGetError();
	}


	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": textureHandle = " << textureHandle);

	glBindTexture(GL_TEXTURE_2D,textureHandle);
	if (logger->isDebugEnabled()) {
		glError = glGetError();
		while (glError!= GL_NO_ERROR) {
			LOG4CXX_DEBUG(logger,"\tError in glBindTexture = " << glError);
			glError = glGetError();
		}

	}

	//
	if (textureData.getWidth() & 1) {
		packAlignment = 1;
	} else {
		if (textureData.getWidth() & 2) {
			packAlignment = 2;
		} else {

			if (textureData.getWidth() & 4) {
				packAlignment = 4;
			} else {
				packAlignment = 8;
	}	}	}
	LOG4CXX_DEBUG(logger,"\tpackAlignment = " << packAlignment);

	GLint orgPackAlignment = 4;
	GLint orgUnPackAlignment = 4;
	glGetIntegerv(GL_PACK_ALIGNMENT,&orgPackAlignment);
	glGetIntegerv(GL_UNPACK_ALIGNMENT,&orgUnPackAlignment);

	glPixelStorei(GL_PACK_ALIGNMENT,packAlignment);
	if (logger->isDebugEnabled()) {
		glError = glGetError();
		while (glError!= GL_NO_ERROR) {
			LOG4CXX_DEBUG(logger,"\tError in glPixelStorei(GL_PACK_ALIGNMENT...) =" << glError);
			glError = glGetError();
		}

	}
	glPixelStorei(GL_UNPACK_ALIGNMENT,packAlignment);
	if (logger->isDebugEnabled()) {
		glError = glGetError();
		while (glError!= GL_NO_ERROR) {
			LOG4CXX_DEBUG(logger,"\tError in glPixelStorei(GL_UNPACK_ALIGNMENT...) =" << glError);
			glError = glGetError();
		}

	}
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
	if (logger->isDebugEnabled()) {
		glError = glGetError();
		while (glError!= GL_NO_ERROR) {
			LOG4CXX_DEBUG(logger,"\tError in glTexImage2D) =" << glError);
			glError = glGetError();
		}

	}
	LOG4CXX_DEBUG(logger,"\t call glTexImage2D (target=" << std::hex << GL_TEXTURE_2D << std::dec
			<< ", level="<< mipMapLevel
			<< ", internalformat=0x" << std::hex << textureData.getGlFormat() << std::dec
			<< ", width=" << textureData.getWidth()
			<< ", height=textureData.getHeight()"<< textureData.getHeight()
			<< ", border=0, format=0x" << std::hex << textureData.getGlFormat() << std::dec
			<< ", type=0x" << std::hex << textureData.getDataType() << std::dec
			<< ", pixels=" << textureData.getDataPtr()
			<< ")");
	glPixelStorei(GL_PACK_ALIGNMENT,orgPackAlignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT,orgUnPackAlignment);

	glBindTexture(GL_TEXTURE_2D,0);

}

void GLTexture::generateMipmap()
{
	createTextureHandle();
	glBindTexture(GL_TEXTURE_2D,textureHandle);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,0);
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
