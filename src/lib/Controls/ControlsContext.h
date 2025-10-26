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

#include <memory>
#include <string>

#include "VecMat.h"
#include "GLBufferObject.h"

namespace OevGLES {

class SDLRenderSurface;
using SDLRenderSurfaceSharedPtr = std::shared_ptr<SDLRenderSurface>;
using SDLRenderSurfaceWeakPtr = std::weak_ptr<SDLRenderSurface>;

}

namespace OevControls {
 
struct ControlsContext {
	
	OevGLES::Vec4ShPtr foregroundColorPtr;
	OevGLES::Vec4ShPtr backgroundColorPtr;

	/// \brief by default mapped to \ref foregroundColorPtr
	OevGLES::Vec4ShPtr textForegroundPtr;
	/// \brief by default mapped to \ref backgroundColorPtr
	OevGLES::Vec4ShPtr textBackgroundPtr;

	/// \brief by default mapped to \ref foregroundColorPtr
	OevGLES::Vec4ShPtr buttonForegroundPtr;
	/// \brief by default mapped to \ref backgroundColorPtr
	OevGLES::Vec4ShPtr buttonBackgroundPtr;

	/// \brief by default mapped to \ref foregroundColorPtr
	OevGLES::Vec4ShPtr dialogTitleForegroundPtr;
	/// \brief by default mapped to \ref backgroundColorPtr
	OevGLES::Vec4ShPtr dialogTitleBackgroundPtr;

	/// \brief by default mapped to \ref foregroundColorPtr
	OevGLES::Vec4ShPtr scrollBarForegroundPtr;
	/// \brief by default mapped to \ref backgroundColorPtr
	OevGLES::Vec4ShPtr scrollBarBackgroundPtr;

	std::string fontNameList = "Noto Sans";
	double textSizePoints = 11.0;
	double screenResolutionDpiX = 96.0;
	double screenResolutionDpiY = 96.0;
	
	OevGLES::SDLRenderSurfaceWeakPtr sdlRenderSurfacePtr;

	/** \brief vertex buffer handle to a quad vertex buffer with corners at 0,0 and 1,1 (and z at 0)
	 *
	 * It serves as a reusable vertex buffer for the myriad of rectangles to be drawn for dialogs.
	 * You just need to scale it yourself to the desired size with a 
	 */
	OevGLES::GLBufferObjectSharedPtr quadVertexBufferPtr;
	
	ControlsContext (
		OevGLES::SDLRenderSurfaceWeakPtr const& sdlRenderSurfacePtr,
		OevGLES::Vec4 const &foregroundColor = OevGLES::Vec4{0,0,0,1},
		OevGLES::Vec4 const &backgroundColor = (OevGLES::Vec4{1,1,1,1})
		);

	// The rest is default fare.
	ControlsContext (ControlsContext const& source) = default;
	ControlsContext (ControlsContext && source) = default;
	~ControlsContext() = default;
	
	ControlsContext & operator = (ControlsContext const& source) = default;
	ControlsContext & operator = (ControlsContext && source) = default;
};

} // namespace OevControls

#endif /* LIB_CONTROLS_CONTROLSCONTEXT_H_ */
