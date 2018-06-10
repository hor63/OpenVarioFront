/*
 * AnalogHandRenderer.cpp
 *
 *  Created on: May 17, 2018
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

#include "Renderers/AnalogHandRenderer.h"

#if defined HAVE_LOG4CXX_H
#include "OVFCommon.h"
	static log4cxx::LoggerPtr logger;
#endif

AnalogHandRenderer::AnalogHandRenderer()
	:
		// Setup the positions
	    // The normals are computed in the constructor body
	  vertexArray {
		// 0 triangle
		 0.0f, 0.0f,0.5f,1.0f, // Pos 0
		 0.0f, 0.0f,0.0f,0.0f, // Normal 0
		10.0f, 0.0f,0.0f,1.0f, // Pos 1
		 0.0f, 0.0f,0.0f,0.0f, // Normal 1
		 0.0f, 1.0f,0.0f,1.0f, // Pos 2
		 0.0f, 0.0f,0.0f,0.0f, // Normal 2
		// 1st triangle
		 0.0f, 0.0f,0.5f,1.0f, // Pos 0
		 0.0f, 0.0f,0.0f,0.0f, // Normal 0
		 0.0f,-1.0f,0.0f,1.0f, // Pos 1
		 0.0f, 0.0f,0.0f,0.0f, // Normal 1
		10.0f, 0.0f,0.0f,1.0f, // Pos 2
		 0.0f, 0.0f,0.0f,0.0f, // Normal 2
		// 2nd triangle
		 0.0f, 0.0f,0.5f,1.0f, // Pos 0
		 0.0f, 0.0f,0.0f,0.0f, // Normal 0
		 0.0f, 1.0f,0.0f,1.0f, // Pos 1
		 0.0f, 0.0f,0.0f,0.0f, // Normal 1
		-1.0f, 0.0f,0.0f,1.0f, // Pos 2
		 0.0f, 0.0f,0.0f,0.0f, // Normal 2
		// 3rd triangle
		 0.0f, 0.0f,0.5f,1.0f, // Pos 0
		 0.0f, 0.0f,0.0f,0.0f, // Normal 0
		-1.0f, 0.0f,0.0f,1.0f, // Pos 1
		 0.0f, 0.0f,0.0f,0.0f, // Normal 1
		 0.0f,-1.0f,0.0f,1.0f, // Pos 2
		 0.0f, 0.0f,0.0f,0.0f // Normal 2
	}
	{

#if defined HAVE_LOG4CXX_H
		// Get the logger if necessary
		if (!logger) {
			logger = log4cxx::Logger::getLogger("OpenVarioFront.AnalogHandRenderer");
		}
#endif

		LOG4CXX_DEBUG(logger,"AnalogHandRenderer::AnalogHandRenderer()");

		// Run through the 4 triangles, and calculate the normal of the areas
		for (int i=0; i < 4 ; i++) {

			LOG4CXX_DEBUG(logger,"Triangle #" << i);

			GLfloat* p0 = vertexArray + (i*24);
			Eigen::Map<OevGLES::Vec3> pos0 (p0);
			Eigen::Map<OevGLES::Vec3> pos1 (p0 + 8);
			Eigen::Map<OevGLES::Vec3> pos2 (p0 + 16);

			OevGLES::Vec3 normal = (pos1 - pos0).normalized().cross((pos2-pos0).normalized()).normalized();
			// Copy the normal into all normal vectors of the triangle
			for (int k=1;k<6;k+=2) {
				Eigen::Map<OevGLES::Vec3> posK (p0 + (k*4));
				posK = normal;
			}

#if defined HAVE_LOG4CXX_H
			if (logger->getLevel() == log4cxx::Level::getDebug()) {
				LOG4CXX_DEBUG(logger,"Triangle # " << i << ":");
				for (int k = 0;k < 6 ; k+= 2) {
					Eigen::Map<OevGLES::Vec4> vecX ( p0 + (k*4));
					Eigen::Map<OevGLES::Vec4> vecXNormal ( p0 + (k*4) + 4);

					LOG4CXX_DEBUG(logger,"Vec4 [" << k << "] = [" << vecX.transpose() << "], Normal = [" << vecXNormal.transpose() << ']');

				}
			}
#endif
		}

	}


AnalogHandRenderer::~AnalogHandRenderer() { }

void AnalogHandRenderer::setupVertexBuffers() {

	// First get the program
	glProgram = OevGLES::GLProgDiffuseLight::getProgram();

	// make the program current
	glProgram->useProgram();

	glGenBuffers(1,&vertexBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER,vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertexArray),vertexArray,GL_STATIC_DRAW);


}

void AnalogHandRenderer::draw(
		const OevGLES::Mat4& modelMatrix,
		const OevGLES::Mat4& viewMatrix, const OevGLES::Mat4& ProjMatrix,
		const OevGLES::Mat4& MVMatrix, const OevGLES::Mat4& MVPMatrix,
		const OevGLES::Vec3& lightDir, const OevGLES::Vec4& lightColor,
		const OevGLES::Vec4& ambientLightColor ) {

	GLfloat* bufferOffset = 0;

	// make my program current
	glProgram->useProgram();


	LOG4CXX_DEBUG(logger,"lightDir = " << lightDir.transpose());
	GLfloat* p0 = vertexArray;
	for (int k = 0;k < 6 ; k+= 2) {
		Eigen::Map<OevGLES::Vec4> vecXNormal4 ( p0 + (k*4) + 4);
		OevGLES::Vec3 vecXNormal = (MVMatrix * vecXNormal4).block<3,1>(0,0);

		LOG4CXX_DEBUG(logger,"Vec4 [" << k << "] Normal = [" << vecXNormal4.transpose() << ']');
		LOG4CXX_DEBUG(logger,"Vec4 [" << k << "] MVMatrix * Normal = [" << vecXNormal.transpose() << ']');
		LOG4CXX_DEBUG(logger,"lightDir dot normal = " << lightDir.dot(vecXNormal));

	}

	// Set the uniforms
	glUniformMatrix4fv(glProgram->getMvpMatrixLocation(),1,GL_FALSE,&(MVPMatrix(0,0)));
	glUniformMatrix4fv(glProgram->getMvMatrixLocation(),1,GL_FALSE,&(MVMatrix(0,0)));
	glUniformMatrix4fv(glProgram->getMvMatrixLocation(),1,GL_FALSE,&(MVMatrix(0,0)));

	glUniform3fv(glProgram->getLightDirLocation(),1,&(lightDir(0)));
	glUniform4fv(glProgram->getLightColorLocation(),1,&(lightColor(0)));
	glUniform4fv(glProgram->getAmbientLightColorLocation(),1,&(ambientLightColor(0)));


	// set the color attribute constant
	glDisableVertexAttribArray(glProgram->getVertexColorLocation());
	glVertexAttrib4fv(glProgram->getVertexColorLocation(),handColor);

	// re-bind the buffer object
	glBindBuffer(GL_ARRAY_BUFFER,vertexBufferHandle);

	// setup the vertex coordinates
	glEnableVertexAttribArray(glProgram->getVertexPosLocation());
	glVertexAttribPointer(glProgram->getVertexPosLocation(),4,GL_FLOAT,GL_FALSE,8 * sizeof (GLfloat),bufferOffset);
	// setup the vertex coordinates
	bufferOffset += 4; // Advance the offset by 4 floats to the vertex normals.
	glEnableVertexAttribArray(glProgram->getVertexNormalLocation());
	glVertexAttribPointer(glProgram->getVertexNormalLocation(),4,GL_FLOAT,GL_FALSE,8 * sizeof (GLfloat),bufferOffset);

	// The object is opaque. Use the depth buffer, and write to the depth buffer
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLES,0,12);


}

