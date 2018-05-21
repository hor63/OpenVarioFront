/*
 * GLProgBase.cpp
 *
 *  Created on: May 17, 2018
 *      Author: kai_horstmann
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

#include <sstream>

#include "GLPrograms/GLProgBase.h"

namespace OevGLES {

GLProgBase::GLProgBase() {

}

GLProgBase::~GLProgBase() {

}

void GLProgBase::createProgram() {

	OevGLES::GLVertexShader *vertShader = new OevGLES::GLVertexShader (
			getVertexShaderCode());

	OevGLES::GLFragmentShader *fragShader = new OevGLES::GLFragmentShader (
			getFragmentShaderCode());

	prog.attachVertexShader(vertShader);
	prog.attachFragmentShader(fragShader);

	prog.linkProgram();

	retrieveShaderVariableInfo();

}

GLProgram::ShaderVariableInfo const * OevGLES::GLProgBase::retrieveSingleUniformInfo (
		const char *uniformName,
		GLint &uniformLocation) const {

	auto inf = prog.getUniformInfo(uniformName);

	if (!inf) {
		std::ostringstream str;

		str << "Uniform " << uniformName << " does not exist or is not active in the program.";

		throw ProgramException(str.str().c_str());

	}

	uniformLocation = glGetUniformLocation(prog.getProgramHandle(),uniformName);
	if (uniformLocation == -1) {
		std::ostringstream str;

		str << "Location of uniform  " << uniformName << " cannot be retrieved.";

		throw ProgramException(str.str().c_str());

	}


	return inf;
}

GLProgram::ShaderVariableInfo const * OevGLES::GLProgBase::retrieveSingleAttributeInfo (
		const char *attributeName,
		GLint &attributeLocation) const {

	auto inf = prog.getAttributeInfo(attributeName);

	if (!inf) {
		std::ostringstream str;

		str << "Vertex attribute " << attributeName << " does not exist or is not active in the program.";


		throw ProgramException(str.str().c_str());

	}

	attributeLocation = glGetAttribLocation(prog.getProgramHandle(),attributeName);
	if (attributeLocation == -1) {
		std::ostringstream str;

		str << "Location of vertex attribute  " << attributeName << " cannot be retrieved.";


		throw ProgramException(str.str().c_str());

	}


	return inf;
}


} /* namespace OevGLES */
