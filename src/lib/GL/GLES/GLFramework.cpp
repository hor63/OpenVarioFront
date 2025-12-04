/*
 * GLFramework.cpp
 *
 *  Created on: Feb 2, 2025
 *      Author: hor
 */

#include <memory>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "GLES/SDL/SDLRenderSurface.h"
#include <log4cxx/logger.h>

#include "GLES/GLFramework.h"
#include "SDLUtil.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

GLFrameworkSharedPtr GLFramework::createFramework() {
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLFramework");
	}
#endif

	GLFrameworkSharedPtr ret ( new GLFramework);

	return ret;
}

GLFramework::GLFramework()

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

		initDone = true;
	
	}
	LOG4CXX_INFO(logger,__PRETTY_FUNCTION__ << "Create native window, eglSurface and eglContext. Window size = "
			<< width << "x" << height);
	auto SDLRenderSurfaceRawPtr = new SDLRenderSurface(*this);
	SDLRenderSurfaceSharedPtr sdlSurfaceSharedPtr (SDLRenderSurfaceRawPtr);
	
	// Use new and reset the shared pointer here instead using make_shared.
	// The constructor of RenderContext is private, and I am the only friend, but not the STL class
	// which implements make_shared.
	auto renderContextRawPtr = new RenderContext(sdlSurfaceSharedPtr);
	sdlSurfaceSharedPtr->renderContextSharedPointer.reset(renderContextRawPtr);
	
	sdlSurfaceSharedPtr->createRenderSurface(width,height,windowName);
	renderSurfacePtrMap.insert(std::pair(SDL_GetWindowID(sdlSurfaceSharedPtr->getNativeWindow()),sdlSurfaceSharedPtr));

	LOG4CXX_DEBUG(logger, "\t SDL Window ID = " 
		<< SDL_GetWindowID(sdlSurfaceSharedPtr->getNativeWindow()));

	return sdlSurfaceSharedPtr;
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
