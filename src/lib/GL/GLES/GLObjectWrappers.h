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
class GLBufferObject {
public:
	/** \brief Constructor creates an OpenGL buffer object.
	 * 
	 * Only the buffer object is being created. Nothing else.
	 * Use it for whatever you want, either a vertex buffer, or an
	 * index buffer or whatever your GL version allows.
	 */
	GLBufferObject();
	
	/** \brief Delete the buffer object in \ref bufferHandle when one is managed by \p this.
	 *
	 */
	virtual ~GLBufferObject();
	GLBufferObject(const GLBufferObject &other) = delete;
	GLBufferObject(GLBufferObject &&other);
	GLBufferObject& operator=(const GLBufferObject &other) = delete;
	GLBufferObject& operator=(GLBufferObject &&other);
	
	/// \brief Use the object instead of the raw buffer handle
	unsigned int get() const {return bufferHandle;}
	operator unsigned int const & () const {return bufferHandle;}
private:

	unsigned int bufferHandle = 0;

};

using GLBufferObjectSharedPtr = std::shared_ptr<GLBufferObject>;
using GLBufferObjectWeakPtr = std::weak_ptr<GLBufferObject>;
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

public:
	/** \brief Constructor creates an OpenGL buffer object.
	 * 
	 * Only the buffer object is being created. Nothing else.
	 * Use it for whatever you want, either a vertex buffer, or an
	 * index buffer or whatever your GL version allows.
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
	operator bool () const {return (vertexArrayHandle != 0);}

private:
	GLuint vertexArrayHandle = 0;
	
	PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES = nullptr;
	PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES = nullptr;
	bool vertexArrayIsUsable = false;

};

} /* namespace OevGLES { */

#endif /* LIB_UTIL_GLBUFFEROBJECT_H_ */
