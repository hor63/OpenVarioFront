/*
 * SDLRenderSurface.cpp
 *
 *  Created on: Apr 24, 2018
 *	  Author: hor
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
#include "Renderers/RendererBase.h"
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "OVFCommon.h"

#include "RootControl.h"

#include "GLES/GLFramework.h"
#include "SDLUtil.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif



SDLRenderSurface::SDLRenderSurface(GLFramework& framework)
	:glFramework{framework}
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.SDLRenderSurface");
	}
#endif

}


SDLRenderSurface::~SDLRenderSurface() {

	SDL_GL_DestroyContext(glContext);
	nativeWindow.closeNativeWindow();
}

void SDLRenderSurface::createRenderSurface (GLint width, GLint height,
		char const* windowName) {

	// Leave defaults SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES)
		|| !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2)
		|| !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0)
		|| !SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1)
		|| !SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)
		|| !SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16)
// Some GLES implementation do not support multisampled rendering, e.g 
// older Intel I915 integrated GPUs
//		|| !SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1)
//		|| !SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4)
		) {
		OevUtil::reportSDLError(std::source_location::current(), "SDL_GL_SetAttribute (mult.)");
	}

	nativeWindow.openNativeGLES2Window(
			width, height, windowName);

	glContext = SDL_GL_CreateContext (nativeWindow);
	if (glContext == nullptr) {
		OevUtil::reportSDLError(std::source_location::current(), "SDL_GL_CreateContext");
	}

	if(!SDL_GL_MakeCurrent(nativeWindow,glContext)) {
		OevUtil::reportSDLError(std::source_location::current(), "SDL_GL_MakeCurrent");
	}

	LOG4CXX_DEBUG(logger,"renderContext is now current");

	int swapInterval = -2;
	SDL_GL_GetSwapInterval(&swapInterval);
	LOG4CXX_DEBUG(logger,"SDL_GL_GetSwapInterval returns "
			<< swapInterval);

	int RenderWidth = -1, RenderHeight = -1;
	SDL_GetWindowSizeInPixels(nativeWindow,&RenderWidth,&RenderHeight);
	LOG4CXX_DEBUG(logger,"Window size = " << RenderWidth
			<< "x" << RenderHeight);

	auto videoDriverName = SDL_GetCurrentVideoDriver();
	LOG4CXX_DEBUG(logger,"Name of the video driver = "
			<<  videoDriverName);

	struct ViewportCoords {
		GLint x; GLint y;
		GLint width; GLint height;
	} viewportCoords;

	SDL_GetWindowSizeInPixels(nativeWindow, &windowSize.widthPixel,&windowSize.heightPixel);
	LOG4CXX_DEBUG(logger, 
		" Window size = " << windowSize.widthPixel << 'x' << windowSize.heightPixel);

	glGetIntegerv (GL_VIEWPORT, &viewportCoords.x);
	LOG4CXX_DEBUG(logger,"Viewport pos = "
			<< viewportCoords.x << 'x' << viewportCoords.y
			<< ", size = " << viewportCoords.width << 'x' << viewportCoords.height
			);

	onWindowResize();

}

void SDLRenderSurface::makeContextCurrent() {

	if(!SDL_GL_MakeCurrent(nativeWindow,glContext)) {
		OevUtil::reportSDLError(std::source_location::current(), "SDL_GL_MakeCurrent");
	}
	LOG4CXX_DEBUG(logger,"renderContext is now current");

}

OevControls::RootControlWeakPtr SDLRenderSurface::getRootControlPtr() {
	
	if (!rootControlPtr) {
		rootControlPtr = OevControls::RootControl::makeRootControl(*this);
	}
	
	return rootControlPtr;
}

bool SDLRenderSurface::handleSLEDvent (SDL_Event& event) {
	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__ 
		<< ": event.type = " << event.type);

	switch (event.type) {
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			LOG4CXX_DEBUG(logger, "\tSDL event is SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED");
			LOG4CXX_DEBUG(logger, "\t Window size changed to " << event.window.data1 
				<< 'x' << event.window.data2 << " Pixels");
			
			SDL_GetWindowSizeInPixels(nativeWindow, &windowSize.widthPixel,&windowSize.heightPixel);
			LOG4CXX_DEBUG(logger, 
				" Window size = " << windowSize.widthPixel << 'x' << windowSize.heightPixel);
		
			onWindowResize();
		break;
	} // switch (event.type)


	return true;
}
void SDLRenderSurface::onWindowResize() {
	struct {
		GLint x; GLint y;
		GLint width; GLint height;
	} viewPortDimensions;

	makeContextCurrent();

	glGetIntegerv(GL_VIEWPORT, &viewPortDimensions.x);
	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__ << ": GL viewport at " << viewPortDimensions.x
		<< 'x' << viewPortDimensions.y
		<< ", size = " << viewPortDimensions.width
		<< 'x' << viewPortDimensions.height);
	glViewport(0, 0, windowSize.widthPixel, windowSize.heightPixel);
	glGetIntegerv(GL_VIEWPORT, &viewPortDimensions.x);
	LOG4CXX_DEBUG(logger, "\t New GL viewport at " << viewPortDimensions.x
		<< 'x' << viewPortDimensions.y
		<< ", size = " << viewPortDimensions.width
		<< 'x' << viewPortDimensions.height);

	calculateViewProjectionMatrix();
	
	if (rootControlPtr) {
		rootControlPtr->setSize(windowSize);
	}
}

void SDLRenderSurface::calculateViewProjectionMatrix() {
	
	OevGLES::Vec3 camPos = {static_cast<GLfloat>(windowSize.widthPixel) / 2.0f,
		static_cast<GLfloat>(windowSize.heightPixel) / 2.0f,
		static_cast<float>(windowSize.heightPixel*2)};

	// Assume the initial view point is exactly on the z-axis.
	// My goal is to find the aperture angle at which from this viewpoint
	// one coordinate unit in x or y direction is exactly one pixel.
	// Thus with the aperture angle I see exactly the window height.
	// To calculate the aperture angle the the ArcTan of
	// (windowHeight/2) / viewerDistance
	// is half of the aperture angle.
	OevGLES::AngleRad apertureAngle = 
		OevGLES::AngleRad::makeAngle(atan((
			static_cast<double>(windowSize.heightPixel)/2.0)/camPos(2,0)) * 2.0);

	OevGLES::Vec3 up = {0,1,0};
	OevGLES::Vec3 lookAtPos = camPos;
	lookAtPos(2,0) = 0.0f;

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__ << ": camPos \n" << camPos
		<< ", aperture angle = " << OevGLES::AngleDeg(apertureAngle).getAngleValue());

	baseUniforms.getViewMatrix() =
		OevGLES::viewMatrix(camPos, lookAtPos, up);
		
	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__ << ": viewMatrix \n" << baseUniforms.getViewMatrixC());
	
	baseUniforms.getProjMatrix() =
		OevGLES::projectionMatrix(
			static_cast<GLfloat>(windowSize.heightPixel),
			static_cast<GLfloat>(windowSize.heightPixel * 3),
			static_cast<GLfloat>(windowSize.widthPixel) /
			static_cast<GLfloat>(windowSize.heightPixel),
			apertureAngle);

	LOG4CXX_DEBUG(logger, "\t projectionMatrix =\n" << baseUniforms.getProjMatrixC()
		<< "\n\t ViewMatrix = \n" << baseUniforms.getViewMatrixC()
		<< "\n\t MVPMatrix = \n" << baseUniforms.getMVPMatrixC()
		);

/* Just a number tests how the MVP matrix applies incl. division by w +/
	Vec4 pos1 {0,0,0,1};
	Vec4 projectedPos1 = baseUniforms.getMVPMatrixC() * pos1;
	LOG4CXX_DEBUG(logger, "\t pos1 = " << pos1.transpose()
		<< ", projected pos1 = " << (projectedPos1.transpose() / projectedPos1(3,0)));

	Vec4 pos2 {1000,1000,0,1};
	Vec4 projectedPos2 = baseUniforms.getMVPMatrixC() * pos2;
	LOG4CXX_DEBUG(logger, "\t pos2 = " << pos2.transpose()
		<< ", projected pos2 = " << (projectedPos2.transpose() / projectedPos2(3,0)));

	pos1(2,0) = -500;
	projectedPos1 = baseUniforms.getMVPMatrixC() * pos1;
	LOG4CXX_DEBUG(logger, "\t pos1 = " << pos1.transpose()
		<< ", projected pos1 = " << (projectedPos1.transpose() / projectedPos1(3,0)));

	pos2(2,0) = -500;
	projectedPos2 = baseUniforms.getMVPMatrixC() * pos2;
	LOG4CXX_DEBUG(logger, "\t pos2 = " << pos2.transpose()
		<< ", projected pos2 = " << (projectedPos2.transpose() / projectedPos2(3,0)));

	pos2(2,0) = -1000;
	projectedPos2 = baseUniforms.getMVPMatrixC() * pos2;
	LOG4CXX_DEBUG(logger, "\t pos2 = " << pos2.transpose()
		<< ", projected pos2 = " << (projectedPos2.transpose() / projectedPos2(3,0)));

	pos2(2,0) = 1000;
	projectedPos2 = baseUniforms.getMVPMatrixC() * pos2;
	LOG4CXX_DEBUG(logger, "\t pos2 = " << pos2.transpose()
		<< ", projected pos2 = " << (projectedPos2.transpose() / projectedPos2(3,0)));
/+ */

}

#if defined HAVE_LOG4CXX_H

/*
void EGLRenderSurface::debugPrintConfig (EGLConfig *configs,EGLint numReturnedConfigs) {

#if defined HAVE_LOG4CXX_H

	for (EGLint i = 0; i < numReturnedConfigs; i++) {

		struct TConfigAttrs {
			char const * attrName;
			EGLint attrVal;
		} configAttrs[32];

		#define queryConfigAttr(attr,k) {eglGetConfigAttrib(eglDisplay,configs[i],attr,&configAttrs[k].attrVal); \
										configAttrs[k].attrName = #attr; }

		queryConfigAttr(EGL_CONFIG_ID,0)
		queryConfigAttr(EGL_BUFFER_SIZE,1)
		queryConfigAttr(EGL_RED_SIZE,2)
		queryConfigAttr(EGL_GREEN_SIZE,3)
		queryConfigAttr(EGL_BLUE_SIZE,4)
		queryConfigAttr(EGL_LUMINANCE_SIZE,5)
		queryConfigAttr(EGL_ALPHA_SIZE,6)
		queryConfigAttr(EGL_ALPHA_MASK_SIZE,7)
		queryConfigAttr(EGL_BIND_TO_TEXTURE_RGB,8)
		queryConfigAttr(EGL_BIND_TO_TEXTURE_RGBA,9)
		queryConfigAttr(EGL_COLOR_BUFFER_TYPE,10)
		queryConfigAttr(EGL_CONFIG_CAVEAT,11)
		queryConfigAttr(EGL_CONFORMANT,12)
		queryConfigAttr(EGL_DEPTH_SIZE,13)
		queryConfigAttr(EGL_LEVEL,14)
		queryConfigAttr(EGL_MAX_PBUFFER_WIDTH,15)
		queryConfigAttr(EGL_MAX_PBUFFER_HEIGHT,16)
		queryConfigAttr(EGL_MAX_PBUFFER_PIXELS,17)
		queryConfigAttr(EGL_MAX_SWAP_INTERVAL,18)
		queryConfigAttr(EGL_MIN_SWAP_INTERVAL,19)
		queryConfigAttr(EGL_NATIVE_RENDERABLE,20)
		queryConfigAttr(EGL_NATIVE_VISUAL_ID,21)
		queryConfigAttr(EGL_NATIVE_VISUAL_TYPE,22)
		queryConfigAttr(EGL_RENDERABLE_TYPE,23)
		queryConfigAttr(EGL_SAMPLE_BUFFERS,24)
		queryConfigAttr(EGL_SAMPLES,25)
		queryConfigAttr(EGL_STENCIL_SIZE,26)
		queryConfigAttr(EGL_SURFACE_TYPE,27)
		queryConfigAttr(EGL_TRANSPARENT_TYPE,28)
		queryConfigAttr(EGL_TRANSPARENT_RED_VALUE,29)
		queryConfigAttr(EGL_TRANSPARENT_GREEN_VALUE,30)
		queryConfigAttr(EGL_TRANSPARENT_BLUE_VALUE,31)

		LOG4CXX_DEBUG(logger,"Configuration [ " << i << "] = \n" <<
				"\t\t\t" << configAttrs[0].attrName << " =\t" << configAttrs[0].attrVal << "\n"
				"\t\t\t" << configAttrs[1].attrName << " =\t" << configAttrs[1].attrVal << "\n"
				"\t\t\t" << configAttrs[2].attrName << " =\t" << configAttrs[2].attrVal << "\n"
				"\t\t\t" << configAttrs[3].attrName << " =\t" << configAttrs[3].attrVal << "\n"
				"\t\t\t" << configAttrs[4].attrName << " =\t" << configAttrs[4].attrVal << "\n"
				"\t\t\t" << configAttrs[5].attrName << " =\t" << configAttrs[5].attrVal << "\n"
				"\t\t\t" << configAttrs[6].attrName << " =\t" << configAttrs[6].attrVal << "\n"
				"\t\t\t" << configAttrs[7].attrName << " =\t" << configAttrs[7].attrVal << "\n"
				"\t\t\t" << configAttrs[8].attrName << " =\t" << configAttrs[8].attrVal << "\n"
				"\t\t\t" << configAttrs[9].attrName << " =\t" << configAttrs[9].attrVal << "\n"
				"\t\t\t" << configAttrs[10].attrName << " =\t" << configAttrs[10].attrVal << "\n"
				"\t\t\t" << configAttrs[11].attrName << " =\t" << configAttrs[11].attrVal << "\n"
				"\t\t\t" << configAttrs[12].attrName << " =\t" << configAttrs[12].attrVal << "\n"
				"\t\t\t" << configAttrs[13].attrName << " =\t" << configAttrs[13].attrVal << "\n"
				"\t\t\t" << configAttrs[14].attrName << " =\t" << configAttrs[14].attrVal << "\n"
				"\t\t\t" << configAttrs[15].attrName << " =\t" << configAttrs[15].attrVal << "\n"
				"\t\t\t" << configAttrs[16].attrName << " =\t" << configAttrs[16].attrVal << "\n"
				"\t\t\t" << configAttrs[17].attrName << " =\t" << configAttrs[17].attrVal << "\n"
				"\t\t\t" << configAttrs[18].attrName << " =\t" << configAttrs[18].attrVal << "\n"
				"\t\t\t" << configAttrs[19].attrName << " =\t" << configAttrs[19].attrVal << "\n"
				"\t\t\t" << configAttrs[20].attrName << " =\t" << configAttrs[20].attrVal << "\n"
				"\t\t\t" << configAttrs[21].attrName << " =\t" << configAttrs[21].attrVal << "\n"
				"\t\t\t" << configAttrs[22].attrName << " =\t" << configAttrs[22].attrVal << "\n"
				"\t\t\t" << configAttrs[23].attrName << " =\t" << configAttrs[23].attrVal << "\n"
				"\t\t\t" << configAttrs[24].attrName << " =\t" << configAttrs[24].attrVal << "\n"
				"\t\t\t" << configAttrs[25].attrName << " =\t" << configAttrs[25].attrVal << "\n"
				"\t\t\t" << configAttrs[26].attrName << " =\t" << configAttrs[26].attrVal << "\n"
				"\t\t\t" << configAttrs[27].attrName << " =\t" << configAttrs[27].attrVal << "\n"
				"\t\t\t" << configAttrs[28].attrName << " =\t" << configAttrs[28].attrVal << "\n"
				"\t\t\t" << configAttrs[29].attrName << " =\t" << configAttrs[29].attrVal << "\n"
				"\t\t\t" << configAttrs[30].attrName << " =\t" << configAttrs[30].attrVal << "\n"
				"\t\t\t" << configAttrs[31].attrName << " =\t" << configAttrs[31].attrVal << "\n"
				);

	}

#endif // if defined HAVE_LOG4CXX_H

}
*/

#endif // if defined HAVE_LOG4CXX_H

} /* namespace OevGLES */
