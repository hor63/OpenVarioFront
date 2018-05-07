/*
 * GLShader.cpp
 *
 *  Created on: May 4, 2018
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


#include "GLES/GLShader.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif


GLShader::GLShader(const char *shaderSource)
:shaderText{shaderSource}
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLShader");
	}
#endif

}

GLShader::GLShader(std::istream &istream) {
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLShader");
	}
#endif

	char *buf = new char[1024];
	int numRead;

	// read the text in chunks of one KB.
	// leave one byte at the end for string termination.

	do  {
		istream.read(buf,1023);
		numRead = istream.gcount();
		LOG4CXX_DEBUG(logger,"numRead " << numRead );
		if (numRead > 0) {
			// 0-terminate the input buffer
			buf[numRead] = '\0';
			shaderText.append(buf);
		}
	} while (numRead > 0 && !istream.fail() && !istream.bad() && !istream.eof());

	delete buf;
}

GLShader::~GLShader() {
	if (shaderHandle != 0) {
		glDeleteShader(shaderHandle);
	}
}


void GLShader::compileShader() {

	if (!isCompiled) {
		char const *shaderCStr = shaderText.c_str();
		GLint compileResult = GL_FALSE;
		std::string exceptString;

		if (shaderHandle == 0) {
			shaderHandle = glCreateShader(getShaderType());

			if (shaderHandle == 0) {
				std::ostringstream errStr;
				errStr << "Cannot create a shader of type " << ((getShaderType() == GL_VERTEX_SHADER) ? " vertex shader" : "fragment shader");
				throw ShaderException(errStr.str().c_str());
			}

		}

		// Set the source to the shader.
		glShaderSource(shaderHandle,1,&shaderCStr,NULL);

		// and compile the source
		glCompileShader(shaderHandle);

		glGetShaderiv(shaderHandle,GL_COMPILE_STATUS,&compileResult);
		if (compileResult == GL_FALSE) {
			GLint infoLen = 0;
			char *infoStr = 0;
			GLint infoStrLen = 0;
			GLint shaderSourceLength = 0;
			char* shaderSource = 0;

			exceptString = "Compilation of the ";
			exceptString.append((getShaderType() == GL_VERTEX_SHADER) ? " vertex shader" : "fragment shader");
			exceptString.append(" failed.");

			glGetShaderiv(shaderHandle,GL_INFO_LOG_LENGTH,&infoLen);
			if (infoLen > 1) {
				infoStr = new char[infoLen];
				glGetShaderInfoLog(shaderHandle,infoLen,&infoStrLen,infoStr);

				exceptString.append("Compilation error message =");
				exceptString.append("\n----------------------------------\n");
				exceptString.append(infoStr);
				exceptString.append("\n----------------------------------");

				delete infoStr;

			} else {
				exceptString.append("No compilation error message available");
			}

			glGetShaderiv(shaderHandle,GL_SHADER_SOURCE_LENGTH,&shaderSourceLength);
			if (shaderSourceLength > 1) {
				shaderSource = new char[shaderSourceLength + 10 ];
				glGetShaderSource(shaderHandle,shaderSourceLength + 9,NULL,shaderSource);
				exceptString.append("\nShader source = \n-------------------------------------\n");
				exceptString.append(shaderSource);
				exceptString.append("\n-------------------------------------");
				delete shaderSource;
			}

			LOG4CXX_FATAL(logger,exceptString);
			throw ShaderException(exceptString.c_str());
		}

	}
}

GLuint GLVertexShader::getShaderType() const {
	return GL_VERTEX_SHADER;
}

GLuint GLFragmentShader::getShaderType() const {
	return GL_FRAGMENT_SHADER;
}

} /* namespace OevGLES */

