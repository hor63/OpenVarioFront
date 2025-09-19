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

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <memory>
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
	
	You can replace shared pointers with the reset routines.
	The reset routines will also create new instances of the MV or MVP matrixes
	when necessary.
	Once you replace a shared pointer with a new one the attribute will become
	independent from the copy source.
	
	You can also edit the data in-place with the non-const get methods. This
	will change the data for all copies of this which share the shared pointers
	In case that you modify model, view or perspective matrix the MV and MVP
	matrixes will be re-calculated when you call a getter method for them.
*/
class RenderStandardUniforms {
public:

	struct Mat4WithChangeCounter {
		Mat4 matrix4;
		int32_t changeCounter;
		
		Mat4WithChangeCounter(); 
		Mat4WithChangeCounter(Mat4WithChangeCounter const &s) = default;
		Mat4WithChangeCounter & operator = (Mat4WithChangeCounter const & s) = default;
	};
	using Mat4WithChangeCounterPtr = std::shared_ptr<Mat4WithChangeCounter>;

	RenderStandardUniforms();

	Mat4 & getModelMatrix ();
	Mat4 & getViewMatrix ();
	Mat4 & getProjMatrix ();
	// There is no writable access to the MV and MVP matrixes.
	// They are derived from the M, V and P matrixes automatically.
	
	Vec3 & getLightDir () {
		return *lightDirPtr;
	}
	Vec4 & getLightColor () {
		return *lightColorPtr;
	}
	Vec4 & getAmbientLightColor () {
		return *ambientLightColorPtr;
	}

	Mat4 const & getModelMatrixC () const {
		return modelMatrixPtr->matrix4;
	}
	Mat4 const & getViewMatrixC () const {
		return viewMatrixPtr->matrix4;
	}
	Mat4 const & getProjMatrixC () const {
		return projMatrixPtr->matrix4;
	}
	Mat4 const &getMVMatrixC() const;
	Mat4 const &getMVPMatrixC() const;
	Vec3 const &getLightDirC() const {
		return *lightDirPtr;
	}
	Vec4 const & getLightColorC () const {
		return *lightColorPtr;
	}
	Vec4 const & getAmbientLightColorC () const {
		return *ambientLightColorPtr;
	}

	void resetModelMatrixPtr();
	void resetViewMatrixPtr();
	void resetProjMatrixPtr();
	void resetLightDirPtr();
	void resetLightColorPtr();
	void resetAmbientLightColorPtr();

	void recalcMVMatrix() const;
	
	void recalcMVPMatrix() const;

private:

	static int32_t maxMatrixChangeCounter;

	Mat4WithChangeCounterPtr modelMatrixPtr;
	Mat4WithChangeCounterPtr viewMatrixPtr;
	Mat4WithChangeCounterPtr projMatrixPtr;
	mutable Mat4WithChangeCounterPtr MVMatrixPtr;
	mutable Mat4WithChangeCounterPtr MVPMatrixPtr;
	Vec3ShPtr lightDirPtr;
	Vec4ShPtr lightColorPtr;
	Vec4ShPtr ambientLightColorPtr;

	void resetMVMatrix();
	void resetMVPMatrix();
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
