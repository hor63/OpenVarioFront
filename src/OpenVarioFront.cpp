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

	} catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
	}


	sleep(3);

	return rc;
}
