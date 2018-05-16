/*
 * GLProgDiffuseLight.h
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

#ifndef GLPROGDIFFUSELIGHT_H_
#define GLPROGDIFFUSELIGHT_H_

#include "GLES/GLProgram.h"

namespace OevGLES {

/** \brief GL program with diffuse Gouraud lighting
 *
 */
class GLProgDiffuseLight {
public:
	virtual ~GLProgDiffuseLight();

	/** \brief Return the only instance of the program
	 *
	 * If the instance did not exist before it is created, the shaders are created, and the program is linked.
	 *
	 * @return Pointer to the instance of the program
	 */
	static GLProgDiffuseLight *getProgram();

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

private:
	/// \brief The only instance of this program object.
	static GLProgDiffuseLight* theProgram;

	/// \brief The GL program object
	GLProgram prog;

	GLProgDiffuseLight() {

	}

	/** \brief Creates the shaders, compiles them, and links the program.
	 *
	 * This class holds the source code for the shaders.
	 *
	 */
	void createProgram();


};

} /* namespace OevGLES */

#endif /* GLPROGDIFFUSELIGHT_H_ */
