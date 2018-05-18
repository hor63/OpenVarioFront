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
	: RendererBase{OevGLES::GLProgDiffuseLight::getProgram()},
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

			LOG4CXX_DEBUG(logger,"Triangle # " << i << ":");
			for (int k = 0;k < 6 ; k+= 2) {
				Eigen::Map<OevGLES::Vec4> vecX ( p0 + (k*4));
				Eigen::Map<OevGLES::Vec4> vecXNormal ( p0 + (k*4) + 4);

				LOG4CXX_DEBUG(logger,"Vec4 [" << k << "] = [" << vecX.transpose() << "], Normal = [" << vecXNormal.transpose() << ']');

			}

		}

	}


AnalogHandRenderer::~AnalogHandRenderer() { }

void AnalogHandRenderer::setupVertexBuffers() {


}

void AnalogHandRenderer::draw(const OevGLES::Mat4& modelMatrix,
		const OevGLES::Mat4& viewMatrix, const OevGLES::Mat4& ProjMatrix,
		const OevGLES::Mat4& MVMatrix, const OevGLES::Mat4& MVPMatrix) {
}

