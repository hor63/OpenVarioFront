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

#include "ControlBase.h"
#include "GLES/SDL/SDLRenderSurface.h"
#include "SDL3/SDL_stdinc.h"
#include <GLES2/gl2.h>
#include <string>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <memory>
#include <iostream>
#include <fstream>
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_video.h"

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
#include "RootControl.h"
#include "PlainFieldControl.h"
#include "TextFieldControl.h"

// Success is defined in X headers, but collides with an enum value in lib Eigen.
#if defined Success
#	undef Success
#endif

#include "VecMat.h"

using namespace OevGLES;

static void printEventType (SDL_Event& event);
static bool handleSLEDvent (SDL_Event& event,OevGLES::GLFramework &framework);

#include "resources/Vario5m.h"
#include "resources/Vario5mJPG.h"

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

	LOG4CXX_INFO (logger,"Vario5m_data = " << reinterpret_cast<void const * const>(Vario5m_data)
		<< ",Vario5m_end = " << reinterpret_cast<void const * const>(Vario5m_data_end)
		<< ", Vario5m_size = " << Vario5m_size
		<< ", &Vario5m_size = " << &Vario5m_size
		<< ", Vario5m_filename = " << Vario5m_filename
		<< ", Vario5m_filename_length = " << Vario5m_filename_length
		);

	LOG4CXX_INFO (logger,"Vario5mJPG_data = " << reinterpret_cast<void const * const>(Vario5mJPG_data)
		<< ",Vario5mJPG_end = " << reinterpret_cast<void const * const>(Vario5mJPG_data_end)
		<< ", Vario5mJPG_size = " << Vario5mJPG_size
		<< ", &Vario5mJPG_size = " << &Vario5mJPG_size
		<< ", Vario5mJPG_filename = " << Vario5mJPG_filename
		<< ", Vario5mJPG_filename_length = " << Vario5mJPG_filename_length
		);

    try {
    	auto glFramework = OevGLES::GLFramework::createFramework();


		auto renderSurfacePtr1 =
			glFramework->createRenderSurface(1000, 1000, PACKAGE_STRING).lock();
			
//		auto renderSurfacePtr2 =
//			glFramework->createRenderSurface(1024, 1024, "SecondaryWindow").lock();

		renderSurfacePtr1->getRenderContextPtr()->glTextGlobSharedPtr->setResolutionDPI(96, 96);

		renderSurfacePtr1->makeContextCurrent();

		OevGLES::SquareTextureRenderer varioBackground(renderSurfacePtr1->getRenderContextPtr());
//		varioBackground.setPNGFileName("../../resources/Vario5m.jpg");
		varioBackground.setImageMemoryData(Vario5mJPG_data,
			Vario5mJPG_size, Vario5mJPG_filename);

		auto rootCtrlPtr = renderSurfacePtr1->getRootControlPtr();

		int windowWidth = -1, windowHeight = -1;
		SDL_GetWindowSize(renderSurfacePtr1->getNativeWindow(),&windowWidth,&windowHeight);

		OevGLES::AngleDeg objectRotationAngle = 0.0_deg;
		// OevGLES::Vec4 camPos = {3,4,static_cast<float>(windowWidth*2),1};
		OevGLES::Vec4 camPos = {0,0,static_cast<float>(windowHeight*2),1};
		OevGLES::Vec3 up = {0,1,0};
		OevGLES::Vec3 origin = {0,0,0};
		OevGLES::Vec4 lightDir4;
		OevGLES::Vec4 whiteColor {1.0f,1.0f,1.0f,1.0f};
		OevGLES::Vec4 whiteTransparent0_5Color {1.0f,1.0f,1.0f,0.5f};
		OevGLES::Vec4 whiteTransparent0_8Color {1.0f,1.0f,1.0f,0.8f};
// 		OevGLES::Vec4 blackColor {0.0f,0.0f,0.0f,0.5f};
		OevGLES::Vec4 blackColor {0.0f,0.0f,0.0f,1.0f};
		OevGLES::Vec4 orangeColor {1.0f,0.5f,0.0f,1.0f};

		OevGLES::AnalogHandRenderer hand (renderSurfacePtr1->getRenderContextPtr());
		hand.setupVertexBuffers();
		varioBackground.setupVertexBuffers();

		OevGLES::CircleBaseRenderer ring1 (renderSurfacePtr1->getRenderContextPtr());
		ring1.setPrimaryRadius(200);
		ring1.setSecondaryRadius(160);
		ring1.setPrimarySecondaryZOffset(20);
		ring1.setupVertexBuffers();
		ring1.setBodyColor(whiteTransparent0_8Color);

		OevGLES::CircleFilledRenderer circ1 (renderSurfacePtr1->getRenderContextPtr());
		circ1.setPrimaryRadius(100);
		circ1.setCenterZOffset(50);
		circ1.setupVertexBuffers();
		circ1.setBodyColor(whiteTransparent0_8Color);

		OevGLES::CirclePartialArcRenderer arc1 (renderSurfacePtr1->getRenderContextPtr());
		arc1.setPrimaryRadius(160);
		arc1.setSecondaryRadius(100);
		arc1.setPrimarySecondaryZOffset(20);
		//arc1.setStartAngle(180.0_deg);
		arc1.setStartAngle(180.0_deg);
		arc1.setArcRange(-60.0_deg);
		arc1.setupVertexBuffers();
		arc1.setBodyColor(whiteTransparent0_8Color);

/*
		// Assume the initial view point is exactly on the z-axis.
		// My goal is to find the aperture angle at which from this viewpoint
		// one coordinate unit in x or y direction is exactly one pixel.
		// Thus with the aperture angle I see exactly the window height.
		// To calculate the aperture angle the the ArcTan of
		// (windowHeight/2) / viewerDistance
		// is half of the aperture angle.
		static const OevGLES::AngleRad apertureAngle = OevGLES::AngleRad::makeAngle(atan((windowHeight/2.0)/camPos(2,0))) * 2.0f;
*/

		std::cout << "Extensions are : " << glGetString(GL_EXTENSIONS) << std::endl;
		void* glGenVertexArraysOESPtr = reinterpret_cast<void*>(SDL_GL_GetProcAddress("glGenVertexArraysOES"));
		std::cout << "glGenVertexArraysOESPtr = " << glGenVertexArraysOESPtr << std::endl;

		glGenVertexArraysOESPtr = reinterpret_cast<void*>(SDL_GL_GetProcAddress("xx"));
		std::cout << "glGenVertexArraysOESPtr = " << glGenVertexArraysOESPtr << std::endl;

//		std::cout << "Pointer to glBindVertexArrayOES = " << reinterpret_cast<void*>(eglGetProcAddress("glBindVertexArrayOES")) << std::endl;
//		std::cout << "Pointer to glDeleteVertexArraysOES = " << reinterpret_cast<void*>(eglGetProcAddress("glDeleteVertexArraysOES")) << std::endl;
//		std::cout << "Pointer to glGenVertexArraysOES = " << reinterpret_cast<void*>(eglGetProcAddress("glGenVertexArraysOES")) << std::endl;
//		std::cout << "Pointer to glIsVertexArrayOES = " << reinterpret_cast<void*>(eglGetProcAddress("glIsVertexArrayOES")) << std::endl;

		OevGLES::GLTextRenderer glTextRend (renderSurfacePtr1->getRenderContextPtr());

		glTextRend.setFontSize(20);
//		glTextRend.setFonts("Noto Sans");
		glTextRend.setFonts("Noto Sans, Noto Naskh Arabic, Noto Serif Thai, Noto Sans Bengali, Noto Sans CJK SC");
//		glTextRend.setFonts("Noto Serif, Noto Naskh Arabic, Noto Serif Thai, Noto Serif Bengali, Noto Serif CJK SC");

		std::string const str {"0123456789||0ABCDEFGHIJK"
"\n一个对此心怀恶意的流氓"
"\nคนชั่วที่คิดชั่วกับเรื่องนี้"
"\nএকজন দুর্বৃত্ত যে এটাকে"
"\n খারাপ মনে করে"
"\nمحتال يعتقد الشر في هذا"
"\n<span foreground=\"red\" underline_color=\"yellow\">LMN^OPQRSTUVW</span><span background=\"purple\" foreground=\"yellow\">XA</span>BB<span foreground=\"green\">Ya</span>Z e<sup>x</sup>abcdefg"
"\nhijk<i>lmn<span foreground=\"blue\">opq</span></i>rstuvwxzy!@#"
"\n$%*()_+&lt;&gt;[]{}&amp;'.\\\\//.\\:\"|"
"\n,./?€üöäÜÖÄ"
"\níéóúêîôû'´`îêôû°ß-="};

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

        PangoAttrList* attrList = nullptr;
		char* parsedString = nullptr;
		gunichar accel_char = 0;
		GError* parseError = nullptr;

		gboolean parseRc =
			pango_parse_markup(str.c_str(), -1, '^',
							   &attrList, &parsedString,
							   &accel_char, &parseError);

		if (!parseRc) {
			std::cerr << "pango_parse_markup returned false. "
			<< " Error is " << parseError->code << ": " << parseError->message
			<< "\nFini" << std::endl;
			return 1; 
		} else {
			std::cout << "Accel char = " << accel_char << " = " << static_cast<char>(accel_char)
			<< std::endl;
		}
		
		std::string strParsedString {parsedString};
		free(parsedString);
		glTextRend.setText(strParsedString);
		pango_layout_set_attributes( glTextRend.getPangoLayout(),attrList);

		// glTextRend.setTextColor(std::make_shared<Vec4>(blackColor));
		glTextRend.setTextColor(std::make_shared<Vec4>(whiteColor));
		glTextRend.setBackgroundColor(std::make_shared<Vec4>(whiteTransparent0_5Color));
		glTextRend.setDrawBackground(true);

		glTextRend.renderLayout();
		// glTextGlobPtr->getFontCache().exportTextureBitmaps();

		glTextRend.setupVertexBuffers();

		OevGLES::RenderStandardUniforms handUniforms;
		handUniforms.getProjMatrix() =
		/*
			OevGLES::orthographicProjectionMatrix(windowHeight, windowHeight * 3,
						  static_cast<double>(windowWidth),
							  static_cast<double>(windowHeight));
							  */
			OevGLES::perspectiveProjectionMatrix(windowHeight, windowHeight * 3,
									  static_cast<double>(windowWidth),
										  static_cast<double>(windowHeight));

		std::cout << "perspecive matrix = \n" << handUniforms.getProjMatrixC() << std::endl;

		std::cout << "orthographic matrix = \n" << 
			OevGLES::orthographicProjectionMatrix(windowHeight, windowHeight * 3,
						  static_cast<double>(windowWidth),
							  static_cast<double>(windowHeight))
			<< std::endl;
		handUniforms.getAmbientLightColor() =
			OevGLES::Vec4{0.5f, 0.5f, 0.5f, 1.0f};

		handUniforms.getLightColor() =
			OevGLES::Vec4{0.5f, 0.5f, 0.3f, 1.0f};

		handUniforms.getLightDir() = OevGLES::Vec3{0.0f,0.0f,1.0f};

		OevGLES::RenderStandardUniforms circ1Uniforms (handUniforms);
		circ1Uniforms.resetModelMatrixPtr();
		OevGLES::RenderStandardUniforms arc1Uniforms (circ1Uniforms);
		arc1Uniforms.resetModelMatrixPtr();

		OevGLES::RenderStandardUniforms backgroundImgUniforms (handUniforms);
		backgroundImgUniforms.resetModelMatrixPtr();
		backgroundImgUniforms.getModelMatrix() = 
			OevGLES::translationMatrix(-0,0,-1);

		OevGLES::RenderStandardUniforms textUniforms (handUniforms);
		textUniforms.resetModelMatrixPtr();
		textUniforms.getModelMatrix() = OevGLES::translationMatrix(-450,20,0);
		textUniforms.resetViewMatrixPtr();
		textUniforms.getViewMatrix() =
			OevGLES::viewMatrix(camPos.block<3, 1>(0, 0), origin, up);

		{
			auto rootCtrlPtr = renderSurfacePtr1->getRootControlPtr().lock();

			auto plainFieldControl = new OevControls::PlainFieldControl(
								rootCtrlPtr,
								renderSurfacePtr1->getRenderContextPtr(),
								OevUtil::Uuid("93a22a0c-e282-11f0-b16a-9347e013a17a"),
								"PLainField1");
			OevControls::ControlBasePtr plainFieldCtrlPtr(plainFieldControl);
			rootCtrlPtr->addControl(plainFieldCtrlPtr);
			
			plainFieldCtrlPtr->setPosition({400,500});
			plainFieldCtrlPtr->setSize ({300,40});
			plainFieldCtrlPtr->setHasFrame(true);
			plainFieldControl->setFrameColor(std::make_shared<OevGLES::Vec4>(0,0,0.5f,1));

			{
				auto textFieldControl = new OevControls::TextFieldControl			(
												rootCtrlPtr,
												renderSurfacePtr1->getRenderContextPtr(),
												OevUtil::Uuid("04f0f49a-ec8d-11f0-a4bc-5bdf9f28664a"),
												"PLainField1");
				OevControls::ControlBasePtr textFieldCtrlPtr(textFieldControl);
	
				rootCtrlPtr->addControl(textFieldCtrlPtr);
				textFieldControl->setPosition({402,402});
				textFieldControl->setSize ({250,40});
				textFieldControl->setTextColor(std::make_shared<OevGLES::Vec4>(0,0,0.5,1));
				textFieldControl->setFontSize(20);
				textFieldControl->setText("This 流氓 is gq|J|Jgq a test\nThis 流氓 is gq|IIJgq|J the second line.");
				textFieldControl->setWidthIsFixed();
				textFieldControl->setHasFrame(true);
			}
			{
				auto textFieldControl = new OevControls::TextFieldControl			(
												rootCtrlPtr,
												renderSurfacePtr1->getRenderContextPtr(),
												OevUtil::Uuid("2cd9507e-eff8-11f0-91d0-db75aaf51cb6"),
												"PLainField2");
				OevControls::ControlBasePtr textFieldCtrlPtr(textFieldControl);

				rootCtrlPtr->addControl(textFieldCtrlPtr);
				textFieldControl->setPosition({502,602});
				textFieldControl->setSize ({200,90});
				textFieldControl->setTextColor(std::make_shared<OevGLES::Vec4>(0,0,0.5,1));
				textFieldControl->setFontSize(15);
				textFieldControl->setText("This 流氓ชั่วq is gq|J|Jgq a test\nThis 流氓ชั่วที่ is gq|Jgq|J the second line.");
				textFieldControl->setWidthIsFixed();
				textFieldControl->setHeightIsFixed();
				// textFieldControl->setHasFrame(true);
			}
			rootCtrlPtr->setupVertexBuffers();
		}

		for (OevGLES::AngleDeg rotationAngle = 0.0_deg; /*rotationAngle<360.0_deg*/;rotationAngle = rotationAngle + 0.01_deg) {
			SDL_Event sdlEvent;
			while (SDL_PollEvent(&sdlEvent)){
				
				if (!handleSLEDvent(sdlEvent,*glFramework)) {
					return 0;
				}
			}

			renderSurfacePtr1->makeContextCurrent();

			if (rotationAngle >= 360.0_deg) {
				// Let the scene rotate forever.
				rotationAngle = rotationAngle - 360.0_deg;
				// Stop the rotation after one round
				// rotationAngle = 360.0f;
			}

			handUniforms.getModelMatrix() =
				OevGLES::rotationMatrixZ(objectRotationAngle) *
				OevGLES::Mat4::Identity();
			handUniforms.getViewMatrix() = OevGLES::viewMatrix(
				(OevGLES::rotationMatrixY(rotationAngle) * camPos)
					.block<3, 1>(0, 0),
				origin, up);
			//handUniforms.recalcMVMatrix();

			circ1Uniforms.getModelMatrix() = OevGLES::translationMatrix(0.0f,0.0f,20.0f) * handUniforms.getModelMatrix();
			arc1Uniforms.getModelMatrix() = OevGLES::translationMatrix(0.0f,0.0f,20.0f) * circ1Uniforms.getModelMatrix();
			// Light dir is in eye space, rotate the light with the viewers
			// point of view
			lightDir4 = handUniforms.getViewMatrix() *
						(OevGLES::rotationMatrixY(rotationAngle) *
						 OevGLES::Vec4{-6.0f, 10.0f, 10.0f, 0.0f});
			handUniforms.getLightDir() = Vec3(lightDir4.block<3, 1>(0, 0));
			handUniforms.getLightDir().normalize();

			glClearColor(0.2f,0.2f,0.01f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			hand.draw(handUniforms);
			varioBackground.draw(backgroundImgUniforms);

			glTextRend.draw(textUniforms);

			arc1.setArcRange(objectRotationAngle / 2.0f - 90.0_deg);
			arc1.draw(arc1Uniforms);

			ring1.draw(circ1Uniforms);
			circ1.draw(circ1Uniforms);

			{
				auto rootCtrlPtr = renderSurfacePtr1->getRootControlPtr().lock();
				rootCtrlPtr->draw();
			}

			// sleep(3);

			SDL_GL_SwapWindow(renderSurfacePtr1->getNativeWindow());

/*			
			renderSurfacePtr2->makeContextCurrent();
			glClearColor(0.2f,0.2f,0.01f,1.0f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			hand.draw(handUniforms);
			glTextRend.draw(textUniforms);
			arc1.draw(arc1Uniforms);
			SDL_GL_SwapWindow(renderSurfacePtr2->getNativeWindow());
			renderSurfacePtr1->makeContextCurrent();
*/
			objectRotationAngle = objectRotationAngle + 1.0_deg;

			if (objectRotationAngle >= 360.0_deg) {
				objectRotationAngle = 0.0_deg;
			}
		}

		// sleep(10);

		LOG4CXX_INFO(logger,"Destroy the diffuse light program");
		OevGLES::GLProgDiffuseLight::destroyProgram();

	    LOG4CXX_INFO(logger,"Destroy eglSurface and eglContext and native window.");

	} catch (std::exception const& e) {
		std::cerr << "Exception of type " << typeid(e).name() 
			<< " caught. Reason is: " << e.what() << std::endl;
	}


	sleep(3);

	return rc;
}

static void printEventType (SDL_Event& event) {
	struct {
		GLint x; GLint y;
		GLint width; GLint height;
	} viewPortDimensions;
	
	
	switch (event.type) {
			
			case SDL_EVENT_QUIT:
			std::cout << "SDL event is SDL_EVENT_QUIT" << std::endl;
			break;
		    
			case SDL_EVENT_TERMINATING:
			std::cout << "SDL event is SDL_EVENT_TERMINATING" << std::endl;
			break;
		    
			case SDL_EVENT_LOW_MEMORY:
			std::cout << "SDL event is SDL_EVENT_LOW_MEMORY" << std::endl;
			break;
		    
			case SDL_EVENT_WILL_ENTER_BACKGROUND:
			std::cout << "SDL event is SDL_EVENT_WILL_ENTER_BACKGROUND" << std::endl;
			break;
		    
			case SDL_EVENT_DID_ENTER_BACKGROUND:
			std::cout << "SDL event is SDL_EVENT_DID_ENTER_BACKGROUND" << std::endl;
			break;
		    
			case SDL_EVENT_WILL_ENTER_FOREGROUND:
			std::cout << "SDL event is SDL_EVENT_WILL_ENTER_FOREGROUND" << std::endl;
			break;
		    
			case SDL_EVENT_DID_ENTER_FOREGROUND:
			std::cout << "SDL event is SDL_EVENT_DID_ENTER_FOREGROUND" << std::endl;
			break;
		    
			case SDL_EVENT_LOCALE_CHANGED:
			std::cout << "SDL event is SDL_EVENT_LOCALE_CHANGED" << std::endl;
			break;
		    
			case SDL_EVENT_SYSTEM_THEME_CHANGED:
			std::cout << "SDL event is SDL_EVENT_SYSTEM_THEME_CHANGED" << std::endl;
			break;
		    
		    /* Display events */
		    /* 0x150 was SDL_DISPLAYEVENT, reserve the number for sdl2-compat */
		    
			case SDL_EVENT_DISPLAY_ORIENTATION:
			std::cout << "SDL event is SDL_EVENT_DISPLAY_ORIENTATION" << std::endl;
			break;
		    
			case SDL_EVENT_DISPLAY_ADDED:
			std::cout << "SDL event is SDL_EVENT_DISPLAY_ADDED" << std::endl;
			break;
		    
			case SDL_EVENT_DISPLAY_REMOVED:
			std::cout << "SDL event is SDL_EVENT_DISPLAY_REMOVED" << std::endl;
			break;
		    
			case SDL_EVENT_DISPLAY_MOVED:
			std::cout << "SDL event is SDL_EVENT_DISPLAY_MOVED" << std::endl;
			break;
		    
			case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED:
			std::cout << "SDL event is SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED" << std::endl;
			break;
		    
			case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED:
			std::cout << "SDL event is SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED" << std::endl;
			break;
		    
			case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
			std::cout << "SDL event is SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED" << std::endl;
			break;
		
		    
			case SDL_EVENT_WINDOW_SHOWN:
			std::cout << "SDL event is SDL_EVENT_WINDOW_SHOWN" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_HIDDEN:
			std::cout << "SDL event is SDL_EVENT_WINDOW_HIDDEN" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_EXPOSED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_EXPOSED" << std::endl;
				std::cout << " Live change = " << event.window.data1 << std::endl;
				glGetIntegerv(GL_VIEWPORT, &viewPortDimensions.x);
				std::cout << "GL viewport at " << viewPortDimensions.x
					<< 'x' << viewPortDimensions.y
					<< ", size = " << viewPortDimensions.width
					<< 'x' << viewPortDimensions.height << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_MOVED:
			break;
		    
			case SDL_EVENT_WINDOW_RESIZED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_RESIZED" << std::endl;
				std::cout << " Window was resized to " << event.window.data1 
					<< 'x' << event.window.data2 << std::endl;
				glGetIntegerv(GL_VIEWPORT, &viewPortDimensions.x);
				std::cout << "GL viewport at " << viewPortDimensions.x
					<< 'x' << viewPortDimensions.y
					<< ", size = " << viewPortDimensions.width
					<< 'x' << viewPortDimensions.height << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
				std::cout << "SDL event is SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED" << std::endl;
				std::cout << " Pixel size changed to " << event.window.data1 
					<< 'x' << event.window.data2 << std::endl;
				glGetIntegerv(GL_VIEWPORT, &viewPortDimensions.x);
				std::cout << "GL viewport at " << viewPortDimensions.x
					<< 'x' << viewPortDimensions.y
					<< ", size = " << viewPortDimensions.width
					<< 'x' << viewPortDimensions.height << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_METAL_VIEW_RESIZED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_MINIMIZED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_MINIMIZED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_MAXIMIZED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_MAXIMIZED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_RESTORED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_RESTORED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_MOUSE_ENTER:
			std::cout << "SDL event is SDL_EVENT_WINDOW_MOUSE_ENTER" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			std::cout << "SDL event is SDL_EVENT_WINDOW_MOUSE_LEAVE" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_FOCUS_GAINED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_FOCUS_GAINED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_FOCUS_LOST:
			std::cout << "SDL event is SDL_EVENT_WINDOW_FOCUS_LOST" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_CLOSE_REQUESTED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_HIT_TEST:
			std::cout << "SDL event is SDL_EVENT_WINDOW_HIT_TEST" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_ICCPROF_CHANGED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_DISPLAY_CHANGED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_SAFE_AREA_CHANGED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_OCCLUDED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_OCCLUDED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
			std::cout << "SDL event is SDL_EVENT_WINDOW_ENTER_FULLSCREEN" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
			std::cout << "SDL event is SDL_EVENT_WINDOW_LEAVE_FULLSCREEN" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_DESTROYED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_DESTROYED" << std::endl;
			break;
		    
			case SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
			std::cout << "SDL event is SDL_EVENT_WINDOW_HDR_STATE_CHANGED" << std::endl;
			break;
		
		    /* Keyboard events */
		    
			case SDL_EVENT_KEY_DOWN:
			std::cout << "SDL event is SDL_EVENT_KEY_DOWN" << std::endl;
			break;
		    
			case SDL_EVENT_KEY_UP:
			std::cout << "SDL event is SDL_EVENT_KEY_UP" << std::endl;
			break;
		    
			case SDL_EVENT_TEXT_EDITING:
			std::cout << "SDL event is SDL_EVENT_TEXT_EDITING" << std::endl;
			break;
		    
			case SDL_EVENT_TEXT_INPUT:
			std::cout << "SDL event is SDL_EVENT_TEXT_INPUT" << std::endl;
			break;
		    
			case SDL_EVENT_KEYMAP_CHANGED:
			std::cout << "SDL event is SDL_EVENT_KEYMAP_CHANGED" << std::endl;
			break;
		    
			case SDL_EVENT_KEYBOARD_ADDED:
			std::cout << "SDL event is SDL_EVENT_KEYBOARD_ADDED" << std::endl;
			break;
		    
			case SDL_EVENT_KEYBOARD_REMOVED:
			std::cout << "SDL event is SDL_EVENT_KEYBOARD_REMOVED" << std::endl;
			break;
		    
			case SDL_EVENT_TEXT_EDITING_CANDIDATES:
			std::cout << "SDL event is SDL_EVENT_TEXT_EDITING_CANDIDATES" << std::endl;
			break;
		
		    /* Mouse events */
		    
			case SDL_EVENT_MOUSE_MOTION:
			// std::cout << "SDL event is SDL_EVENT_MOUSE_MOTION" << std::endl;
			break;
		    
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			std::cout << "SDL event is SDL_EVENT_MOUSE_BUTTON_DOWN" << std::endl;
			break;
		    
			case SDL_EVENT_MOUSE_BUTTON_UP:
			std::cout << "SDL event is SDL_EVENT_MOUSE_BUTTON_UP" << std::endl;
			break;
		    
			case SDL_EVENT_MOUSE_WHEEL:
			std::cout << "SDL event is SDL_EVENT_MOUSE_WHEEL" << std::endl;
			break;
		    
			case SDL_EVENT_MOUSE_ADDED:
			std::cout << "SDL event is SDL_EVENT_MOUSE_ADDED" << std::endl;
			break;
		    
			case SDL_EVENT_MOUSE_REMOVED:
			std::cout << "SDL event is SDL_EVENT_MOUSE_REMOVED" << std::endl;
			break;
		
		    /* Joystick events */
		
		    /* Gamepad events */
		
		    /* Touch events */
		    
			case SDL_EVENT_FINGER_DOWN:
			std::cout << "SDL event is SDL_EVENT_FINGER_DOWN" << std::endl;
			break;
		    
			case SDL_EVENT_FINGER_UP:
			std::cout << "SDL event is SDL_EVENT_FINGER_UP" << std::endl;
			break;
		    
			case SDL_EVENT_FINGER_MOTION:
			std::cout << "SDL event is SDL_EVENT_FINGER_MOTION" << std::endl;
			break;
		    
			case SDL_EVENT_FINGER_CANCELED:
			std::cout << "SDL event is SDL_EVENT_FINGER_CANCELED" << std::endl;
			break;
		
		    /* Clipboard events */
		    
			case SDL_EVENT_CLIPBOARD_UPDATE:
			std::cout << "SDL event is SDL_EVENT_CLIPBOARD_UPDATE" << std::endl;
			break;
		
		    /* Drag and drop events */
		    
			case SDL_EVENT_DROP_FILE:
			std::cout << "SDL event is SDL_EVENT_DROP_FILE" << std::endl;
			break;
		    
			case SDL_EVENT_DROP_TEXT:
			std::cout << "SDL event is SDL_EVENT_DROP_TEXT" << std::endl;
			break;
		    
			case SDL_EVENT_DROP_BEGIN:
			std::cout << "SDL event is SDL_EVENT_DROP_BEGIN" << std::endl;
			break;
		    
			case SDL_EVENT_DROP_COMPLETE:
			std::cout << "SDL event is SDL_EVENT_DROP_COMPLETE" << std::endl;
			break;
		    
			case SDL_EVENT_DROP_POSITION:
			std::cout << "SDL event is SDL_EVENT_DROP_POSITION" << std::endl;
			break;
		
		    /* Audio hotplug events */
		    
			case SDL_EVENT_AUDIO_DEVICE_ADDED:
			std::cout << "SDL event is SDL_EVENT_AUDIO_DEVICE_ADDED" << std::endl;
			break;
		    
			case SDL_EVENT_AUDIO_DEVICE_REMOVED:
			std::cout << "SDL event is SDL_EVENT_AUDIO_DEVICE_REMOVED" << std::endl;
			break;
		    
			case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED:
			std::cout << "SDL event is SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED" << std::endl;
			break;
		
		    /* Sensor events */
		    
			case SDL_EVENT_SENSOR_UPDATE:
			std::cout << "SDL event is SDL_EVENT_SENSOR_UPDATE" << std::endl;
			break;
		
		    /* Pressure-sensitive pen events */
		
		    /* Camera hotplug events */
		
		    /* Render events */
		    
			case SDL_EVENT_RENDER_TARGETS_RESET:
			std::cout << "SDL event is SDL_EVENT_RENDER_TARGETS_RESET" << std::endl;
			break;
		    
			case SDL_EVENT_RENDER_DEVICE_RESET:
			std::cout << "SDL event is SDL_EVENT_RENDER_DEVICE_RESET" << std::endl;
			break;
		    
			case SDL_EVENT_RENDER_DEVICE_LOST:
			std::cout << "SDL event is SDL_EVENT_RENDER_DEVICE_LOST" << std::endl;
			break;
		
		    /* Reserved events for private platforms */
		    
			case SDL_EVENT_PRIVATE0:
			std::cout << "SDL event is SDL_EVENT_PRIVATE0" << std::endl;
			break;
		    
			case SDL_EVENT_PRIVATE1:
			std::cout << "SDL event is SDL_EVENT_PRIVATE1" << std::endl;
			break;
		    
			case SDL_EVENT_PRIVATE2:
			std::cout << "SDL event is SDL_EVENT_PRIVATE2" << std::endl;
			break;
		    
			case SDL_EVENT_PRIVATE3:
			std::cout << "SDL event is SDL_EVENT_PRIVATE3" << std::endl;
			break;
		
		    /* Internal events */
		
		    /** Events SDL_EVENT_USER through SDL_EVENT_LAST are for your use,
		     *  and should be allocated with SDL_RegisterEvents()
		     */
		    
			case SDL_EVENT_USER:
			std::cout << "SDL event is SDL_EVENT_USER" << std::endl;
			break;

		default :
			std::cout << "Event type = " << event.type << std::endl;
			break;
	}
}

static bool handleSLEDvent (SDL_Event& event,OevGLES::GLFramework &framework) {

	printEventType (event);
	framework.handleSdlEvent(event);
	
	if (event.type == SDL_EVENT_QUIT) {
		return false;
	}
	
	if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
		auto windowID = event.button.windowID;
		auto renderSurfacePtr = 
			framework.getRenderSurfacePtr(windowID).lock();
		if (renderSurfacePtr) {
			SDL_Window *sdlWindow = renderSurfacePtr->getNativeWindow();
			std::cout << "sdlWindow = " << reinterpret_cast<void*>(sdlWindow)
				<< ", SDL_TextInputActive(sdlWindow) = " << SDL_TextInputActive(sdlWindow)
				<< std::endl;
				
			if (SDL_TextInputActive(sdlWindow)) {
				SDL_StopTextInput(sdlWindow);
			} else {
				
				auto rc = SDL_StartTextInput(sdlWindow);
				
				std::cout << "SDL_StartTextInput returned " << rc << std::endl;
				
				SDL_Rect rect;
				int cursor;
				rc = SDL_GetTextInputArea (sdlWindow,&rect,&cursor);
				std::cout << "SDL_GetTextInputArea returned " << rc
					<< ", pos = " << rect.x << "x" << rect.y
					<< ", size (WxH) = " << rect.w << "x" << rect.h
					<< ", cursor = " << cursor
					<< std::endl;
			}
		} // if (renderSurfacePtr) {
	}

	if (event.type == SDL_EVENT_TEXT_EDITING) {
		std::cout 
			<< "text = " << event.edit.text
			<< ", start = " << event.edit.start
			<< ", length = " << event.edit.length
			<< std::endl;
	}

	if (event.type == SDL_EVENT_TEXT_INPUT) {
		std::cout 
			<< "text = " << event.text.text
			<< std::endl;
	}
	
	return true;
}

