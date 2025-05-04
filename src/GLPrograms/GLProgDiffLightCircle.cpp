/*
 * GLProgDiffLightCircle.cpp
 *
 *  Created on: May 04, 2025
 *      Author: hor
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2025  Kai Horstmann
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


#include "GLPrograms/GLProgDiffLightCircle.h"

namespace OevGLES {

GLProgDiffLightCircle* GLProgDiffLightCircle::theProgram = 0;

GLProgDiffLightCircle::~GLProgDiffLightCircle() {

	// This deletes the only instance of the program
	theProgram = 0;

}

GLProgDiffLightCircle* GLProgDiffLightCircle::getProgram() {

	if (!theProgram) {
		theProgram = new GLProgDiffLightCircle;

		theProgram->createProgram();
	}

	return theProgram;

}

void GLProgDiffLightCircle::destroyProgram() {
	if (theProgram) {
		delete theProgram;
		theProgram = 0;
	}
}

const char* GLProgDiffLightCircle::getVertexShaderCode() const {

	return
R"(
#version 100

precision mediump float;

const mat4 m4Unity = (
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
);

const float one = 1.0;

// Model matrix for shifting the secondary vertex of each segment of a circle
// around to form a ring like a washer, or like a tire.
uniform mat4 mMatrixSecondVertex;

// MVP matrix is used to transform points
uniform mat4 mvpMatrix;

// MV matrix is used to transform normal vectors to eye space
uniform mat4 mvMatrix;

// Light directory vector is already in eye space
uniform vec3 lightDir;
uniform vec4 lightColor;
uniform vec4 ambientLightColor;

attribute vec4 vertexPos;
attribute vec4 vertexNormal;
attribute vec4 vertexColor;
// Must have values 1.0 or 0.0, and serves as a flag, but can be used
// in numeric expressions to avoid conditional statements.
attribute float isSecondaryVertex;

varying vec4 varFragColor;

void main () { 
	float diffuseLightFactor = abs(dot(lightDir,normalize(vec3((mvMatrix * vertexNormal)))));
	vec4 lightColor = ambientLightColor + (diffuseLightFactor * lightColor);

	// Use mMatrixSecondVertex when isSecondaryVertex is 1.0
	// but use just a Unity matrix if isSecondaryVertex is 0.0.
	mat4 effMvpMatrix = mvpMatrix * (
		(isSecondaryVertex * mMatrixSecondVertex) +
		((one - isSecondaryVertex) * m4Unity));

	
	lightColor.a = one;
	varFragColor = vertexColor * lightColor;
	gl_Position = effMvpMatrix * vertexPos;
}
)";

}

const char* GLProgDiffLightCircle::getFragmentShaderCode() const {

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


void OevGLES::GLProgDiffLightCircle::retrieveShaderVariableInfo() {

	// The uniforms
	mMatrixSecondVertexInfo	= *retrieveSingleUniformInfo("mMatrixSecondVertex",mMatrixSecondVertexLocation);
	mvpMatrixInfo			= *retrieveSingleUniformInfo("mvpMatrix",mvpMatrixLocation);
	mvMatrixInfo			= *retrieveSingleUniformInfo("mvMatrix",mvMatrixLocation);
	lightDirInfo			= *retrieveSingleUniformInfo("lightDir",lightDirLocation);
	lightColorInfo			= *retrieveSingleUniformInfo("lightColor",lightColorLocation);
	ambientLightColorInfo	= *retrieveSingleUniformInfo("ambientLightColor",ambientLightColorLocation);

	// The vertex attributes
	vertexPosInfo			= *retrieveSingleAttributeInfo("vertexPos",vertexPosLocation);
	vertexNormalInfo		= *retrieveSingleAttributeInfo("vertexNormal",vertexNormalLocation);
	vertexColorInfo			= *retrieveSingleAttributeInfo("vertexColor",vertexColorLocation);
	isSecondaryVertexInfo	= *retrieveSingleAttributeInfo("isSecondaryVertex",isSecondaryVertexLocation);

}

} /* namespace OevGLES */
