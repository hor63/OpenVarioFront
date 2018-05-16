/*
 *  OpenVarioFront.cpp
 *
 *  start module. Contains the main() function. Startup, initialization, and start of the main loop.
 *
 *  Created on: Apr 23, 2018
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

#include <unistd.h>
#include <iostream>
#include <fstream>

#include "OVFCommon.h"

#include "GLES/EGLRenderSurface.h"
#include "GLES/GLShader.h"
#include "GLES/GLProgram.h"

// Success is defined in X headers, but collides with an enum value in lib Eigen.
#if defined Success
#	undef Success
#endif

#include "GLES/VecMat.h"

int main(int argint,char** argv) {
	int rc = 0;

#if defined HAVE_LOG4CXX_H
	// create a basic configuration as fallback
	log4cxx::BasicConfigurator::configure();

    // The configuration file (when I can load it) will overwrite the command line settings.
    log4cxx::PropertyConfigurator::configure(log4cxx::File("OpenVarioFront.logger.properties"));

    log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("OpenVarioFront");
#endif // if defined HAVE_LOG4CXX_H


    try {
		OevGLES::EGLRenderSurface eglSurface;
		OevGLES::GLVertexShader *vertShader = new OevGLES::GLVertexShader (
				"precision mediump float;\n"
				"\n"
				"uniform vec4 mvpMatrix;\n"
				"\n"
				"uniform vec4 lightDir;\n"
				"uniform vec4 lightColor;\n"
				"uniform vec4 ambientLightColor;\n"
				"\n"
				"attribute vec4 vertexPos;\n"
				"attribute vec4 vertexNormal;\n"
				"attribute vec4 vertexColor;\n"
				"\n"
				"varying vec4 fragColor;\n"
				"\n"
				"void main () { \n"
				"	float diffuseLightFactor = dot(lightDir.xyz,(mvpMatrix * vertexNormal).xyz);\n"
				"	\n"
				"	fragColor = vertexColor * (ambientLightColor + diffuseLightFactor * lightColor);\n"
				"	gl_Position = mvpMatrix * vertexPos;\n"
				"}\n");

		OevGLES::GLFragmentShader *fragShader = new OevGLES::GLFragmentShader (
				"precision mediump float;\n"
				"varying vec4 fragColor;\n"
				"\n"
				"void main () {\n"
				"	gl_FragColor = fragColor;\n"
				"}\n");

		LOG4CXX_INFO(logger,"Create native window, eglSurface and eglContext.");
		eglSurface.createRenderSurface(320,240,PACKAGE_STRING);


		OevGLES::GLProgram prog(vertShader,fragShader);

		prog.linkProgram();

		sleep(3);
	    LOG4CXX_INFO(logger,"Destroy eglSurface and eglContext and native window.");

	    // Some testing of the MVP matrixes and effects on positions
	    OevGLES::Mat4 transMat = OevGLES::translationMatrix(0.5f,-1.2f,-0.3f);
	    OevGLES::Vec4 pos = {1.0f,1.0f,1.0f,1.0f};
	    OevGLES::Vec4 pos1 = transMat * pos;
	    LOG4CXX_DEBUG(logger,"Translation: \n" << transMat << "\n * \n" << pos << "\n = \n" << pos1);

	    OevGLES::Mat4 scaleMat = OevGLES::scalingMatrix(0.5f,-1.2f,-0.3f);
	    pos1 = scaleMat * pos;
	    LOG4CXX_DEBUG(logger,"Scaling: \n" << scaleMat << "\n * \n" << pos << "\n = \n" << pos1);

	    OevGLES::Mat4  rotMat = OevGLES::rotationMatrixX(30.0f);
	    pos = {1.0f,0.0f,0.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating X: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);

	    pos = {0.0f,1.0f,0.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating X: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);

	    pos = {0.0f,0.0f,1.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating X: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);

	    pos = {1.0f,1.0f,1.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating X: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);

	    rotMat = OevGLES::rotationMatrixY(30.0f);
	    pos = {1.0f,0.0f,0.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating Y: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);

	    pos = {0.0f,1.0f,0.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating Y: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);

	    pos = {0.0f,0.0f,1.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating Y: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);

	    pos = {1.0f,1.0f,1.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating Y: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);

	    rotMat = OevGLES::rotationMatrixZ(30.0f);
	    pos = {1.0f,0.0f,0.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating Z: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);

	    pos = {0.0f,1.0f,0.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating Z: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);

	    pos = {0.0f,0.0f,1.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating Z: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);

	    pos = {1.0f,1.0f,1.0f,1.0f};
	    pos1 = rotMat * pos;
	    LOG4CXX_DEBUG(logger,"Rotating Z: \n" << rotMat << "\n * \n" << pos << "\n = \n" << pos1);


	    OevGLES::Vec3 viewPos = {0.0f,0.2f,3.0f};
	    OevGLES::Mat4 viewMat = OevGLES::viewMatrix(viewPos,OevGLES::Vec3(0,0,0),OevGLES::Vec3(0,1,0));
	    pos = {1.0f,1.0f,1.0f,1.0f};
	    pos1 = viewMat * pos;
	    LOG4CXX_DEBUG(logger,"View transformation: \n" << viewMat << "\n * \n" << pos << "\n = \n" << pos1);

	    OevGLES::Mat4 projectionMat = OevGLES::projectionMatrix(0.5f,10.0f,320.0f/240.0f,66.0f);
	    pos = projectionMat * pos1;
	    LOG4CXX_DEBUG(logger,"projection transformation: \n" << projectionMat << "\n * \n" << pos1 << "\n = \n" << pos);

	    LOG4CXX_DEBUG(logger,"In normalized device space: \n" << pos(0)/pos(3)<< "\n" << pos(1)/pos(3)<< "\n" << pos(2)/pos(3));

	    pos = {1.0f,1.0f,-3.0f,1.0f};
	    pos1 = projectionMat * viewMat * pos;
	    LOG4CXX_DEBUG(logger,"View-projection transformation of (1,1,-3): \n" << pos1);
	    LOG4CXX_DEBUG(logger,"In normalized device space: \n" << pos1(0)/pos1(3)<< "\n" << pos1(1)/pos1(3)<< "\n" << pos1(2)/pos1(3));




	} catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
	}


	sleep(3);

	return rc;
}
