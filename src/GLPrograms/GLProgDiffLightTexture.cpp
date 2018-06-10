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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif



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
			"precision mediump float;\n"
			"\n"
			"// MVP matrix is used to transform points\n"
			"uniform mat4 mvpMatrix;\n"
			"\n"
			"// MV matrix is used to transform normal vectors to eye space\n"
			"uniform mat4 mvMatrix;\n"
			"\n"
			"// Light diretory vector is already in eye space\n"
			"uniform vec3 lightDir;\n"
			"uniform vec4 lightColor;\n"
			"uniform vec4 ambientLightColor;\n"
			"\n"
			"attribute vec4 vertexPos;\n"
			"attribute vec4 vertexNormal;\n"
			"attribute vec4 vertexColor;\n"
			"\n"
			"varying vec4 fragColor;\n"
			"\n"
			"const float cZero = 0.0f;"
			"\n"
			"void main () { \n"
			"	float diffuseLightFactor = max(cZero,dot(lightDir,vec3((mvMatrix * vertexNormal))));\n"
			"	\n"
			"	fragColor = vertexColor * (ambientLightColor + (diffuseLightFactor * lightColor));\n"
			"	gl_Position = mvpMatrix * vertexPos;\n"
			"}\n";

}

const char* GLProgDiffLightTexture::getFragmentShaderCode() const {
	return
			"precision mediump float;\n"
			"varying vec4 fragColor;\n"
			"\n"
			"void main () {\n"
			"	gl_FragColor = fragColor;\n"
			"}\n";
}


void OevGLES::GLProgDiffLightTexture::retrieveShaderVariableInfo() {

	// The uniforms
	mvpMatrixInfo			= *retrieveSingleUniformInfo("mvpMatrix",mvpMatrixLocation);
	mvMatrixInfo			= *retrieveSingleUniformInfo("mvMatrix",mvMatrixLocation);
	lightDirInfo			= *retrieveSingleUniformInfo("lightDir",lightDirLocation);
	lightColorInfo			= *retrieveSingleUniformInfo("lightColor",lightColorLocation);
	ambientLightColorInfo	= *retrieveSingleUniformInfo("ambientLightColor",ambientLightColorLocation);

	// The vertex attributes
	vertexPosInfo		= *retrieveSingleAttributeInfo("vertexPos",vertexPosLocation);
	vertexNormalInfo	= *retrieveSingleAttributeInfo("vertexNormal",vertexNormalLocation);
	vertexColorInfo		= *retrieveSingleAttributeInfo("vertexColor",vertexColorLocation);

}

} /* namespace OevGLES */
