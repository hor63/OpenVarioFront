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

#include "GLES/SDLRenderSurface.h"
#include "GLES/GLShader.h"
#include "GLES/GLProgram.h"
#include "Renderers/AnalogHandRenderer.h"
#include "Renderers/SquareTextureRenderer.h"
#include "GLTextRender/GLTextGlobals.h"
#include "GLTextRender/GLTextRenderer.h"
#include "GLTextRender/PangoGLTextRender.h"


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
		OevGLES::Vec4 camPos = {3,4,20,1};
		OevGLES::Vec3 up = {0,1,0};
		OevGLES::Vec3 origin = {0,0,0};
		OevGLES::Vec4 lightDir4;
		OevGLES::Vec3 lightDir;
		OevGLES::Vec4 ambientLightColor {0.5f,0.5f,0.5f,1.0f};
		OevGLES::Vec4 lightColor {0.5f,0.5f,0.3f,1.0f};
		OevGLES::GLTextGlobals glTextGlob;

		glTextGlob.init();

		OevGLES::GLTextRenderer glTextRend (glTextGlob);

		glTextRend.setText(
				  "01234567890"
				"\nABCDEFGHIJK"
				"\nLMNOPQRSTUV"
				"\nWXZYabcdefg"
				"\nhijklmnopqr"
				"\nstuvwxzy!@#"
				"\n$%^&*()_+<>"
				"\n[]{};'\\:\"|"
				"\n,./?€üöäÜÖÄ"
				"\níéóúêîôû^'´`"
				"îêôû°ß-="
				);

		pango_gl_text_render_layout(glTextRend.getPangoLayout(),0,0);


		glTextRend.setText("BlaBlaBla. الأرواح عالية نادرا ما تفعل بشكل جيد."
				"\n兴致很高兴. 興致很高興. วิญญาณสูงไม่ค่อยทำได้ดี"
				"\nDer Plüschelch rührt keinen Huf."
				"\nDie efter effter efffter effer efffer effffer efier effier efffier Ligatur"
				);

		pango_gl_text_render_layout(glTextRend.getPangoLayout(),0,0);

		SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
// Not in SDL3	    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
	    SDL_SetHint(SDL_HINT_VIDEO_FORCE_EGL,"1");

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);


		OevGLES::SDLRenderSurface SDLSurface;
		LOG4CXX_INFO(logger,"Create native window, eglSurface and eglContext.");
		SDLSurface.createRenderSurface(640,480,PACKAGE_STRING);
		LOG4CXX_INFO(logger,"Create the diffuse light program");

		AnalogHandRenderer hand;
		SquareTextureRenderer varioBackground;


		hand.setupVertexBuffers();
		varioBackground.setupVertexBuffers();

		GLfloat k = 0.0f;
		OevGLES::Mat4 modelMatrixBack = OevGLES::Mat4::Identity();

		std::cout << "Extensions are : " << glGetString(GL_EXTENSIONS) << std::endl;
//		std::cout << "Pointer to glBindVertexArrayOES = " << reinterpret_cast<void*>(eglGetProcAddress("glBindVertexArrayOES")) << std::endl;
//		std::cout << "Pointer to glDeleteVertexArraysOES = " << reinterpret_cast<void*>(eglGetProcAddress("glDeleteVertexArraysOES")) << std::endl;
//		std::cout << "Pointer to glGenVertexArraysOES = " << reinterpret_cast<void*>(eglGetProcAddress("glGenVertexArraysOES")) << std::endl;
//		std::cout << "Pointer to glIsVertexArrayOES = " << reinterpret_cast<void*>(eglGetProcAddress("glIsVertexArrayOES")) << std::endl;

		for (GLfloat i = 0.0f; i<360.0f;i += 0.1f) {
			SDL_Event sdlEvent;
			while (SDL_PollEvent(&sdlEvent)){
				if (sdlEvent.type == SDL_EVENT_QUIT) {
					exit (0);
				}
			}

			OevGLES::Mat4 modelMatrix = OevGLES::rotationMatrixZ(k) * OevGLES::Mat4::Identity();
			OevGLES::Mat4 viewMatrix = OevGLES::viewMatrix((OevGLES::rotationMatrixY(i) * camPos).block<3,1>(0,0),origin,up);
			OevGLES::Mat4 projMatrix = OevGLES::projectionMatrix(5,35,320.0/240.0,66);
			OevGLES::Mat4 MVMatrix = viewMatrix * modelMatrix;
			OevGLES::Mat4 MVPMatrix = projMatrix * viewMatrix * modelMatrix;
			OevGLES::Mat4 MVMatrixBack = viewMatrix * modelMatrixBack;
			OevGLES::Mat4 MVPMatrixBack = projMatrix * viewMatrix * modelMatrixBack;

			// Light dir is in eye space, rotate the light with the viewers point of view
			lightDir4 = viewMatrix * (OevGLES::rotationMatrixY(i) * OevGLES::Vec4  {-6.0f,10.0f,10.0f,0.0f});
			lightDir = lightDir4.block<3,1>(0,0);
			lightDir.normalize();

			glClearColor(0.2f,0.2f,0.01f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			hand.draw(modelMatrix,viewMatrix,projMatrix,MVMatrix,MVPMatrix,lightDir,lightColor,ambientLightColor);
			varioBackground.draw(modelMatrixBack,viewMatrix,projMatrix,MVMatrixBack,MVPMatrixBack,lightDir,lightColor,ambientLightColor);

			// sleep(3);

			SDL_GL_SwapWindow(SDLSurface.getNativeWindow());

			k += 1.0f;
		}

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
