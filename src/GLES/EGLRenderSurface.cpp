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


#include "GLES/EGLRenderSurface.h"
#include "GLES/sysEGLWindow.h"
#include "GLES/ExceptionBase.h"

namespace OevGLES {

EGLRenderSurface::EGLRenderSurface()
#if defined HAVE_LOG4CXX_H
    :logger{log4cxx::Logger::getLogger("OpenVarioFront.EGLRenderSurface")}
#endif
		{

}


EGLRenderSurface::~EGLRenderSurface() {
	// TODO Auto-generated destructor stub
}

void EGLRenderSurface::createRenderSurface (GLint width, GLint height,
		char const* windowName, char const* displayName) {
	EGLint numConfigs = 0;
	EGLConfig *configs;


	openNativeWindow(nativeDisplay,nativeWindow,
			width, height, windowName, displayName);

	eglDisplay = eglGetDisplay(nativeDisplay);
    LOG4CXX_DEBUG(logger,"eglDisplay = " << eglDisplay);

	if (eglDisplay == EGL_NO_DISPLAY) {
		throw EGLException("Cannot get an EGL display");
	}

	if (eglInitialize(eglDisplay,&eglMajorVersion,&eglMinorVersion) == EGL_FALSE) {
		throw EGLException("Cannot initialize EGL");
	}
    LOG4CXX_INFO(logger,"Initialized EGL. EGL Version = " << eglMajorVersion << '.' << eglMinorVersion);

    eglGetConfigs(eglDisplay,0,0,&numConfigs);
    LOG4CXX_DEBUG(logger,"Number of configurations = " << numConfigs);

}

} /* namespace OevGLES */
