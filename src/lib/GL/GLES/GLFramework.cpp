/*
 * GLFramework.cpp
 *
 *  Created on: Feb 2, 2025
 *      Author: hor
 */

#include <log4cxx/logger.h>
#include <memory>
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
	:circleVertexContainer()
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLFramework");
	}
#endif

}

GLFramework::~GLFramework() {
}

SDLRenderSurfaceWeakPtr GLFramework::createRenderSurface(GLint width, GLint height,
		const char *windowName) {

	SDL_ClearError();

	if (!initDone) {
		SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
	
		// I want EGL as bridge to the native window and display system.
		if (!SDL_SetHint(SDL_HINT_VIDEO_FORCE_EGL,"1")) {
			OevUtil::reportSDLError(std::source_location::current(),
				 "SDL_Init(SDL_HINT_VIDEO_FORCE_EGL,1)");
		}
	
		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
			OevUtil::reportSDLError(std::source_location::current(),
				 "SDL_Init");
		}
	
	}
	LOG4CXX_INFO(logger,__PRETTY_FUNCTION__ << "Create native window, eglSurface and eglContext. Window size = "
			<< width << "x" << height);
	auto sdlSurface = std::make_shared<SDLRenderSurface>(*this);
	sdlSurface->createRenderSurface(width,height,windowName);
	renderSurfacePtrMap.insert(std::pair(SDL_GetWindowID(sdlSurface->getNativeWindow()),sdlSurface));

	if (!initDone) {

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
		
		initDone = true;
	}

	return sdlSurface;
}

SDLRenderSurfaceWeakPtr GLFramework::getRenderSurfacePtr(SDL_WindowID windowID) {
	SDLRenderSurfaceWeakPtr ret;
	
	auto iter = renderSurfacePtrMap.find(windowID);
	
	if (iter != renderSurfacePtrMap.end()) {
		ret = iter->second;
	}
	
	return ret;
}

bool GLFramework::handleSdlEvent (SDL_Event& event) {

	if (event.type == SDL_EVENT_QUIT) {
		LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
			<< ": Event type = SDL_EVENT_QUIT; return false, and terminate"
		);

		return false;
		}

	SDL_WindowID windowID = 0;
	
	if (event.common.type >= SDL_EVENT_WINDOW_FIRST &&
		event.common.type <= SDL_EVENT_WINDOW_LAST)
		windowID = event.window.windowID;
	else if (event.common.type == SDL_EVENT_KEY_DOWN ||
		event.common.type == SDL_EVENT_KEY_UP) 
		windowID = event.key.windowID;
	else if (event.common.type == SDL_EVENT_TEXT_EDITING)
		windowID = event.edit.windowID;
	else if (event.common.type == SDL_EVENT_TEXT_EDITING_CANDIDATES)
		windowID = event.edit_candidates.windowID;
	else if (event.common.type == SDL_EVENT_TEXT_INPUT)
		windowID = event.text.windowID;
	else if (event.common.type == SDL_EVENT_MOUSE_MOTION)
		windowID = event.motion.windowID;
	else if (event.common.type == SDL_EVENT_MOUSE_BUTTON_DOWN 
		|| event.common.type == SDL_EVENT_MOUSE_BUTTON_UP)
		windowID = event.button.windowID;
	else if (event.common.type == SDL_EVENT_MOUSE_WHEEL)
		windowID = event.wheel.windowID;
	else if (event.common.type == SDL_EVENT_FINGER_DOWN
		|| event.common.type == SDL_EVENT_FINGER_UP
		|| event.common.type == SDL_EVENT_FINGER_MOTION
		|| event.common.type == SDL_EVENT_FINGER_CANCELED)
		windowID = event.tfinger.windowID;
	else if (event.common.type == SDL_EVENT_DROP_BEGIN
		|| event.common.type == SDL_EVENT_DROP_FILE
		|| event.common.type == SDL_EVENT_DROP_TEXT
		|| event.common.type == SDL_EVENT_DROP_COMPLETE
		|| event.common.type == SDL_EVENT_DROP_POSITION)
		windowID = event.drop.windowID;
	else if (event.common.type >= SDL_EVENT_USER &&
		event.common.type <= SDL_EVENT_LAST-1)
		windowID = event.user.windowID;

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
		<< ": windowID = " << windowID
		);

	if (windowID != 0) {
		auto renderSurfaceIter = renderSurfacePtrMap.find(windowID);
		if (renderSurfaceIter != renderSurfacePtrMap.end()) {
			LOG4CXX_DEBUG(logger, "\tFound window  " 
				<< SDL_GetWindowTitle(renderSurfaceIter->second->getNativeWindow())
			);
			return renderSurfaceIter->second->handleSLEDvent(event);
		}
	}

	return true;
}

} /* namespace OevGLES */
