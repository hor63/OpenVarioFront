/*
 * GLFramework.h
 *
 *  Created on: Feb 2, 2025
 *      Author: hor
 */

#ifndef GLES_GLFRAMEWORK_H_
#define GLES_GLFRAMEWORK_H_

#include "OVFCommon.h"


#include <GLES2/gl2platform.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <unordered_map>

#include "SDL/SDLRenderSurface.h"
#include "GLTextRender/GLTextGlobals.h"
#include "GLTextRender/GLTextRenderer.h"
#include "Renderers/CirclePolygonVertexContainer.h"

#include "GLFrameWorkPtr.h"
#include "SDL3/SDL_video.h"

namespace OevGLES {

class GLTextGlobals;
class SDLRenderSurface;
using SDLRenderSurfaceSharedPtr = std::shared_ptr<SDLRenderSurface>;
using SDLRenderSurfaceWeakPtr = std::weak_ptr<SDLRenderSurface>;

class GLFramework final {
public:

	using SDLRenderSurfacePtrMap = std::unordered_map<SDL_WindowID, SDLRenderSurfaceSharedPtr>;

	static PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
	static PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;
	static PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
	static PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES;

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

	/** \Return a render surface pointer for an SDL window ID
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

	GLTextGlobalsWeakPtr getGlTextGlob () {
		return glTextGlob;
	}

	CirclePolygonVertexContainer& getCircleVertexContainer() {
		return circleVertexContainer;
	}

	static bool isVertexArrayUsable() {
		return vertexArrayUsable;
	}

	/** \brief SDL event handler and dispatcher
	 * 
	 * Window related events are forwarded to the respective \ref SDLRenderSurface
	 * and releated
	 */
	bool handleSLEDvent (SDL_Event& event);

private:

	GLTextGlobalsSharedPtr glTextGlob;

	static bool vertexArrayUsable;
	
	SDLRenderSurfacePtrMap renderSurfacePtrMap;

	CirclePolygonVertexContainer circleVertexContainer;
	
	bool initDone = false;

	GLFramework();

};

using GLFrameworkWeakPtr = std::weak_ptr<GLFramework>;
using GLFrameworkSharedPtr = std::shared_ptr<GLFramework>;

} /* namespace OevGLES */

#endif /* GLES_GLFRAMEWORK_H_ */
