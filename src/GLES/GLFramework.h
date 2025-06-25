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

#include "SDL/SDLRenderSurface.h"
#include "GLTextRender/GLTextGlobals.h"
#include "GLTextRender/GLTextRenderer.h"
#include "Renderers/CirclePolygonVertexContainer.h"

#include "GLFrameWorkPtr.h"

namespace OevGLES {

class GLTextGlobals;
class SDLRenderSurface;

class GLFramework;

using GLFrameworkWeakPtr = std::weak_ptr<GLFramework>;
using GLFrameworkSharedPtr = std::shared_ptr<GLFramework>;

class GLFramework final {
public:
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
	 * \param width Width of the native window, and of the render context
	 * \param height Height of the native window, and of the render context
	 * \param windowName Name of the window in the native GUI framework
	 */
	void createRenderSurface (GLint width, GLint height,
			char const* windowName);

	GLTextGlobalsWeakPtr getGlTextGlob () {
		return glTextGlob;
	}

	SDLRenderSurface& getSDLSurface() {
		return sdlSurface;
	}

	CirclePolygonVertexContainer& getCircleVertexContainer() {
		return circleVertexContainer;
	}

	static bool isVertexArrayUsable() {
		return vertexArrayUsable;
	}

private:

	GLTextGlobalsSharedPtr glTextGlob;

	SDLRenderSurface sdlSurface;

	static bool vertexArrayUsable;

	CirclePolygonVertexContainer circleVertexContainer;

	GLFramework();

};

} /* namespace OevGLES */

#endif /* GLES_GLFRAMEWORK_H_ */
