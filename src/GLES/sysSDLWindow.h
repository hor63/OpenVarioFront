/*
 *  sysEGLWindow.h
 *
 *  Declare system dependent functions to obtain the EGLNativeDisplay and EGLNativeWindow, and input functions
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

#include <SDL.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>

namespace OevGLES {


/** \brief Obtains the default system display, and opens a window within
 *
 * The function obtains the default system display, and opens a window with the passed dimensions \ref width and \ref height.
 * If width and heights are both 0 the function tries to open the window full-screen.
 *
 * @param[out] display The display to which EGL interfaces
 * @param[out] window A new window which this call opens; into this window's surface OpenGL ES2 wil draw.
 * @param[in] width Width of the window in the display dimension, usually in Pixel
 * @param[in] height Height of the window in the display dimension, usually in Pixel
 * @param[in] windowName Title of the window. Is only relevant for windows based systems which have borders and a window title.
 * @param[in] displayName Optional. System dependent. If omitted the default display is used. Only useful on multiple headed systems with multiple monitors.
 */
void openNativeWindow(
		GLint width = 0,GLint height = 0,
		char const* windowName = NULL,
		char const* displayName = NULL);

/** \brief Close and destroy the native window
 *
 * @param[in] display The Display (may not be necessary in most cases
 * @param[in] window The window which shall be closed and resources destroyed.
 */
void closeNativeWindow(EGLNativeDisplayType display,
		EGLNativeWindowType window);

} // namespace OevGLES


#ifndef SRC_SYSEGLWINDOW_H_
#define SRC_SYSEGLWINDOW_H_





#endif /* SRC_SYSEGLWINDOW_H_ */
