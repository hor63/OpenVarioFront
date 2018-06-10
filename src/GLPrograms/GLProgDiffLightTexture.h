/*
 * GLProgDiffLightTexture.h
 *
 *  Created on: Jun 4, 2018
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

#ifndef GLPROGDIFFLIGHTTEXTURE_H_
#define GLPROGDIFFLIGHTTEXTURE_H_

#include "GLPrograms/GLProgBase.h"


namespace OevGLES {

class GLProgDiffLightTexture :public GLProgBase {
public:

	virtual ~GLProgDiffLightTexture();

	/** \brief Return the only instance of the program
	 *
	 * If the instance did not exist before it is created, the shaders are created, and the program is linked.
	 *
	 * @return Pointer to the instance of the program
	 */
	static GLProgDiffLightTexture *getProgram();

	/** \brief Destroy the single instance of the program.
	 *
	 * This call should be made at the end of the program, or if the GLProgram is really no longer used.
	 * After it is called all pointers obtained by \ref getProgram become invalid
	 * If you call \ref getProgram after this call a new program object is created and returned to the caller.
	 *
	 */
	static void destroyProgram();

	/** \brief Retrieve the vertex shader code.
	 *
	 * @return Vertex shader code as one C string
	 */
	virtual char const* getVertexShaderCode() const override;

	/** \brief Retrieve the frament shader code.
	 *
	 * @return Fragment shader code as one C string
	 */
	virtual char const* getFragmentShaderCode() const override;

	// The uniforms
	GLProgram::ShaderVariableInfo const &getMvpMatrixInfo() const {
		return mvpMatrixInfo;
	}
	GLint getMvpMatrixLocation () const {
		return mvpMatrixLocation;
	}


	GLProgram::ShaderVariableInfo  const &getMvMatrixInfo() const {
		return mvMatrixInfo;
	}
	GLint getMvMatrixLocation () const {
		return mvMatrixLocation;
	}

	GLProgram::ShaderVariableInfo  const &getLightDirInfo() const {
		return lightDirInfo;
	}
	GLint getLightDirLocation () const {
		return lightDirLocation;
	}

	GLProgram::ShaderVariableInfo  const &getLightColorInfo() const {
		return lightColorInfo;
	}
	GLint getLightColorLocation () const {
		return lightColorLocation;
	}

	GLProgram::ShaderVariableInfo  const &getAmbientLightColorInfo() const {
		return ambientLightColorInfo;
	}
	GLint getAmbientLightColorLocation () const {
		return ambientLightColorLocation;
	}

	GLProgram::ShaderVariableInfo  const &getTexture0Info() const {
		return texture0Info;
	}
	GLint getTexture0Location () const {
		return texture0Location;
	}

	// The vertex attributes
	GLProgram::ShaderVariableInfo  const &getVertexPosInfo() const {
		return vertexPosInfo;
	}
	GLint getVertexPosLocation () const {
		return vertexPosLocation;
	}

	GLProgram::ShaderVariableInfo  const &getVertexNormalInfo() const {
		return vertexNormalInfo;
	}
	GLint getVertexNormalLocation () const {
		return vertexNormalLocation;
	}

	GLProgram::ShaderVariableInfo  const &getVertexColorInfo() const {
		return vertexColorInfo;
	}
	GLint getVertexColorLocation () const {
		return vertexColorLocation;
	}

	GLProgram::ShaderVariableInfo  const &getVertexTexture0PosInfo() const {
		return vertexTexture0PosInfo;
	}
	GLint getVertexTexture0PosLocation () const {
		return vertexTexture0PosLocation;
	}


protected:

	/** \brief Called after linking the GL program to retrieve and store the shader variable information in the sub-class of this class.
	 *
	 * The variables which are to be retrieved are depending on the declarations and the actual shader code.
	 *
	 */
	virtual void retrieveShaderVariableInfo() override;


private:
	/// \brief The only instance of this program object.
	static GLProgDiffLightTexture* theProgram;

	// The uniforms
	GLProgram::ShaderVariableInfo	mvpMatrixInfo;
	GLint 							mvpMatrixLocation = 0;
	GLProgram::ShaderVariableInfo	mvMatrixInfo;
	GLint							mvMatrixLocation = 0;
	GLProgram::ShaderVariableInfo	lightDirInfo;
	GLint							lightDirLocation = 0;
	GLProgram::ShaderVariableInfo	lightColorInfo;
	GLint							lightColorLocation = 0;
	GLProgram::ShaderVariableInfo	ambientLightColorInfo;
	GLint							ambientLightColorLocation = 0;
	GLProgram::ShaderVariableInfo	texture0Info;
	GLint							texture0Location = 0;

	// The vertex attributes
	GLProgram::ShaderVariableInfo	vertexPosInfo;
	GLint							vertexPosLocation = 0;
	GLProgram::ShaderVariableInfo	vertexNormalInfo;
	GLint							vertexNormalLocation = 0;
	GLProgram::ShaderVariableInfo	vertexColorInfo;
	GLint							vertexColorLocation = 0;
	GLProgram::ShaderVariableInfo	vertexTexture0PosInfo;
	GLint							vertexTexture0PosLocation = 0;

	/** \brief private constructor
	 *
	 * The constructor is private because only the static method \ref getProgram() will create the only object of this class on demand
	 *
	 */
	GLProgDiffLightTexture() {

	}
};

} /* namespace OevGLES */

#endif /* GLPROGDIFFLIGHTTEXTURE_H_ */
