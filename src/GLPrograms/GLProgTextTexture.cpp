/*
 * GLProgTextTexture.cpp
 *
 *  Created on: Jun 4, 2018
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

#include "GLPrograms/GLProgTextTexture.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

GLProgTextTexture* GLProgTextTexture::theProgram = 0;

GLProgTextTexture::~GLProgTextTexture() {

	// This deletes the only instance of the program
	theProgram = 0;

}

GLProgTextTexture* GLProgTextTexture::getProgram() {

#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLPrograms.GLProgTextTexture");
	}
#endif

	if (!theProgram) {
		theProgram = new GLProgTextTexture;

		theProgram->createProgram();
	}

	return theProgram;

}

void GLProgTextTexture::destroyProgram() {
	if (theProgram) {
		delete theProgram;
		theProgram = 0;
	}
}

const char* GLProgTextTexture::getVertexShaderCode() const {

	return
			"precision mediump float;\n"
			"\n"
			"uniform mat4 unMvpMatrix;\n"
			"\n"
			"attribute vec4 attVertexPos;\n"
			"attribute vec2 attTexture0Pos;"
			"\n"
			"varying vec2 varTexture0Pos;\n"
			"\n"
			"void main () { \n"
			"	varTexture0Pos = attTexture0Pos;\n"
			"	gl_Position = unMvpMatrix * attVertexPos;\n"
			"}\n";

}

const char* GLProgTextTexture::getFragmentShaderCode() const {
	return
			"precision mediump float;\n"
			"\n"
			"uniform sampler2D unTexture0;\n"
			"uniform vec4 unFragColor;\n"
			"\n"
			"varying vec2 varTexture0Pos;\n"
			"\n"
			"void main () {\n"
			//"	gl_FragColor = vec4 (unFragColor.rgb, texture2D(unTexture0,varTexture0Pos.r);\n"
			"	gl_FragColor = vec4 (unFragColor.rgb * texture2D(unTexture0,varTexture0Pos).r,1.0);\n"
			"}\n";
}


void OevGLES::GLProgTextTexture::retrieveShaderVariableInfo() {

	// The uniforms
	unMvpMatrixInfo			= *retrieveSingleUniformInfo("unMvpMatrix",unMvpMatrixLocation);
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": index of unMvpMatrix = " << unMvpMatrixLocation);
	unFragColorInfo			= *retrieveSingleUniformInfo("unFragColor",unFragColorLocation);
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": index of unFragColor = " << unFragColorLocation);
	unTexture0Info			= *retrieveSingleUniformInfo("unTexture0",unTexture0Location);
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": index of unTexture0 = " << unTexture0Location);

	// The vertex attributes
	attVertexPosInfo			= *retrieveSingleAttributeInfo("attVertexPos",attVertexPosLocation);
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": index of attVertexPos = " << attVertexPosLocation);
	attTexture0PosInfo	= *retrieveSingleAttributeInfo("attTexture0Pos",attTexture0PosLocation);
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": index of attTexture0Pos = " << attTexture0PosLocation);

}

} /* namespace OevGLES */
