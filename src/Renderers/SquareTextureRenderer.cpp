/*
 * SquareTextureRenderer.cpp
 *
 *  Created on: Jun 10, 2018
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "Renderers/SquareTextureRenderer.h"
#include "GLES/TexHelper/PngReader.h"

#if defined HAVE_LOG4CXX_H
#include "OVFCommon.h"
	static log4cxx::LoggerPtr logger;
#endif

SquareTextureRenderer::SquareTextureRenderer()
	:
	// Setup the positions
	/* The texture is rendered by 2 triangles in a fan forming a square.
	 *
	 *	V2	  V1
	 *	-------
	 *	|\    |
	 *	| \   |
	 *	|  \  |
	 *	|   \ |
	 *	|    \|
	 *	-------
	 *	V3	  V0
	 *
	 */
	  vertexArray {
		// Vertex0
		 10.0f,-10.0f,-1.0f,1.0f,	// Pos 0
		 1.0f,  0.0f,				// Texture coordinate 0
		 10.0f, 10.0f,-1.0f,1.0f,	// Pos 1
		 1.0f, 1.0f,				// Texture coordinate 1
		-10.0f, 10.0f,-1.0f,1.0f,	// Pos 2
		 0.0f, 1.0f,				// Texture coordinate 2
		-10.0f,-10.0f,-1.0f,1.0f,	// Pos 3
		 0.0f, 0.0f					// Texture coordinate 3
	}
	{

#if defined HAVE_LOG4CXX_H
		// Get the logger if necessary
		if (!logger) {
			logger = log4cxx::Logger::getLogger("OpenVarioFront.SquareTextureRenderer");
		}


		LOG4CXX_DEBUG(logger,"SquareTextureRenderer::SquareTextureRenderer()");

		if (logger->getLevel() == log4cxx::Level::getDebug()) {
			// Print the 4 vertexes
			for (int i=0; i < 4 ; i++) {

				GLfloat* p0 = vertexArray + (i*6);
				Eigen::Map<OevGLES::Vec4> vecX ( p0 );
				Eigen::Map<OevGLES::Vec2> vecXTexPos ( p0 + 4);

				LOG4CXX_DEBUG(logger,"Vertex #" << i << ": Position = [" << vecX.transpose() << "], NormaTexture coordinates = [" << vecXTexPos.transpose() << ']');

			}

			{
				Eigen::Map<OevGLES::Vec4> vecNormal (textureNormal);
				LOG4CXX_DEBUG(logger,"Normal of all vertextes = [" << vecNormal.transpose() << ']');
			}
		}
#endif

	}


SquareTextureRenderer::~SquareTextureRenderer() { }

void SquareTextureRenderer::setupVertexBuffers() {

	// First get the program
	glProgram = OevGLES::GLProgDiffLightTexture::getProgram();

	// make the program current
	glProgram->useProgram();

	glGenBuffers(1,&vertexBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER,vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertexArray),vertexArray,GL_STATIC_DRAW);

	// Load the texture into GL
	OevGLES::PngReader varioBackgoundReader ("./Vario5m.png");
	OevGLES::TextureData texData (8,8,OevGLES::TextureData::RGB,OevGLES::TextureData::Byte);
	varioBackgoundReader.readPngToTexture(texData);
	varioBackgoundTexture.setTextureData(texData);

}

void SquareTextureRenderer::draw(
		const OevGLES::Mat4& modelMatrix,
		const OevGLES::Mat4& viewMatrix, const OevGLES::Mat4& ProjMatrix,
		const OevGLES::Mat4& MVMatrix, const OevGLES::Mat4& MVPMatrix,
		const OevGLES::Vec3& lightDir, const OevGLES::Vec4& lightColor,
		const OevGLES::Vec4& ambientLightColor ) {

	GLfloat* bufferOffset = 0;

	// make my program current
	glProgram->useProgram();


	LOG4CXX_DEBUG(logger,"lightDir = " << lightDir.transpose());

	/*
	GLfloat* p0 = vertexArray;
	for (int k = 0;k < 6 ; k+= 2) {
		Eigen::Map<OevGLES::Vec4> vecXNormal4 ( p0 + (k*4) + 4);
		OevGLES::Vec3 vecXNormal = (MVMatrix * vecXNormal4).block<3,1>(0,0);

		LOG4CXX_DEBUG(logger,"Vec4 [" << k << "] Normal = [" << vecXNormal4.transpose() << ']');
		LOG4CXX_DEBUG(logger,"Vec4 [" << k << "] MVMatrix * Normal = [" << vecXNormal.transpose() << ']');
		LOG4CXX_DEBUG(logger,"lightDir dot normal = " << lightDir.dot(vecXNormal));

	}
	*/

	// Set the uniforms
	glUniformMatrix4fv(glProgram->getMvpMatrixLocation(),1,GL_FALSE,&(MVPMatrix(0,0)));
	glUniformMatrix4fv(glProgram->getMvMatrixLocation(),1,GL_FALSE,&(MVMatrix(0,0)));
	glUniformMatrix4fv(glProgram->getMvMatrixLocation(),1,GL_FALSE,&(MVMatrix(0,0)));

	glUniform3fv(glProgram->getLightDirLocation(),1,&(lightDir(0)));
	glUniform4fv(glProgram->getLightColorLocation(),1,&(lightColor(0)));
	glUniform4fv(glProgram->getAmbientLightColorLocation(),1,&(ambientLightColor(0)));


	// set the color attribute constant
	glDisableVertexAttribArray(glProgram->getVertexColorLocation());
	glVertexAttrib4fv(glProgram->getVertexColorLocation(),textureBaseColor);

	// set the vertex normal constant
	glEnableVertexAttribArray(glProgram->getVertexNormalLocation());
	glVertexAttrib4fv(glProgram->getVertexNormalLocation(),textureNormal);

	// re-bind the buffer object
	glBindBuffer(GL_ARRAY_BUFFER,vertexBufferHandle);

	// setup the vertex coordinates
	glEnableVertexAttribArray(glProgram->getVertexPosLocation());
	glVertexAttribPointer(glProgram->getVertexPosLocation(),4,GL_FLOAT,GL_FALSE,6 * sizeof (GLfloat),bufferOffset);
	// setup the texture coordinates
	bufferOffset += 4; // Advance the offset by 4 floats to the texture coordinate.
	glEnableVertexAttribArray(glProgram->getVertexTexture0PosLocation());
	glVertexAttribPointer(glProgram->getVertexTexture0PosLocation(),2,GL_FLOAT,GL_FALSE,6 * sizeof (GLfloat),bufferOffset);

	// Assign the texture to Texure engine 0, and set the sampler uniform accordingly
	varioBackgoundTexture.bindToUniformLocation(GL_TEXTURE0,0,glProgram->getTexture0Location());

	// The object is opaque. Use the depth buffer, and write to the depth buffer
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLE_FAN,0,4);


}

