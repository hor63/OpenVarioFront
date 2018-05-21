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
#include "GLPrograms/GLProgDiffuseLight.h"
#include "Renderers/AnalogHandRenderer.h"

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
		OevGLES::Mat4 modelMatrix = OevGLES::Mat4::Identity();
		OevGLES::Vec3 camPos = {3,4,20};
		OevGLES::Vec3 up = {0,1,0};
		OevGLES::Vec3 origin = {0,0,0};
		OevGLES::Mat4 viewMatrix = OevGLES::viewMatrix(camPos,origin,up);
		OevGLES::Mat4 projMatrix = OevGLES::projectionMatrix(18,22,320.0/240.0,66);
		OevGLES::Mat4 MVMatrix = viewMatrix * modelMatrix;
		OevGLES::Mat4 MVPMatrix = projMatrix * MVMatrix;
		OevGLES::Vec4 lightDir4;
		OevGLES::Vec3 lightDir;
		OevGLES::Vec4 ambientLightColor {0.5f,0.5f,0.5f,1.0f};
		OevGLES::Vec4 lightColor {0.5f,0.2f,0.2f,1.0f};

		// Light dir is in eye space
		lightDir4 = viewMatrix * OevGLES::Vec4  {-1.0f,10.0f,-10.0f,0.0f};
		lightDir(0) = lightDir4(0);
		lightDir(1) = lightDir4(1);
		lightDir(2) = lightDir4(2);


		OevGLES::EGLRenderSurface eglSurface;
		LOG4CXX_INFO(logger,"Create native window, eglSurface and eglContext.");
		eglSurface.createRenderSurface(640,480,PACKAGE_STRING);
		LOG4CXX_INFO(logger,"Create the diffuse light program");

		AnalogHandRenderer hand;

		hand.setupVertexBuffers();

		glClearColor(0.4f,0.2f,0.01f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		hand.draw(modelMatrix,viewMatrix,projMatrix,MVMatrix,MVPMatrix,lightDir,lightColor,ambientLightColor);

		sleep(3);

		eglSwapBuffers(eglSurface.getDisplay(),eglSurface.getRenderSurface());

		sleep(10);

		LOG4CXX_INFO(logger,"Destroy the diffuse light program");
		OevGLES::GLProgDiffuseLight::destroyProgram();

	    LOG4CXX_INFO(logger,"Destroy eglSurface and eglContext and native window.");

	} catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
	}


	sleep(3);

	return rc;
}
