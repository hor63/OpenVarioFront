/*
 * RendererBase.h
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

#ifndef RENDERERBASE_H_
#define RENDERERBASE_H_

#include "GLPrograms/GLProgBase.h"

#if defined Success
#	undef Success
#endif

#include "GLES/VecMat.h"

class RendererBase {
public:

	/** \brief Constructor
	 *
	 */
	RendererBase();

	/** \brief destructor
	 *
	 */
	virtual ~RendererBase();

	/** \brief Setup the vertex arrays, calculate normals... and setup VBOs
	 *
	 */
	virtual void setupVertexBuffers () = 0;

	/** \brief Draw the rendered object.
	 *
	 * To be generic
	 *
	 * @param modelMatrix Model matrix, moves the object around from model to world space
	 * @param viewMatrix View matrix, used to move from world to eye space
	 * @param ProjMatrix Projection matrix, used to create the 3-dimensional effects on a 2D screen
	 * @param MVMatrix Model-View Matrix
	 * @param MVPMatrix Model/View/Projection matrix
	 */
	virtual void draw(
			OevGLES::Mat4 const &modelMatrix,
			OevGLES::Mat4 const &viewMatrix,
			OevGLES::Mat4 const &ProjMatrix,
			OevGLES::Mat4 const &MVMatrix,
			OevGLES::Mat4 const &MVPMatrix,
			OevGLES::Vec3 const &lightDir,
			OevGLES::Vec4 const &lightColor,
			OevGLES::Vec4 const &ambientLightColor
			) = 0;

protected:

};

#endif /* RENDERERBASE_H_ */
