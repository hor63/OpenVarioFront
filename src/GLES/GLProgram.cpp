/*
 * GLProgram.cpp
 *
 *  Created on: May 6, 2018
 *      Author: hor
 *
 *  Wrapper for an OpenGL program. Attaches Vertex and fragment shader, and links the entire program
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


#include "GLES/GLProgram.h"
#include "GLES/ExceptionBase.h"


namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif


GLProgram::GLProgram()
	:programHandle{0},
	 vertexShader {0},
	 fragmentShader {0},
	 isLinked {false}
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLProgram");
	}
#endif
}

GLProgram::GLProgram(GLVertexShader &vertexShader,GLFragmentShader &fragmentShader)
	:programHandle{0},
	 vertexShader {&vertexShader},
	 fragmentShader {&fragmentShader},
	 isLinked {false}
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLProgram");
	}
#endif
}

GLProgram::~GLProgram() {
	// TODO Auto-generated destructor stub
}


void GLProgram::attachVertexShader (GLVertexShader &vertexShader) {
	this->vertexShader = &vertexShader;
}
void GLProgram::attachFragmentShader (GLFragmentShader &fragmentShader){
	this->fragmentShader = &fragmentShader;
}

void GLProgram::linkProgram () {
	GLint linkResult = GL_FALSE;

	if (!vertexShader) {
		LOG4CXX_FATAL(logger,"Vertex shader is undefined");
		throw ProgramException("Vertex shader is undefined");
	}
	if (!fragmentShader) {
		LOG4CXX_FATAL(logger,"Fragment shader is undefined");
		throw ProgramException("Fragment shader is undefined");
	}

	// Re-compile the shaders if necessary
	vertexShader->compileShader();
	fragmentShader->compileShader();

	// now create the program, attach the shaders, and link the program
	programHandle = glCreateProgram();
	LOG4CXX_DEBUG(logger, "program handle = " << programHandle);
	if (programHandle == 0) {
		throw ProgramException ("GLCreateProgram returned 0.");
	}

	// Attach the shaders
	glAttachShader(programHandle,*vertexShader);
	glAttachShader(programHandle,*fragmentShader);

	glLinkProgram(programHandle);
	glGetProgramiv(programHandle,GL_LINK_STATUS,&linkResult);

	LOG4CXX_DEBUG(logger,"glLinkProgram result = " <<  linkResult);

	if (linkResult != GL_TRUE) {
		GLint infoLen = 0;
		char* infoString = 0;
		std::string errString;

		glGetProgramiv(programHandle,GL_INFO_LOG_LENGTH,&infoLen);
		infoString = new char [infoLen+10];

		glGetProgramInfoLog(programHandle,infoLen+9,NULL,infoString);
		errString = "Cannot link GL program. Error message = \n-----------------------------------\n";
		errString.append(infoString);
		errString.append("\n-----------------------------------");

		delete infoString;

		LOG4CXX_FATAL(logger,errString);
		throw ProgramException(errString.c_str());
	}



}



} /* namespace OevGLES */
