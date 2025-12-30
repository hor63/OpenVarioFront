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

#include "Renderers/RendererBase.h"
#include "GLES/sysSDLWindow.h"
#include "GLES/GLFramework.h"

// Forward declaration
struct SDL_GLContextState;

namespace OevGLES {

class SDLRenderSurface;
using SDLRenderSurfaceSharedPtr = std::shared_ptr<SDLRenderSurface>;
using SDLRenderSurfaceWeakPtr = std::weak_ptr<SDLRenderSurface>;

} // namespace OevGLES {

namespace OevControls {

class RootControl;

using RootControlSharedPtr = std::shared_ptr<RootControl>;
using RootControlWeakPtr = std::weak_ptr<RootControl>;


} // namespace OevControls

namespace OevGLES {

struct RenderContext;

using RenderContextSharedPtr = std::shared_ptr<RenderContext>;

class GLFramework;


class SDLRenderSurface {
	friend class GLFramework;
public:

	struct SizePixel {
		int widthPixel = 0;
		int heightPixel = 0;
	};

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
	
	/** \brief Return the root control associated with the render surface.
	 * 
	 * The root control is being created when it did not yet exist.
	 *
	 */
	OevControls::RootControlWeakPtr getRootControlPtr();
	
	/** \brief SDL event handler and dispatcher
	 * 
	 * Window related events are forwarded here, and must be handled here.
	 */
	bool handleSLEDvent (SDL_Event& event);

	/// \brief Write access to \ref baseUniforms
	///
	/// \see \ref baseUniforms
	OevGLES::RenderStandardUniforms& getBaseUniforms() {
		return baseUniforms;
	}

	/// \brief Read-only access to \ref baseUniforms
	///
	/// \see \ref baseUniforms
	OevGLES::RenderStandardUniforms const& getBaseUniforms() const {
		return baseUniforms;
	}
	
	RenderContextSharedPtr const &getRenderContextPtr() const {
		return contextPtr;
	}

	/** \brief Called when a resize message is received or when the window is created.
	 *
	 * The default implementation re-calculates \ref baseUniforms to adjust the projection and view matrixes to
	 * the described properties of \ref baseUniforms.
	 * 
	 * Override this method when you are fiddeling with the view or perspective matrixes. Otherwise any resize event
	 * will re-calculate and overwrite them.

	 * \see \ref baseUniforms how projection and view matrix are calculated by default.
	 */
	virtual void onWindowResize();

protected:

	GLFramework& glFramework;
	SDLNativeWindow nativeWindow;

	/** \brief Current window size.
	 *
	 * Is being set by \ref onWindowResize() which is called when the window is created initially,
	 * or when a resize event is received.
	 */
	SizePixel windowSize;

	SDL_GLContextState* glContext = nullptr;

    GLint eglMajorVersion = 2;
    GLint eglMinorVersion = 0;

	/// \brief Each render surface, a.k.a. base window can have one root control.
	OevControls::RootControlSharedPtr rootControlPtr;
	
	/** \brief Base set of render uniforms
	 * 
	 * Default values are:
	 * - Camera position is z = 2*WindowHeight back, x = indowWidth/2 and y = windowHeight/2, looking at the 
	 *		lookAt point.\n
	 *		The lookAt point has the same x and y position as the camera position, but y = 0.
	 * - Projection Matrix: Builds a frustum box 
	 * 		of -WindowWidth/2 <= x <= WindowWidth, -WindowHeight/2 <= y<= 
	 * 		WindowHeight, -WindowHeight <= z <= WindowHeight at the camera position \n
	 *		The aperture angle is calculated from the camera position that one unit in the model space is exactly
	 *		one pixel on the screen.
	 * - View matrix is the looking from the camera position to the lookAt point. Up is positive Y direction.
	 *		This means in view space x=0 and y=0 is the lower left corner of the frustum on the y=0 plane.
	 *		For 2D modelling using y=0 x and y are in pixel size on the screen, and originate in the lower left corner
	 *		of the screen.
	 * - Model matrix is unity.
	 * - (Spot) Light direction is 0,0,1
	 * - (Spot) Light color is 0.5, 0.5, 0.3, 1.0 (Alpha)
	 * - Ambient Light color is 0.5, 0.5, 0.5, 1.0 (Alpha)
	 *
	 * This setup allows direct 2-D rendering when z=0 with every unit in model space is a pixel on the screen.
	 *
	 * If you do not like it use \ref getBaseUniforms to change any of the matrixes to your needs.
	 * In this case you need to override \ref onWindowResize() and/or \ref calculateViewProjectionMatrix() because the
	 * latter would over-write the view and projection matrix for any window size change.
	 */
	OevGLES::RenderStandardUniforms baseUniforms;

	/** \brief Shared pointer to the render context associated with \p this.
	 *
	 * The ownership of render context is shared with all controls which are based of \p this render surface.
	 * 
	 * The render context object, and the shared pointer are created and set in 
	 * \ref GLFramework::createRenderSurface ().
	 * Look no further where the shared point is being set \emoji smiley.
	 *
	 */
	RenderContextSharedPtr contextPtr;

	/// Only the friend \ref GLFramework shall be able to create a render surface.
	SDLRenderSurface(GLFramework& framework);

	/** \brief Calculate the view and the projection matrix according to the window size
	 *
	 * \see \ref baseUniforms how projection and view matrix are calculated by default.
	 */
	virtual void calculateViewProjectionMatrix();
	
};

// using SDLRenderSurfaceSharedPtr = std::shared_ptr<SDLRenderSurface>;
// using SDLRenderSurfaceWeakPtr = std::weak_ptr<SDLRenderSurface>;

} /* namespace OevGLES */

#endif /* GLES_EGLRENDERSURFACE_H_ */
