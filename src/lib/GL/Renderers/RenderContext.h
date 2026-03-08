/*
 * ControlsContext.h
 *
 *  Created on: Oct 24, 2025
 *      Author: hor
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2025  Kai Horstmann
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

#ifndef LIB_CONTROLS_CONTROLSCONTEXT_H_
#define LIB_CONTROLS_CONTROLSCONTEXT_H_

#include "VecMat.h"
#include "GLES/GLObjectWrappers.h"
#include "Renderers/CirclePolygonVertexContainer.h"

namespace OevGLES {

class GLFramework;

class SDLRenderSurface;
using SDLRenderSurfaceSharedPtr = std::shared_ptr<SDLRenderSurface>;
using SDLRenderSurfaceWeakPtr = std::weak_ptr<SDLRenderSurface>;

class GLTextGlobals;
using GLTextGlobalsWeakPtr = std::weak_ptr<GLTextGlobals>;
using GLTextGlobalsSharedPtr = std::shared_ptr<GLTextGlobals>;

struct RenderContext {
	friend class SDLRenderSurface;
	friend class GLFramework;
	
	PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = nullptr;
	PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES = nullptr;
	PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES = nullptr;
	PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES = nullptr;
	bool vertexArrayIsUsable = false;

	Vec4ShPtr foregroundColorPtr;
	Vec4ShPtr backgroundColorPtr;

	Vec4ShPtr inactiveForegroundColorPtr;
	Vec4ShPtr inactiveBackgroundColorPtr;

	Vec4ShPtr textForegroundColorPtr;
	Vec4ShPtr textBackgroundColorPtr;

	Vec4ShPtr buttonForegroundColorPtr;
	Vec4ShPtr buttonBackgroundColorPtr;

	Vec4ShPtr dialogTitleForegroundColorPtr;
	Vec4ShPtr dialogTitleBackgroundColorPtr;

	Vec4ShPtr scrollBarForegroundColorPtr;
	Vec4ShPtr scrollBarBackgroundColorPtr;

	std::string fontNameList = "Noto Sans";
	double textSizePoints = 11.0;
	double screenResolutionDpiX = 96.0;
	double screenResolutionDpiY = 96.0;
	
	SDLRenderSurfaceWeakPtr sdlRenderSurfacePtr;
	
	GLTextGlobalsSharedPtr glTextGlobSharedPtr;


	/** \brief vertex buffer handle to a quad vertex buffer with corners at 0,0 and 1,1 (and z at 0)
	 *
	 * It serves as a reusable vertex buffer for the myriad of rectangles to be drawn for dialogs.
	 * You just need to scale it yourself to the desired size with a uniform
	 * 
	 * The buffer is created in \ref SDLRenderSurface::createRenderSurface(). 
	 */
	GLBufferObject quadVertexBuffer;
	static constexpr GLuint quadVertexBufferNumVertexes = 4;
	GLVertexArrayObject quadVertexArray;
	
	CirclePolygonVertexContainer& getCircleVertexContainer() {
		return circleVertexContainer;
	}


	// Copying is not allowed, but moving.
	RenderContext (RenderContext const& source) = delete;
	RenderContext (RenderContext && source) = default;
	~RenderContext() = default;
	
	RenderContext & operator = (RenderContext const& source) = delete;
	RenderContext & operator = (RenderContext && source) = default;
	
private:
	
	/// Make the constructor private, except for the friends SDLRenderSurface and GLFramework
	RenderContext (
	SDLRenderSurfaceWeakPtr const sdlRenderSurfacePtr,
	Vec4 const &foregroundColor = Vec4{0,0,0.2,1},
	Vec4 const &backgroundColor = Vec4{1,1,1,1}
	);

	CirclePolygonVertexContainer circleVertexContainer;

};

using RenderContextSharedPtr = std::shared_ptr<RenderContext>;
using RenderContextWeakPtr = std::weak_ptr<RenderContext>;

} // namespace OevGLES

#endif /* LIB_CONTROLS_CONTROLSCONTEXT_H_ */
