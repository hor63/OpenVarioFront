/*
 *  sysEGLWindow.cpp
 *
 *  System dependent part of the EGL classes. Here for X11
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

#include <sstream>
#include <memory.h>

#include "OVFCommon.h"

#include "sysEGLWindow.h"
#include "ExceptionBase.h"

namespace OevGLES {

void openNativeWindow(EGLNativeDisplayType& display,
		EGLNativeWindowType& window, GLint width, GLint height,
		char const* windowName, char const* displayName) {

	std::ostringstream errString;
	static fbdev_window win = {0,0};

#if defined HAVE_LOG4CXX_H
    log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("OpenVarioFront.openNativeWindow");
#endif

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    LOG4CXX_DEBUG(logger,"display = " << display);
    if (display == EGL_NO_DISPLAY) {
    	throw NativeWindowException("Cannot obtain default display");
    }

    win.height = height;
    win.width = width;

    window = &win;

}

void closeNativeWindow(EGLNativeDisplayType display,
		EGLNativeWindowType window) {

	;
}


} // namespace OevGLES
