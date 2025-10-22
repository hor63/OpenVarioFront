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
	data.
	Thus copying such an object shares the uniform data, i.e. change one of the
	uniforms in one objects, the change applies to all copies.
	
	You can replace shared pointers with the reset... method.
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

	/** \brief Combines a 4x4 matrix with a change sequence counter
	 
	 The change counter is used for shared objects to indicate a change of
	 content for consumers of the matrix.
	*/
	struct Mat4WithChangeCounter {
		Mat4 matrix4;
		int32_t changeCounter;
		
		Mat4WithChangeCounter(); 
		Mat4WithChangeCounter(Mat4WithChangeCounter const &s) = default;
		Mat4WithChangeCounter & operator = (Mat4WithChangeCounter const & s) = default;
	};
	using Mat4WithChangeCounterPtr = std::shared_ptr<Mat4WithChangeCounter>;

	RenderStandardUniforms();
	
	/** \brief Writable access to the model matrix
	 
	 Upon calling the changeCounter is being updated to indicate a possible
	 change of the content.
	 
	 Use only when you intend to modify the content.
	 Calling this method for read access only works of course but is inefficient
	 because it forces a re-caculation of \ref MVMatrixPtr and
	 \ref MVPMatrixPtr contents when you access them.
	 If you only want to read the content call \ref getModelMatrixC instead.
	 	 
	 \see \ref modelMatrixPtr
	 \see \ref getModelMatrixC read-only access()
	*/
	Mat4 & getModelMatrix ();
	
	/** \brief Writable access to the view matrix
	 
	 Upon calling the changeCounter is being updated to indicate a possible
	 change of the content.
	 
	 Use only when you intend to modify the content. If you only want to read
	 the content call \ref getViewMatrixC.
	 Calling this method for read access only works of course but is inefficient
	 because it forces a re-caculation of \ref MVMatrixPtr and
	 \ref MVPMatrixPtr contents when you access them.
	 
	 \see \ref viewMatrixPtr
	 \see \ref getViewMatrixC() read-only access
	*/
	Mat4 & getViewMatrix ();
	
	/** \brief Writable access to the projection matrix
	 
	 Upon calling the changeCounter is being updated to indicate a possible
	 change of the content.
	 
	 Use only when you intend to modify the content. If you only want to read
	 the content call \ref getProjMatrixC.
	 Calling this method for read access only works of course but is inefficient
	 because it forces a re-caculation of
	 \ref MVPMatrixPtr contents when you access them.
	 
	 \see \ref projectionMatrixPtr
	 \see \ref getProjMatrixC() read-only access
	*/
	Mat4 & getProjMatrix ();
	
	// There is no writable access to the MV and MVP matrixes.
	// They are derived from the M, V and P matrixes automatically.
	
	/** \brief Writable access to the light direction vector.
	 
	 \see \ref lightDirPtr
	 \see \ref getLightDirC() read-only access
	*/
	Vec3 & getLightDir () {
		return *lightDirPtr;
	}
	
	/** \brief Writable access to the light color array.
	 
	 \see \ref lightColorPtr
	 \see \ref getLightColorC() read-only access
	*/
	Vec4 & getLightColor () {
		return *lightColorPtr;
	}
	
	/** \brief Writable access to the ambient light color array.
	 
	 \see \ref ambientLightColorPtr
	 \see \ref getAmbientLightColorC() read-only access
	*/
	Vec4 & getAmbientLightColor () {
		return *ambientLightColorPtr;
	}

	/** \brief Read-only access to the model matrix.
	 
	 \see \ref modelMatrixPtr
	 \see \ref getModelMatrix() read/write access
	*/
	Mat4 const & getModelMatrixC () const {
		return modelMatrixPtr->matrix4;
	}

	/** \brief Read-only access to the view matrix.
	 
	 \see \ref viewMatrixPtr
	 \see \ref getViewMatrix() read/write access
	*/
	Mat4 const & getViewMatrixC () const {
		return viewMatrixPtr->matrix4;
	}

	/** \brief Read-only access to the projection matrix.
	 
	 \see \ref projectionMatrixPtr
	 \see \ref getProjMatrix() read/write access
	*/
	Mat4 const & getProjMatrixC () const {
		return projectionMatrixPtr->matrix4;
	}

	/** \brief Read-only access to the model-view matrix.
	 
	 \ref MVMatrixPtr is automatically re-calculated when \ref modelMatrixPtr
	 and/or \ref viewMatrixPtr have been modified by calls to 
	 \ref getModelMatrix() or \ref getViewMatrix().
	 
	 Therefore \ref MVMatrixPtr is declared mutable to be updated also for
	 constant objects.
	 
	 There is no writable access method for \ref MVMatrixPtr because it is a
	 derived value, and calculated automatically.
	 
	 \see \ref MVMatrixPtr
	*/
	Mat4 const &getMVMatrixC() const;

	/** \brief Read-only access to the model-view-projection matrix.
	 
	 \ref MVPMatrixPtr is automatically re-calculated when \ref modelMatrixPtr
	 and/or \ref viewMatrixPtr and/or \ref projectionMatrixPtr have been modified by
	 calls to \ref getModelMatrix() or \ref getViewMatrix() or
	 \ref getProjMatrix().
	 
	 Therefore \ref MVPMatrixPtr is declared mutable to be updated also for
	 constant objects.
	 
	 There is no writable access method for \ref MVPMatrixPtr because it is a
	 derived value, and calculated automatically.
	 
	 \see \ref MVPMatrixPtr
	*/
	Mat4 const &getMVPMatrixC() const;

	/** \brief Read-only access to the light direction vector.
	 
	 \see \ref lightDirPtr
	 \see \ref getLightDir() read/write access
	*/
	Vec3 const &getLightDirC() const {
		return *lightDirPtr;
	}

	/** \brief Read-only access to the light color array.
	 
	 \see \ref lightColorPtr
	 \see \ref getLightColor() read/write access
	*/
	Vec4 const & getLightColorC () const {
		return *lightColorPtr;
	}

	/** \brief Read-only access to the ambient light color array.
	 
	 \see \ref ambientLightColorPtr
	 \see \ref getAmbientLightColor() read/write access
	*/
	Vec4 const & getAmbientLightColorC () const {
		return *ambientLightColorPtr;
	}

	/** \brief Create a copy of the model matrix, and store it in a new shared pointer.
	 
	 This method un-shares the model matrix with all other copies of \p this.
	 The value of the model matrix remains the same.
	 But changes to it will not affect any previous copies of \p this.
	 To keep the derived matrixes consistent \ref resetMVMatrix() and 
	 \ref resetMVPMatrix() are called to create new independent instances of
	 these shared pointers and their data too.
	
	 \see \ref modelMatrixPtr
	*/
	void resetModelMatrixPtr();
	
	/** \brief Set a new shared pointer for the model matrix.
	
	 This method un-shares the model matrix with all other copies of \p this
	 And replaces it with \p modelMatrixPtr.
	 You are responsible to filling the matrix to which the shared pointer links
	 yourself with valid data.
	 
	 To keep the derived matrixes consistent \ref resetMVMatrix() and 
	 \ref resetMVPMatrix() are called to create new independent instances of
	 these shared pointers and their data too.
	
	 \see \ref modelMatrixPtr
	*/
	void setModelMatrixPtr(Mat4WithChangeCounterPtr const& modelMatrixPtr);

	/// \see \ref modelMatrixPtr
	auto const& getModelMatrixPtr() const {
		return modelMatrixPtr;
	}

	/** \brief Create a copy of the view matrix, and store it in a new shared pointer.
	 
	 This method un-shares the view matrix with all other copies of \p this.
	 The value of the view matrix remains the same.
	 But changes to it will not affect any previous copies of \p this.
	 To keep the derived matrixes consistent \ref resetMVMatrix() and 
	 \ref resetMVPMatrix() are called to create new independent instances of
	 these shared pointers and their data too.
	
	\see \ref viewMatrixPtr
	*/
	void resetViewMatrixPtr();

	/** \brief Set a new shared pointer for the view matrix.
	
	 This method un-shares the view matrix with all other copies of \p this
	 And replaces it with \p viewMatrixPtr.
	 You are responsible to filling the matrix to which the shared pointer links
	 yourself with valid data.
	 
	 To keep the derived matrixes consistent \ref resetMVMatrix() and 
	 \ref resetMVPMatrix() are called to create new independent instances of
	 these shared pointers and their data too.
	
	 \see \ref viewMatrixPtr
	*/
	void setViewMatrixPtr(Mat4WithChangeCounterPtr const& viewMatrixPtr);

	/// \see \ref viewMatrixPtr
	auto const& getViewMatrixPtr() const {
		return viewMatrixPtr;
	}

	/** \brief Create a copy of the projection matrix, and store it in a new shared pointer.
	 
	 This method un-shares the projection matrix with all other copies of \p this.
	 The value of the projection matrix remains the same.
	 But changes to it will not affect any previous copies of \p this.
	 To keep the derived matrixes consistent 
	 \ref resetMVPMatrix() is called to create new independent instance of
	 that shared pointer and its data too.
	
	\see \ref projectionMatrixPtr
	*/
	void resetProjectionMatrixPtr();

	/** \brief Set a new shared pointer for the projection matrix.
	
	 This method un-shares the projection matrix with all other copies of \p this
	 And replaces it with \p projectionMatrixPtr.
	 You are responsible to filling the matrix to which the shared pointer links
	 yourself with valid data.
	 
	 To keep the derived matrixes consistent \ref resetMVMatrix() and 
	 \ref resetMVPMatrix() are called to create new independent instances of
	 these shared pointers and their data too.
	
	 \see \ref projectionMatrixPtr
	*/
	void setProjectionMatrixPtr(Mat4WithChangeCounterPtr const& projectionMatrixPtr);

	/// \see \ref projectionMatrixPtr
	auto const& getProjectionMatrixPtr() const {
		return projectionMatrixPtr;
	}

	/** Create a copy of the light direction vector, and store it in a new
	 shared pointer.
	 
	 This method un-shares the light direction vector with all other copies 
	 of \p this.
	 The value of the light direction vector remains the same.
	 But changes to it will not affect any previous copies of \p this.
	
	\see \ref lightDirPtr
	*/
	void resetLightDirPtr();

	/** Create a copy of the light color array, and store it in a new
	 shared pointer.
	 
	 This method un-shares the light color array with all other copies 
	 of \p this.
	 The value of the light color array remains the same.
	 But changes to it will not affect any previous copies of \p this.
	
	\see \ref lightColorPtr
	*/
	void resetLightColorPtr();

	/** Create a copy of the ambient light color array, and store it in a new
	 shared pointer.
	 
	 This method un-shares the ambient light color array with all other copies 
	 of \p this.
	 The value of the ambient light color array remains the same.
	 But changes to it will not affect any previous copies of \p this.
	
	\see \ref ambientLightColorPtr
	*/
	void resetAmbientLightColorPtr();

	/** \brief Recalculate the model-view matrix calling also 
	 \ref recalcMVPMatrix()
	 
	 Despite declared const it changes \ref MVMatrixPtr which is therefore
	 declared mutable
	*/
	void recalcMVMatrix() const;
	
	/** \brief Recalculate the model-view-projection matrix
	 
	 Despite declared const it changes \ref MVPMatrixPtr which is therefore
	 declared mutable
	*/
	void recalcMVPMatrix() const;

private:

	static int32_t maxMatrixChangeCounter;

	/// \brief Initialized to unity matrix.
	Mat4WithChangeCounterPtr modelMatrixPtr;
	/// \brief Initialized to unity matrix.
	Mat4WithChangeCounterPtr viewMatrixPtr;
	/// \brief Initialized to unity matrix.
	Mat4WithChangeCounterPtr projectionMatrixPtr;
	
	/** \brief Initialized to unity matrix.
	 
	 It is declared mutable because it is called indirectly from
	 \ref getMVMatrixC() or \ref getMVPMatrixC()
	 */
	mutable Mat4WithChangeCounterPtr MVMatrixPtr;
	
	/** \brief Initialized to unity matrix.
	 
	 It is declared mutable because it is called indirectly from
	 \ref getMVMatrixC() or \ref getMVPMatrixC()
	 */
	mutable Mat4WithChangeCounterPtr MVPMatrixPtr;
	Vec3ShPtr lightDirPtr;
	Vec4ShPtr lightColorPtr;
	Vec4ShPtr ambientLightColorPtr;

	/** Create a copy of the model-view matrix, and store it in a new shared pointer.
	 
	 This method un-shares the MV matrix with all other copies of \p this.
	 The value of the MV matrix remains the same.
	 But changes to it will not affect any previous copies of \p this.
	 To keep the derived matrixes consistent
	 
	 \ref resetMVPMatrix() is called to create new independent instance of
	 that shared pointer and its data too.
	 
	 It is private because the MV matrix is a derived value, and must not be
	 writeable accessible from outside.
	
	\see \ref MVMatrixPtr
	*/
	void resetMVMatrix();

	/** Create a copy of the model-view-projection matrix, and store it in a new
	 shared pointer.
	 
	 This method un-shares the MVP matrix with all other copies of \p this.
	 The value of the MVP matrix remains the same.
	 But changes to it will not affect any previous copies of \p this.
	 
	 It is private because the MV matrix is a derived value, and must not be
	 writeable accessible from outside.
	
	\see \ref MVPMatrixPtr
	*/
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
