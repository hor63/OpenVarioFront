/*
 * SquareTextureRenderer.h
 *
 *  Created on: Jun 10, 2018
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

#ifndef SQUARETEXTURERENDERER_H_
#define SQUARETEXTURERENDERER_H_


#include "GLPrograms/GLProgDiffLightTexture.h"
#include "Renderers/RendererBase.h"
#include "GLES/GLTexture.h"

class SquareTextureRenderer : public RendererBase {
public:
	SquareTextureRenderer();

	virtual ~SquareTextureRenderer();

	/** \brief Setup the vertex arrays, calculate normals... and setup VBOs
	 *
	 */
	virtual void setupVertexBuffers () override;

	/** \brief Draw the rendered object.
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
			)  override;


private:

	// the memory layout of the vertex array is as follows:
	//	0123	45
	//	Pos		TextureCoordinates
	// The base color and the normals for the texture are constant vertex attributes, therefore not part of the vertex array

	/* The texture is rendered by 2 triangles in a fan forming a square.
	 *
	 *	V2	  V1
	 *1	-------
	 *	|\    |
	 *	| \   |
	 *	|  \  |
	 *	|   \ |
	 *	|    \|
	 *	-------
	 *0	V3	  V0
	 *	0	  1
	 */

	GLfloat vertexArray[ 4 // 4 vertexes in the
						*6]; // Per vertex position as Vec4 and texture coordinate as Vec2

	GLfloat textureBaseColor [4] = {1.0f,1.0f,1.0f,1.0f};
	GLfloat textureNormal [4] = {0.0f, 0.0f, 1.0f, 0.0f};

	OevGLES::GLProgDiffLightTexture* glProgram = 0;

	GLuint vertexBufferHandle = 0;

	OevGLES::GLTexture varioBackgoundTexture;

};

#endif /* SQUARETEXTURERENDERER_H_ */
