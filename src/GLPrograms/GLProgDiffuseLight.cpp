/*
 * GLProgDiffuseLight.cpp
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

#include "GLPrograms/GLProgDiffuseLight.h"

namespace OevGLES {

GLProgDiffuseLight* GLProgDiffuseLight::theProgram = 0;

GLProgDiffuseLight::~GLProgDiffuseLight() {

	// This deletes the only instance of the program
	theProgram = 0;

}

GLProgDiffuseLight* GLProgDiffuseLight::getProgram() {

	if (!theProgram) {
		theProgram = new GLProgDiffuseLight;

		theProgram->createProgram();
	}

	return theProgram;

}

void GLProgDiffuseLight::createProgram() {

	OevGLES::GLVertexShader *vertShader = new OevGLES::GLVertexShader (
			"precision mediump float;\n"
			"\n"
			"uniform vec4 mvpMatrix;\n"
			"\n"
			"uniform vec4 lightDir;\n"
			"uniform vec4 lightColor;\n"
			"uniform vec4 ambientLightColor;\n"
			"\n"
			"attribute vec4 vertexPos;\n"
			"attribute vec4 vertexNormal;\n"
			"attribute vec4 vertexColor;\n"
			"\n"
			"varying vec4 fragColor;\n"
			"\n"
			"void main () { \n"
			"	float diffuseLightFactor = dot(lightDir.xyz,(mvpMatrix * vertexNormal).xyz);\n"
			"	\n"
			"	fragColor = vertexColor * (ambientLightColor + diffuseLightFactor * lightColor);\n"
			"	gl_Position = mvpMatrix * vertexPos;\n"
			"}\n");

	OevGLES::GLFragmentShader *fragShader = new OevGLES::GLFragmentShader (
			"precision mediump float;\n"
			"varying vec4 fragColor;\n"
			"\n"
			"void main () {\n"
			"	gl_FragColor = fragColor;\n"
			"}\n");

	prog.attachVertexShader(vertShader);
	prog.attachFragmentShader(fragShader);

	prog.linkProgram();

}

} /* namespace OevGLES */
