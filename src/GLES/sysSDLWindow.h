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

class SDLNativeWindow {
public:

	SDLNativeWindow () {}
	/** \brief The destructor will implicitly close and destroz an open native window if it was open.
	 *
	 * \see \ref openNativeWindow()
	 * \see \ref closeNativeWindow()
	 */
	~SDLNativeWindow();

	SDLNativeWindow (SDLNativeWindow&& source);
	SDLNativeWindow (SDLNativeWindow const& source) = delete;
	SDLNativeWindow operator = (SDLNativeWindow const& source) = delete;

/** \brief Opens a native Window with OpenGL ES2 capabilities via SDL
 *
 * The function obtains the default system display, and opens a window with the passed dimensions \ref width and \ref height.
 * If width and heights are both 0 the function tries to open the window full-screen.
 *
 * @param[in] width Width of the window in the display dimension, usually in Pixel
 * @param[in] height Height of the window in the display dimension, usually in Pixel
 * @param[in] windowName Title of the window. Is only relevant for windows based systems which have borders and a window title.
 */
void openNativeWindow(
		GLint width,GLint height,
		char const* windowName);

/** \brief Close and destroy the native window
 *
 */
void closeNativeWindow();

protected:

SDL_Window * sdlWindow = nullptr;

}; // class SDLNativeWindow

} // namespace OevGLES


#ifndef SRC_SYSEGLWINDOW_H_
#define SRC_SYSEGLWINDOW_H_





#endif /* SRC_SYSEGLWINDOW_H_ */
