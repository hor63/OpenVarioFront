/*
 * GLFramework.h
 *
 *  Created on: Feb 2, 2025
 *      Author: hor
 */

#ifndef GLES_GLFRAMEWORK_H_
#define GLES_GLFRAMEWORK_H_

#include "SDL/SDLRenderSurface.h"

#include "GLFrameWorkPtr.h"

namespace OevGLES {

class GLTextGlobals;
class SDLRenderSurface;
using SDLRenderSurfaceSharedPtr = std::shared_ptr<SDLRenderSurface>;
using SDLRenderSurfaceWeakPtr = std::weak_ptr<SDLRenderSurface>;

class GLFramework final {
public:

	using SDLRenderSurfacePtrMap = std::unordered_map<SDL_WindowID, SDLRenderSurfaceSharedPtr>;

	static GLFrameworkSharedPtr createFramework();

	~GLFramework();
	GLFramework(const GLFramework &other) = delete;
	GLFramework(GLFramework &&other) = delete;
	GLFramework& operator=(const GLFramework &other) = delete;
	GLFramework& operator=(GLFramework &&other) = delete;

	/** \brief Creates a native window, and a GLES 2.0 render context into that window
	 *
	 * A render surface and SDL window are owned and stored in \p this.
	 * You can retrieve it later with \ref getRenderSurfacePtr.
	 *
	 * \param width Width of the native window, and of the render context
	 * \param height Height of the native window, and of the render context
	 * \param windowName Name of the window in the native GUI framework
	 * \return Weak pointer to a new SDLRenderSurface object. 
	 * 		\p this remains owner.
	 * \see \ref getRenderSurfacePtr
	 */
	SDLRenderSurfaceWeakPtr createRenderSurface (GLint width, GLint height,
			char const* windowName);

	/** \return a render surface pointer for an SDL window ID
	 * 
	 * The render surface and underlying SDL window must have been created by
	 * \ref createRenderSurface before.
	 * 
	 * \param windowID ID of an SDL window which is managed by \p this.
	 * 
	 * \return Weak pointer to render surface which is associated with the SDL window
	 * 	identified by \p windowID. The weak pointer is empty when \p windowID does
	 * 	not identify an SDL window that is managed by 
	 * \see \ref createRenderSurface
	 */
	SDLRenderSurfaceWeakPtr getRenderSurfacePtr(SDL_WindowID windowID);

	/** \brief SDL event handler and dispatcher
	 * 
	 * Window related events are forwarded to the respective \ref SDLRenderSurface
	 * and related window
	 */
	bool handleSdlEvent (SDL_Event& event);

private:

	SDLRenderSurfacePtrMap renderSurfacePtrMap;

	bool initDone = false;
	
	GLFramework();

};

using GLFrameworkWeakPtr = std::weak_ptr<GLFramework>;
using GLFrameworkSharedPtr = std::shared_ptr<GLFramework>;

} /* namespace OevGLES */

#endif /* GLES_GLFRAMEWORK_H_ */
