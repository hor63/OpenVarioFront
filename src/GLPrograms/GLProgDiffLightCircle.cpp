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

const float one = 1.0;

// The factors Provides the real radius and z-offset of the primary circles.
uniform vec4 vecFactorPrimaryVertex;
uniform vec4 vecFactorSecondVertex;
// The factor provides the real normal vector
uniform vec4 vecFactorNormalVector;

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
	float diffuseLightFactor = abs(dot(lightDir,
		normalize(vec3((mvMatrix * (vertexNormal*vecFactorNormalVector)))));
	vec4 lightColor = ambientLightColor + (diffuseLightFactor * lightColor);

	// Use vecFactorSecondVertex when isSecondaryVertex is 1.0
	// but use vecFactorPrimaryVertex if isSecondaryVertex is 0.0.
	vec4 effFactorVertex = 
		(isSecondaryVertex * vecFactorSecondVertex) +
		((one - isSecondaryVertex) * vecFactorPrimaryVertex);

	
	lightColor.a = one;
	varFragColor = vertexColor * lightColor;
	gl_Position = mvpMatrix * (vertexPos * effFactorVertex);
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
	vecFactorPrimaryVertexInfo	=
			*retrieveSingleUniformInfo("vecFactorPrimaryVertex",vecFactorPrimaryVertexLocation);
	vecFactorSecondVertexInfo	=
			*retrieveSingleUniformInfo("vecFactorSecondVertex",vecFactorSecondVertexLocation);
	vecFactorNormalVectorInfo	=
			*retrieveSingleUniformInfo("vecFactorNormalVector",vecFactorNormalVectorLocation);
	mvpMatrixInfo				=
			*retrieveSingleUniformInfo("mvpMatrix",mvpMatrixLocation);
	mvMatrixInfo				=
			*retrieveSingleUniformInfo("mvMatrix",mvMatrixLocation);
	lightDirInfo				=
			*retrieveSingleUniformInfo("lightDir",lightDirLocation);
	lightColorInfo				=
			*retrieveSingleUniformInfo("lightColor",lightColorLocation);
	ambientLightColorInfo		=
			*retrieveSingleUniformInfo("ambientLightColor",ambientLightColorLocation);

	// The vertex attributes
	vertexPosInfo			=
			*retrieveSingleAttributeInfo("vertexPos",vertexPosLocation);
	vertexNormalInfo		=
			*retrieveSingleAttributeInfo("vertexNormal",vertexNormalLocation);
	vertexColorInfo			=
			*retrieveSingleAttributeInfo("vertexColor",vertexColorLocation);
	isSecondaryVertexInfo	=
			*retrieveSingleAttributeInfo("isSecondaryVertex",isSecondaryVertexLocation);

}

} /* namespace OevGLES */
