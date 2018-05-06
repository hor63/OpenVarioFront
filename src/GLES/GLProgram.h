/*
 * GLProgram.h
 *
 *  Created on: May 6, 2018
 *      Author: hor
 *
 *  Wrapper for an OpenGL program. Attaches Vertex and fragment shader, and links the entire program
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


#ifndef GLES_GLPROGRAM_H_
#define GLES_GLPROGRAM_H_

#include "GLES/GLShader.h"

namespace OevGLES {

class GLProgram {
public:
	GLProgram();
	GLProgram(GLVertexShader &vertexShader,GLFragmentShader &fragmentShader);
	virtual ~GLProgram();

	void attachVertexShader (GLVertexShader &vertexShader);
	void attachFragmentShader (GLFragmentShader &fragmentShader);

	void linkProgram ();

private:

	GLuint programHandle = 0;

	GLVertexShader		*vertexShader = 0;
	GLFragmentShader	*fragmentShader  = 0;

	bool isLinked = false;

};

} /* namespace OevGLES */

#endif /* GLES_GLPROGRAM_H_ */
