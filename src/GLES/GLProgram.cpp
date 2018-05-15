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

static char const * glEnumToString (GLenum type) {

	char const * rc;

	switch (type) {
	case GL_FLOAT:
		rc = "GL_FLOAT";
		break;
	case GL_FLOAT_VEC2:
		rc = "GL_FLOAT_VEC2";
		break;
	case GL_FLOAT_VEC3:
		rc = "GL_FLOAT_VEC3";
		break;
	case GL_FLOAT_VEC4:
		rc = "GL_FLOAT_VEC4";
		break;
	case GL_INT:
		rc = "GL_INT";
		break;
	case GL_INT_VEC2:
		rc = "GL_INT_VEC2";
		break;
	case GL_INT_VEC3:
		rc = "GL_INT_VEC3";
		break;
	case GL_INT_VEC4:
		rc = "GL_INT_VEC4";
		break;
	case GL_BOOL:
		rc = "GL_BOOL";
		break;
	case GL_BOOL_VEC2:
		rc = "GL_BOOL_VEC2";
		break;
	case GL_BOOL_VEC3:
		rc = "GL_BOOL_VEC3";
		break;
	case GL_BOOL_VEC4:
		rc = "GL_BOOL_VEC4";
		break;
	case GL_FLOAT_MAT2:
		rc = "GL_FLOAT_MAT2";
		break;
	case GL_FLOAT_MAT3:
		rc = "GL_FLOAT_MAT3";
		break;
	case GL_FLOAT_MAT4:
		rc = "GL_FLOAT_MAT4";
		break;
	case GL_SAMPLER_2D:
		rc = "GL_SAMPLER_2D";
		break;
	case GL_SAMPLER_CUBE:
		rc = "GL_SAMPLER_CUBE";
		break;

	default:
		rc = "???";
	}

	return rc;

}


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

GLProgram::GLProgram(GLVertexShader *vertexShader,GLFragmentShader *fragmentShader)
	:programHandle{0},
	 vertexShader {vertexShader},
	 fragmentShader {fragmentShader},
	 isLinked {false}
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLProgram");
	}
#endif
}

GLProgram::~GLProgram() {

	detachVertexShader();
	detachFragmentShader();

	if (programHandle != 0) {
		LOG4CXX_DEBUG(logger,"Delete GL program " << programHandle);
		glDeleteProgram(programHandle);
	}

}


void GLProgram::attachVertexShader (GLVertexShader *newVertexShader) {

	if (vertexShader) {
		detachVertexShader();
	}

	vertexShader = newVertexShader;
}
void GLProgram::attachFragmentShader (GLFragmentShader *newFragmentShader){

	if (fragmentShader) {
		detachFragmentShader();
	}

	fragmentShader = newFragmentShader;
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

	retrieveShaderVariableInfos();


}


void GLProgram::retrieveShaderVariableInfos() {

	GLint maxLenAttributes = 0;

	GLint maxLenUniforms = 0;

	glGetProgramiv(programHandle,GL_ACTIVE_ATTRIBUTES,&numAttributes);
	LOG4CXX_DEBUG(logger,"numAttributes = " << numAttributes);

	glGetProgramiv(programHandle,GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,&maxLenAttributes);
	LOG4CXX_DEBUG(logger,"maxLenAttributes = " << maxLenAttributes);

	{
		char * buf = new char [maxLenAttributes];

		for (GLuint i = 0; i < numAttributes; i++) {
			GLint arraySize = 0;
			GLenum type = 0;

			glGetActiveAttrib(
					programHandle,
					i,
					maxLenAttributes,
					NULL,
					&arraySize,
					&type,
					buf);

			LOG4CXX_DEBUG(logger,"Attribute [" << i << "] = "	<< buf
					<< ", type =  " << type << " = " << glEnumToString(type)
					<< ", array size = " << arraySize);

			attributeMap.insert (ShaderVariableInfoPair(std::string(buf),ShaderVariableInfo(buf,type,i)));
		}

		delete buf;
	}


	glGetProgramiv(programHandle,GL_ACTIVE_UNIFORMS,&numUniforms);
	LOG4CXX_DEBUG(logger,"numUniforms = " << numUniforms);

	glGetProgramiv(programHandle,GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxLenUniforms);
	LOG4CXX_DEBUG(logger,"maxLenUniforms = " << maxLenUniforms);

	{
		char * buf = new char [maxLenUniforms];

		for (GLuint i = 0; i < numUniforms; i++) {
			GLint arraySize = 0;
			GLenum type = 0;

			glGetActiveUniform(
					programHandle,
					i,
					maxLenUniforms,
					NULL,
					&arraySize,
					&type,
					buf);

			LOG4CXX_DEBUG(logger,"Uniform [" << i << "] = "	<< buf
					<< ", type =  " << type << " = " << glEnumToString(type)
					<< ", array size = " << arraySize);

			uniformMap.insert (ShaderVariableInfoPair(std::string(buf),ShaderVariableInfo(buf,type,i)));

		}

		delete buf;
	}



}

void GLProgram::detachVertexShader() {

	if (vertexShader) {
		LOG4CXX_DEBUG(logger,"Detach vertex shader " << GLuint(*vertexShader));

		glDetachShader(programHandle,*vertexShader);

		delete vertexShader;
		vertexShader = 0;

		isLinked = false;
	}
}

const GLProgram::ShaderVariableInfo* GLProgram::getUniformInfo(const char* uniformName) {
	auto it = uniformMap.find(std::string(uniformName));

	if (it == uniformMap.cend()) {
		return 0;
	} else {
		return &it->second;
	}
}

const GLProgram::ShaderVariableInfo* GLProgram::getAttributeInfo(const char* attributeName) {
	auto it = attributeMap.find(std::string(attributeName));

	if (it == attributeMap.cend()) {
		return 0;
	} else {
		return &it->second;
	}
}

void GLProgram::detachFragmentShader() {

	if (fragmentShader) {
		LOG4CXX_DEBUG(logger,"Detach fragment shader " << GLuint(*fragmentShader));

		glDetachShader(programHandle, *fragmentShader);

		delete fragmentShader;
		fragmentShader = 0;

		isLinked = false;
	}

}


} /* namespace OevGLES */
