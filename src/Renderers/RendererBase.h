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

namespace OevGLES {

/** \brief Class which sets blending attributes with the constructor, and restores original values in the destructor
 *
 * Please note that the destination parameters are template parameters, assuming that your blending mode is fixed at compile time.
 */
template <GLboolean destBlendFlag = GL_TRUE,
		GLint destFuncSRGB = GL_SRC_ALPHA,
		GLint destFuncDRGB = GL_ONE_MINUS_SRC_ALPHA,
		GLint destFuncSAlpha = GL_ONE,
		GLint destFuncDAlpha = GL_ZERO,

		GLint destEquationRGB = GL_FUNC_ADD,
		GLint destEquationAlpha = GL_FUNC_ADD
		>
class BlendAttributeSetRestore {

public:

	BlendAttributeSetRestore() {

		glGetIntegerv(GL_BLEND_SRC_RGB,&funcSRGB);
		glGetIntegerv(GL_BLEND_SRC_ALPHA,&funcSAlpha);
		glGetIntegerv(GL_BLEND_DST_RGB,&funcDRGB);
		glGetIntegerv(GL_BLEND_DST_ALPHA,&funcDAlpha);

		glGetIntegerv(GL_BLEND_EQUATION_RGB,&equationRGB);
		glGetIntegerv(GL_BLEND_EQUATION_ALPHA,&equationAlpha);

		if (
				destFuncSRGB != funcSRGB ||
				destFuncDRGB != funcDRGB ||
				destFuncSAlpha != funcSAlpha ||
				destFuncDAlpha != funcDAlpha ||
				destEquationRGB != equationRGB ||
				destEquationAlpha != equationAlpha
		) {
			doRestoreBlendAttributes = true;

			glBlendFuncSeparate(
					destFuncSRGB,
					destFuncDRGB,
					destFuncSAlpha,
					destFuncDAlpha);

			glBlendEquationSeparate(
					destEquationRGB,
					destEquationAlpha);

		}

		glGetBooleanv (GL_BLEND,&blendFlag);
		if (blendFlag != destBlendFlag) {
			doRestoreBlendFlag = true;
			if (destBlendFlag == GL_TRUE) {
				glEnable (GL_BLEND);
			} else {
				glDisable (GL_BLEND);
			}
		}

	}

	~BlendAttributeSetRestore(){
		if (doRestoreBlendFlag ) {
			if (blendFlag == GL_TRUE) {
				glEnable (GL_BLEND);
			} else {
				glDisable (GL_BLEND);
			}
		}

		if (doRestoreBlendAttributes) {

			glBlendFuncSeparate(
					funcSRGB,
					funcDRGB,
					funcSAlpha,
					funcDAlpha);

			glBlendEquationSeparate(
					equationRGB,
					equationAlpha);

		}

	}

private:
	GLboolean blendFlag = destBlendFlag;
	bool doRestoreBlendFlag = false;

	GLint funcSRGB = destFuncSRGB;
	GLint funcDRGB = destFuncDRGB;
	GLint funcSAlpha = destFuncSAlpha;
	GLint funcDAlpha = destFuncDAlpha;

	GLint equationRGB = destEquationRGB;
	GLint equationAlpha = destEquationAlpha;

	bool doRestoreBlendAttributes = false;
};

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
	 * Pure virtual interface
	 *
	 */
	virtual void setupVertexBuffers () = 0;

	/** \brief Draw the rendered object.
	 *
	 * Pure virtual interface
	 *
	 * \param modelMatrix Model matrix, moves the object around from model to world space
	 * \param viewMatrix View matrix, used to move from world to eye space
	 * \param ProjMatrix Projection matrix, used to create the 3-dimensional effects on a 2D screen
	 * \param MVMatrix Model-View Matrix
	 * \param MVPMatrix Model/View/Projection matrix
	 * \param lightDir Direction to the light source, normalized
	 * \param lightColor Color of the light source, normalized color values.
	 * \param ambientLightColor Color of the ambient light, normalized color values.
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

} // namespace OevGLES

#endif /* RENDERERBASE_H_ */
