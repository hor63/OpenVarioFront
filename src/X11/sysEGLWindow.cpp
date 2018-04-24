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

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "sysEGLWindow.h"
#include "ExceptionBase.h"
#include <sstream>
#include <memory.h>

namespace OpenVarioFront {

void openNativeWindow(EGLNativeDisplayType& display,
		EGLNativeWindowType& window, GLint width, GLint height,
		char const* windowName, char const* displayName) {

	std::ostringstream errString;

	display = XOpenDisplay(displayName);
    Window   rootWindow;
    XSetWindowAttributes winAttr;
    XWMHints wmHints;
    Atom wmState;
    XEvent xEv;

	if (!display) {
		errString << "XOpenDisplay (";
		if (displayName) {
			errString << "\"" << displayName << "\"";
		} else {
			errString << "NULL";
		}
		errString << " returned NULL. No X11 Display available.";

		throw ExceptionBase(errString.str().c_str());
	}

	rootWindow = XDefaultRootWindow(display);
	if (!rootWindow) {
		throw ExceptionBase("Cannot obtain root window");
	}

	winAttr.event_mask =  ExposureMask | PointerMotionMask | KeyPressMask;
	winAttr.override_redirect = 0;

	window = XCreateWindow(
			display,
			rootWindow,
			0,0, // x,y
			width,height,
			0, // border width
            CopyFromParent, // depth
			InputOutput, // class
            CopyFromParent, // Visual
			CWEventMask | CWOverrideRedirect, // value mask
            &winAttr);

	if (!rootWindow) {
		throw ExceptionBase("Cannot obtain root window");
	}

	wmHints.input = 1;
	wmHints.flags = InputHint;
	XSetWMHints(display, window, &wmHints);

	// make the window visible on the screen
    XMapWindow (display, window);

    // Set a title when defined
    if (windowName) {
    	XStoreName (display, window, windowName);
    }

    // get identifiers for the provided atom name strings
    wmState = XInternAtom (display, "_NET_WM_STATE", 0);

    // Notify the parent that a new window was created.
    memset ( &xEv, 0, sizeof(xEv) );
    xEv.type                 = ClientMessage;
    xEv.xclient.window       = window;
    xEv.xclient.message_type = wmState;
    xEv.xclient.format       = 32;
    xEv.xclient.data.l[0]    = 1;
    xEv.xclient.data.l[1]    = 0;
    XSendEvent (
       display,
       rootWindow,
       0,
       SubstructureNotifyMask,
       &xEv );

}

} // namespace OpenVarioFront
