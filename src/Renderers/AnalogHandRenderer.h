/*
 * AnalogHandRenderer.h
 *
 *  Created on: May 17, 2018
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

#ifndef ANALOGHANDRENDERER_H_
#define ANALOGHANDRENDERER_H_

#include "GLPrograms/GLProgDiffuseLight.h"
#include "Renderers/RendererBase.h"

class AnalogHandRenderer : public RendererBase {
public:
	AnalogHandRenderer();

	virtual ~AnalogHandRenderer();

	/** \brief Setup the vertex arrays, calculate normals... and setup VBOs
	 *
	 */
	virtual void setupVertexBuffers () override;

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
			OevGLES::Mat4 const &MVPMatrix
			)  override;


private:

	// the memory layout of the vertex array is as follows:
	//	0123	4567
	//	Pos		Normal
	// The color for the hand is constant vertex attribute, therefore not part of the vertex array

	// The hand has 4 triangles. Since I do not want to smooth colors the normals per triangle must be constant per triangle.
	// This means I cannot use a triangle strip or fan.

	GLfloat vertexArray[ 4  // 4 triangles
						*3 // 3 vertexes per triangle
						*8]; // Per vertex position and normal as Vec4
};

#endif /* ANALOGHANDRENDERER_H_ */
