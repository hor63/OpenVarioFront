/*
 * GLBufferObject.h
 *
 *  Created on: Oct 25, 2025
 *      Author: hor
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2025  Kai Horstcppmann
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

#ifndef LIB_UTIL_GLBUFFEROBJECT_H_
#define LIB_UTIL_GLBUFFEROBJECT_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <memory>

#include "ExceptionBase.h"

namespace OevGLES {

/** \brief Thin wrapper around GL buffer objects
 *
 * Manages the lifetime of a GL buffer by its own lifetime
 * 
 * You can move it but you cannot copy it. If you need a new
 * buffer object create a new GLBufferObject.
 
 * If you move it the source's buffer handle is moved to the target.
 * If the target had a buffer object that is destroyed.
 * After the source's \ref bufferHandle is 0, i.e. no valid buffer any more.
 */
class GLBufferObject final {
public:
	/** \brief Constructor creates an OpenGL buffer object.
	 * 
	 * Only the buffer object is being created. Nothing else.
	 * Use it for whatever you want, either a vertex buffer, or an
	 * index buffer or whatever your GL version allows.
	 *
	 * You can create an object without actually creating a GL buffer, and defer the GL buffer creation.
	 * When you are creating an empty object you can assign it a defined buffer by move assign an object with a valid
	 * GL buffer.
	 *
	 * \param doCreateBuffer Indicates if a GL buffer shall be created upon construction or not
	 */
	GLBufferObject(bool doCreateBuffer);
	
	GLBufferObject(const GLBufferObject &other) = delete;
	GLBufferObject(GLBufferObject &&other);
	GLBufferObject& operator=(const GLBufferObject &other) = delete;
	GLBufferObject& operator=(GLBufferObject &&other);

	// \brief Delete the buffer object in \ref bufferHandle when one is managed by \p this.
	~GLBufferObject();
	
	/// \brief Use the object instead of the raw buffer handle
	unsigned int get() const {return bufferHandle;}

	/** \brief Check if the buffer object can be used.
	 *
	 *	\return \p true when the buffer object exists and is usable. Else return \p false.
	 */
	bool valid() const {return (bufferHandle != 0);}

private:

	unsigned int bufferHandle = 0;

};

using GLBufferObjectSharedPtr = std::shared_ptr<GLBufferObject>;
using GLBufferObjectWeakPtr = std::weak_ptr<GLBufferObject>;

template <GLenum bufferType>
class GlBindBufferObject final {
	
	static_assert(bufferType == GL_ARRAY_BUFFER || bufferType == GL_ELEMENT_ARRAY_BUFFER,
		"Template parameter bufferType must be either GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER");
public:

	static constexpr GLenum glGetPname = 
		bufferType == GL_ARRAY_BUFFER ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING;

	/** \brief Constructs an un-bound object for deferred binding
	 */
	GlBindBufferObject () {}

	GlBindBufferObject (GLBufferObject const& bufferObject) {
		
		// Store the currently bound buffer, or no buffer (handle = 0)
		GLint tmpBoundBuffer = 0;
		glGetIntegerv (glGetPname,&tmpBoundBuffer);
		backupBoundBuffer = tmpBoundBuffer;
		
		glBindBuffer(bufferType, bufferObject.get());
		boundBufferHandle = bufferObject.get();
		bufferObjectWasBound = true;
	}

	GlBindBufferObject (GlBindBufferObject const& source) = delete;
	
	GlBindBufferObject (GlBindBufferObject && source) :
		bufferObjectWasBound{source.bufferObjectWasBound},
		boundBufferHandle{source.boundBufferHandle},
		backupBoundBuffer{source.backupBoundBuffer}
	{
		source.bufferObjectWasBound = false;
		source.backupBoundBuffer = 0;
	}

	GlBindBufferObject & operator = (GlBindBufferObject const& source) = delete;

	GlBindBufferObject & operator = (GlBindBufferObject && source) {
		
		if (bufferObjectWasBound) [[unlikely]] {
			if (source.bufferObjectWasBound) {
				// Trying to overwrite a bound buffer with another bound buffer is frowned upon
				throw GLObjectWrapperException(
					"You must not move a bound buffer wrapper to another bound buffer wrapper");
			} else {
				// I am resetting this to an empty object.
				// This is allowed. This is equivalent to deleting this ahead of time
				// Therefore binding the backup buffer.
				glBindBuffer(bufferType, backupBoundBuffer);
			}
		}
		
		// In any case copy all source information to this.
		bufferObjectWasBound = source.bufferObjectWasBound;
		boundBufferHandle = source.boundBufferHandle;
		backupBoundBuffer = source.backupBoundBuffer;
		
		// ... and make the source an empty object, thus making the move complete.
		source.bufferObjectWasBound = false;
		source.boundBufferHandle = 0U;
		source.backupBoundBuffer = 0U;
		
		return *this;
	}

	~GlBindBufferObject () {
		if (bufferObjectWasBound) {
			glBindBuffer(bufferType, backupBoundBuffer);
		}
	}
	
	/** \brief Re-binds and stores a different buffer in this.
	 *
	 *
	 * The stored buffer handle which is restored by the destructor is *not* changed, when the object
	 * was valid bore.
	 *
	 * If this was empty before this is move-assigned a GlBindBufferObject constructed from \p bufferObject.
	 */
	void reset (GLBufferObject const& bufferObject) {
		if (bufferObjectWasBound) {
		glBindBuffer(bufferType, bufferObject.get());
		boundBufferHandle = bufferObject.get();
		} else {
			*this = GlBindBufferObject(bufferObject);
		}
	}

	bool valid () {return bufferObjectWasBound;}

	
private:
	bool bufferObjectWasBound = false;
	/// \brief The currently bound buffer when \ref bufferObjectWasBound is \p true.
	GLuint boundBufferHandle = 0U;
	
	/// \brief used to save an already bound buffer which is restored in the destructor
	/// 	when \ref bufferObjectWasBound is \p true.
	GLuint backupBoundBuffer = 0U;
}; // class GlBindBufferObject

using GlBindArrayBufferObject 		 = GlBindBufferObject<GL_ARRAY_BUFFER>;
using GlBindElementArrayBufferObject = GlBindBufferObject<GL_ELEMENT_ARRAY_BUFFER>;

// forward declaration
struct RenderContext;
using RenderContextSharedPtr = std::shared_ptr<RenderContext>;

/** \brief Thin wrapper around GL vertex array objects
 *
 * Manages the lifetime of a GL vertex array by its own lifetime
 * 
 * You can move it but you cannot copy it. If you need a new
 * buffer object create a new GLBufferObject.
 
 * If you move it the source's buffer handle is moved to the target.
 * If the target had a buffer object that is destroyed.
 * After the source's \ref bufferHandle is 0, i.e. no valid buffer any more.
 */
class GLVertexArrayObject final {

	friend class GLBindVertexArrayObject;

public:

	/** \brief Creates an empty unusable vertex array object
	 */
	GLVertexArrayObject() {}

	/** \brief Constructor creates an OpenGL vertex array object.
	 * 
	 * 
	 */
	GLVertexArrayObject(RenderContextSharedPtr const &contextPtr);
	
	/** \brief Delete the buffer object in \ref bufferHandle when one is managed by \p this.
	 *
	 */
	~GLVertexArrayObject();
	GLVertexArrayObject(const GLVertexArrayObject &other) = delete;
	GLVertexArrayObject(GLVertexArrayObject &&other);
	GLVertexArrayObject& operator=(const GLVertexArrayObject &other) = delete;
	GLVertexArrayObject& operator=(GLVertexArrayObject &&other);
	
	/// \brief Use the object instead of the raw buffer handle
	unsigned int get() const {return vertexArrayHandle;}
	
	/** \brief Check if the vertex array can be used.
	 *
	 *	\return \p true when the vertex array exists and is usable. Else return \p false.
	 */
	bool valid() const {return (vertexArrayHandle != 0);}

private:
	GLuint vertexArrayHandle = 0;
	
	PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = nullptr;
	PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES = nullptr;
	PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES = nullptr;
	PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES = nullptr;
	bool vertexArrayIsUsable = false;

}; // GLVertexArrayObject

class GLBindVertexArrayObject final {
public:

public:

	/** \brief Constructs an un-bound object for deferred binding
	 */
	GLBindVertexArrayObject () {}

	GLBindVertexArrayObject (GLVertexArrayObject const& vertexArrayObject) :
		vertexArrayWasBound {false},
		boundVertexArrayHandle {0U},
		backupVertexArrayBuffer {0U},
		glBindVertexArrayOES {vertexArrayObject.glBindVertexArrayOES}
	{
		
		if (vertexArrayObject.vertexArrayIsUsable &&
			glBindVertexArrayOES != nullptr) {
			// Store the currently bound buffer, or no buffer (handle = 0)
			GLint tmpBoundVertexArray = 0;
			glGetIntegerv (GL_VERTEX_ARRAY_BINDING_OES,&tmpBoundVertexArray);
			backupVertexArrayBuffer = tmpBoundVertexArray;
			
			glBindVertexArrayOES( vertexArrayObject.get());
			boundVertexArrayHandle = vertexArrayObject.get();
			vertexArrayWasBound = true;
		}
	}

	GLBindVertexArrayObject (GLBindVertexArrayObject const& source) = delete;
	
	GLBindVertexArrayObject (GLBindVertexArrayObject && source) :
		vertexArrayWasBound{source.vertexArrayWasBound},
		boundVertexArrayHandle{source.boundVertexArrayHandle},
		backupVertexArrayBuffer{source.backupVertexArrayBuffer},
		glBindVertexArrayOES{source.glBindVertexArrayOES}
	{
		source.vertexArrayWasBound = false;
		source.boundVertexArrayHandle = 0U;
		source.backupVertexArrayBuffer = 0;
		source.glBindVertexArrayOES = nullptr;
	}

	GLBindVertexArrayObject & operator = (GLBindVertexArrayObject const& source) = delete;

	GLBindVertexArrayObject & operator = (GLBindVertexArrayObject && source) {
		
		if (vertexArrayWasBound) [[unlikely]] {
			if (source.vertexArrayWasBound) {
				// Mmmh, I am overwriting a bound object with another bound object.
				// That is not intended.
				// Overwriting an assigned vertex array attribute with another one is frowned upon
				throw GLObjectWrapperException(
					"You must not move a defined bound vertex array object "
						"to another defined vertex array object");
			} else {
				// I am resetting this to an empty object.
				// Therefore binding the backup buffer.
				glBindVertexArrayOES( backupVertexArrayBuffer);
			}
		}

		// In any case copy all source information to this.
		vertexArrayWasBound = source.vertexArrayWasBound;
		boundVertexArrayHandle = source.boundVertexArrayHandle;
		backupVertexArrayBuffer = source.backupVertexArrayBuffer;
		glBindVertexArrayOES = source.glBindVertexArrayOES;
		
		// ... and make the source an empty object, thus making the move complete.
		source.vertexArrayWasBound = false;
		source.boundVertexArrayHandle = 0U;
		source.backupVertexArrayBuffer = 0U;
		source.glBindVertexArrayOES = nullptr;
		
		return *this;
	}

	~GLBindVertexArrayObject () {
		if (vertexArrayWasBound) {
			glBindVertexArrayOES( backupVertexArrayBuffer);
		}
	}

	void reset (GLVertexArrayObject const& vertexArrayObject) {
		
		if (vertexArrayWasBound) {
			// Just re-bind the vertex array and store the new handle.
			glBindVertexArrayOES( vertexArrayObject.get());
			boundVertexArrayHandle = vertexArrayObject.get();
			vertexArrayWasBound = true;
		} else {
			// Reset the entire object including the backup handle to
			// restore upon destruction.
			*this = GLBindVertexArrayObject (vertexArrayObject);
		}
	}

	bool valid() {return vertexArrayWasBound;}
	
private:
	bool vertexArrayWasBound = false;
	/// \brief The currently bound buffer when \ref bufferObjectWasBound is \p true.
	GLuint boundVertexArrayHandle = 0U;
	
	/// \brief used to save an already bound buffer which is restored in the destructor
	/// 	when \ref bufferObjectWasBound is \p true.
	GLuint backupVertexArrayBuffer = 0U;

	PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = nullptr;
	
}; // class GLBindVertexArrayObject

/** \brief Wrapper around the GL functions glEnableVertexAttribArray and glDisableVertexAttribArray
 *
 * An object stores the previous status if an vertex array buffer was enabled for a vertex attribute index.
 * 
 * The default constructor creates an empty object which does neither change the array enable attribute nor
 * stores and restores the previous attribute status. Such an object is an *empty* *object*.
 * \ref valid() returns \p false. \n
 * The non-default constructor sets the enable status. It stores the previous status, and restores it upon destruction.
 * Such an object is non-empty. \ref valid() returns \p true.
 
  * The move constructor and move assignment operator allow resetting an object with some restrictions:
 *   - You can always move an empty object into any target object. If the target was not-empty its previous status
 *     is restored, and after the target is empty.\n
 *     If the target was emtpy before nothing happens. The target is empty.
 *   - You can move a non-empty object only into an empty object. When you move a non-empty object into a non-empty
 *     target you earn a \ref GLObjectWrapperException.
 *     The move simply moves the content of the source object into the target. The assumption is that any status change
 *     has happened before during construction of the source object.
 *
 * In any case the source object of a move is always empty.
 * 
 */
class GLVertexArrayAttribObject final {
	
	public:
	
	/// \brief Create an empty object for deferred vertex attribute setting
	GLVertexArrayAttribObject() {}
	
	GLVertexArrayAttribObject(bool enableVertexArray,GLuint vertexIndex)
	{
		GLint isVertexArrayEnabled = 0;
		glGetVertexAttribiv(vertexIndex,GL_VERTEX_ATTRIB_ARRAY_ENABLED,&isVertexArrayEnabled);
		
		// Only if the current status is different from the target status
		// set the status and store the function address to restore the status.
		// Else just skip over it and leave this with no index reference.
		if ((isVertexArrayEnabled != 0) != enableVertexArray) {
			
			this->vertexIndex = vertexIndex;
	
			
			// Remember how to restore the array attribute status.
			if (isVertexArrayEnabled == 0) {
				arrayStatusRestoreFunction = glDisableVertexAttribArray;
			} else {
				arrayStatusRestoreFunction = glEnableVertexAttribArray;
			}
			
			if (enableVertexArray) {
				glEnableVertexAttribArray (vertexIndex);
			} else {
				glDisableVertexAttribArray (vertexIndex);
			}
		} // if ((isVertexArrayEnabled != 0) != enableVertexArray) {
		
	}
	
	GLVertexArrayAttribObject (GLVertexArrayAttribObject const& source) = delete;
	GLVertexArrayAttribObject (GLVertexArrayAttribObject && source) :
		vertexIndex {source.vertexIndex},
		arrayStatusRestoreFunction {source.arrayStatusRestoreFunction}
	{
		// and now the move part.
		source.vertexIndex = 0U;
		source.arrayStatusRestoreFunction = nullptr;
	}

	~GLVertexArrayAttribObject () {
		if (arrayStatusRestoreFunction != nullptr) {
			arrayStatusRestoreFunction(vertexIndex);
		}
	}

	GLVertexArrayAttribObject& operator = (GLVertexArrayAttribObject const& source) = delete;
	GLVertexArrayAttribObject& operator = (GLVertexArrayAttribObject && source) {
		// If this has already a defined status things are getting a bit complex.
		if (arrayStatusRestoreFunction != nullptr) [[unlikely]] {
			if (source.arrayStatusRestoreFunction == nullptr) {
				// If source is empty restore the status stored in this.
				arrayStatusRestoreFunction(vertexIndex);
			} else {
				// Overwriting an assigned vertex array attribute with another one is frowned upon
				throw GLObjectWrapperException(
					"You must not move a defined vertex array attribute object "
						"to another defined vertex array attribute object");
			} 
		}

		vertexIndex = source.vertexIndex;
		arrayStatusRestoreFunction = source.arrayStatusRestoreFunction;
		// and now the move part.
		source.vertexIndex = 0U;
		source.arrayStatusRestoreFunction = nullptr;

		return *this;
	}
	
	bool valid () const {return arrayStatusRestoreFunction == nullptr;}
	
	private:
	
	GLuint vertexIndex = 0U;

	/** \brief Can point either to \p glEnableVertexAttribArray or \p glDisableVertexAttribArray.
	 *
	 * If it is \p nullptr the object is not pointing to a valid vertex index. No action is taken in the destructor.
	 */
	void (*arrayStatusRestoreFunction) (GLuint index) = nullptr;
}; // class VertexArryAttribObject

} /* namespace OevGLES { */

#endif /* LIB_UTIL_GLBUFFEROBJECT_H_ */
