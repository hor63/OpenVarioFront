/*
 * GLShader.h
 *
 *  Created on: May 4, 2018
 *      Author: hor
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

#ifndef GLES_GLSHADER_H_
#define GLES_GLSHADER_H_

#include <string>
#include <istream>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>

#include "GLES/ExceptionBase.h"

namespace OevGLES {

class GLShader {
public:

	/** \brief Constructor with in-memory shader source
	 *
	 * @param shaderSource Shader source text
	 */
	GLShader(const char *shaderSource);

	/** \brief Constructor with an opened input stream for reading the source text
	 *
	 * The source text is read in chunks of 1KB bytes.
	 *
	 * @param istream Input stream pointing to the shader source. Can either be a text file or a istringstream.
	 */
	GLShader(std::istream &istream);

	/** \brief pure virtual method returning the shader type
	 *
	 * To he overridden by subclasses.
	 *
	 * @return GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	 */
	virtual GLuint getShaderType() const = 0;

	virtual ~GLShader();

	/** \brief Creates the shader and compiles it from \ref shaderText.
	 *
	 */
	void compileShader();

	/** \brief Obtain the handle of the shader object
	 *
	 * Before the shader is created and compiled with \ref compileShader the method returns 0.
	 *
	 * @return Shader handle
	 */
	GLuint getShaderHandle () const {
		return shaderHandle;
	}

	/** \brief Conversion operator to the shader handle type
     *
	 * @return Shader handle
	 */
	operator GLuint () const {
		return shaderHandle;
	}

	bool isShaderCompiled() const {
		return isCompiled;
	}

private:

	std::string shaderText;

	GLuint shaderHandle = 0;

	bool isCompiled = false;

};

class GLVertexShader :public GLShader {
public:

	/** \brief Constructor with in-memory shader source
	 *
	 * \see GLShader(const char *shaderSource)
	 *
	 * @param shaderSource Shader source text
	 */
	GLVertexShader(const char *shaderSource)
		:GLShader{shaderSource}
		{}

	/** \brief Constructor with an opened input stream for reading the source text
	 *
	 * \see GLShader(std::istream &istream)
	 *
	 * @param istream Input stream pointing to the shader source. Can either be a text file or a istringstream.
	 */
	GLVertexShader(std::istream &istream)
		:GLShader{istream}
	{}

	/** \brief Returning the shader type
	 *
	 * To he overridden by subclasses.
	 *
	 * @return GL_VERTEX_SHADER
	 */
	virtual GLuint getShaderType() const override;

};

class GLFragmentShader :public GLShader {
public:

	/** \brief Constructor with in-memory shader source
	 *
	 * \see GLShader(const char *shaderSource)
	 *
	 * @param shaderSource Shader source text
	 */
	GLFragmentShader(const char *shaderSource)
		:GLShader{shaderSource}
		{}

	/** \brief Constructor with an opened input stream for reading the source text
	 *
	 * \see GLShader(std::istream &istream)
	 *
	 * @param istream Input stream pointing to the shader source. Can either be a text file or a istringstream.
	 */
	GLFragmentShader(std::istream &istream)
		:GLShader{istream}
	{}

	/** \brief Returning the shader type
	 *
	 * To he overridden by subclasses.
	 *
	 * @return GL_FRAGMENT_SHADER
	 */
	virtual GLuint getShaderType() const override;

};


} /* namespace OevGLES */

#endif /* GLES_GLSHADER_H_ */
