/*
 * GLFramework.cpp
 *
 *  Created on: Feb 2, 2025
 *      Author: hor
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "GLFramework.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif


GLFramework::GLFramework()
	: glTextGlob{*this},
	  sdlSurface{*this}
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

	SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
    SDL_SetHint(SDL_HINT_VIDEO_FORCE_EGL,"1");

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	LOG4CXX_INFO(logger,__PRETTY_FUNCTION__ << "Create native window, eglSurface and eglContext. Window size = "
			<< width << "x" << height);
	sdlSurface.createRenderSurface(width,height,PACKAGE_STRING);

	glBindVertexArrayOES = reinterpret_cast<PFNGLBINDVERTEXARRAYOESPROC>(SDL_GL_GetProcAddress("glBindVertexArrayOES"));
	glDeleteVertexArraysOES = reinterpret_cast<PFNGLDELETEVERTEXARRAYSOESPROC>(SDL_GL_GetProcAddress("glDeleteVertexArraysOES"));
	glGenVertexArraysOES = reinterpret_cast<PFNGLGENVERTEXARRAYSOESPROC>(SDL_GL_GetProcAddress("glGenVertexArraysOES"));
	glIsVertexArrayOES = reinterpret_cast<PFNGLISVERTEXARRAYOESPROC>(SDL_GL_GetProcAddress("glIsVertexArrayOES"));

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

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": vertexArrayUsable = " << vertexArrayUsable);

}

} /* namespace OevGLES */
