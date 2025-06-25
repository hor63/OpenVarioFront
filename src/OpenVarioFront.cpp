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

#include <iostream>
#include <fstream>

#include "OVFCommon.h"

#include "GLES/GLFramework.h"
#include "GLES/GLShader.h"
#include "GLES/GLProgram.h"
#include "Renderers/AnalogHandRenderer.h"
#include "Renderers/SquareTextureRenderer.h"
#include "Renderers/CircleBaseRenderer.h"
#include "Renderers/CircleFilledRenderer.h"
#include "Renderers/CirclePartialArcRenderer.h"
#include "GLTextRender/GLTextRenderer.h"


// Success is defined in X headers, but collides with an enum value in lib Eigen.
#if defined Success
#	undef Success
#endif

#include "GLES/VecMat.h"

using namespace OevGLES;


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

		glFramework->createRenderSurface(1024,1024,PACKAGE_STRING);

		OevGLES::SquareTextureRenderer varioBackground;

		int windowWidth = -1, windowHeight = -1;
		SDL_GetWindowSize(glFramework->getSDLSurface().getNativeWindow(),&windowWidth,&windowHeight);

		OevGLES::AngleDeg objectRotationAngle = 0.0_deg;
		OevGLES::Mat4 modelMatrixBack = OevGLES::translationMatrix(-0,0,-1);
		// OevGLES::Vec4 camPos = {3,4,static_cast<float>(windowWidth*2),1};
		OevGLES::Vec4 camPos = {0,0,static_cast<float>(windowHeight*2),1};
		OevGLES::Vec3 up = {0,1,0};
		OevGLES::Vec3 origin = {0,0,0};
		OevGLES::Vec4 lightDir4;
		OevGLES::Vec3 lightDir;
		OevGLES::Vec4 ambientLightColor {0.5f,0.5f,0.5f,1.0f};
		OevGLES::Vec4 lightColor {0.5f,0.5f,0.3f,1.0f};
		OevGLES::Vec4 whiteTransparent0_5Color {1.0f,1.0f,1.0f,0.5f};
		OevGLES::Vec4 whiteTransparent0_8Color {1.0f,1.0f,1.0f,0.8f};
// 		OevGLES::Vec4 blackColor {0.0f,0.0f,0.0f,0.5f};
		OevGLES::Vec4 blackColor {0.0f,0.0f,0.0f,1.0f};

		OevGLES::AnalogHandRenderer hand;
		hand.setupVertexBuffers();
		varioBackground.setupVertexBuffers();

		OevGLES::CircleBaseRenderer ring1 (glFramework->getCircleVertexContainer());
		ring1.setPrimaryRadius(200);
		ring1.setSecondaryRadius(160);
		ring1.setPrimarySecondaryZOffset(20);
		ring1.setupVertexBuffers();
		ring1.setBodyColor(whiteTransparent0_8Color);

		OevGLES::CircleFilledRenderer circ1 (glFramework->getCircleVertexContainer());
		circ1.setPrimaryRadius(160);
		circ1.setCenterZOffset(50);
		circ1.setupVertexBuffers();
		circ1.setBodyColor(whiteTransparent0_8Color);

		OevGLES::CirclePartialArcRenderer arc1 (glFramework->getCircleVertexContainer());
		arc1.setPrimaryRadius(160);
		arc1.setSecondaryRadius(100);
		arc1.setPrimarySecondaryZOffset(20);
		//arc1.setStartAngle(180.0_deg);
		arc1.setStartAngle(180.0_deg);
		arc1.setArcRange(-60.0_deg);
		arc1.setupVertexBuffers();
		arc1.setBodyColor(whiteTransparent0_8Color);

		// Assume the initial view point is exactly on the z-axis.
		// My goal is to find the aperture angle at which from this viewpoint
		// one coordinate unit in x or y direction is exactly one pixel.
		// Thus with the aperture angle I see exactly the window height.
		// To calculate the aperture angle the the ArcTan of
		// (windowHeight/2) / viewerDistance
		// is half of the aperture angle.
		static const OevGLES::AngleRad apertureAngle = OevGLES::AngleRad::makeAngle(atan((windowHeight/2.0)/camPos(2,0))) * 2.0f;

		std::cout << "Extensions are : " << glGetString(GL_EXTENSIONS) << std::endl;
		void* glGenVertexArraysOESPtr = reinterpret_cast<void*>(SDL_GL_GetProcAddress("glGenVertexArraysOES"));
		std::cout << "glGenVertexArraysOESPtr = " << glGenVertexArraysOESPtr << std::endl;

		glGenVertexArraysOESPtr = reinterpret_cast<void*>(SDL_GL_GetProcAddress("xx"));
		std::cout << "glGenVertexArraysOESPtr = " << glGenVertexArraysOESPtr << std::endl;

//		std::cout << "Pointer to glBindVertexArrayOES = " << reinterpret_cast<void*>(eglGetProcAddress("glBindVertexArrayOES")) << std::endl;
//		std::cout << "Pointer to glDeleteVertexArraysOES = " << reinterpret_cast<void*>(eglGetProcAddress("glDeleteVertexArraysOES")) << std::endl;
//		std::cout << "Pointer to glGenVertexArraysOES = " << reinterpret_cast<void*>(eglGetProcAddress("glGenVertexArraysOES")) << std::endl;
//		std::cout << "Pointer to glIsVertexArrayOES = " << reinterpret_cast<void*>(eglGetProcAddress("glIsVertexArrayOES")) << std::endl;

		OevGLES::Mat4 projMatrix = OevGLES::projectionMatrix(windowHeight,windowHeight*3,
				static_cast<double>(windowWidth)/static_cast<double>(windowHeight),apertureAngle);

		OevGLES::Mat4 modelMatrixText = OevGLES::translationMatrix(-300,220,0);
		OevGLES::GLTextRenderer glTextRend (glTextGlob);

		glTextRend.setFontSize(30);
//		glTextRend.setFonts("Noto Sans");
		glTextRend.setFonts("Noto Serif, Noto Naskh Arabic, Noto Serif Thai, Noto Serif Bengali, Noto Serif CJK SC");
		glTextRend.setText(
				  "0123456789||0ABCDEFGHIJK"
				"\n一个对此心怀恶意的流氓"
				"\nคนชั่วที่คิดชั่วกับเรื่องนี้"
				"\nএকজন দুর্বৃত্ত যে এটাকে"
				"\n খারাপ মনে করে"
				"\nمحتال يعتقد الشر في هذا"
				"\nLMNOPQRSTUVWXAYaZabcdefg"
				"\nhijklmnopqrstuvwxzy!@#"
				"\n$%^&*()_+<>[]{};'.\\\\//.\\:\"|"
				"\n,./?€üöäÜÖÄ"
				"\níéóúêîôû^'´`îêôû°ß-="

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
		glTextGlobPtr->getFontCache().exportTextureBitmaps();

		glTextRend.setupVertexBuffers();
		glTextRend.setTextColor(blackColor);
		glTextRend.setBackgroundColor(whiteTransparent0_5Color);
		glTextRend.setDrawBackground(true);

		for (OevGLES::AngleDeg rotationAngle = 0.0_deg; /*rotationAngle<360.0_deg*/;rotationAngle = rotationAngle + 0.01_deg) {
			SDL_Event sdlEvent;
			while (SDL_PollEvent(&sdlEvent)){
				if (sdlEvent.type == SDL_EVENT_QUIT) {
					exit (0);
				}
			}

			if (rotationAngle >= 360.0_deg) {
				// Let the scene rotate forever.
				rotationAngle = rotationAngle - 360.0_deg;
				// Stop the rotation after one round
				// rotationAngle = 360.0f;
			}

			OevGLES::Mat4 modelMatrix = OevGLES::rotationMatrixZ(objectRotationAngle) * OevGLES::Mat4::Identity();

			OevGLES::Mat4 modelMatrixCirc1 = OevGLES::translationMatrix(0.0f,0.0f,20.0f) * modelMatrix;

			OevGLES::Mat4 viewMatrix = OevGLES::viewMatrix((OevGLES::rotationMatrixY(rotationAngle) * camPos).block<3,1>(0,0),origin,up);
			OevGLES::Mat4 MVMatrix = viewMatrix * modelMatrix;
			OevGLES::Mat4 MVPMatrix = projMatrix * MVMatrix;

			OevGLES::Mat4 MVMatrixCirc1 = viewMatrix * modelMatrixCirc1;
			OevGLES::Mat4 MVPMatrixCirc1 = projMatrix * MVMatrixCirc1;

			OevGLES::Mat4 MVMatrixBack = viewMatrix * modelMatrixBack;
			OevGLES::Mat4 MVPMatrixBack = projMatrix * viewMatrix * modelMatrixBack;

			OevGLES::Mat4 viewMatrixText = OevGLES::viewMatrix(camPos.block<3,1>(0,0),origin,up);
//			OevGLES::Mat4 MVMatrixText = viewMatrixText * modelMatrixText;
			OevGLES::Mat4 MVMatrixText = viewMatrix * modelMatrixText;
//			OevGLES::Mat4 MVPMatrixText = projMatrix * viewMatrixText * modelMatrixText;
			OevGLES::Mat4 MVPMatrixText = projMatrix * viewMatrix * modelMatrixText;

			// Light dir is in eye space, rotate the light with the viewers point of view
			lightDir4 = viewMatrix * (OevGLES::rotationMatrixY(rotationAngle) * OevGLES::Vec4  {-6.0f,10.0f,10.0f,0.0f});
			lightDir = lightDir4.block<3,1>(0,0);
			lightDir.normalize();

			glClearColor(0.2f,0.2f,0.01f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			hand.draw(modelMatrix,viewMatrix,projMatrix,MVMatrix,MVPMatrix,lightDir,lightColor,ambientLightColor);
			varioBackground.draw(modelMatrixBack,viewMatrix,projMatrix,MVMatrixBack,MVPMatrixBack,lightDir,lightColor,ambientLightColor);

			glTextRend.draw(modelMatrixText,viewMatrixText , projMatrix, MVMatrixText, MVPMatrixText, lightDir, lightColor, ambientLightColor);

			ring1.draw(modelMatrixCirc1,viewMatrix,projMatrix,MVMatrix,MVPMatrix,lightDir,lightColor,ambientLightColor);
			//circ1.draw(modelMatrixCirc1,viewMatrix,projMatrix,MVMatrixCirc1,MVPMatrixCirc1,lightDir,lightColor,ambientLightColor);
			
			arc1.setArcRange(objectRotationAngle / 2.0f - 90.0_deg);
			arc1.draw(modelMatrixCirc1,viewMatrix,projMatrix,MVMatrixCirc1,MVPMatrixCirc1,lightDir,lightColor,ambientLightColor);

			// sleep(3);

			SDL_GL_SwapWindow(glFramework->getSDLSurface().getNativeWindow());

			objectRotationAngle = objectRotationAngle + 1.0_deg;

			if (objectRotationAngle >= 360.0_deg) {
				objectRotationAngle = 0.0_deg;
			}
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
