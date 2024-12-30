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



#include "GLPrograms/GLProgTextTexture.h"

namespace OevGLES {


GLProgTextTexture* GLProgTextTexture::theProgram = 0;

GLProgTextTexture::~GLProgTextTexture() {

	// This deletes the only instance of the program
	theProgram = 0;

}

GLProgTextTexture* GLProgTextTexture::getProgram() {

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
			"attribute vec2 attVertexTexture0Pos;"
			"attribute vec4 attVertexColor;\n"
			"\n"
			"varying vec4 varFragColor;\n"
			"varying vec2 varTexture0Pos;\n"
			"\n"
			"void main () { \n"
			"	varFragColor = attVertexColor;\n"
			"	varTexture0Pos = varTexture0Pos;\n"
			"	gl_Position = unMvpMatrix * attVertexPos;\n"
			"}\n";

}

const char* GLProgTextTexture::getFragmentShaderCode() const {
	return
			"precision mediump float;\n"
			"\n"
			"uniform sampler2D unTexture0;\n"
			"\n"
			"varying vec4 varFragColor;\n"
			"varying vec2 varTexture0Pos;\n"
			"\n"
			"void main () {\n"
			"	gl_FragColor = varFragColor * texture2D(unTexture0,varTexture0Pos);\n"
			"}\n";
}


void OevGLES::GLProgTextTexture::retrieveShaderVariableInfo() {

	// The uniforms
	unMvpMatrixInfo			= *retrieveSingleUniformInfo("unMvpMatrix",unMvpMatrixLocation);
	unTexture0Info			= *retrieveSingleUniformInfo("unTexture0",unTexture0Location);

	// The vertex attributes
	attVertexPosInfo			= *retrieveSingleAttributeInfo("attVertexPos",attVertexPosLocation);
	attVertexColorInfo			= *retrieveSingleAttributeInfo("attVertexColor",attVertexColorLocation);
	attVertexTexture0PosInfo	= *retrieveSingleAttributeInfo("attVertexTexture0Pos",attVertexTexture0PosLocation);

}

} /* namespace OevGLES */
