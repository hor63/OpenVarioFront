/*
 * EGLRenderSurface.h
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

#ifndef GLES_EGLRENDERSURFACE_H_
#define GLES_EGLRENDERSURFACE_H_

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>


#include "OVFCommon.h"

namespace OevGLES {

class EGLRenderSurface {
public:
	EGLRenderSurface();

	virtual ~EGLRenderSurface();

	void createRenderSurface (GLint width, GLint height,
			char const* windowName = 0, char const* displayName = 0);

	void makeContextCurrent();

	EGLSurface getRenderSurface () const {
		return renderSurface;
	}

	operator EGLSurface () const {
		return renderSurface;
	}

	EGLContext getRenderContext() const {
		return renderContext;
	}

	operator EGLContext () const {
		return renderContext;
	}

#if defined HAVE_LOG4CXX_H
	void debugPrintConfig (EGLConfig *configs,EGLint numReturnedConfigs);
#endif // if defined HAVE_LOG4CXX_H

protected:

#if defined HAVE_LOG4CXX_H
    log4cxx::LoggerPtr logger;
#endif

    EGLNativeDisplayType	nativeDisplay = 0;
    EGLNativeWindowType		nativeWindow = 0;
    EGLDisplay				eglDisplay = EGL_NO_DISPLAY;
    EGLSurface				renderSurface = EGL_NO_SURFACE;
    EGLContext				renderContext = EGL_NO_CONTEXT;

    EGLint eglMajorVersion = 0;
    EGLint eglMinorVersion = 0;
};



} /* namespace OevGLES */

#endif /* GLES_EGLRENDERSURFACE_H_ */
