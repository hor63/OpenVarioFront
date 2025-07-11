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

#include <memory>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "OVFCommon.h"

#include "GLES/GLFramework.h"
#include "Renderers/SquareTextureRenderer.h"
#include "GLES/TexHelper/PngReader.h"

#if defined HAVE_LOG4CXX_H
	static log4cxx::LoggerPtr logger = 0;
#endif

namespace OevGLES {

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
		{
			{256.0f,-256.0f,-26.0f,1.0f},
			{1.0f,  0.0f}
		},
		{
			{256.0f, 256.0f,-26.0f,1.0f},
			{ 1.0f, 1.0f}
		},
		{
			{-256.0f, 256.0f,-26.0f,1.0f},
			{0.0f, 1.0f}
		},
		{
			{-256.0f,-256.0f,-26.0f,1.0f},
			{ 0.0f, 0.0f}
		},
	
	}
	{

#if defined HAVE_LOG4CXX_H
		// Get the logger if necessary
		if (!logger) {
			logger = log4cxx::Logger::getLogger("OpenVarioFront.SquareTextureRenderer");
		}


		LOG4CXX_DEBUG(logger,"SquareTextureRenderer::SquareTextureRenderer()");

		if (logger->getEffectiveLevel() == log4cxx::Level::getDebug()) {
			// Print the 4 vertexes
			for (int i=0; i < 4 ; i++) {

				VertexType& vertex = vertexArray [i];
				Eigen::Map<OevGLES::Vec4> const vecX ( &vertex.position[0] );
				Eigen::Map<OevGLES::Vec2> const vecXTexPos ( &vertex.textureCoordinate[0]);

				LOG4CXX_DEBUG(logger,"Vertex #" << i << ": Position = [" << vecX.transpose() << "], NormaTexture coordinates = [" << vecXTexPos.transpose() << ']');

				Eigen::Map<OevGLES::Vec4> vecNormal (textureNormal);
				LOG4CXX_DEBUG(logger,"Normal of all vertextes = [" << vecNormal.transpose() << ']');
			}
		}
#endif

	}


SquareTextureRenderer::~SquareTextureRenderer() {

		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
		<< ": vertexBufferHandle = " << vertexBufferHandle
		<< ", vertexArrayHandle  = " << vertexArrayHandle
		);

	if (vertexBufferHandle != 0U) {
		glDeleteBuffers(1, &vertexBufferHandle);
		vertexBufferHandle = 0U;
	}
	if (vertexArrayHandle != 0U) {
		GLFramework::glDeleteVertexArraysOES(1,&vertexArrayHandle);
		vertexArrayHandle = 0U;
	}


 }

void SquareTextureRenderer::setPNGFileName (std::string const &pngFileName) {
	
	// Check if the the same file was set before, and in-memory data were
	// not being setup before.
	if (memLocation == nullptr && fileName == pngFileName) {
		// no action required.
		return;
	}
	
	fileName = pngFileName;
	// Reset in-memory data if it existed before.
	memLocation = nullptr;
	memLen = 0;
	dirty = true;
}

void SquareTextureRenderer::setPNGMemoryData(
		char const * memLocationPNGData,
		int lenPNGData,
		std::string const &pngImageName) {

	memLocation = memLocationPNGData;
	memLen = lenPNGData;
	fileName = pngImageName;

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
		<< ": memLocation = " << reinterpret_cast<void const *>(memLocation)
		<< ", lenPNGData = " << memLen
		<< ", pngImageName" << fileName);

	dirty = true;

}

void SquareTextureRenderer::setupVertexBuffers() {

		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
			<< ": dirty = " << dirty);


	if (dirty) {
	
		// First get the program
		glProgram = OevGLES::GLProgDiffLightTexture::getProgram();
	
		glGenBuffers(1,&vertexBufferHandle);
		glBindBuffer(GL_ARRAY_BUFFER,vertexBufferHandle);
		glBufferData(GL_ARRAY_BUFFER,sizeof(vertexArray),vertexArray,GL_STATIC_DRAW);
	
		std::unique_ptr<OevGLES::PngReader> varioBackgoundReader;
		// Load the texture into GL
		if (memLocation != nullptr) {
			LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
				<< ": create PngReader with memLocation = "
				<< reinterpret_cast<void const *>(memLocation)
				<< ", lenPNGData = " << memLen
				<< ", pngImageName" << fileName);
			varioBackgoundReader.reset(new PngReader (memLocation,memLen,fileName));
		} else {
			varioBackgoundReader.reset(new PngReader (fileName.c_str()));
		}
		OevGLES::TextureData texData (8,8,OevGLES::TextureData::RGB,OevGLES::TextureData::Byte);
		varioBackgoundReader->readPngToTexture(texData);
		varioBackgoundTexture.setTextureData(texData);
	
		varioBackgoundTexture.setMagnificationFilter(OevGLES::GLTexture::Linear);
		varioBackgoundTexture.setMinificationFilter(OevGLES::GLTexture::Linear);
	
		if (GLFramework::isVertexArrayUsable() && vertexArrayHandle == 0U) {
	
			GLFramework::glGenVertexArraysOES(1,&vertexArrayHandle);
			GLFramework::glBindVertexArrayOES(vertexArrayHandle);
	
			// setup the vertex coordinates
			glEnableVertexAttribArray(glProgram->getVertexPosLocation());
			glVertexAttribPointer(glProgram->getVertexPosLocation(),
				4,GL_FLOAT,GL_FALSE,
				sizeof (VertexType),
				reinterpret_cast<void*>(offsetof(VertexType,position)));
			// setup the texture coordinates
			glEnableVertexAttribArray(glProgram->getVertexTexture0PosLocation());
			glVertexAttribPointer(glProgram->getVertexTexture0PosLocation(),
				2,GL_FLOAT,GL_FALSE,
				sizeof (VertexType),
				reinterpret_cast<void*>(offsetof(VertexType,textureCoordinate)));
	
			GLFramework::glBindVertexArrayOES(0U);
		}
	
		glBindBuffer(GL_ARRAY_BUFFER,0);
		
		dirty = false;
	} // if (dirty) {
}

void SquareTextureRenderer::draw(
		const OevGLES::Mat4& modelMatrix,
		const OevGLES::Mat4& viewMatrix, const OevGLES::Mat4& ProjMatrix,
		const OevGLES::Mat4& MVMatrix, const OevGLES::Mat4& MVPMatrix,
		const OevGLES::Vec3& lightDir, const OevGLES::Vec4& lightColor,
		const OevGLES::Vec4& ambientLightColor ) {

	if (dirty) {
		setupVertexBuffers();
	}

	// make my program current
	glProgram->useProgram();


	LOG4CXX_TRACE(logger,"lightDir = " << lightDir.transpose());

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

	glUniform3fv(glProgram->getLightDirLocation(),1,&(lightDir(0)));
	glUniform4fv(glProgram->getLightColorLocation(),1,&(lightColor(0)));
	glUniform4fv(glProgram->getAmbientLightColorLocation(),1,&(ambientLightColor(0)));


	// set the color attribute constant
	glDisableVertexAttribArray(glProgram->getVertexColorLocation());
	glVertexAttrib4fv(glProgram->getVertexColorLocation(),textureBaseColor);

	// set the vertex normal constant
	glDisableVertexAttribArray(glProgram->getVertexNormalLocation());
	glVertexAttrib4fv(glProgram->getVertexNormalLocation(),textureNormal);

	if (vertexArrayHandle != 0U) {
		GLFramework::glBindVertexArrayOES(vertexArrayHandle);
	} else {
		// re-bind the buffer object
		glBindBuffer(GL_ARRAY_BUFFER,vertexBufferHandle);
	
		// setup the vertex coordinates
		glEnableVertexAttribArray(glProgram->getVertexPosLocation());
		glVertexAttribPointer(glProgram->getVertexPosLocation(),
			4,GL_FLOAT,GL_FALSE,
			sizeof (VertexType),
			reinterpret_cast<void*>(offsetof(VertexType,position)));
		// setup the texture coordinates
		glEnableVertexAttribArray(glProgram->getVertexTexture0PosLocation());
		glVertexAttribPointer(glProgram->getVertexTexture0PosLocation(),
			2,GL_FLOAT,GL_FALSE,
			sizeof (VertexType),
			reinterpret_cast<void*>(offsetof(VertexType,textureCoordinate)));
	} // if (vertexArrayHandle != 0U) {

	// Assign the texture to Texure engine 0, and set the sampler uniform accordingly
	varioBackgoundTexture.bindToUniformLocation(GL_TEXTURE0,0,glProgram->getTexture0Location());

	// The object is opaque. Use the depth buffer, and write to the depth buffer
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	if (vertexArrayHandle != 0U) {
		GLFramework::glBindVertexArrayOES(0U);
	} else {
		glDisableVertexAttribArray(glProgram->getVertexPosLocation());
		glDisableVertexAttribArray(glProgram->getVertexTexture0PosLocation());
		glBindBuffer(GL_ARRAY_BUFFER,0);
	} // if (vertexArrayHandle != 0U) {

	glUseProgram(0);

}

} // namespace OevGLES
