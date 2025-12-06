/*
 * GLObjectWrappers.cpp
 *
 *  Created on: Oct 25, 2025
 *      Author: hor
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2025  Kai Horstmann
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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "OVFCommon.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>

// includes also "GLObjectWrappers.h
#include "Renderers/RenderContext.h"


namespace OevGLES {

GLBufferObject::GLBufferObject(bool doCreateBuffer) {
	if (doCreateBuffer) {
		glGenBuffers(1, &bufferHandle);
	}
}

GLBufferObject::~GLBufferObject() {
	if (bufferHandle != 0U) {
		glDeleteBuffers(1, &bufferHandle);
		bufferHandle = 0;
	}
}

GLBufferObject::GLBufferObject(GLBufferObject &&other) :
	bufferHandle {other.bufferHandle}
{

	other.bufferHandle = 0;

}

GLBufferObject& GLBufferObject::operator=(GLBufferObject &&other) {
	if (bufferHandle != 0U) {
		glDeleteBuffers(1, &bufferHandle);
	}

	bufferHandle = other.bufferHandle;
	other.bufferHandle = 0;

	return *this;
}

GLVertexArrayObject::GLVertexArrayObject(RenderContextSharedPtr const &contextPtr) :
	vertexArrayHandle {0U},
	glBindVertexArrayOES {contextPtr->glBindVertexArrayOES},
	glDeleteVertexArraysOES {contextPtr->glDeleteVertexArraysOES},
	glGenVertexArraysOES {contextPtr->glGenVertexArraysOES},
	glIsVertexArrayOES {contextPtr->glIsVertexArrayOES},
	vertexArrayIsUsable {contextPtr->vertexArrayIsUsable}
{
	if (vertexArrayIsUsable && glGenVertexArraysOES != nullptr) {
		glGenVertexArraysOES(1,&vertexArrayHandle);
	}
}

GLVertexArrayObject::~GLVertexArrayObject() {
	if (vertexArrayHandle != 0U && glDeleteVertexArraysOES != nullptr){
		glDeleteVertexArraysOES(1,&vertexArrayHandle);
		vertexArrayHandle = 0U;
	}
}

GLVertexArrayObject::GLVertexArrayObject(GLVertexArrayObject &&other) :
	vertexArrayHandle {other.vertexArrayHandle},
	glDeleteVertexArraysOES {other.glDeleteVertexArraysOES},
	glGenVertexArraysOES {other.glGenVertexArraysOES},
	vertexArrayIsUsable {other.vertexArrayIsUsable}
{
	other.vertexArrayHandle = 0U;
	other.glDeleteVertexArraysOES = nullptr;
	other.glGenVertexArraysOES = nullptr;
	other.vertexArrayIsUsable = false;
	
}

GLVertexArrayObject& GLVertexArrayObject::operator=(GLVertexArrayObject &&other) {
	if (vertexArrayHandle != 0U && glDeleteVertexArraysOES != nullptr){
		glDeleteVertexArraysOES(1,&vertexArrayHandle);
		vertexArrayHandle = 0U;
	}

	vertexArrayHandle = other.vertexArrayHandle;
	glDeleteVertexArraysOES = other.glDeleteVertexArraysOES;
	glGenVertexArraysOES = other.glGenVertexArraysOES;
	vertexArrayIsUsable = other.vertexArrayIsUsable;

	other.vertexArrayHandle = 0U;
	other.glDeleteVertexArraysOES = nullptr;
	other.glGenVertexArraysOES = nullptr;
	other.vertexArrayIsUsable = false;

	return *this;
}

} /* namespace OevGLES {*/
