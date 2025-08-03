/*
 * GLFramework.cpp
 *
 *  Created on: Feb 2, 2025
 *      Author: hor
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "GLES/GLFramework.h"
#include "SDLUtil.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

PFNGLBINDVERTEXARRAYOESPROC GLFramework::glBindVertexArrayOES = nullptr;
PFNGLDELETEVERTEXARRAYSOESPROC GLFramework::glDeleteVertexArraysOES = nullptr;
PFNGLGENVERTEXARRAYSOESPROC GLFramework::glGenVertexArraysOES = nullptr;
PFNGLISVERTEXARRAYOESPROC GLFramework::glIsVertexArrayOES = nullptr;
bool GLFramework::vertexArrayUsable = false;


GLFrameworkSharedPtr GLFramework::createFramework() {
	GLFrameworkSharedPtr ret ( new GLFramework);

	ret->glTextGlob.reset (new GLTextGlobals(ret));

	return ret;
}

GLFramework::GLFramework()
	:sdlSurface{*this},
	 circleVertexContainer()
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLFramework");
	}
#endif

}

GLFramework::~GLFramework() {
}

void GLFramework::createRenderSurface(GLint width, GLint height,
		const char *windowName) {

	SDL_ClearError();

	SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);

	// I want EGL as bridge to the native window and display system.
	if (!SDL_SetHint(SDL_HINT_VIDEO_FORCE_EGL,"1")) {
		reportSDLError(std::source_location::current(),
			 "SDL_Init(SDL_HINT_VIDEO_FORCE_EGL,1)");
	}

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		reportSDLError(std::source_location::current(),
			 "SDL_Init");
	}

	LOG4CXX_INFO(logger,__PRETTY_FUNCTION__ << "Create native window, eglSurface and eglContext. Window size = "
			<< width << "x" << height);
	sdlSurface.createRenderSurface(width,height,PACKAGE_STRING);

	if (glBindVertexArrayOES == nullptr) {
		glBindVertexArrayOES = reinterpret_cast<PFNGLBINDVERTEXARRAYOESPROC>(SDL_GL_GetProcAddress("glBindVertexArrayOES"));
	}
	if (glDeleteVertexArraysOES == nullptr) {
		glDeleteVertexArraysOES = reinterpret_cast<PFNGLDELETEVERTEXARRAYSOESPROC>(SDL_GL_GetProcAddress("glDeleteVertexArraysOES"));
	}
	if (glGenVertexArraysOES == nullptr) {
		glGenVertexArraysOES = reinterpret_cast<PFNGLGENVERTEXARRAYSOESPROC>(SDL_GL_GetProcAddress("glGenVertexArraysOES"));
	}
	if (glIsVertexArrayOES == nullptr) {
		glIsVertexArrayOES = reinterpret_cast<PFNGLISVERTEXARRAYOESPROC>(SDL_GL_GetProcAddress("glIsVertexArrayOES"));
	}

	if (!vertexArrayUsable) {
		std::string glExtensions (reinterpret_cast<char const *>(glGetString(GL_EXTENSIONS)));
		auto foundPos = glExtensions.find("GL_OES_vertex_array_object");
	
		if (glBindVertexArrayOES != nullptr
				&& glDeleteVertexArraysOES != nullptr
				&& glGenVertexArraysOES != nullptr
				&& glIsVertexArrayOES != nullptr
				&& foundPos != std::string::npos
				) {
			vertexArrayUsable = true;
		} else {
			vertexArrayUsable = false;
		}
	} // if (!vertexArrayUsable) {

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": vertexArrayUsable = " << vertexArrayUsable);

}

} /* namespace OevGLES */
