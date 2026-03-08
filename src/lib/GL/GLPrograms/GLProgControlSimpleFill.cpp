/*
 * GLProgControlSimpleFill.cpp
 *
 *  Created on: Dec 25, 2025
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
#include "OVFCommon.h"

#include "GLPrograms/GLProgControlSimpleFill.h"

namespace OevGLES {

GLProgControlSimpleFill* GLProgControlSimpleFill::theProgram = 0;

GLProgControlSimpleFill::~GLProgControlSimpleFill() {

	// This deletes the only instance of the program
	theProgram = 0;

}

GLProgControlSimpleFill* GLProgControlSimpleFill::getProgram() {

	if (!theProgram) {
		theProgram = new GLProgControlSimpleFill;

		theProgram->createProgram();
	}

	return theProgram;

}

void GLProgControlSimpleFill::destroyProgram() {
	if (theProgram) {
		delete theProgram;
		theProgram = 0;
	}
}

const char* GLProgControlSimpleFill::getVertexShaderCode() const {

	return
R"(
#version 100

precision mediump float;

// MVP matrix is used to transform points
uniform mat4 mvpMatrix;

attribute vec4 vertexPos;

void main () { 
	gl_Position = mvpMatrix * vertexPos;
}
)";

}

const char* GLProgControlSimpleFill::getFragmentShaderCode() const {

	return
R"(
#version 100

precision mediump float;

uniform vec4 fillColor;

void main () {
	gl_FragColor = fillColor;
};
)";
}


void OevGLES::GLProgControlSimpleFill::retrieveShaderVariableInfo() {

	// The uniforms
	mvpMatrixInfo	= *retrieveSingleUniformInfo("mvpMatrix",mvpMatrixLocation);
	fillColorInfo	= *retrieveSingleUniformInfo("fillColor",fillColorLocation);

	// The vertex attributes
	vertexPosInfo	= *retrieveSingleAttributeInfo("vertexPos",vertexPosLocation);

}

} /* namespace OevGLES */
