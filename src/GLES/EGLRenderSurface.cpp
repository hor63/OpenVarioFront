/*
 * EGLRenderSurface.cpp
 *
 *  Created on: Apr 24, 2018
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

#include <sstream>

#include "OVFCommon.h"


#include "GLES/EGLRenderSurface.h"
#include "GLES/sysEGLWindow.h"
#include "GLES/ExceptionBase.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif



EGLRenderSurface::EGLRenderSurface()
		{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.EGLRenderSurface");
	}
#endif

}


EGLRenderSurface::~EGLRenderSurface() {

	if (eglDisplay != EGL_NO_DISPLAY) {
		eglMakeCurrent(eglDisplay,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
	}


	if (renderContext != EGL_NO_CONTEXT) {
		eglDestroyContext(eglDisplay,renderContext);
	}
	if (renderSurface != EGL_NO_SURFACE) {
		eglDestroySurface(eglDisplay,renderSurface);
	}
	if (eglDisplay != EGL_NO_DISPLAY) {
		eglTerminate(eglDisplay);
	}

	closeNativeWindow(nativeDisplay,nativeWindow);
}

void EGLRenderSurface::createRenderSurface (GLint width, GLint height,
		char const* windowName, char const* displayName) {
	EGLConfig config = EGL_NO_CONFIG_KHR;


	openNativeWindow(nativeDisplay,nativeWindow,
			width, height, windowName, displayName);

	eglDisplay = eglGetDisplay(nativeDisplay);
    LOG4CXX_DEBUG(logger,"eglDisplay = " << eglDisplay);

	if (eglDisplay == EGL_NO_DISPLAY) {
		std::ostringstream errStr;
		errStr << "Cannot get an EGL display. Error = " << eglGetError();

		throw EGLException(errStr.str().c_str());
	}

	if (eglInitialize(eglDisplay,&eglMajorVersion,&eglMinorVersion) == EGL_FALSE) {
		std::ostringstream errStr;
		errStr << "Cannot initialize EGL. Error = " << eglGetError();

		throw EGLException(errStr.str().c_str());
	}
    LOG4CXX_INFO(logger,"Initialized EGL. EGL Version = " << eglMajorVersion << '.' << eglMinorVersion);

    {
    	EGLint numReturnedConfigs = 0;

        EGLint attribList [] = {
        		EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
        		EGL_RENDERABLE_TYPE	, EGL_OPENGL_ES2_BIT,
				EGL_RED_SIZE 		, 8,
				EGL_GREEN_SIZE		, 8,
				EGL_BLUE_SIZE		, 8,
				EGL_ALPHA_SIZE		, 8,
				EGL_DEPTH_SIZE		, 16,
				EGL_SAMPLE_BUFFERS  , 1,
				EGL_SAMPLES         , 2,
				EGL_NONE
        };

        if (eglChooseConfig(eglDisplay,attribList,&config,1,&numReturnedConfigs)){
			if (numReturnedConfigs == 0) {
				// Do not prescribe a sample number
				attribList[16] = EGL_NONE;
		        eglChooseConfig(eglDisplay,attribList,&config,1,&numReturnedConfigs);
				if (numReturnedConfigs == 0) {
					// Do not prescribe a sample buffer at all
					attribList[14] = EGL_NONE;
			        eglChooseConfig(eglDisplay,attribList,&config,1,&numReturnedConfigs);
				}
			}


#if defined HAVE_LOG4CXX_H
			if (logger->getLevel() == log4cxx::Level::getDebug()) {
				debugPrintConfig (&config,numReturnedConfigs);
			}

#endif // if defined HAVE_LOG4CXX_H
			if (numReturnedConfigs == 0) {
    		std::ostringstream errStr;
    		errStr << "Could not retrieve valid EGL configuration. Error = " << eglGetError();

    		throw EGLException(errStr.str().c_str());
			}


        } else {
    		std::ostringstream errStr;
    		errStr << "Could not retrieve valid EGL configuration. Error = " << eglGetError();

    		throw EGLException(errStr.str().c_str());
        }

    }
    
    {
    	EGLint attribList[] = {
    			EGL_RENDER_BUFFER , EGL_BACK_BUFFER,
				EGL_NONE
    	};

    	renderSurface = eglCreateWindowSurface(eglDisplay,config,nativeWindow,attribList);

        LOG4CXX_DEBUG(logger,"renderSurface = " << renderSurface);

    	if (renderSurface == EGL_NO_SURFACE) {
    		std::ostringstream errStr;
    		errStr << "Error calling eglCreateWindowSurface. Error = " << eglGetError();

    		throw EGLException(errStr.str().c_str());
    	}

    };

    {
    	EGLint attribList[] = {
    			EGL_CONTEXT_CLIENT_VERSION , 2,
				EGL_NONE
    	};

    	renderContext = eglCreateContext(eglDisplay,config,EGL_NO_CONTEXT,attribList);

        LOG4CXX_DEBUG(logger,"renderContext = " << renderContext);

    	if (renderContext == EGL_NO_CONTEXT) {
    		std::ostringstream errStr;
    		errStr << "Error calling eglCreateContext. Error = " << eglGetError();

    		throw EGLException(errStr.str().c_str());
    	}

    };
    
	if (!eglMakeCurrent(eglDisplay,renderSurface,renderSurface,renderContext)) {
		std::ostringstream errStr;
		errStr << "Error calling eglMakeCurrent. Error = " << eglGetError();

		throw EGLException(errStr.str().c_str());
	}

	LOG4CXX_DEBUG(logger,"renderContext is now current");



}

void EGLRenderSurface::makeContextCurrent() {
	if (!eglMakeCurrent(eglDisplay,renderSurface,renderSurface,renderContext)) {
		std::ostringstream errStr;
		errStr << "Error calling eglMakeCurrent. Error = " << eglGetError();

		throw EGLException(errStr.str().c_str());
	}

	LOG4CXX_DEBUG(logger,"renderContext is now current");

}

#if defined HAVE_LOG4CXX_H

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

#endif // if defined HAVE_LOG4CXX_H

} /* namespace OevGLES */
