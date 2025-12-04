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
	
	/** \brief Delete the buffer object in \ref bufferHandle when one is managed by \p this.
	 *
	 */
	~GLBufferObject();
	GLBufferObject(const GLBufferObject &other) = delete;
	GLBufferObject(GLBufferObject &&other);
	GLBufferObject& operator=(const GLBufferObject &other) = delete;
	GLBufferObject& operator=(GLBufferObject &&other);
	
	/// \brief Use the object instead of the raw buffer handle
	unsigned int get() const {return bufferHandle;}
	operator unsigned int const & () const {return bufferHandle;}

	/** \brief Check if the buffer object can be used.
	 *
	 *	\return \p true when the buffer object exists and is usable. Else return \p false.
	 */
	operator bool () const {return (bufferHandle != 0);}

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
		
		glBindBuffer(bufferType, bufferObject);
		boundBufferHandle = bufferObject;
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
				// Mmmh, I am overwriting a bound object with another bound object.
				// That is not intended.
				// I will deal with it as good as possible:
				// Bind the source buffer since this is going to be overwritten.
				// But I am backing up the currently bound buffer in the system.
				GLint tmpBoundBuffer = 0;
				glGetIntegerv (glGetPname,&tmpBoundBuffer);
				// overwrite the backup buffer in source too.
				source.backupBoundBuffer = tmpBoundBuffer;

				glBindBuffer(bufferType, source.boundBufferHandle);
			} else {
				// I am resetting this to an empty object.
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
	GLVertexArrayObject(RenderContext const &context);
	
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
	operator unsigned int const & () const {return vertexArrayHandle;}
	
	/** \brief Check if the vertex array can be used.
	 *
	 *	\return \p true when the vertex array exists and is usable. Else return \p false.
	 */
	operator bool () const {return (vertexArrayHandle != 0);}

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
		boundVertexArrayrHandle {0U},
		backupVertexArrayBuffer {0U},
		glBindVertexArrayOES {nullptr}
	{
		
		if (vertexArrayObject.vertexArrayIsUsable &&
			glBindVertexArrayOES != nullptr) {
			// Store the currently bound buffer, or no buffer (handle = 0)
			GLint tmpBoundVertexArray = 0;
			glGetIntegerv (GL_VERTEX_ARRAY_BINDING_OES,&tmpBoundVertexArray);
			backupVertexArrayBuffer = tmpBoundVertexArray;
			
			glBindVertexArrayOES( vertexArrayObject);
			boundVertexArrayrHandle = vertexArrayObject;
			vertexArrayWasBound = true;
		}
	}

	GLBindVertexArrayObject (GLBindVertexArrayObject const& source) = delete;
	
	GLBindVertexArrayObject (GLBindVertexArrayObject && source) :
		vertexArrayWasBound{source.vertexArrayWasBound},
		boundVertexArrayrHandle{source.boundVertexArrayrHandle},
		backupVertexArrayBuffer{source.backupVertexArrayBuffer}
	{
		source.vertexArrayWasBound = false;
		source.backupVertexArrayBuffer = 0;
	}

	GLBindVertexArrayObject & operator = (GLBindVertexArrayObject const& source) = delete;

	GLBindVertexArrayObject & operator = (GLBindVertexArrayObject && source) {
		
		if (vertexArrayWasBound) [[unlikely]] {
			if (source.vertexArrayWasBound) {
				// Mmmh, I am overwriting a bound object with another bound object.
				// That is not intended.
				// I will deal with it as good as possible:
				// Bind the source buffer since this is going to be overwritten.
				// But I am backing up the currently bound buffer in the system.
				GLint tmpBoundBuffer = 0;
				glGetIntegerv (GL_VERTEX_ARRAY_BINDING_OES,&tmpBoundBuffer);
				// overwrite the backup buffer in source too.
				source.backupVertexArrayBuffer = tmpBoundBuffer;

				glBindVertexArrayOES( source.boundVertexArrayrHandle);
			} else {
				// I am resetting this to an empty object.
				// Therefore binding the backup buffer.
				glBindVertexArrayOES( backupVertexArrayBuffer);
			}
		}
		
		// In any case copy all source information to this.
		vertexArrayWasBound = source.vertexArrayWasBound;
		boundVertexArrayrHandle = source.boundVertexArrayrHandle;
		backupVertexArrayBuffer = source.backupVertexArrayBuffer;
		
		// ... and make the source an empty object, thus making the move complete.
		source.vertexArrayWasBound = false;
		source.boundVertexArrayrHandle = 0U;
		source.backupVertexArrayBuffer = 0U;
		
		return *this;
	}

	~GLBindVertexArrayObject () {
		if (vertexArrayWasBound) {
			glBindVertexArrayOES( backupVertexArrayBuffer);
		}
	}
	
private:
	bool vertexArrayWasBound = false;
	/// \brief The currently bound buffer when \ref bufferObjectWasBound is \p true.
	GLuint boundVertexArrayrHandle = 0U;
	
	/// \brief used to save an already bound buffer which is restored in the destructor
	/// 	when \ref bufferObjectWasBound is \p true.
	GLuint backupVertexArrayBuffer = 0U;

	PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = nullptr;
	
}; // class GLBindVertexArrayObject

} /* namespace OevGLES { */

#endif /* LIB_UTIL_GLBUFFEROBJECT_H_ */
