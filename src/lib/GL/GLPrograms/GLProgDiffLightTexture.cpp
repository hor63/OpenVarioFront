/*
 * GLProgDiffLightTexture.cpp
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
#include "OVFCommon.h"

#include "GLPrograms/GLProgDiffLightTexture.h"

namespace OevGLES {


GLProgDiffLightTexture* GLProgDiffLightTexture::theProgram = 0;

GLProgDiffLightTexture::~GLProgDiffLightTexture() {

	// This deletes the only instance of the program
	theProgram = 0;

}

GLProgDiffLightTexture* GLProgDiffLightTexture::getProgram() {

	if (!theProgram) {
		theProgram = new GLProgDiffLightTexture;

		theProgram->createProgram();
	}

	return theProgram;

}

void GLProgDiffLightTexture::destroyProgram() {
	if (theProgram) {
		delete theProgram;
		theProgram = 0;
	}
}

const char* GLProgDiffLightTexture::getVertexShaderCode() const {

	return
R"(
#version 100

precision mediump float;

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
attribute vec2 vertexTexture0Pos;

varying vec4 fragColor;
varying vec2 varyTexture0Pos;

const float cZero = 0.0;

void main () { 
	float diffuseLightFactor = abs(dot(lightDir,normalize(vec3((mvMatrix * vertexNormal)))));
	vec4 lightColor = ambientLightColor + (diffuseLightFactor * lightColor);
	
	lightColor.a = 1.0;
	
	fragColor = vertexColor * lightColor;
	varyTexture0Pos = vertexTexture0Pos;
	gl_Position = mvpMatrix * vertexPos;
}
)";

}

const char* GLProgDiffLightTexture::getFragmentShaderCode() const {
	return
R"(
precision mediump float;

uniform sampler2D texture0;

varying vec4 fragColor;
varying vec2 varyTexture0Pos;

void main () {
	gl_FragColor = fragColor * texture2D(texture0,varyTexture0Pos);
}
)";
}


void OevGLES::GLProgDiffLightTexture::retrieveShaderVariableInfo() {

	// The uniforms
	mvpMatrixInfo			= *retrieveSingleUniformInfo("mvpMatrix",mvpMatrixLocation);
	mvMatrixInfo			= *retrieveSingleUniformInfo("mvMatrix",mvMatrixLocation);
	lightDirInfo			= *retrieveSingleUniformInfo("lightDir",lightDirLocation);
	lightColorInfo			= *retrieveSingleUniformInfo("lightColor",lightColorLocation);
	ambientLightColorInfo	= *retrieveSingleUniformInfo("ambientLightColor",ambientLightColorLocation);
	texture0Info			= *retrieveSingleUniformInfo("texture0",texture0Location);

	// The vertex attributes
	vertexPosInfo			= *retrieveSingleAttributeInfo("vertexPos",vertexPosLocation);
	vertexNormalInfo		= *retrieveSingleAttributeInfo("vertexNormal",vertexNormalLocation);
	vertexColorInfo			= *retrieveSingleAttributeInfo("vertexColor",vertexColorLocation);
	vertexTexture0PosInfo	= *retrieveSingleAttributeInfo("vertexTexture0Pos",vertexTexture0PosLocation);

}

} /* namespace OevGLES */
