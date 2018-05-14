/*
 * GLProgram.h
 *
 *  Created on: May 6, 2018
 *      Author: hor
 *
 *  Wrapper for an OpenGL program. Attaches Vertex and fragment shader, and links the entire program
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


#ifndef GLES_GLPROGRAM_H_
#define GLES_GLPROGRAM_H_

#include "GLES/GLShader.h"

namespace OevGLES {

/** \brief GL program wrapper.
 *
 * The class manages the vertex and fragment shader.
 *
 * It automatically retrieves the uniform and attribute names, and types
 *
 */
class GLProgram {
public:

	/** \brief Constructor. You must attach vertex and fragment shader with \ref attachVertexShader \ref attachFragmentShader manually
	 *
	 */
	GLProgram();

	/** \brief Constructor which attaches vertex and fragment shader immediately. You can \ref linkProgram immediately.
	 *
	 * @param vertexShader
	 * @param fragmentShader
	 */
	GLProgram(GLVertexShader *vertexShader,GLFragmentShader *fragmentShader);

	/// \brief Destructor
	virtual ~GLProgram();

	/** \brief Attach the vertex shader to the program.
	 *
	 * The pointer to the shader object is passed. This program object becomes owner of the shader, and will delete it upon destruction.
	 *
	 * If a vertex shader was attached before, and this method is called again the previously attached shader is detached and deleted.
	 * The program is considered not linked any more. All attributes and uniform information are deleted.
	 * Vertex pointers are made invalid.
	 *
	 * If you call the method with vertexShader = NULL a previously existing shader is detached, deleted...
	 *
	 * @param vertexShader Pointer to the vertex shader
	 */
	void attachVertexShader (GLVertexShader *vertexShader);

	/** \brief Returns a pointer to the vertex shader.
	 *
	 * If no vertex shader was attached the method returns NULL.
	 *
	 * @return Pointer to the vertex shader.
	 */
	GLVertexShader *getVertexShader() {
		return vertexShader;
	}

	/** \brief Returns a pointer to the constant vertex shader.
	 *
	 * If no vertex shader was attached the method returns NULL.
	 *
	 * @return Pointer to the constant vertex shader.
	 */
	GLVertexShader const *getCVertexShader() const {
		return vertexShader;
	}

	/** \brief Attach the fragment shader to the program.
	 *
	 * The pointer to the shader object is passed. This program object becomes owner of the shader, and will delete it upon destruction.
	 *
	 * If a fragment shader was attached before, and this method is called again the previously attached shader is detached and deleted.
	 * The program is considered not linked any more. All attributes and uniform information are deleted.
	 * fragment pointers are made invalid.
	 *
	 * If you call the method with fragmentShader = NULL a previously existing shader is detached, deleted...
	 *
	 * @param fragmentShader Pointer to the fragment shader
	 */
	void attachFragmentShader (GLFragmentShader *fragmentShader);

	/** \brief Returns a pointer to the fragment shader.
	 *
	 * If no fragment shader was attached the method returns NULL.
	 *
	 * @return Pointer to the fragment shader.
	 */
	GLFragmentShader *getFragmentShader() {
		return fragmentShader;
	}

	/** \brief Returns a pointer to the constant fragment shader.
	 *
	 * If no fragment shader was attached the method returns NULL.
	 *
	 * @return Pointer to the constant fragment shader.
	 */
	GLFragmentShader const *getFragmentShader() const {
		return fragmentShader;
	}

	/** \brief Link the program consisting of vertex and fragment shader.
	 *
	 * The vertex and fragment shader are compiled if necessary before the program is linked.
	 * If compilation and linking are successful the uniforms and vertex attribute information are immediately queried.
	 *
	 * @throws ProgramException
	 * @throws ShaderException
	 */
	void linkProgram ();

private:

	GLuint programHandle = 0;

	GLVertexShader		*vertexShader = 0;
	GLFragmentShader	*fragmentShader  = 0;

	bool isLinked = false;

	GLint numAttributes = 0;
	GLint numUniforms = 0;

	/** \brief queries the program for uniform and vertex attributes, and their index, type and other information
	 *
	 * Called by \ref linkProgram() when linking is successful;
	 *
	 */
	void retrieveShaderVariableInfos();

	/** \brief Detach the vertex shader from the program, delete the shader in GL and delete the shader object.
	 *
	 * After detaching the program is no longer considered being linked.
	 *
	 */
	void detachVertexShader();

	/** \brief Detach the fragment shader from the program, delete the shader in GL and delete the shader object.
	 *
	 * After detaching the program is no longer considered being linked.
	 *
	 */
	void detachFragmentShader();




};

} /* namespace OevGLES */

#endif /* GLES_GLPROGRAM_H_ */
