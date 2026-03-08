/*
 * GLProgSimpleFill.cpp
 *
 *  Created on: May 16, 2018
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

#include "GLPrograms/GLProgSimpleFill.h"

namespace OevGLES {

GLProgSimpleFill* GLProgSimpleFill::theProgram = 0;

GLProgSimpleFill::~GLProgSimpleFill() {

	// This deletes the only instance of the program
	theProgram = 0;

}

GLProgSimpleFill* GLProgSimpleFill::getProgram() {

	if (!theProgram) {
		theProgram = new GLProgSimpleFill;

		theProgram->createProgram();
	}

	return theProgram;

}

void GLProgSimpleFill::destroyProgram() {
	if (theProgram) {
		delete theProgram;
		theProgram = 0;
	}
}

const char* GLProgSimpleFill::getVertexShaderCode() const {

	return
R"(
#version 100

precision mediump float;

// MVP matrix is used to transform points
uniform mat4 mvpMatrix;

uniform vec4 ambientLightColor;

attribute vec4 vertexPos;
attribute vec4 vertexColor;

varying vec4 varFragColor;

void main () { 
	vec4 lightColor = ambientLightColor;
	
	lightColor.a = 1.0;
	varFragColor = vertexColor * lightColor;
	gl_Position = mvpMatrix * vertexPos;
}
)";

}

const char* GLProgSimpleFill::getFragmentShaderCode() const {

	return
R"(
#version 100

precision mediump float;
varying vec4 varFragColor;

void main () {
	gl_FragColor = varFragColor;
};
)";
}


void OevGLES::GLProgSimpleFill::retrieveShaderVariableInfo() {

	// The uniforms
	mvpMatrixInfo			= *retrieveSingleUniformInfo("mvpMatrix",mvpMatrixLocation);
	ambientLightColorInfo	= *retrieveSingleUniformInfo("ambientLightColor",ambientLightColorLocation);

	// The vertex attributes
	vertexPosInfo		= *retrieveSingleAttributeInfo("vertexPos",vertexPosLocation);
	vertexColorInfo		= *retrieveSingleAttributeInfo("vertexColor",vertexColorLocation);

}

} /* namespace OevGLES */
