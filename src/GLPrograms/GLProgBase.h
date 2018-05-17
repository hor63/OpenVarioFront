/*
 * GLProgBase.h
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

#ifndef GLPROGBASE_H_
#define GLPROGBASE_H_

#include "GLES/GLProgram.h"

namespace OevGLES {

class GLProgBase {
public:

	/** \brief Retrieve the GL program
	 *
	 * @return reference to the GL program
	 */
	GLProgram &getGLProgram() {
		return prog;
	}

	/** \brief Use the program for subsequent rendering
	 *
	 */
	void useProgram() {
		prog.useProgram();
	}

	/** \brief Retrieve the vertex shader code.
	 *
	 *	The method must be overridden by sub-classes which provide their specific shader code.
	 *
	 * @return Vertex shader code as one C string
	 */
	virtual char const* getVertexShaderCode() const = 0;

	/** \brief Retrieve the frament shader code.
	 *
	 *	The method must be overridden by sub-classes which provide their specific shader code.
	 *
	 * @return Fragment shader code as one C string
	 */
	virtual char const* getFragmentShaderCode() const = 0;

protected:
	GLProgBase();
	virtual ~GLProgBase();

	/// \brief The GL program object
	GLProgram prog;

	/** \brief Creates the shaders, compiles them, and links the program.
	 *
	 * This class holds the source code for the shaders.
	 *
	 */
	void createProgram();



};

} /* namespace OevGLES */

#endif /* GLPROGBASE_H_ */
