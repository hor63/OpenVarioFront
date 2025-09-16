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

#include "VecMat.h"

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

/// \brief Alias for setting standard mode blending, i.e. simulating transparency
/// by the source (vertex color) alpha value
using BlendAttributeSetRestoreStd =
		BlendAttributeSetRestore<
			GL_TRUE,GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,
			GL_ZERO,GL_FUNC_ADD,GL_FUNC_ADD>;

/** \brief Holds the standard vectors and matrixes commonly used as uniforms for
	rendering stuff, particular for \ref RendererBase::draw().
	
	The class stores shared pointers to the uniform data.
	This makes it economical to copy objects because they reference to the same
	data, and also makes it easy to inherit shared data.
	
	You can replace data with the set routines.
	The set routines will also create new instances of the MV or MVP matrixes.
	
	You can also edit the data in-place with the non-const get methods this
	will change the data for all copies of this!
	In case that you modify model, view or perspective matrix do not forget
	to call \ref recalcMVMatrix() or \ref recalcMVPMatrix() respective. Use or 
	abuse this functionality at your own peril.
*/
class RenderStandardUniforms {
public:
	void setModelMatrix (Mat4ShPtr const &modelMatrixPtr) {
		modelMatrix = modelMatrixPtr;
		resetMVMatrix();
	}
	
	void setViewMatrix (Mat4ShPtr const &viewMatrixPtr) {
		viewMatrix = viewMatrixPtr;
		resetMVMatrix();
	}

	void setProjMatrix (Mat4ShPtr const &projMatrixPtr) {
		projMatrix = projMatrixPtr;
		resetMVPMatrix();
	}
	void setLightDir (Vec3ShPtr const &lightDirPtr) {
		lightDir = lightDirPtr;
	}
	void setLightColor (Vec4ShPtr const &lightColorPtr) {
		lightColor = lightColorPtr;
	}
	void setAmbientLightColor (Vec4ShPtr const &ambientLightColorPtr) {
		ambientLightColor = ambientLightColorPtr;
	}

	Mat4ShPtr & getModelMatrix () {
		return modelMatrix;
	}
	Mat4ShPtr & getViewMatrix () {
		return viewMatrix;
	}
	Mat4ShPtr & getProjMatrix () {
		return projMatrix;
	}
	Mat4ShPtr & getMVMatrix () {
		return MVMatrix;
	}
	Mat4ShPtr & getMVPMatrix () {
		return MVPMatrix;
	}
	Vec3ShPtr & getLightDir () {
		return lightDir;
	}
	Vec4ShPtr & getLightColor () {
		return lightColor;
	}
	Vec4ShPtr & getAmbientLightColor () {
		return ambientLightColor;
	}

	Mat4ShPtr const & getModelMatrix () const {
		return modelMatrix;
	}
	Mat4ShPtr const & getViewMatrix () const {
		return viewMatrix;
	}
	Mat4ShPtr const & getProjMatrix () const {
		return projMatrix;
	}
	Mat4ShPtr const & getMVMatrix () const {
		return MVMatrix;
	}
	Mat4ShPtr const & getMVPMatrix () const {
		return MVPMatrix;
	}
	Vec3ShPtr const & getLightDir () const {
		return lightDir;
	}
	Vec4ShPtr const & getLightColor () const {
		return lightColor;
	}
	Vec4ShPtr const & getAmbientLightColor () const {
		return ambientLightColor;
	}

	void recalcMVMatrix() {
		if (modelMatrix && viewMatrix) {
			// At this point MVMatrix is guaranteed to be valid.
			*MVMatrix = *viewMatrix.get() * *modelMatrix.get();
			recalcMVPMatrix();
		}
	}
	
	void recalcMVPMatrix() {
		if (MVMatrix && projMatrix) {
			// At this point MVPMatrix is guaranteed to be valid.
			*MVPMatrix = *projMatrix.get() * *MVMatrix.get();
		}
	}

private:
	Mat4ShPtr modelMatrix;
	Mat4ShPtr viewMatrix;
	Mat4ShPtr projMatrix;
	Mat4ShPtr MVMatrix;
	Mat4ShPtr MVPMatrix;
	Vec3ShPtr lightDir;
	Vec4ShPtr lightColor;
	Vec4ShPtr ambientLightColor;

	void resetMVMatrix() {
		if (modelMatrix && viewMatrix) {
			MVMatrix.reset(new Mat4(*viewMatrix.get() * *modelMatrix.get()));
			resetMVPMatrix();
		} else {
			MVMatrix.reset();
			MVPMatrix.reset();
		}
	}
	
	void resetMVPMatrix() {
		if (MVMatrix && projMatrix) {
							MVPMatrix.reset(new Mat4(*projMatrix.get() * *MVMatrix.get()));
		}
	}
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
	 * \param stdUniformData Reference to the standard uniforms for rendering.
	 */
	virtual void draw(RenderStandardUniforms const &stdUniformData) = 0;

protected:
};

} // namespace OevGLES

#endif /* RENDERERBASE_H_ */
