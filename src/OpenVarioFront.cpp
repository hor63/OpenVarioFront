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

#include "GLES/GLFramework.h"
#include "GLES/GLShader.h"
#include "GLES/GLProgram.h"
#include "Renderers/AnalogHandRenderer.h"
#include "Renderers/SquareTextureRenderer.h"
#include "GLTextRender/GLTextRenderer.h"


// Success is defined in X headers, but collides with an enum value in lib Eigen.
#if defined Success
#	undef Success
#endif

#include "GLES/VecMat.h"

int main(int argint,char** argv) {
	int rc = 0;

	setlocale(LC_ALL, "");

#if defined HAVE_LOG4CXX_H
	// create a basic configuration as fallback
	log4cxx::BasicConfigurator::configure();

    // The configuration file (when I can load it) will overwrite the command line settings.
    log4cxx::PropertyConfigurator::configure(log4cxx::File("OpenVarioFront.logger.properties"));

    log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("OpenVarioFront");
#endif // if defined HAVE_LOG4CXX_H


    try {
    	auto glFramework = OevGLES::GLFramework::createFramework();
		auto glTextGlob = glFramework->getGlTextGlob();
		auto glTextGlobPtr = glTextGlob.lock();

		glTextGlobPtr->setResolutionDPI(96, 96);

		glFramework->createRenderSurface(640,640,PACKAGE_STRING);


		{
			std::string sampleString = pango_language_get_sample_string (pango_language_get_default());
			OevGLES::GLTextRenderer glTextRend (glTextGlob);

			glTextRend.setFontSize(36);
			glTextRend.setText(sampleString);

			LOG4CXX_INFO (logger,"Text to render at size "<< glTextRend.getFontSize() << " = " << glTextRend.getText());

			glTextRend.renderLayout();
		}

		{
			OevGLES::GLTextRenderer glTextRend (glTextGlob);

			glTextRend.setFontSize(40);
			glTextRend.setText(
					  "0123456789||0"
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

			LOG4CXX_INFO (logger,"Text to render at size "<< glTextRend.getFontSize() << " = " << glTextRend.getText());

			glTextRend.renderLayout();
			glTextRend.setFontSize(10);
			glTextRend.renderLayout();
			glTextRend.setFontSize(24);
			glTextRend.renderLayout();
			glTextRend.setFontSize(30);
			glTextRend.renderLayout();

			glTextRend.setFontSize(12);
			glTextRend.renderLayout();

			//glTextRend.setupVertexBuffers();
		}
		{
			OevGLES::GLTextRenderer glTextRend (glTextGlob);

			glTextRend.setFontSize(40);
			glTextRend.setText(
					  "01234567891"
					);

			LOG4CXX_INFO (logger,"Text to render at size "<< glTextRend.getFontSize() << " = " << glTextRend.getText());

			glTextRend.renderLayout();
			glTextRend.setFontSize(10);
			glTextRend.renderLayout();
			glTextRend.setFontSize(24);
			glTextRend.renderLayout();
			glTextRend.setFontSize(30);
			glTextRend.renderLayout();

			glTextRend.setFontSize(12);
			glTextRend.renderLayout();
		}
		{
			OevGLES::GLTextRenderer glTextRend (glTextGlob);

			glTextRend.setFontSize(40);
			glTextRend.setText(
					  "01234567892"
					);

			LOG4CXX_INFO (logger,"Text to render at size "<< glTextRend.getFontSize() << " = " << glTextRend.getText());

			glTextRend.renderLayout();
			glTextRend.setFontSize(10);
			glTextRend.renderLayout();
			glTextRend.setFontSize(24);
			glTextRend.renderLayout();
			glTextRend.setFontSize(30);
			glTextRend.renderLayout();

			glTextRend.setFontSize(12);
			glTextRend.renderLayout();
		}
		{
			OevGLES::GLTextRenderer glTextRend (glTextGlob);

			glTextRend.setFontSize(40);
			glTextRend.setText(
					  "012345678903"
					);

			LOG4CXX_INFO (logger,"Text to render at size "<< glTextRend.getFontSize() << " = " << glTextRend.getText());

			glTextRend.renderLayout();
			glTextRend.setFontSize(10);
			glTextRend.renderLayout();
			glTextRend.setFontSize(24);
			glTextRend.renderLayout();
			glTextRend.setFontSize(30);
			glTextRend.renderLayout();

			glTextRend.setFontSize(12);
			glTextRend.renderLayout();
		}
		{
			OevGLES::GLTextRenderer glTextRend (glTextGlob);

			glTextRend.setText(
					  "01234567894"
					);
			glTextRend.setFontSize(40);

			LOG4CXX_INFO (logger,"Text to render at size "<< glTextRend.getFontSize() << " = " << glTextRend.getText());

			glTextRend.renderLayout();
			glTextRend.setFontSize(10);
			glTextRend.renderLayout();
			glTextRend.setFontSize(24);
			glTextRend.renderLayout();
			glTextRend.setFontSize(30);
			glTextRend.renderLayout();

			glTextRend.setFontSize(12);
			glTextRend.renderLayout();
		}

		glTextGlobPtr->getFontCache().exportTextureBitmaps();



		AnalogHandRenderer hand;
		SquareTextureRenderer varioBackground;

		int windowWidth = -1, windowHeight = -1;
		SDL_GetWindowSize(glFramework->getSDLSurface().getNativeWindow(),&windowWidth,&windowHeight);

		hand.setupVertexBuffers();
		varioBackground.setupVertexBuffers();

		// The arc tan in degrees is 0.5
		// With this angle, and the 0-plane=2*width, and near=width,
		// and far=3*width one unit in client space is one pixel on the screen
		// in the 0-plane (z=0)
		static constexpr double apertureAngleQuarterDeg = 26.565051177;

		GLfloat k = 0.0f;
		OevGLES::Mat4 modelMatrixBack = OevGLES::translationMatrix(-0,0,-1);
		// OevGLES::Vec4 camPos = {3,4,static_cast<float>(windowWidth*2),1};
		OevGLES::Vec4 camPos = {0,0,static_cast<float>(windowWidth*2),1};
		OevGLES::Vec3 up = {0,1,0};
		OevGLES::Vec3 origin = {0,0,0};
		OevGLES::Vec4 lightDir4;
		OevGLES::Vec3 lightDir;
		OevGLES::Vec4 ambientLightColor {0.5f,0.5f,0.5f,1.0f};
		OevGLES::Vec4 lightColor {0.5f,0.5f,0.3f,1.0f};

		std::cout << "Extensions are : " << glGetString(GL_EXTENSIONS) << std::endl;
		void* glGenVertexArraysOESPtr = reinterpret_cast<void*>(SDL_GL_GetProcAddress("glGenVertexArraysOES"));
		std::cout << "glGenVertexArraysOESPtr = " << glGenVertexArraysOESPtr << std::endl;

		glGenVertexArraysOESPtr = reinterpret_cast<void*>(SDL_GL_GetProcAddress("xx"));
		std::cout << "glGenVertexArraysOESPtr = " << glGenVertexArraysOESPtr << std::endl;

//		std::cout << "Pointer to glBindVertexArrayOES = " << reinterpret_cast<void*>(eglGetProcAddress("glBindVertexArrayOES")) << std::endl;
//		std::cout << "Pointer to glDeleteVertexArraysOES = " << reinterpret_cast<void*>(eglGetProcAddress("glDeleteVertexArraysOES")) << std::endl;
//		std::cout << "Pointer to glGenVertexArraysOES = " << reinterpret_cast<void*>(eglGetProcAddress("glGenVertexArraysOES")) << std::endl;
//		std::cout << "Pointer to glIsVertexArrayOES = " << reinterpret_cast<void*>(eglGetProcAddress("glIsVertexArrayOES")) << std::endl;

		OevGLES::Mat4 projMatrix = OevGLES::projectionMatrix(windowWidth,windowWidth*3,
				static_cast<double>(windowWidth)/static_cast<double>(windowHeight),apertureAngleQuarterDeg);

		OevGLES::Mat4 modelMatrixText = OevGLES::translationMatrix(-200,0,0);
		OevGLES::Mat4 viewMatrixText = OevGLES::viewMatrix(camPos.block<3,1>(0,0),origin,up);
		OevGLES::Mat4 MVMatrixText = viewMatrixText * modelMatrixText;
		OevGLES::Mat4 MVPMatrixText = projMatrix * viewMatrixText * modelMatrixText;
		OevGLES::GLTextRenderer glTextRend (glTextGlob);

		glTextRend.setFontSize(40);
		glTextRend.setText(
				  "0123456789||0"
				"\nABCDEFGHIJK"
/*
				"\nLMNOPQRSTUV"
				"\nWXZYabcdefg"
				"\nhijklmnopqr"
				"\nstuvwxzy!@#"
				"\n$%^&*()_+<>"
				"\n[]{};'\\:\"|"
				"\n,./?€üöäÜÖÄ"
				"\níéóúêîôû^'´`"
				"îêôû°ß-="
*/
				);

		glTextRend.renderLayout();
		glTextRend.setupVertexBuffers();

		glTextRend.draw(modelMatrixText,viewMatrixText , projMatrix, MVMatrixText, MVPMatrixText, lightDir, lightColor, ambientLightColor);


		for (GLfloat rotationAngleDeg = 0.0f; rotationAngleDeg<360.0f;rotationAngleDeg += 0.1f) {
			SDL_Event sdlEvent;
			while (SDL_PollEvent(&sdlEvent)){
				if (sdlEvent.type == SDL_EVENT_QUIT) {
					exit (0);
				}
			}

			OevGLES::Mat4 modelMatrix = OevGLES::rotationMatrixZ(k) * OevGLES::Mat4::Identity();

			OevGLES::Mat4 viewMatrix = OevGLES::viewMatrix((OevGLES::rotationMatrixY(rotationAngleDeg) * camPos).block<3,1>(0,0),origin,up);
			OevGLES::Mat4 MVMatrix = viewMatrix * modelMatrix;
			OevGLES::Mat4 MVPMatrix = projMatrix * viewMatrix * modelMatrix;
			OevGLES::Mat4 MVMatrixBack = viewMatrix * modelMatrixBack;
			OevGLES::Mat4 MVPMatrixBack = projMatrix * viewMatrix * modelMatrixBack;

			// Light dir is in eye space, rotate the light with the viewers point of view
			lightDir4 = viewMatrix * (OevGLES::rotationMatrixY(rotationAngleDeg) * OevGLES::Vec4  {-6.0f,10.0f,10.0f,0.0f});
			lightDir = lightDir4.block<3,1>(0,0);
			lightDir.normalize();

			glClearColor(0.2f,0.2f,0.01f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			hand.draw(modelMatrix,viewMatrix,projMatrix,MVMatrix,MVPMatrix,lightDir,lightColor,ambientLightColor);
			varioBackground.draw(modelMatrixBack,viewMatrix,projMatrix,MVMatrixBack,MVPMatrixBack,lightDir,lightColor,ambientLightColor);

			glTextRend.draw(modelMatrixText,viewMatrixText , projMatrix, MVMatrixText, MVPMatrixText, lightDir, lightColor, ambientLightColor);

			// sleep(3);

			SDL_GL_SwapWindow(glFramework->getSDLSurface().getNativeWindow());

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
