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

#include "GLES/sysSDLWindow.h"

#include "SDL3/SDL_opengles2.h"

namespace OevGLES {

class SDLRenderSurface {
public:
	SDLRenderSurface();

	virtual ~SDLRenderSurface();

	void createRenderSurface (GLint width, GLint height,
			char const* windowName);

	void makeContextCurrent();

	SDLNativeWindow& getNativeWindow() {
		return nativeWindow;
	}

protected:

	SDLNativeWindow nativeWindow;

	SDL_GLContext glContext = nullptr;

    GLint eglMajorVersion = 2;
    GLint eglMinorVersion = 0;

};



} /* namespace OevGLES */

#endif /* GLES_EGLRENDERSURFACE_H_ */
