/*
 * SDLRenderSurface.h
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

#include <memory>

#include "GLES/GLFramework.h"
#include "GLES/sysSDLWindow.h"

// Forward declaration
struct SDL_GLContextState;

namespace OevGLES {

class SDLRenderSurface;
using SDLRenderSurfaceSharedPtr = std::shared_ptr<SDLRenderSurface>;
using SDLRenderSurfaceWeakPtr = std::weak_ptr<SDLRenderSurface>;

} // namespace OevGLES {

#include "RootControl.h"

namespace OevGLES {

class GLFramework;


class SDLRenderSurface {
	friend class GLFramework;
public:

	SDLRenderSurface(GLFramework& framework);

	virtual ~SDLRenderSurface();

	void createRenderSurface (GLint width, GLint height,
			char const* windowName);

	void makeContextCurrent();

	SDLNativeWindow& getNativeWindow() {
		return nativeWindow;
	}

	GLFramework& getGlFramework() {
		return glFramework;
	}
	
	/* \brief Return the root control associated with the render surface.
	 * 
	 * The root control is being created when it did not yet exist.
	 */
	OevControls::RootControlWeakPtr getRootControlPtr();

protected:

	GLFramework& glFramework;
	SDLNativeWindow nativeWindow;

	SDL_GLContextState* glContext = nullptr;

    GLint eglMajorVersion = 2;
    GLint eglMinorVersion = 0;

	OevControls::RootControlSharedPtr rootControlPtr;

};

using SDLRenderSurfaceSharedPtr = std::shared_ptr<SDLRenderSurface>;
using SDLRenderSurfaceWeakPtr = std::weak_ptr<SDLRenderSurface>;

} /* namespace OevGLES */

#endif /* GLES_EGLRENDERSURFACE_H_ */
