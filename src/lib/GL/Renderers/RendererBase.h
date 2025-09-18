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

#include <cassert>
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

	void setModelMatrixPtr (Mat4ShPtr const &modelMatrixPtr) {
		this->modelMatrixPtr = modelMatrixPtr;
		resetMVMatrix();
	}
	
	void setViewMatrixPtr (Mat4ShPtr const &viewMatrixPtr) {
		this->viewMatrixPtr = viewMatrixPtr;
		resetMVMatrix();
	}

	void setProjMatrixPtr (Mat4ShPtr const &projMatrixPtr) {
		this->projMatrixPtr = projMatrixPtr;
		resetMVPMatrix();
	}
	void setLightDirPtr (Vec3ShPtr const &lightDirPtr) {
		this->lightDirPtr = lightDirPtr;
	}
	void setLightColorPtr (Vec4ShPtr const &lightColorPtr) {
		this->lightColorPtr = lightColorPtr;
	}
	void setAmbientLightColorPtr (Vec4ShPtr const &ambientLightColorPtr) {
		this->ambientLightColorPtr = ambientLightColorPtr;
	}

	Mat4ShPtr & getModelMatrixPtr () {
		return modelMatrixPtr;
	}
	Mat4ShPtr & getViewMatrixPtr () {
		return viewMatrixPtr;
	}
	Mat4ShPtr & getProjMatrixPtr () {
		return projMatrixPtr;
	}
	Mat4ShPtr & getMVMatrixPtr () {
		return MVMatrixPtr;
	}
	Mat4ShPtr & getMVPMatrixPtr () {
		return MVPMatrixPtr;
	}
	Vec3ShPtr & getLightDirPtr () {
		return lightDirPtr;
	}
	Vec4ShPtr & getLightColorPtr () {
		return lightColorPtr;
	}
	Vec4ShPtr & getAmbientLightColorPtr () {
		return ambientLightColorPtr;
	}

	Mat4ShPtr const & getModelMatrixPtr () const {
		return modelMatrixPtr;
	}
	Mat4ShPtr const & getViewMatrixPtr () const {
		return viewMatrixPtr;
	}
	Mat4ShPtr const & getProjMatrixPtr () const {
		return projMatrixPtr;
	}
	Mat4ShPtr const & getMVMatrixPtr () const {
		return MVMatrixPtr;
	}
	Mat4ShPtr const & getMVPMatrixPtr () const {
		return MVPMatrixPtr;
	}
	Vec3ShPtr const & getLightDirPtr () const {
		return lightDirPtr;
	}
	Vec4ShPtr const & getLightColorPtr () const {
		return lightColorPtr;
	}
	Vec4ShPtr const & getAmbientLightColorPtr () const {
		return ambientLightColorPtr;
	}

	Mat4 & getModelMatrix () {
		
		assert (modelMatrixPtr.operator bool());
		return *modelMatrixPtr;
	}
	Mat4 & getViewMatrix () {
		assert (viewMatrixPtr.operator bool());
		return *viewMatrixPtr;
	}
	Mat4 & getProjMatrix () {
		assert (projMatrixPtr.operator bool());
		return *projMatrixPtr;
	}
	// There is no writable access to the MV and MVP matrixes.
	// They are derived from the M, V and P matrixes automatically.
	
	Vec3 & getLightDir () {
		assert (lightDirPtr.operator bool());
		return *lightDirPtr;
	}
	Vec4 & getLightColor () {
		assert (lightColorPtr.operator bool());
		return *lightColorPtr;
	}
	Vec4 & getAmbientLightColor () {
		assert (ambientLightColorPtr.operator bool());
		return *ambientLightColorPtr;
	}

	Mat4 const & getModelMatrixC () const {
		
		assert (modelMatrixPtr.operator bool());
		return *modelMatrixPtr;
	}
	Mat4 const & getViewMatrixC () const {
		assert (viewMatrixPtr.operator bool());
		return *viewMatrixPtr;
	}
	Mat4 const & getProjMatrixC () const {
		assert (projMatrixPtr.operator bool());
		return *projMatrixPtr;
	}
	Mat4 const & getMVMatrixC () const {
		assert (MVMatrixPtr.operator bool());
		return *MVMatrixPtr;
	}
	Mat4 const & getMVPMatrixC () const {
		assert (MVPMatrixPtr.operator bool());
		return *MVPMatrixPtr;
	}
	Vec3 const & getLightDirC () const {
		assert (lightDirPtr.operator bool());
		return *lightDirPtr;
	}
	Vec4 const & getLightColorC () const {
		assert (lightColorPtr.operator bool());
		return *lightColorPtr;
	}
	Vec4 const & getAmbientLightColorC () const {
		assert (ambientLightColorPtr.operator bool());
		return *ambientLightColorPtr;
	}


	void recalcMVMatrix() {
		if (modelMatrixPtr && viewMatrixPtr) {
			// At this point MVMatrix is guaranteed to be valid.
			*MVMatrixPtr = *viewMatrixPtr.get() * *modelMatrixPtr.get();
			recalcMVPMatrix();
		}
	}
	
	void recalcMVPMatrix() {
		if (MVMatrixPtr && projMatrixPtr) {
			// At this point MVPMatrix is guaranteed to be valid.
			*MVPMatrixPtr = *projMatrixPtr.get() * *MVMatrixPtr.get();
		}
	}

private:
	Mat4ShPtr modelMatrixPtr;
	Mat4ShPtr viewMatrixPtr;
	Mat4ShPtr projMatrixPtr;
	Mat4ShPtr MVMatrixPtr;
	Mat4ShPtr MVPMatrixPtr;
	Vec3ShPtr lightDirPtr;
	Vec4ShPtr lightColorPtr;
	Vec4ShPtr ambientLightColorPtr;

	void resetMVMatrix() {
		if (modelMatrixPtr && viewMatrixPtr) {
			MVMatrixPtr.reset(new Mat4(*viewMatrixPtr.get() * *modelMatrixPtr.get()));
			resetMVPMatrix();
		} else {
			MVMatrixPtr.reset();
			MVPMatrixPtr.reset();
		}
	}
	
	void resetMVPMatrix() {
		if (MVMatrixPtr && projMatrixPtr) {
			MVPMatrixPtr.reset(new Mat4(*projMatrixPtr.get() * *MVMatrixPtr.get()));
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
